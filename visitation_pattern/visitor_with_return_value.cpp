#include <iostream>
#include <type_traits>
using namespace std;

#define GENERATE_TAG(Base,Type) template<>\
std::size_t TagHolder<const Base, const Type>::s_tag = GetTag<Base,Type>()

template<class... Types>
struct TypeCollection;

template<>
struct TypeCollection<> {
  constexpr static std::size_t size = 0;

  template<class Type>
  constexpr static std::size_t GetTag() {
    return 0;
  }
};

template<class Head, class... Tail>
struct TypeCollection<Head, Tail...> {
  typedef Head head;
  typedef TypeCollection<Tail...> tail;
  constexpr static std::size_t size = tail::size + 1;

  template<class Type>
  constexpr static std::size_t GetTag() {
    return std::is_same<typename std::remove_cv<Type>::type,head>::value?
        size : tail::template GetTag<Type>();
  }

  constexpr static std::size_t GetTagOfFirst() {
    return size;
  }
};

template<class ReturnType>
struct Visitor {
  typedef ReturnType return_type;
};

template<class Collection>
struct Dispatcher {
  template<class Visitor>
  static typename Visitor::return_type Dispatch(std::size_t tag, void* data, Visitor& visitor) {
    if (tag == Collection::GetTagOfFirst()) {
      auto* casted = (typename Collection::head*)data;
      return visitor.Visit(*casted);
    } else {
      return Dispatcher<typename Collection::tail>::Dispatch(tag, data, visitor);
    }
  }
};

template<>
struct Dispatcher<TypeCollection<>> {
  template<class Visitor>
  static typename Visitor::return_type Dispatch(std::size_t tag, void* data, Visitor& visitor) {
    throw std::out_of_range("invalid tag");
  }
};

template<>
struct Dispatcher<TypeCollection<>> {
  template<class Visitor>
  static typename Visitor::return_type Dispatch(std::size_t tag, void* data, Visitor& visitor) {
    throw std::out_of_range("invalid tag");
  }
};

class Number {
  public:

    template<class T>
    Number(T data) : tag_(AllowedTypes::GetTag<T>()), data_(new T(std::move(data))) {
      static_assert(AllowedTypes::GetTag<T>() != 0);
    }

    template<class Visitor>
    typename Visitor::return_type Accept(Visitor& visitor) {
      Dispatcher<AllowedTypes>::Dispatch(tag_, data_, visitor);
    }

    ~Number() {
      Destructor destructor;
      Dispatcher<AllowedTypes>::Dispatch(tag_, data_, destructor);
    }

  private:
    typedef TypeCollection<int,double> AllowedTypes;

    struct Data {
      std::size_t tag;
      void* ptr;
    };

    Data 

    std::size_t tag_;
    void* data_;
};

class Printer : public Visitor<void> {
  public:
    template<class T>
    void Visit(const T& value) {
      cout << value;
    }
};

int main() {
  //typedef TypeCollection<int, double> Collection;
  //cout << Collection::size << endl;
  //cout << Collection::GetTag<int>() << endl;
  //cout << Collection::GetTag<double>() << endl;
  //cout << Collection::GetTag<long long>() << endl;
  Printer printer;
  Number n(10.5);
  n.Accept(printer);
  cout << endl;
  //cout << GetTag<Number,double>() << endl;
  //cout << GetTag<Number,int>() << endl;
  //cout << GetTag<Number,int>() << endl;
  //cout << GetTag<Number,double>() << endl;
  //any x;
  //x = unique_ptr<int>(new int(5));
  //cout << *any_cast<unique_ptr<int>&>(x) << endl;
  //TimesTwoVisitor times_two;
  //auto x = NumberPtr(new Integer{5});
  //any result = x->Accept(times_two);
  //cout << any_cast<NumberPtr&>(result).use_count() << endl;
  //Number* ret = any_cast<Number>(&result);
  //cout << ret << endl;
}

