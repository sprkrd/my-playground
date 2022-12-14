#include <typeinfo>
#include <typeindex>
#include <map>
#include <memory>
#include <tuple>
#include <iostream>
#include <utility>
#include <functional>

template<typename T, size_t size>
struct tuple_n;

template<typename T, size_t size>
struct tuple_n {
  template<typename... Args>
  using type = typename tuple_n<T,size-1>::template type<T,Args...>;
};

template<typename T>
struct tuple_n<T,0> {
  template<typename... Args>
  using type = std::tuple<Args...>;
};

template<size_t size, typename T>
using tuple_of = typename tuple_n<T,size>::template type<>;

template<typename T>
std::type_index type_index(T&& t) {
  return std::type_index(typeid(t));
}

template<typename ...Args>
struct type_indices;

template<>
struct type_indices<> {
  static std::tuple<> get() {
    static std::tuple<> value = std::tuple<>();
    return value;
  }
};

template<typename Head, typename ...Tail>
struct type_indices<Head,Tail...> {
  static tuple_of<1+sizeof...(Tail),std::type_index> get() {
    static tuple_of<1+sizeof...(Tail),std::type_index> value = std::tuple_cat(
        std::make_tuple(std::type_index(typeid(Head))),
        type_indices<Tail...>::get());
    return value;
  }
};

template<typename U, typename T>
struct safe_cast {
  static U cast(T&& t) {
    return static_cast<U>(std::forward<T>(t));
  }
};

template<typename U, typename T>
struct safe_cast<std::shared_ptr<U>,std::shared_ptr<T>> {
  static std::shared_ptr<U> cast(std::shared_ptr<T> t) {
    return std::static_pointer_cast<U>(t);
  }
};

template<typename ReturnType, typename ...BaseClasses>
class Multimethod {
  public:
    typedef std::function<ReturnType(BaseClasses...)> Handler;
    typedef tuple_of<sizeof...(BaseClasses), std::type_index> TypeKey;

    template<typename ...ArgsIn>
    void addMethod(std::function<ReturnType(ArgsIn...)> handler) {
      _vtable[type_indices<ArgsIn...>::get()] = [handler](BaseClasses... args_in) {
        return handler(safe_cast<ArgsIn,BaseClasses>::cast(args_in)...);
      };
    }

    ReturnType operator()(BaseClasses&& ...args) const {
      auto key = TypeKey{type_index(args)...};
      auto entry = _vtable.find(key);
      if (entry == _vtable.end())
        throw std::runtime_error("Method not found");
      auto handler = entry->second;
      return handler(std::forward<BaseClasses>(args)...);
    }

  private:
    std::map<TypeKey, Handler> _vtable;
};

struct Thing {
  virtual ~Thing() {
  }
};

struct Asteroid : public Thing {
};

struct Spaceship : public Thing {
};

// declare_method(void, collisionOutcome, const Thing&, const Thing&)

Multimethod<void,const Thing&, const Thing&>& get_collisionOutcomeMultimethod() {
  static Multimethod<void,const Thing&,const Thing&> multimethod;
  return multimethod;
}

void collisionOutcome(const Thing& thing1, const Thing& thing2) {
  return get_collisionOutcomeMultimethod()(thing1,thing2);
}

//define_method(collisionOutcome, void, const Asteroid& asteroid, const Spaceship& spaceship) {
  //std::cout << "Spaceship destroyed!" << std::endl;
//}

// translates to

namespace {

bool collisionOutcome__0__init() {
  std::function<void(const Asteroid& asteroid, const Spaceship& spaceship)> fn = [](const Asteroid&, const Spaceship&) {
    std::cout << "Spaceship destroyed!" << std::endl;
  };
  get_collisionOutcomeMultimethod().addMethod(fn);
  return true;
}

bool collisionOutcome__0 = collisionOutcome__0__init();

bool collisionOutcome__1__init() {
  std::function<void(const Asteroid& asteroid1, const Asteroid& asteroid2)> fn = [](const Asteroid&, const Asteroid&) {
    std::cout << "Both asteroids destroyed!" << std::endl;
  };
  get_collisionOutcomeMultimethod().addMethod(fn);
  return true;
}

bool collisionOutcome__1 = collisionOutcome__1__init();

}

void print(const std::shared_ptr<Thing>& thing) {
  std::cout << "Thing" << std::endl;
}

int main() {
  collisionOutcome(Asteroid(), Spaceship());
  collisionOutcome(Asteroid(), Asteroid());
  auto asteroid = std::make_shared<Asteroid>();
  print(asteroid);
}


