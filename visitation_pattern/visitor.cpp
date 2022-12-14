#include <iostream>
using namespace std;

class Number;
class Integer;
class Double;

class Visitor {
  public:
    virtual void Visit(Integer &integer) = 0;

    virtual void Visit(Double &double_) = 0;
};

class ConstVisitor {
  public:
    virtual void Visit(const Integer &integer) = 0;

    virtual void Visit(const Double &double_) = 0;
};

class Number {
  public:
    virtual void Accept(Visitor &visitor) = 0;

    virtual void Accept(ConstVisitor &visitor) const = 0;

    virtual void PrintInStream(ostream &out) const = 0;

    virtual ~Number() {}
};

ostream &operator<<(ostream &out, const Number &number) {
  number.PrintInStream(out);
  return out;
}

template<class Derived>
class NumberCrtp : public Number {
  public:
    void Accept(Visitor &visitor) override {
      visitor.Visit(static_cast<Derived&>(*this));
    }

    void Accept(ConstVisitor &visitor) const override {
      visitor.Visit(static_cast<const Derived&>(*this));
    }
};

class Integer : public NumberCrtp<Integer> {
  public:
    int data;

    Integer(int data) : data(data) {}

    void PrintInStream(ostream &out) const override {
      out << data;
    }
};

class Double : public NumberCrtp<Double> {
  public:
    double data;

    Double(double data) : data(data) {}

    void PrintInStream(ostream &out) const override {
      out << data;
    }
};

class TimesTwoVisitor : public Visitor {
  public:
    void Visit(Integer &integer) override {
      integer.data *= 2;
    }

    void Visit(Double &double_) override {
      double_.data *= 2;
    }
};

int main() {
  TimesTwoVisitor times_two;
  Number* number1 = new Integer(21);
  Number* number2 = new Double(3.1415);
  number1->Accept(times_two);
  number2->Accept(times_two);
  cout << *number1 << endl;
  cout << *number2 << endl;
}

