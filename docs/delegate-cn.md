# `delegate.h`

- 一个适用于 C++23 的、可在编译期配置的多功能委托（多播/单播）库。
- 支持静态签名委托（`delegate<Ret(Args...)>`）与运行时多态委托（`delegate<void>`）。
- 提供四种存储策略（`shared_ptr`、`pointer`、`shared_ptrs`、`pointers`）以及父/子组合机制。
- 提供丰富的调用方法：`cast`、`pick`、`seek`、`mono`、`rash` 以及简单的调用运算符。
- 可注入编译期“切面” lambda，这些 lambda 会在每个委托操作（赋值、调用、数量查询等）被实例化时被实例化，从而实现元编程内省。

---

## 概述

`delegate.h` 头文件定义了两个主要的类模板：

```c++
namespace fake {
    template<typename Signature, typename Policy = void, template<typename> typename Aspect = pack_t>
    struct delegate;
    
    template<typename Policy, template<typename> typename Aspect>
    struct delegate<void, Policy, Aspect>;   // runtime‑polymorphic version
}
```

- `Signature` 可以是函数类型（例如 `int(const std::string&, int, float)`）用于静态签名委托，或者是 `void` 用于运行时多态版本。
- `Policy` 定制存储方式和父子关系。预定义的策略辅助类位于 `fake::tool::delegate` 中。
- `Aspect` 是一个模板，它将每个操作包装成一个“消息”类型；可用于注入编译期行为（例如记录被实例化的签名）。

---

## 预定义存储策略

这些策略定义在 `fake::tool::delegate` 中，并在自定义策略中用作 `transform_type`。

| 策略类                      | 存储表示形式                                                |
|-----------------------------|-------------------------------------------------------------|
| `shared_ptr`                | `std::shared_ptr<const Delegate>`                           |
| `pointer`                   | `const Delegate*`                                           |
| `shared_ptrs`               | `std::vector<std::shared_ptr<const Delegate>>`（多播）      |
| `pointers`                  | `std::vector<const Delegate*>`（多播）                      |

当你定义自己的策略（见下文）时，必须将其中之一指定为 `transform_type`。

---

## 基本静态签名委托

### 简介

`delegate<Ret(Args...)>` 持有一组可调用对象，这些对象都接受可转换为 `Args...` 的参数，并返回可转换为 `Ret` 的类型。
它可以作为单播或多播委托使用，具体取决于是否存储多个可调用对象。
可以通过多种成员函数进行调用，这些函数以不同的方式处理多重性。

### 语法

```c++
template<typename Ret, typename... Args, typename Policy = void, template<typename> typename Aspect = pack_t>
struct delegate<Ret(Args...), Policy, Aspect>;
```

### 示例

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

**输出（省略颜色）：**

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

## 运行时多态委托（`delegate<void>`）

### 简介

`delegate<void>` 存储**任意**签名的可调用对象。
当使用具体参数调用时，它会选择所有参数类型与所提供参数兼容的已存储可调用对象。
返回类型会被忽略，除非通过 `cast`、`pick` 等函数的模板参数显式请求。

### 语法

```c++
template<typename Policy = void, template<typename> typename Aspect = pack_t>
struct delegate<void, Policy, Aspect>;
```

### 示例

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

**输出（省略颜色）：**

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

## 基于策略的委托组合

### 简介

通过提供一个自定义的 `Policy` 类型，你可以控制委托如何存储子委托（`parent` 关系），以及它是否持有一个对“对象”的引用，该对象将作为第一个参数传递给某些可调用对象。
策略必须定义：

- `static constexpr auto policy = fake::view_v<"fake::delegate::policy">;` （必需标签）
- `using object_type = ...` （将要传递的对象类型，如果有的话）
- `using transform_type = ...` （四个工具类之一：`shared_ptr`、`pointer`、`shared_ptrs`、`pointers`）

当提供了 `object_type` 时，那些接受对该对象引用（作为第一个参数）的可调用对象将获得存储的引用。
当 `transform_type` 指示一个*多播*容器（`shared_ptrs` 或 `pointers`）时，该委托可以 `attach` 和 `detach` 其他委托。

### 语法

```c++
struct MyPolicy {
    static constexpr fake::view_c auto policy = fake::view_v<"fake::delegate::policy">;
    using object_type = MyObject;               // optional
    using transform_type = fake::tool::delegate::pointers;   // or shared_ptr, pointer, shared_ptrs
};

fake::delegate<Signature, MyPolicy, Aspect> d;
```

### 示例（`pointers` 策略）

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

**输出（省略颜色，首先是编译期切面日志，然后是运行时输出）：**

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

### 示例（`shared_ptrs` 策略）

与上一个类似，但使用 `std::shared_ptr<delegate_type>` 进行存储。

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

**输出（省略颜色，首先是编译期切面日志，然后是运行时输出）：**

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

## 成员函数参考

### `operator=`

将可调用对象赋给委托，返回一个句柄（ID），以后可用于删除它。

**语法（静态签名）：**

```c++
template<std::convertible_to<function_t> Functor>
std::size_t operator=(Functor&& functor);

template<std::convertible_to<function_ref_t> Functor>  // if object_type present
std::size_t operator=(Functor&& functor);

template<std::convertible_to<function_cref_t> Functor> // if object_type present
std::size_t operator=(Functor&& functor);
```

**语法（运行时多态）：**

```c++
template<fake::functor_c Functor>
std::size_t operator=(Functor&& functor);
```

### `size()`

返回存储在此委托及其所有子委托中的可调用对象总数。

**语法：**

```c++
[[nodiscard]] std::size_t size() const;
```

### `amount()`

对于静态签名委托：返回可以使用给定参数类型（编译时或运行时检查）调用的可调用对象数量。
对于运行时多态委托：返回签名与提供的参数类型匹配的可调用对象数量。

**语法：**

```c++
template<typename... Params>
[[nodiscard]] std::size_t amount() const;

[[nodiscard]] std::size_t amount(const auto&... args) const;
```

### `contains()`、`attach()`、`detach()`（仅在 `parent` 为 true 时可用）

这些函数管理子委托。

**语法：**

```c++
[[nodiscard]] bool contains(fake::delegate_handle id) const;

bool attach(fake::detail::delegate::emplace_c<delegate> auto&& child);
bool detach(fake::delegate_handle id);
```

- `contains` 通过子委托的 `delegate::id` 句柄递归检查该子委托是否存在于父存储中。
- `attach` 将一个子委托（指针或 `shared_ptr`）添加到父存储中。
- `detach` 通过子委托的 `delegate::id` 句柄将其移除。

### `cast()`

调用所有匹配的可调用对象，并返回一个 `(id, std::optional<result>)` 向量。对于 `void` 结果，返回一个 `(id, bool)` 向量，其中 `bool` 指示可调用对象是否返回 `void`（对于返回 `void` 的可调用对象始终为 true）。

**语法（静态签名）：**

```c++
template<typename Retn>
std::vector<std::pair<std::size_t, std::optional<Retn>>> cast(auto&&... args) const;

// for void
template<typename Retn>
void cast(auto&&... args) const;
```

**语法（运行时多态）：**

```c++
template<typename Retn>
std::vector<std::pair<std::size_t, std::optional<Retn>>> cast(auto&&... args) const;

// for void
template<typename Retn>
std::vector<std::pair<std::size_t, bool>> cast(auto&&... args) const;
```

### `pick()`

调用所有**返回所请求类型**的匹配可调用对象，并返回一个 `(id, result)` 向量。对于 `void`，返回返回 `void` 的可调用对象的 ID 向量。

**语法（静态签名）：**

```c++
template<typename Retn>
std::vector<std::pair<std::size_t, Retn>> pick(auto&&... args) const;

// for void
template<typename Retn>
void pick(auto&&... args) const;
```

**语法（运行时多态）：**

```c++
template<typename Retn>
std::vector<std::pair<std::size_t, Retn>> pick(auto&&... args) const;

// for void
template<typename Retn>
std::vector<std::size_t> pick(auto&&... args) const;
```

### `seek()`

如果恰好有一个可调用对象匹配参数，则调用它并返回 `optional<result>{value}`（对于 `void` 返回 `bool true`）；如果没有匹配，则返回 `optional<result>{nullopt}`（对于 `void` 返回 `bool false`）。如果多个匹配则抛出异常。

**语法：**

```c++
template<typename Retn>
std::optional<Retn> seek(auto&&... args) const;
```

### `mono()`

如果恰好有一个可调用对象匹配，则返回其结果；如果没有匹配，则返回默认构造的结果（对于 `void` 不返回任何内容）。如果多个匹配则抛出异常。

**语法：**

```c++
template<typename Retn>
Retn mono(auto&&... args) const;
```

### `rash()`

要求恰好有一个匹配的可调用对象；如果没有或多个匹配则抛出异常。

**语法：**

```c++
template<typename Retn>
Retn rash(auto&&... args) const;
```

### `operator()`

调用所有匹配的可调用对象，如果至少有一个被调用则返回 `true`。

**语法：**

```c++
bool operator()(auto&&... args) const;
```

### `erase()`

通过句柄移除一个可调用对象。返回被移除的条目数（0 或 1）。

**语法：**

```c++
std::size_t erase(std::size_t id);
```

### 转换为 `bool`

如果委托至少持有一个可调用对象，则返回 `true`。

**语法：**

```c++
explicit operator bool() const;
```

---

## 高级：编译期切面注入

`delegate` 的第三个模板参数 `Aspect` 是一个模板，它将每个操作（赋值、数量查询、调用等）包装成一个“消息”类型。
当相应的成员函数被实例化时，此消息类型也会被实例化。
通过提供自定义的 `Aspect`（例如 `fake::patterns::registry<T, printer>`），你可以为每次这样的实例化执行任意的编译期代码。

在上面的示例中，`tool::aspect` 在编译期记录每个委托操作，打印策略、切面（操作类型）、涉及的可调用对象的签名，以及它是动态的还是调用。这对于元编程和调试非常有用。

---

## 附加工具：`fake::storage`

该头文件还提供了一个简单的键值存储 `fake::storage`，它将编译期字符串视图映射到 `std::any` 值。
示例中未直接使用，但可用于存储与委托关联的配置或状态。

### 示例

```c++
fake::storage store;

store.emplace<int>("key"_v, 114514);

int &ref = store.ref<int>("key"_v);
const int &cref = std::as_const(store).ref<int>("key"_v);

store.erase("key"_v);
```

---

## 重要说明

- `Aspect` 机制通过注入一个携带类型信息的 `struct message` 来工作。`Aspect` 模板会为每个不同的操作和签名被实例化。它可以用于积累编译期数据或生成静态断言。
- `operator=` 返回的 `std::size_t` 和 `delegate_handle delegate::id` 是顺序 ID；它们不能保证跨二进制边界唯一性。
- 对于运行时多态委托，参数类型使用 `std::remove_cvref_t` 进行比较。引用和 cv 限定符会被剥离；因此 `std::string` 和 `const std::string&` 在匹配时被视为相同。
- 当策略定义了 `object_type` 时，那些将该对象的引用作为其**第一个**参数的可调用对象将自动获得存储的引用。如果没有存储对象（例如在没有宿主的基类委托中），调用此类可调用对象将抛出 `std::runtime_error`，并携带“pure proto type”消息。
