# `functor_info.h`

- 为 C++ 中的各种可调用类型（函子）提供静态反射。
- 提取诸如返回类型、参数类型、函数类型、参数个数以及额外的限定符（对于成员函数）或占位符映射（对于 `std::bind` 表达式）等信息。
- 同时提供了一个概念 `fake::functor_c`，用于检查某个类型是否被 `fake::functor_info` 所支持。

## `fake::functor_c`

### 简介

- 检查一个类型是否被 `fake::functor_info` 识别为可调用对象的概念。

### 语法

```c++
template<typename _Type>
concept functor_c = is_functor_v<_Type>;
```

### 示例

```c++
#include "functor_info.h"

int main()
{
    auto lambda = [](int){ return 0; };
    static_assert(fake::functor_c<decltype(lambda)>);
    static_assert(fake::functor_c<void(*)()>);
    static_assert(fake::functor_c<int(&)(double)>);
    struct S { void f(); };
    static_assert(fake::functor_c<decltype(&S::f)>);
}
```

## `fake::functor_info`

### 简介

- 从可调用类型中提取信息的主模板。特化版本支持 lambda、`std::bind` 表达式、成员函数指针、自由函数、函数指针和函数引用。

### 公共成员（所有特化版本均可用）

| 成员              | 描述                                          |
|-------------------|------------------------------------------------------|
| `name`            | 描述函子种类的字符串字面量。     |
| `size`            | **有效调用**的参数个数。      |
| `func`            | **有效调用**的函数类型。             |
| `retn`            | **有效调用**的返回类型。               |
| `tuple`           | **有效调用**的参数类型的 `std::tuple`。   |
| `args<Idx>`       | 第 `Idx` 个**有效调用**参数的类型。        |

### 成员函数指针的成员

- `object`          – 成员函数所属的类类型。
- `constant`        – 如果成员函数带有 `const` 限定，则为 `true`。
- `volatility`      – 如果成员函数带有 `volatile` 限定，则为 `true`。
- `exception`       – 如果成员函数**不**带有 `noexcept`，则为 `true`。
- `lvalue_reference`– 如果成员函数带有 `&` 引用限定符，则为 `true`。
- `rvalue_reference`– 如果成员函数带有 `&&` 引用限定符，则为 `true`。

### `std::bind` 表达式的成员

- `rec_*`（例如 `rec_size`、`rec_func`、`rec_tuple`、`rec_args<Idx>`） – 关于被绑定的**原始**可调用对象的信息。
- `bind_*`（例如 `bind_size`、`bind_func`、`bind_tuple`、`bind_args<Idx>`） – 关于**绑定参数**（包括占位符）的信息。
- **有效调用**（`size`、`func`、`tuple`、`args<Idx>`）是在占位符替换后得到的可调用类型（即调用 bind 对象时实际调用的内容）。

## 示例：Lambda 对象（functor_object）

**简介**：演示从 lambda 中提取信息。

**示例**：

```c++
#include <string_view>
#include <tuple>
#include "functor_info.h"

int main()
{
    auto lambda = [](double&, char, float) { return 114514; };
    
    static_assert(fake::functor_c<decltype(lambda)>);
    
    using info = fake::functor_info<decltype(lambda)>;
    
    static_assert(std::string_view{info::name} == "functor_object");
    static_assert(info::size == 3);
    static_assert(std::same_as<info::func, int(double&, char, float)>);
    static_assert(std::same_as<info::retn, int>);
    static_assert(std::same_as<info::tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<info::args<1>, char>);
}
```

## 示例：`std::bind` 表达式

**简介**：展示 `functor_info` 如何从 `std::bind` 对象中提取信息，包括原始函子、绑定的参数以及占位符映射后的有效调用签名。

**示例**：

```c++
#include <string_view>
#include <tuple>
#include <functional>
#include "functor_info.h"

int main()
{
    auto lambda = [](double&, char, float) { return 114514; };
    auto bind = std::bind(lambda, std::placeholders::_2, std::placeholders::_1, 114514.0f);
    
    static_assert(fake::functor_c<decltype(bind)>);
    using info = fake::functor_info<decltype(bind)>;
    
    static_assert(std::string_view{info::name} == "bind_expression");
    
    // Original functor info (the lambda)
    static_assert(info::rec_size == 3);
    static_assert(std::same_as<info::rec_func, int(double&, char, float)>);
    static_assert(std::same_as<info::rec_retn, int>);
    static_assert(std::same_as<info::rec_tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<info::rec_args<1>, char>);
    
    using placeholders_1 = std::remove_cvref_t<decltype(std::placeholders::_1)>;
    using placeholders_2 = std::remove_cvref_t<decltype(std::placeholders::_2)>;
    
    // Binding arguments info (as passed to std::bind)
    static_assert(info::bind_size == 3);
    static_assert(std::same_as<info::bind_func, int(placeholders_2, placeholders_1, float)>);
    static_assert(std::same_as<info::bind_retn, int>);
    static_assert(std::same_as<info::bind_tuple, std::tuple<placeholders_2, placeholders_1, float>>);
    static_assert(std::same_as<info::bind_args<2>, float>);
    
    double d = 1919.0;
    bind('@', d);   // after placeholder substitution, effective call takes (char, double&)
    
    // Effective call info (after placeholder mapping)
    static_assert(info::size == 2);
    static_assert(std::same_as<info::func, int(char, double&)>);
    static_assert(std::same_as<info::retn, int>);
    static_assert(std::same_as<info::tuple, std::tuple<char, double&>>);
    static_assert(std::same_as<info::args<1>, double&>);
}
```

## 示例：成员函数指针（各种限定符）

**简介**：从成员函数指针中提取信息，包括 const、volatile、引用限定符和 noexcept。

**示例**：

```c++
#include <string_view>
#include <tuple>
#include "functor_info.h"

int main()
{
    struct object {
        int foo(double&, char, float) noexcept { return 114514; }
        int bar(double&, char, float) const volatile && { return 114514; }
    };
    
    static_assert(fake::functor_c<decltype(&object::foo)>);
    static_assert(fake::functor_c<decltype(&object::bar)>);
    
    using foo = fake::functor_info<decltype(&object::foo)>;
    using bar = fake::functor_info<decltype(&object::bar)>;
    
    // foo: noexcept member function
    static_assert(foo::size == 3);
    static_assert(std::same_as<foo::object, object>);
    static_assert(std::same_as<foo::func, int(double&, char, float)>);
    static_assert(std::same_as<foo::retn, int>);
    static_assert(std::same_as<foo::tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<foo::args<1>, char>);
    
    static_assert(std::string_view{foo::name} == "member_function_pointer_noexcept");
    static_assert(foo::constant == false);
    static_assert(foo::volatility == false);
    static_assert(foo::exception == false);          // noexcept => exception == false
    static_assert(foo::lvalue_reference == false);
    static_assert(foo::rvalue_reference == false);
    
    // bar: const volatile &&, noexcept not specified (so exception == true)
    static_assert(std::string_view{bar::name} == "member_function_pointer_const_volatile_rvalue");
    static_assert(bar::constant == true);
    static_assert(bar::volatility == true);
    static_assert(bar::exception == true);           // not noexcept
    static_assert(bar::lvalue_reference == false);
    static_assert(bar::rvalue_reference == true);
}
```

## 示例：自由函数类型

**简介**：关于普通函数类型（非指针或引用）的信息。这里通过将 lambda 转换为函数指针然后移除指针来获得函数类型。

**示例**：

```c++
#include <string_view>
#include <tuple>
#include "functor_info.h"

int main()
{
    auto lambda = [](double&, char, float) { return 114514; };
    
    using func_t = std::remove_pointer_t<decltype(+lambda)>; // int(double&, char, float)
    static_assert(fake::functor_c<func_t>);
    
    using info = fake::functor_info<func_t>;
    
    static_assert(std::string_view{info::name} == "global_function");
    static_assert(info::size == 3);
    static_assert(std::same_as<info::func, int(double&, char, float)>);
    static_assert(std::same_as<info::retn, int>);
    static_assert(std::same_as<info::tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<info::args<1>, char>);
}
```

## 示例：函数指针

**简介**：关于函数指针的信息。

**示例**：

```c++
#include <string_view>
#include <tuple>
#include "functor_info.h"

int main()
{
    auto lambda = [](double&, char, float) { return 114514; };
    auto fptr = +lambda;   // int(*)(double&, char, float)
    
    static_assert(fake::functor_c<decltype(fptr)>);
    
    using info = fake::functor_info<decltype(fptr)>;
    
    static_assert(std::string_view{info::name} == "global_function_pointer");
    static_assert(info::size == 3);
    static_assert(std::same_as<info::func, int(double&, char, float)>);
    static_assert(std::same_as<info::retn, int>);
    static_assert(std::same_as<info::tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<info::args<1>, char>);
}
```

## 示例：函数引用

**简介**：关于函数引用（通过对函数指针解引用获得）的信息。

**示例**：

```c++
#include <string_view>
#include <tuple>
#include "functor_info.h"

int main()
{
    auto lambda = [](double&, char, float) { return 114514; };
    auto& fref = *+lambda;   // int(&)(double&, char, float)
    
    static_assert(fake::functor_c<decltype(fref)>);
    
    using info = fake::functor_info<decltype(fref)>;
    
    static_assert(std::string_view{info::name} == "global_function_reference");
    static_assert(info::size == 3);
    static_assert(std::same_as<info::func, int(double&, char, float)>);
    static_assert(std::same_as<info::retn, int>);
    static_assert(std::same_as<info::tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<info::args<1>, char>);
}
```
