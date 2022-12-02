#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <variant>
#include <vector>
using namespace std;

class Expression;
ostream& operator<<(ostream& out, const Expression& exp);

class Expression {
    public:
        enum Type {num, var, add, mul, div, mod, eql, neq};

        typedef int64_t integer;

        Expression(integer value = 0) : p_type(num), p_content(value) {
        }

        Expression(string variable) : p_type(var), p_content(move(variable)) {
        }

        Type type() const {
            return p_type;
        }

        int as_num() const {
            return get<integer>(p_content);
        }

        const string& as_var() const {
            return get<string>(p_content);
        }

        bool atomic() const {
            return p_type == num || p_type == var;
        }

        const Expression& arg(int i) const {
            return p_arguments[i];
        }

        int arguments() const {
            return p_arguments.size();
        }

        Expression simplify() const {
            Expression simplified;
            switch (p_type) {
                case num:
                case var: {
                    simplified = *this;
                    vector<Expression> arguments;
                    break;
                }
                case add:
                case mul: {
                    integer neutral = p_type==add? 0 : 1;
                    integer acc = neutral;
                    for (const Expression& arg : p_arguments) {
                        Expression simplified_arg = arg.simplify();
                        if (simplified_arg.type() == num) {
                            integer value = simplified_arg.as_num();
                            if (p_type == add)
                                acc += value;
                            else {
                                acc *= value;
                                if (value == 0) {
                                    simplified.p_arguments.clear();
                                    break;
                                }
                            }
                        }
                        else
                            simplified.p_arguments.push_back(move(simplified_arg));
                    }

                    if (acc != neutral || simplified.arguments() == 0) {
                        simplified.p_arguments.emplace_back(acc);
                    }

                    if (simplified.arguments() == 1)
                        simplified = move(simplified.arg(0));
                    else
                        simplified.p_type = p_type;
                    break;
                }
                case div: {
                    Expression arg0 = p_arguments[0].simplify();
                    Expression arg1 = p_arguments[1].simplify();
                    if (arg0.p_type == div) {
                        arg1 = (arg0.arg(1)*arg1).simplify();
                        arg0 = move(arg0.arg(0));
                    }
                    if (arg0.p_type == num && arg1.p_type == num)
                        simplified = arg0.as_num() / arg1.as_num();
                    else if (arg1.p_type == num && arg1.as_num() == 1) {
                        simplified = move(arg0);
                    }
                    else {
                        simplified.p_arguments.push_back(move(arg0));
                        simplified.p_arguments.push_back(move(arg1));
                        simplified.p_type = div;
                    }
                    break;
                }
                case eql: {
                    Expression arg0 = p_arguments[0].simplify();
                    Expression arg1 = p_arguments[1].simplify();
                    if (arg0.p_type == num && arg1.p_type == num)
                        simplified = integer(arg0.as_num() == arg1.as_num());
                    else if (arg0.p_type == eql && arg1.p_type == num) {
                        if (arg1.as_num() == 1)
                            simplified = move(arg0);
                        else if (arg1.as_num() == 0) {
                            simplified = move(arg0);
                            simplified.p_type = neq;
                        }
                    }
                    else {
                        simplified.p_arguments.push_back(move(arg0));
                        simplified.p_arguments.push_back(move(arg1));
                        simplified.p_type = eql;
                    }
                    break;
                }
                default: {
                    Expression arg0 = p_arguments[0].simplify();
                    Expression arg1 = p_arguments[1].simplify();
                    if (arg0.p_type == num && arg1.p_type == num) {
                        integer arg0_value = arg0.as_num();
                        integer arg1_value = arg1.as_num();
                        if (p_type == mod) simplified = arg0_value % arg1_value;
                        else simplified = integer(arg0_value != arg1_value);
                    }
                    else {
                        simplified.p_arguments.push_back(p_arguments[0].simplify());
                        simplified.p_arguments.push_back(p_arguments[1].simplify());
                        simplified.p_type = p_type;
                    }
                    break;
                }
            }

            return simplified;
        }

        Expression operator+(const Expression& other) const {
            return new_compound_expression(add, other);
        }

        Expression operator*(const Expression& other) const {
            return new_compound_expression(mul, other);
        }

        Expression operator/(const Expression& other) const {
            return new_compound_expression(div, other);
        }

        Expression operator%(const Expression& other) const {
            return new_compound_expression(mod, other);
        }

        Expression operator==(const Expression& other) const {
            return new_compound_expression(eql, other);
        }

        Expression operator!=(const Expression& other) const {
            return new_compound_expression(neq, other);
        }

        string to_str() const {
            ostringstream out;
            out << *this;
            return out.str();
        }

    private:

        Expression new_compound_expression(Type type, const Expression& other) const {
            Expression result;
            result.p_type = type;
            result.add_argument(*this);
            result.add_argument(other);
            return result;
        }

        void add_argument(const Expression& exp) {
            if ((p_type == add || p_type == mul) && p_type == exp.p_type)
                p_arguments.insert(p_arguments.end(),
                        exp.p_arguments.begin(), exp.p_arguments.end());
            else
                p_arguments.push_back(exp);
        }

        Type p_type;
        vector<Expression> p_arguments;
        variant<integer,string> p_content;
};

ostream& operator<<(ostream& out, const Expression& exp) {
    static const char* operator_symbols[] = {"", "", " + ", "*", "/", "%", " == ", " != "};

    switch (exp.type()) {
        case Expression::num: {
            if (exp.as_num() < 0)
                out << '(' << exp.as_num() << ')';
            else
                out << exp.as_num();
            break;
        }
        case Expression::var: {
            out << exp.as_var();
            break;
        }
        default: {
            int op_index = static_cast<int>(exp.type());
            for (int i = 0; i < exp.arguments(); ++i) {
                if (i != 0) 
                    out << operator_symbols[op_index];
                if (exp.type() == Expression::add || exp.arg(i).atomic())
                    out << exp.arg(i);
                else
                    out << '(' << exp.arg(i) << ')';
            }
            break;
        }
    }
    return out;
}


int main() {
}
