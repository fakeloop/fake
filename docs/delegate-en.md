# `delegate.h`

- A versatile, compile‑time configurable delegate (multicast / singlecast) library for C++23.
- Supports static‑signature delegates (`delegate<Ret(Args...)>`) and runtime‑polymorphic delegates (`delegate<void>`).
- Offers four storage policies (`shared_ptr`, `pointer`, `shared_ptrs`, `pointers`) and a parent/child composition mechanism.
- Provides rich invocation methods: `cast`, `pick`, `seek`, `mono`, `rash`, and a simple call operator.
- Can inject compile‑time “aspect” lambdas that are instantiated for every delegate operation (assign, invoke, amount, …), enabling meta‑programming introspection.

---

## Overview

The `delegate.h` header defines two main class templates:

```c++
namespace fake {
    template<typename Signature, typename Policy = void, template<typename> typename Aspect = pack_t>
    struct delegate;
    
    template<typename Policy, template<typename> typename Aspect>
    struct delegate<void, Policy, Aspect>;   // runtime‑polymorphic version
}
```

- `Signature` can be a function type (e.g. `int(const std::string&, int, float)`) for static signature delegates, or `void` for the runtime‑polymorphic version.
- `Policy` customises the storage and parent‑child relationship. Predefined policy helpers live in `fake::tool::delegate`.
- `Aspect` is a template that wraps every operation with a “message” type; it can be used to inject compile‑time behaviour (e.g. logging instantiated signatures).

---

## Predefined storage policies

These are found in `fake::tool::delegate` and are used as `transform_type` inside a custom policy.

| Policy class                | Storage representation                                    |
|-----------------------------|-----------------------------------------------------------|
| `shared_ptr`                | `std::shared_ptr<const Delegate>`                         |
| `pointer`                   | `const Delegate*`                                         |
| `shared_ptrs`               | `std::vector<std::shared_ptr<const Delegate>>` (multicast)|
| `pointers`                  | `std::vector<const Delegate*>` (multicast)                |

When you define your own policy (see below), you must specify one of these as `transform_type`.

---

## Basic delegate with static signature

### brief

A `delegate<Ret(Args...)>` holds a collection of callable objects that all accept arguments convertible to `Args...` and return a type convertible to `Ret`.
It can be used as a single‑cast or multicast delegate depending on whether you store multiple callables.
Invocation can be done through various member functions that handle multiplicity differently.

### syntax

```c++
template<typename Ret, typename... Args, typename Policy = void, template<typename> typename Aspect = pack_t>
struct delegate<Ret(Args...), Policy, Aspect>;
```

### example

```c++
#include <print>
#include <iostream>
#include "delegate.h"
#include "type_format.h"   // for fake::io::fancy (optional, only for pretty printing)

int main()
{
    using namespace fake::literals;
    
    // Helper to print results in a fancy way (from type_format.h)
    auto result = [](auto&& call) {
        try {
            std::cout << fake::io::fancy<>(call()) << std::endl;
        } catch (const std::runtime_error& e) {
            std::println("error @ {}", e.what());
        }
    };
    
    fake::delegate<int(std::string&&, int, float)> delegate;
    
    // Empty delegate: all invocation methods return defaults or throw
    result([&]{ return delegate("foobar", 114, 514.0f); });          // false (bool)
    result([&]{ return delegate.seek("foobar", 114, 514.0f); });     // nullopt
    result([&]{ return delegate.mono("foobar", 114, 514.0f); });     // 0 (default)
    result([&]{ return delegate.rash("foobar", 114, 514.0f); });     // throws
    
    std::println("");
    
    // Assign a first lambda
    std::size_t handle = delegate = [](std::string&& str, auto... args) {
        std::println("lambda1: \"{}\", {}, {}", str, args...);
        return 114514;
    };
    
    std::println("handle: {}, size: {}", handle, delegate.size());            // 2, 1
    std::println("amount<...>: {}", delegate.amount<std::string, int, float>()); // 1
    std::println("amount(...): {}", delegate.amount("foobar", 114, 514.0f));    // 1
    
    result([&]{ return delegate("foobar", 114, 514.0f); });          // true, lambda1 called
    result([&]{ return delegate.seek("foobar", 114, 514.0f); });     // optional{114514}
    result([&]{ return delegate.mono("foobar", 114, 514.0f); });     // 114514
    result([&]{ return delegate.rash("foobar", 114, 514.0f); });     // 114514
    
    std::println("");
    
    // Assign a second lambda (different signature, but compatible)
    delegate = [](const std::string& str, int i, float f) {
        std::println("lambda2: \"{}\", {}, {}", str, i, f);
        return 1919810;
    };
    
    result([&]{ return delegate("foobar", 114, 514.0f); });          // true, both lambdas called
    result([&]{ return delegate.pick("foobar", 114, 514.0f); });     // vector of pairs (id, result)
    // seek/mono/rash now throw because size > 1
    result([&]{ return delegate.seek("foobar", 114, 514.0f); });     // throws
    result([&]{ return delegate.mono("foobar", 114, 514.0f); });     // throws
    result([&]{ return delegate.rash("foobar", 114, 514.0f); });     // throws
    
    std::println("");
    
    // Erase the first handle
    delegate.erase(handle);
    
    result([&]{ return delegate("foobar", 114, 514.0f); });          // true, only lambda2 called
    result([&]{ return delegate.pick("foobar", 114, 514.0f); });     // vector with one result
    result([&]{ return delegate.seek("foobar", 114, 514.0f); });     // optional{1919810}
    result([&]{ return delegate.mono("foobar", 114, 514.0f); });     // 1919810
    result([&]{ return delegate.rash("foobar", 114, 514.0f); });     // 1919810
}
```

**output (colours omitted):**

```plain
bool : 0
std::optional<int32_t> : {
|   
}
int32_t : 0
error @ fake::delegate::rash<int(std::__cxx11::basic_string<char>&&, int, float)>(): delegate is NOT unique.

handle: 2, size: 1
amount<...>: 1
amount(...): 1
lambda1: "foobar", 114, 514
bool : 1
lambda1: "foobar", 114, 514
std::optional<int32_t> : {
|   int32_t value : 114514
}
lambda1: "foobar", 114, 514
int32_t : 114514
lambda1: "foobar", 114, 514
int32_t : 114514

lambda2: "foobar", 114, 514
lambda1: "foobar", 114, 514
bool : 1
lambda2: "foobar", 114, 514
lambda1: "foobar", 114, 514
std::vector<std::pair<uint64_t, int32_t>> : {
|   std::pair<uint64_t, int32_t> : {
|   |   uint64_t first : 3,
|   |   int32_t second : 1919810
|   },
|   std::pair<uint64_t, int32_t> : {
|   |   uint64_t first : 2,
|   |   int32_t second : 114514
|   }
}
error @ fake::delegate::seek<int(std::__cxx11::basic_string<char>&&, int, float)>(): delegate is NOT unique.
error @ fake::delegate::mono<int(std::__cxx11::basic_string<char>&&, int, float)>(): delegate is NOT unique.
error @ fake::delegate::rash<int(std::__cxx11::basic_string<char>&&, int, float)>(): delegate is NOT unique.

lambda2: "foobar", 114, 514
bool : 1
lambda2: "foobar", 114, 514
std::vector<std::pair<uint64_t, int32_t>> : {
|   std::pair<uint64_t, int32_t> : {
|   |   uint64_t first : 3,
|   |   int32_t second : 1919810
|   }
}
lambda2: "foobar", 114, 514
std::optional<int32_t> : {
|   int32_t value : 1919810
}
lambda2: "foobar", 114, 514
int32_t : 1919810
lambda2: "foobar", 114, 514
int32_t : 1919810
```

---

## Runtime‑polymorphic delegate (`delegate<void>`)

### brief

`delegate<void>` stores callable objects of **arbitrary** signatures.
When invoked with concrete arguments, it selects all stored callables whose parameter types are compatible with the supplied arguments.
Return types are ignored unless explicitly requested via the template parameter of `cast`, `pick`, etc.

### syntax

```c++
template<typename Policy = void, template<typename> typename Aspect = pack_t>
struct delegate<void, Policy, Aspect>;
```

### example

```c++
#include <print>
#include <iostream>
#include <string>
#include "delegate.h"
#include "type_format.h"

int main()
{
    using namespace fake::literals;
    
    auto result = [](auto&& call) {
        try {
            std::cout << fake::io::fancy<>(call()) << std::endl;
        } catch (const std::runtime_error& e) {
            std::println("error @ {}", e.what());
        }
    };
    
    fake::delegate<void> delegate;
    
    // Empty delegate
    result([&]{ return delegate(std::string{"foobar"}, 114); });          // false
    result([&]{ return delegate.seek<int>(std::string{"foobar"}, 114); }); // nullopt
    result([&]{ return delegate.mono<int>(std::string{"foobar"}, 114); }); // 0
    result([&]{ return delegate.rash<int>(std::string{"foobar"}, 114); }); // throws
    
    std::println("");
    
    // Assign a lambda that returns int
    std::size_t handle = delegate = [](std::string str, int i) {
        std::println("lambda A: \"{}\", {}", str, i);
        return 114514;
    };
    
    std::println("handle: {}, size: {}", handle, delegate.size());                // 2, 1
    std::println("amount<std::string,int>: {}", delegate.amount<std::string,int>()); // 1
    std::println("amount(...) with int: {}", delegate.amount(std::string{"foobar"}, 114)); // 1
    std::println("amount<std::string,float>: {}", delegate.amount<std::string,float>()); // 0
    std::println("amount(...) with float: {}", delegate.amount(std::string{"foobar"}, 514.0f)); // 0
    
    result([&]{ return delegate(std::string{"foobar"}, 114); });      // true, lambda A called
    result([&]{ return delegate(std::string{"foobar"}, 514.0f); });   // false (no match)
    result([&]{ return delegate.seek<int>(std::string{"foobar"}, 114); });   // optional{114514}
    result([&]{ return delegate.seek<int>(std::string{"foobar"}, 514.0f); }); // nullopt
    result([&]{ return delegate.mono<int>(std::string{"foobar"}, 114); });    // 114514
    result([&]{ return delegate.mono<int>(std::string{"foobar"}, 514.0f); }); // 0
    result([&]{ return delegate.rash<int>(std::string{"foobar"}, 114); });    // 114514
    result([&]{ return delegate.rash<int>(std::string{"foobar"}, 514.0f); }); // throws
    
    std::println("");
    
    // Assign another lambda, this time with const std::string& and float, returns int
    delegate = [](const std::string& str, float f) {
        std::println("lambda B: \"{}\", {}", str, f);
        return 1919;
    };
    
    result([&]{ return delegate(std::string{"foobar"}, 114); });      // true, A matches
    result([&]{ return delegate(std::string{"foobar"}, 514.0f); });   // true, B matches
    result([&]{ return delegate.pick<int>(std::string{"foobar"}, 114); });      // picks only A
    result([&]{ return delegate.pick<int>(std::string{"foobar"}, 514.0f); });   // picks only B
    result([&]{ return delegate.seek<int>(std::string{"foobar"}, 114); });      // optional{114514}
    result([&]{ return delegate.seek<int>(std::string{"foobar"}, 514.0f); });   // optional{1919}
    result([&]{ return delegate.mono<int>(std::string{"foobar"}, 114); });      // 114514
    result([&]{ return delegate.mono<int>(std::string{"foobar"}, 514.0f); });   // 1919
    result([&]{ return delegate.rash<int>(std::string{"foobar"}, 114); });      // 114514
    result([&]{ return delegate.rash<int>(std::string{"foobar"}, 514.0f); });   // 1919
    
    std::println("");
    
    // Assign a third lambda (std::string, float, returns double)
    delegate = [](std::string str, float f) {
        std::println("lambda C: \"{}\", {}", str, f);
        return 810.0;
    };
    
    result([&]{ return delegate(std::string{"foobar"}, 514.0f); });      // true, B and C called
    result([&]{ return delegate.cast<int>(std::string{"foobar"}, 514.0f); }); // vector of (id, optional<int>), B has value, C nullopt
    result([&]{ return delegate.pick<int>(std::string{"foobar"}, 514.0f); }); // vector of (id, int) only from B
    result([&]{ return delegate.seek<int>(std::string{"foobar"}, 514.0f); }); // optional{1919}
    result([&]{ return delegate.mono<int>(std::string{"foobar"}, 514.0f); }); // 1919
    result([&]{ return delegate.rash<int>(std::string{"foobar"}, 514.0f); }); // 1919
    
    std::println("");
    
    // Erase the first handle
    delegate.erase(handle);
    
    result([&]{ return delegate(std::string{"foobar"}, 114); });      // false (A removed, B doesn't match int)
    result([&]{ return delegate(std::string{"foobar"}, 514.0f); });   // true, B and C called
    result([&]{ return delegate.pick<int>(std::string{"foobar"}, 114); });      // empty
    result([&]{ return delegate.pick<int>(std::string{"foobar"}, 514.0f); });   // only B
    result([&]{ return delegate.mono<int>(std::string{"foobar"}, 114); });      // 0
    result([&]{ return delegate.mono<int>(std::string{"foobar"}, 514.0f); });   // 1919
    result([&]{ return delegate.rash<int>(std::string{"foobar"}, 114); });      // throws
    result([&]{ return delegate.rash<int>(std::string{"foobar"}, 514.0f); });   // 1919
    
    // Tag dispatch example
    struct tag_foo{}; struct tag_bar{};
    delegate = [](tag_foo, int e){ std::println("tag_foo: {}", e); };
    delegate = [](tag_bar, int e){ std::println("tag_bar: {}", e); };
    delegate(tag_foo{}, 114);
    delegate(tag_bar{}, 514);
}
```

**output (colours omitted):**

```plain
bool : 0
std::optional<int32_t> : {
|   
}
int32_t : 0
error @ fake::delegate::rash<int(const std::__cxx11::basic_string<char>&, const int&)>(): delegate does NOT exist.

handle: 2, size: 1
amount<std::string,int>: 1
amount(...) with int: 1
amount<std::string,float>: 0
amount(...) with float: 0
lambda A: "foobar", 114
bool : 1
bool : 0
lambda A: "foobar", 114
std::optional<int32_t> : {
|   int32_t value : 114514
}
std::optional<int32_t> : {
|
}
lambda A: "foobar", 114
int32_t : 114514
int32_t : 0
lambda A: "foobar", 114
int32_t : 114514
error @ fake::delegate::rash<int(const std::__cxx11::basic_string<char>&, const float&)>(): delegate does NOT exist.

lambda A: "foobar", 114
bool : 1
lambda B: "foobar", 514
bool : 1
lambda A: "foobar", 114
std::vector<std::pair<uint64_t, int32_t>> : {
|   std::pair<uint64_t, int32_t> : {
|   |   uint64_t first : 2,
|   |   int32_t second : 114514
|   }
}
lambda B: "foobar", 514
std::vector<std::pair<uint64_t, int32_t>> : {
|   std::pair<uint64_t, int32_t> : {
|   |   uint64_t first : 3,
|   |   int32_t second : 1919
|   }
}
lambda A: "foobar", 114
std::optional<int32_t> : {
|   int32_t value : 114514
}
lambda B: "foobar", 514
std::optional<int32_t> : {
|   int32_t value : 1919
}
lambda A: "foobar", 114
int32_t : 114514
lambda B: "foobar", 514
int32_t : 1919
lambda A: "foobar", 114
int32_t : 114514
lambda B: "foobar", 514
int32_t : 1919

lambda B: "foobar", 514
lambda C: "foobar", 514
bool : 1
lambda B: "foobar", 514
lambda C: "foobar", 514
std::vector<std::pair<uint64_t, std::optional<int32_t>>> : {
|   std::pair<uint64_t, std::optional<int32_t>> : {
|   |   uint64_t first : 3,
|   |   std::optional<int32_t> second : {
|   |   |   int32_t value : 1919
|   |   }
|   },
|   std::pair<uint64_t, std::optional<int32_t>> : {
|   |   uint64_t first : 4,
|   |   std::optional<int32_t> second : {
|   |   |
|   |   }
|   }
}
lambda B: "foobar", 514
std::vector<std::pair<uint64_t, int32_t>> : {
|   std::pair<uint64_t, int32_t> : {
|   |   uint64_t first : 3,
|   |   int32_t second : 1919
|   }
}
lambda B: "foobar", 514
std::optional<int32_t> : {
|   int32_t value : 1919
}
lambda B: "foobar", 514
int32_t : 1919
lambda B: "foobar", 514
int32_t : 1919

bool : 0
lambda B: "foobar", 514
lambda C: "foobar", 514
bool : 1
std::vector<std::pair<uint64_t, int32_t>> : {
|
}
lambda B: "foobar", 514
std::vector<std::pair<uint64_t, int32_t>> : {
|   std::pair<uint64_t, int32_t> : {
|   |   uint64_t first : 3,
|   |   int32_t second : 1919
|   }
}
int32_t : 0
lambda B: "foobar", 514
int32_t : 1919
error @ fake::delegate::rash<int(const std::__cxx11::basic_string<char>&, const int&)>(): delegate does NOT exist.
lambda B: "foobar", 514
int32_t : 1919
tag_foo: 114
tag_bar: 514
```

---

## Policy‑based delegate composition

### brief

By supplying a custom `Policy` type, you can control how a delegate stores child delegates (`parent` relationship) and whether it holds a reference to an “object” that will be passed as the first argument to certain callables.
The policy must define:

- `static constexpr auto policy = fake::view_v<"fake::delegate::policy">;` (a required tag)
- `using object_type = ...` (the type of the object that will be passed, if any)
- `using transform_type = ...` (one of the four tool classes: `shared_ptr`, `pointer`, `shared_ptrs`, `pointers`)

When `object_type` is provided, callables that accept a reference to that object (as first parameter) will be passed the stored reference.
When `transform_type` indicates a *multicast* container (`shared_ptrs` or `pointers`), the delegate can `attach` and `detach` other delegates.

### syntax

```c++
struct MyPolicy {
    static constexpr fake::view_c auto policy = fake::view_v<"fake::delegate::policy">;
    using object_type = MyObject;               // optional
    using transform_type = fake::tool::delegate::pointers;   // or shared_ptr, pointer, shared_ptrs
};

fake::delegate<Signature, MyPolicy, Aspect> d;
```

### example (pointers policy)

```c++
#include <print>
#include <iostream>
#include <vector>
#include "delegate.h"
#include "enum_info.h"
#include "type_format.h"

namespace tool {
    // An aspect that logs every delegate operation at compile time
    static constexpr fake::enum_info info = fake::enum_info_for_v<fake::delegate_e::enum_size>;
    static constexpr auto print = []<typename Message> {
        using delegate = fake::take_t<Message::delegate::value>;
        constexpr fake::view_c auto policy = fake::type_view(fake::pack_v<typename delegate::policy_type>);
        constexpr fake::view_c auto aspect = info[fake::mezz_v<Message::aspect::value>];
        constexpr fake::view_c auto type = fake::type_view(Message::value);
        constexpr fake::view_c auto dynamic = fake::value_view(fake::mezz_v<Message::dynamic::value>);
        constexpr fake::view_c auto invoke = fake::value_view(fake::mezz_v<Message::invoke::value>);
        std::println("policy: {}, aspect: {}, type: {}, dynamic: {}, invoke: {}",
                     policy.data(), aspect.data(), type.data(), dynamic.data(), invoke.data());
    };
    template<typename T> using aspect = fake::patterns::registry<T, print>;
}

int main()
{
    using namespace fake::literals;
    
    auto result = [](auto&& call) {
        try {
            std::cout << fake::io::fancy<>(call()) << std::endl;
        } catch (const std::runtime_error& e) {
            std::println("error @ {}", e.what());
        }
    };
    
    struct object; // forward
    
    // Policy: pointers (multicast) with object_type = object
    struct pointers {
        const fake::view_t<"fake::delegate::policy"> policy;
        using object_type = object;
        using transform_type = fake::tool::delegate::pointers;
    };
    
    using delegate_type = fake::delegate<void, pointers, tool::aspect>;
    
    static delegate_type abstracts[] = {std::nullopt, std::nullopt, std::nullopt};
    const auto& bases = std::as_const(abstracts);
    
    struct object {
        std::string info = "object content";
        delegate_type delegate{this, std::vector{&bases[0], &bases[1]}};
    };
    
    // Assign to the "base" delegates
    abstracts[0] = [](fake::view_t<"foo">, int e) {
        std::println("base0: {}", e);
    };
    abstracts[1] = [](object& self, fake::view_t<"bar">, int e) {
        std::println("base1: \"{}\" {}", self.info, e);
    };
    abstracts[2] = [](object& self, fake::view_t<"foo">, int e) {
        std::println("base2(foo): \"{}\" {}", self.info, e);
    };
    abstracts[2] = [](object& self, fake::view_t<"bar">, int e) {
        std::println("base2(bar): \"{}\" {}", self.info, e);
    };
    
    result([&]{ return bases[0]("foo"_v, 114); });   // works
    result([&]{ return bases[1]("bar"_v, 514); });   // throws because no object reference in this delegate
    
    object instance;
    instance.delegate = [](object& self, fake::view_t<"foo">, int e) {
        std::println("instance(foo): \"{}\" {}", self.info, e);
    };
    instance.delegate = [](object& self, fake::view_t<"bar">, int e) {
        std::println("instance(bar): \"{}\" {}", self.info, e);
    };
    instance.delegate = [](fake::view_t<"bar">, int e) {
        std::println("instance(bar) without self: {}", e);
    };
    
    result([&]{ return instance.delegate("foo"_v, 114); });   // calls instance(foo) and base0(foo)
    result([&]{ return instance.delegate("bar"_v, 514); });   // calls instance(bar) with and without self, and base1(bar)
    
    instance.delegate.attach(bases[2]);   // attach the third base
    
    result([&]{ return instance.delegate("foo"_v, 114); });   // now also base2(foo) called
    result([&]{ return instance.delegate("bar"_v, 514); });   // now also base2(bar) called
}
```

**output (colours omitted, compile‑time aspect logs first, then runtime output):**

```plain
policy: main()::pointers, aspect: fake::delegate_e::invoke, type: bool(const fake::view<'f', 'o', 'o'>&, const int&), dynamic: true, invoke: true
policy: main()::pointers, aspect: fake::delegate_e::invoke, type: bool(const fake::view<'b', 'a', 'r'>&, const int&), dynamic: true, invoke: true
policy: main()::pointers, aspect: fake::delegate_e::assign, type: main()::<lambda(fake::view_t<fake::detail::view::string<4>{"foo"}>, int)>, dynamic: true, invoke: false
policy: main()::pointers, aspect: fake::delegate_e::assign, type: main()::<lambda(main()::object&, fake::view_t<fake::detail::view::string<4>{"bar"}>, int)>, dynamic: true, invoke: false
policy: main()::pointers, aspect: fake::delegate_e::assign, type: main()::<lambda(main()::object&, fake::view_t<fake::detail::view::string<4>{"foo"}>, int)>, dynamic: true, invoke: false
policy: main()::pointers, aspect: fake::delegate_e::assign, type: main()::<lambda(main()::object&, fake::view_t<fake::detail::view::string<4>{"bar"}>, int)>, dynamic: true, invoke: false
policy: main()::pointers, aspect: fake::delegate_e::amount, type: long long unsigned int(const fake::view<'f', 'o', 'o'>&, const int&), dynamic: true, invoke: false
policy: main()::pointers, aspect: fake::delegate_e::amount, type: long long unsigned int(const fake::view<'b', 'a', 'r'>&, const int&), dynamic: true, invoke: false
policy: main()::pointers, aspect: fake::delegate_e::assign, type: main()::<lambda(main()::object&, fake::view_t<fake::detail::view::string<4>{"foo"}>, int)>, dynamic: true, invoke: false
policy: main()::pointers, aspect: fake::delegate_e::assign, type: main()::<lambda(main()::object&, fake::view_t<fake::detail::view::string<4>{"bar"}>, int)>, dynamic: true, invoke: false
policy: main()::pointers, aspect: fake::delegate_e::assign, type: main()::<lambda(fake::view_t<fake::detail::view::string<4>{"bar"}>, int)>, dynamic: true, invoke: false
base0: 114
bool : 1
error @ fake::delegate<void(const fake::view<'b', 'a', 'r'>&, const int&)>(): pure proto type.
instance(foo): "object content" 114
base0: 114
bool : 1
instance(bar): "object content" 514
instance(bar) without self: 514
base1: "object content" 514
bool : 1
instance(foo): "object content" 114
base0: 114
base2(foo): "object content" 114
bool : 1
instance(bar): "object content" 514
instance(bar) without self: 514
base1: "object content" 514
base2(bar): "object content" 514
bool : 1
```

### example (shared_ptrs policy)

Similar to the previous, but using `std::shared_ptr<delegate_type>` for storage.

```c++
#include <print>
#include <iostream>
#include <vector>
#include "delegate.h"
#include "enum_info.h"
#include "type_format.h"

namespace tool {
    // An aspect that logs every delegate operation at compile time
    static constexpr fake::enum_info info = fake::enum_info_for_v<fake::delegate_e::enum_size>;
    static constexpr auto print = []<typename Message> {
        using delegate = fake::take_t<Message::delegate::value>;
        constexpr fake::view_c auto policy = fake::type_view(fake::pack_v<typename delegate::policy_type>);
        constexpr fake::view_c auto aspect = info[fake::mezz_v<Message::aspect::value>];
        constexpr fake::view_c auto type = fake::type_view(Message::value);
        constexpr fake::view_c auto dynamic = fake::value_view(fake::mezz_v<Message::dynamic::value>);
        constexpr fake::view_c auto invoke = fake::value_view(fake::mezz_v<Message::invoke::value>);
        std::println("policy: {}, aspect: {}, type: {}, dynamic: {}, invoke: {}",
                     policy.data(), aspect.data(), type.data(), dynamic.data(), invoke.data());
    };
    template<typename T> using aspect = fake::patterns::registry<T, print>;
}

int main()
{
    using namespace fake::literals;
    
    auto result = [](auto&& call) {
        try {
            std::cout << fake::io::fancy<>(call()) << std::endl;
        } catch (const std::runtime_error& e) {
            std::println("error @ {}", e.what());
        }
    };
    
    struct object;
    
    struct shared_ptrs {
        const fake::view_t<"fake::delegate::policy"> policy;
        using object_type = object;
        using transform_type = fake::tool::delegate::shared_ptrs;
    };
    
    using delegate_type = fake::delegate<void, shared_ptrs, tool::aspect>;
    
    static std::shared_ptr<delegate_type> abstracts[] = {
        std::make_shared<delegate_type>(std::nullopt),
        std::make_shared<delegate_type>(std::nullopt),
        std::make_shared<delegate_type>(std::nullopt)
    };
    static std::shared_ptr<const delegate_type> bases[] = {abstracts[0], abstracts[1], abstracts[2]};
    
    struct object {
        std::string info = "object content";
        delegate_type delegate{this, std::vector{bases[0], bases[1]}};
    };
    
    *abstracts[0] = [](fake::view_t<"foo">, int e) {
        std::println("base0: {}", e);
    };
    *abstracts[1] = [](object& self, fake::view_t<"bar">, int e) {
        std::println("base1: \"{}\" {}", self.info, e);
    };
    *abstracts[2] = [](object& self, fake::view_t<"foo">, int e) {
        std::println("base2(foo): \"{}\" {}", self.info, e);
    };
    *abstracts[2] = [](object& self, fake::view_t<"bar">, int e) {
        std::println("base2(bar): \"{}\" {}", self.info, e);
    };
    
    result([&]{ return (*bases[0])("foo"_v, 114); });   // works
    result([&]{ return (*bases[1])("bar"_v, 514); });   // throws because no object reference in this delegate
    
    object instance;
    instance.delegate = [](object& self, fake::view_t<"foo">, int e) {
        std::println("instance(foo): \"{}\" {}", self.info, e);
    };
    instance.delegate = [](object& self, fake::view_t<"bar">, int e) {
        std::println("instance(bar): \"{}\" {}", self.info, e);
    };
    instance.delegate = [](fake::view_t<"bar">, int e) {
        std::println("instance(bar) without self: {}", e);
    };
    
    result([&]{ return instance.delegate("foo"_v, 114); });   // calls instance(foo) and base0(foo)
    result([&]{ return instance.delegate("bar"_v, 514); });   // calls instance(bar) with and without self, and base1(bar)
    
    instance.delegate.attach(bases[2]);   // attach the third base
    
    result([&]{ return instance.delegate("foo"_v, 114); });   // now also base2(foo) called
    result([&]{ return instance.delegate("bar"_v, 514); });   // now also base2(bar) called
}
```

**output (colours omitted, compile‑time aspect logs first, then runtime output):**

```plain
policy: main()::shared_ptrs, aspect: fake::delegate_e::invoke, type: bool(const fake::view<'f', 'o', 'o'>&, const int&), dynamic: true, invoke: true
policy: main()::shared_ptrs, aspect: fake::delegate_e::invoke, type: bool(const fake::view<'b', 'a', 'r'>&, const int&), dynamic: true, invoke: true
policy: main()::shared_ptrs, aspect: fake::delegate_e::assign, type: main()::<lambda(fake::view_t<fake::detail::view::string<4>{"foo"}>, int)>, dynamic: true, invoke: false
policy: main()::shared_ptrs, aspect: fake::delegate_e::assign, type: main()::<lambda(main()::object&, fake::view_t<fake::detail::view::string<4>{"bar"}>, int)>, dynamic: true, invoke: false
policy: main()::shared_ptrs, aspect: fake::delegate_e::assign, type: main()::<lambda(main()::object&, fake::view_t<fake::detail::view::string<4>{"foo"}>, int)>, dynamic: true, invoke: false
policy: main()::shared_ptrs, aspect: fake::delegate_e::assign, type: main()::<lambda(main()::object&, fake::view_t<fake::detail::view::string<4>{"bar"}>, int)>, dynamic: true, invoke: false
policy: main()::shared_ptrs, aspect: fake::delegate_e::amount, type: long long unsigned int(const fake::view<'f', 'o', 'o'>&, const int&), dynamic: true, invoke: false
policy: main()::shared_ptrs, aspect: fake::delegate_e::amount, type: long long unsigned int(const fake::view<'b', 'a', 'r'>&, const int&), dynamic: true, invoke: false
policy: main()::shared_ptrs, aspect: fake::delegate_e::assign, type: main()::<lambda(main()::object&, fake::view_t<fake::detail::view::string<4>{"foo"}>, int)>, dynamic: true, invoke: false
policy: main()::shared_ptrs, aspect: fake::delegate_e::assign, type: main()::<lambda(main()::object&, fake::view_t<fake::detail::view::string<4>{"bar"}>, int)>, dynamic: true, invoke: false
policy: main()::shared_ptrs, aspect: fake::delegate_e::assign, type: main()::<lambda(fake::view_t<fake::detail::view::string<4>{"bar"}>, int)>, dynamic: true, invoke: false
base0: 114
bool : 1
error @ fake::delegate<void(const fake::view<'b', 'a', 'r'>&, const int&)>(): pure proto type.
instance(foo): "object content" 114
base0: 114
bool : 1
instance(bar): "object content" 514
instance(bar) without self: 514
base1: "object content" 514
bool : 1
instance(foo): "object content" 114
base0: 114
base2(foo): "object content" 114
bool : 1
instance(bar): "object content" 514
instance(bar) without self: 514
base1: "object content" 514
base2(bar): "object content" 514
bool : 1
```

---

## Member functions reference

### `operator=`

Assigns a callable to the delegate, returning a handle (ID) that can be used to erase it later.

**Syntax (static signature):**

```c++
template<std::convertible_to<function_t> Functor>
std::size_t operator=(Functor&& functor);

template<std::convertible_to<function_ref_t> Functor>  // if object_type present
std::size_t operator=(Functor&& functor);

template<std::convertible_to<function_cref_t> Functor> // if object_type present
std::size_t operator=(Functor&& functor);
```

**Syntax (runtime‑polymorphic):**

```c++
template<fake::functor_c Functor>
std::size_t operator=(Functor&& functor);
```

### `size()`

Returns the total number of callables stored in this delegate and all its children.

**Syntax:**

```c++
[[nodiscard]] std::size_t size() const;
```

### `amount()`

For static‑signature delegates: returns the number of callables that can be invoked with the given argument types (compile‑time or runtime check).
For runtime‑polymorphic delegates: returns the number of callables whose signature matches the supplied argument types.

**Syntax:**

```c++
template<typename... Params>
[[nodiscard]] std::size_t amount() const;

[[nodiscard]] std::size_t amount(const auto&... args) const;
```

### `contains()`, `attach()`, `detach()` (only when `parent` is true)

These manage child delegates.

**Syntax:**

```c++
[[nodiscard]] bool contains(fake::delegate_handle id) const;

bool attach(fake::detail::delegate::emplace_c<delegate> auto&& child);
bool detach(fake::delegate_handle id);
```

- `contains` obtains if a child is in the parent’s storage (recursively) by its handle `delegate::id`.
- `attach` adds a child (pointer or shared_ptr) to the parent’s storage.
- `detach` removes a child by its handle `delegate::id`.

### `cast()`

Invokes all matching callables and returns a vector of `(id, std::optional<result>)`. For `void` results, returns a vector of `(id, bool)` where `bool` indicates whether the callable returned void (always true for `void`‑returning callables).

**Syntax (static signature):**

```c++
template<typename Retn>
std::vector<std::pair<std::size_t, std::optional<Retn>>> cast(auto&&... args) const;

// for void
template<typename Retn>
void cast(auto&&... args) const;
```

**Syntax (runtime‑polymorphic):**

```c++
template<typename Retn>
std::vector<std::pair<std::size_t, std::optional<Retn>>> cast(auto&&... args) const;

// for void
template<typename Retn>
std::vector<std::pair<std::size_t, bool>> cast(auto&&... args) const;
```

### `pick()`

Invokes all matching callables **that return the requested type** and returns a vector of `(id, result)`. For `void`, returns a vector of IDs of callables that return `void`.

**Syntax (static signature):**

```c++
template<typename Retn>
std::vector<std::pair<std::size_t, Retn>> pick(auto&&... args) const;

// for void
template<typename Retn>
void pick(auto&&... args) const;
```

**Syntax (runtime‑polymorphic):**

```c++
template<typename Retn>
std::vector<std::pair<std::size_t, Retn>> pick(auto&&... args) const;

// for void
template<typename Retn>
std::vector<std::size_t> pick(auto&&... args) const;
```

### `seek()`

If exactly one callable matches the arguments, invokes it and returns an `optional<result>{value}` (or `bool true` for `void`); if none match, returns a `optional<result>{nullopt}` (or `bool false` for `void`). Throws if multiple match.

**Syntax:**

```c++
template<typename Retn>
std::optional<Retn> seek(auto&&... args) const;
```

### `mono()`

If exactly one callable matches, returns its result; if none match, returns a default‑constructed result (or nothing for `void`). Throws if multiple match.

**Syntax:**

```c++
template<typename Retn>
Retn mono(auto&&... args) const;
```

### `rash()`

Requires exactly one matching callable; throws if none or multiple match.

**Syntax:**

```c++
template<typename Retn>
Retn rash(auto&&... args) const;
```

### `operator()`

Invokes all matching callables and returns `true` if at least one was invoked.

**Syntax:**

```c++
bool operator()(auto&&... args) const;
```

### `erase()`

Removes a callable by its handle. Returns the number of erased entries (0 or 1).

**Syntax:**

```c++
std::size_t erase(std::size_t id);
```

### Conversion to `bool`

Returns `true` if the delegate holds at least one callable.

**Syntax:**

```c++
explicit operator bool() const;
```

---

## Advanced: Compile‑time aspect injection

The third template parameter of `delegate`, `Aspect`, is a template that wraps every operation (assign, amount, cast, etc.) into a “message” type.
This message type is instantiated when the corresponding member function is instantiated.
By providing a custom `Aspect` (like `fake::patterns::registry<T, printer>`), you can execute arbitrary compile‑time code for each such instantiation.

In the examples above, `tool::aspect` logs every delegate operation at compile time, printing the policy, aspect (operation type), the signature of the involved callable, and whether it is dynamic or an invocation.
This is extremely useful for meta‑programming and debugging.

---

## Additional utilities: `fake::storage`

The header also provides a simple key‑value store `fake::storage` that maps compile‑time string views to `std::any` values.
It is not directly used in the examples but can be handy for storing configuration or state associated with a delegate.

### example

```c++
fake::storage store;

store.emplace<int>("key"_v, 114514);

int &ref = store.ref<int>("key"_v);
const int &cref = std::as_const(store).ref<int>("key"_v);

store.erase("key"_v);
```

---

## Important notes

- The `Aspect` mechanism works by injecting a `struct message` that carries type information.
  The `Aspect` template is instantiated for each distinct operation and signature.
  It can be used to accumulate compile‑time data or generate static assertions.

- The `std::size_t` returned by `operator=` and `delegate_handle delegate::id` are sequential IDs; it is not guaranteed to be unique across binary boundaries.

- For runtime‑polymorphic delegates, the parameter types are compared using `std::remove_cvref_t`.
  Reference and cv‑qualifiers are stripped; therefore `std::string` and `const std::string&` are considered the same for matching.

- When a policy defines `object_type`, callables that take a reference to that object as their **first** parameter will receive the stored reference automatically.
  If no object is stored (e.g., in a base delegate without a host), invoking such a callable throws `std::runtime_error` with a “pure proto type” message.
