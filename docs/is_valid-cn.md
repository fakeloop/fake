# `is_valid.h`

- 元编程基础工具

## `fake::null_t`

### 简介

- `null_t`是一个标签类型，通常用于表示无效结果。

### 语法

```c++
struct null_t;
```

### 示例

```c++
#include "tuple.h"

int main()
{
    using t = std::tuple<int*, float[], char*, short*>;
    
    using i = fake::tuple::match_index_t<t, fake::detect::convert_from<char[]>>;
    static_assert(i::value == 2);
    
    using m = fake::tuple::match_index_t<t, fake::detect::same<double>>;
    static_assert(std::same_as<m, fake::null_t>);
    
    using n = fake::tuple::match_index_t<t, !fake::detect::convert_from<std::nullptr_t>>;
    static_assert(n::value == 1);
    
    using p = fake::tuple::match_index_t<t, fake::detect::convert_to<int[1]>>;
    static_assert(std::same_as<p, fake::null_t>);
}
```

## `fake::null_c`

### 简介

- 用于检查某个实体是否为`fake::null_t`的概念。

### 语法

```c++
template<auto _value>
concept null_c;
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    static_assert(fake::null_c<fake::null_t{}>);
}
```

## `fake::trait`

### 简介

- 用于生成带有 `类型模板参数` 的模板的类型特性的模板结构体。

### 语法

```c++
template<template<typename...> typename, typename>
struct trait;
```

### 注意

该特性仅能检查参数为类型的模板类型。
不支持 `模板模板参数` 和 `非类型模板参数`。

#### 辅助模板

```c++
template<template<typename...> typename _Tamplate, typename _Type>
constexpr bool trait_v;
```

### 示例

```c++
#include <tuple>

#include "is_valid.h"

int main()
{
    using p = std::pair<float, short>;
    using t = std::tuple<int, char, double>;
    
    static_assert(fake::trait_v<std::pair, p>);
    static_assert(!fake::trait_v<std::tuple, p>);
    static_assert(fake::trait_v<std::tuple, t>);
    static_assert(!fake::trait_v<std::pair, t>);
}
```

## `fake::trait_c`

### 简介

- 用于检查某个类型是否为指定模板实例的概念。

### 语法

```c++
template<typename _Type, template<typename...> typename _Tamplate>
concept trait_c;
```

### 示例

```c++
#include <iostream>
#include <tuple>

#include "is_valid.h"

void foo(const fake::trait_c<std::pair> auto &_e){
    std::cout << "pair version" << std::endl;
}

void foo(const fake::trait_c<std::tuple> auto &_e){
    std::cout << "tuple version" << std::endl;
}

int main()
{
    foo(std::pair<float, short>{});
    foo(std::tuple<int, char, double>{});
}
```

**输出:**

```plain
pair version
tuple version
```

## `fake::trait_auto`

### 简介

- 用于生成带有 `非类型模板参数` 的模板的类型特性的模板结构体。

### 语法

```c++
template<template<auto...> typename, typename>
struct trait_auto;
```

### 注意

该特性仅能检查参数为实体的模板类型。
不支持 `模板模板参数` 和 `类型模板参数`。

#### 辅助模板

```c++
template<template<auto...> typename _Tamplate, typename _Type>
constexpr bool trait_auto_v;
```

### 示例

```c++
#include "is_valid.h"

template<auto...>
struct foo;

template<int...>
struct bar;

int main()
{
    using f = foo<1, 2>;
    using b = bar<3, 4, 5>;
    
    static_assert(fake::trait_auto_v<foo, f>);
    static_assert(!fake::trait_auto_v<bar, f>);
    static_assert(fake::trait_auto_v<bar, b>);
    static_assert(!fake::trait_auto_v<foo, b>);
}
```

## `fake::trait_auto_c`

### 简介

- 用于检查某个类型是否为指定模板实例的概念。

### 语法

```c++
template<typename _Type, template<auto...> typename _Tamplate>
concept trait_auto_c;
```

### 示例

```c++
#include <iostream>

#include "is_valid.h"

template<auto...>
struct bar{};

void foo(const fake::trait_c<std::pair> auto &_e){
    std::cout << "pair version" << std::endl;
}

void foo(const fake::trait_auto_c<bar> auto &_e){
    std::cout << "bar version" << std::endl;
}

int main()
{
    foo(std::pair<float, short>{});
    foo(bar<1, 2, 3>{});
}
```

**输出:**

```plain
pair version
bar version
```

## `fake::to_trait_c`

### 简介

- 将 constexpr bool 值转换为概念的模板概念。

### 语法

```c++
template<bool _Condition>
concept to_trait_c;
```

### 示例

```c++
#include <iostream>
#include <tuple>

#include "is_valid.h"

void foo(auto _e) requires
fake::trait_c<decltype(_e), std::tuple> &&
fake::to_trait_c<std::tuple_size_v<decltype(_e)> == 3>{
    std::cout << "tuple[3] version" << std::endl;
}

void foo(auto){
    std::cout << "default version" << std::endl;
}

int main()
{
    foo(1);
    foo(std::tuple{1, 2.0, '3'});
    foo(std::tuple{1, 2.0, '3', nullptr});
}
```

**输出:**

```plain
default version
tuple[3] version
default version
```

## `fake::adapt_c`

### 简介

- 标准库`type_traits`适配器。

### 语法

```c++
template<typename _Type, template<typename> typename _Template>
concept adapt_c = fake::detail::is_valid::trait_adaptor<_Type, _Template>::value;
```

### 示例

```c++
#include <tuple>
#include "is_valid.h"

template<typename...>
struct types;

int main()
{
    struct base{}; struct derive final : base{};
    
    // adapt std type_traits for concepts
    static_assert(fake::adapt_c<derive, std::is_final>);
    static_assert(fake::adapt_c<base, std::is_final> == false);
    static_assert(fake::adapt_c<base, std::is_empty>);
    static_assert(fake::adapt_c<derive, std::is_empty>);
    static_assert(fake::adapt_c<int, std::is_empty> == false);
    [](fake::adapt_c<std::is_final> auto){}(derive{});
    
    // if the second parameter is not a std type_traits, fallback to fake::trait_c
    static_assert(fake::adapt_c<std::tuple<int, float, char>, std::tuple>);
    static_assert(fake::adapt_c<int, std::tuple> == false);
    static_assert(fake::adapt_c<types<int, float, char>, types>);
    static_assert(fake::adapt_c<int, types> == false);
    [](fake::adapt_c<std::tuple> auto){}(std::make_tuple(114, 514.0, "1919"));
}
```

## `fake::type_package`

### 简介

- 用于保存类型以便于操作和识别的模板结构体。

### 语法

```c++
template<typename _Type>
struct type_package;
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    using t = fake::type_package<float>;
    
    static_assert(std::same_as<t::type, float>);
}
```

## `fake::pack_t`

### 简介

- `fake::type_package`的别名，简化类型包装。

### 语法

```c++
template<typename _Type>
using pack_t = type_package<_Type>;
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    using t = fake::pack_t<double>;
    
    static_assert(std::same_as<t::type, double>);
}
```

## `fake::pack_v`

### 简介

- 用于创建具有特定类型的`type_package`实例的辅助变量。

### 语法

```c++
template<typename _Type>
constexpr type_package<_Type> pack_v{};
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    constexpr fake::pack_c auto t = fake::pack_v<std::tuple<int, float, char>>;
    
    static_assert(std::same_as<fake::take_t<t>, std::tuple<int, float, char>>);
}
```

## `fake::pack_c`

### 简介

- 检查某个类型是否为`type_package`实例的概念。

### 语法

```c++
template<typename _Type>
concept pack_c = trait_v<type_package, std::remove_cvref_t<_Type>>;
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    using t = fake::pack_t<int>;
    
    static_assert(fake::pack_c<t>);
    static_assert(!fake::pack_c<void>);
}
```

## `fake::wrap_v`

### 简介

- 包装 constexpr 值的类型的模板。

### 语法

```c++
template<auto _value>
constexpr type_package<decltype(_value)> wrap_v{};
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    constexpr auto func1 = [](float x) { return x * 2.0f; };
    constexpr auto func2 = [](int x) { return x + 3; };
    
    static_assert(std::same_as<decltype(fake::wrap_v<+func1>)::type, float(*)(float)>);
    static_assert(std::same_as<decltype(fake::wrap_v<+func2>)::type, int(*)(int)>);
}
```

## `fake::number`

### 简介

- 如果提供的类型是整数类型，则创建一个`std::integral_constant`实例的模板。

### 语法

```c++
template<auto _value>
requires std::is_integral_v<std::remove_cvref_t<decltype(_value)>>
constexpr std::integral_constant<std::remove_cvref_t<decltype(_value)>, _value> number{};
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    static_assert(fake::number<true> == std::bool_constant<true>{});
}
```

此代码示例演示了`fake::number`的用法，断言使用`true`调用它会产生一个`std::bool_constant<true>`，即一个整数常量。

## `fake::value_mezzanine`

### 简介

- 用于将编译期值包装成具有`value`成员的类型的模板结构体。

### 语法

```c++
template<auto _Value>
struct value_mezzanine;
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    // using `value_mezzanine` to wrap a literal value. 
    static_assert(fake::value_mezzanine<114>::value == 114);
    
    // using `value_mezzanine` to wrap a constexpr value. 
    constexpr auto v = true;
    static_assert(fake::value_mezzanine<v>::value == true);
}
```

## `fake::mezz_t`

### 简介

- 指向`value_mezzanine`的模板别名。

### 语法

```c++
template<auto _Value>
using mezz_t = value_mezzanine<_Value>;
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    using m = fake::mezz_t<114514>;
    static_assert(m::value == 114514);
}
```

## `fake::mezz_v`

### 简介

- `value_mezzanine`的 constexpr 实例。

### 语法

```c++
template<auto _Value>
constexpr value_mezzanine<_Value> mezz_v{};
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    [](auto _mezz, auto _value){
        static_assert(_mezz.value == 114);
        
        // compile error: non-constant condition for static assertion. 
        // static_assert(_value == 514);
    }(fake::mezz_v<114>, 514);
}
```

## `fake::mezz_c`

### 简介

- 用于判断某个类型是否可视为中间类型（mezzanine type）的概念，允许更安全的模板使用。

### 语法

```c++
template<typename _Type>
concept mezz_c = trait_auto_v<value_mezzanine, std::remove_cvref_t<_Type>>;
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    // using `mezz_c` to check if a type conforms to the `value_mezzanine` structure.
    using m = fake::mezz_t<114514>;
    static_assert(fake::mezz_c<m>);
}
```

## `fake::take_t`, `fake::take_v`

### 简介

从类型包（`pack_c`）或类型包中间体（`pazz_c`）中提取类型或值的工具。

### 语法

```c++
template<auto _arg>
using take_t = typename take<_arg>::type;

template<auto _arg>
constexpr take_t<_arg> take_v{};
```

### 示例

```c++
#include <concepts>
#include "is_valid.h"

int main()
{
    struct foo final{};
    
    constexpr fake::pack_c auto pack = fake::pack_v<foo>;
    static_assert(std::same_as<fake::take_t<pack>, foo>);
    
    constexpr fake::pazz_c auto pazz = fake::pazz_v<foo>;
    static_assert(std::same_as<fake::take_t<pazz>, foo>);
    
    constexpr foo value = fake::take_v<pack>;
    // value is a default-initialized foo object.
}
```

## `fake::generic` `fake::array_like` `fake::view_like` `fake::pattern_t` `fake::gene_c`

### 简介

用于捕获和重新应用模板模式的工具。

- `generic` 处理具有类型参数的模板（例如 `std::tuple`）。
- `array_like` 处理具有类型和大小的模板（例如 `std::array`）。
- `view_like` 处理具有字符包的模板（例如 `fake::view`）。
- `pattern_t` 从实例化类型中提取相应的模式。
- `gene_c` 是一个概念，用于检查某个类型是否为这些模式持有者之一。

### 语法

```c++
template<template<typename...> typename _Template>
struct generic {
    template<typename... _Parameters>
    using type = _Template<_Parameters...>;
};

template<template<typename, std::size_t> typename _Template>
struct array_like {
    template<typename _T, std::size_t _s>
    using type = _Template<_T, _s>;
};

template<template<char...> typename _Template>
struct view_like {
    template<char... _c>
    using type = _Template<_c...>;
};

template<typename _Type>
using pattern_t = typename pattern<std::remove_cvref_t<_Type>>::type;

template<typename _Type>
concept gene_c = is_generic_v<_Type>;
```

### 示例

```c++
#include <concepts>
#include <tuple>
#include <array>
#include "is_valid.h"
#include "view.h"   // for fake::view and fake::view_t

int main()
{
    using tuple_one = fake::generic<std::tuple>;
    using tuple_two = fake::pattern_t<std::tuple<int, double, std::nullptr_t>>;
    static_assert(std::same_as<tuple_one, tuple_two>);
    static_assert(std::same_as<tuple_one::type<float, unsigned>, std::tuple<float, unsigned>>);
    
    using array_one = fake::array_like<std::array>;
    using array_two = fake::pattern_t<std::array<int, 114>>;
    static_assert(std::same_as<array_one, array_two>);
    static_assert(std::same_as<array_one::type<float, 514>, std::array<float, 514>>);
    
    using view_one = fake::view_like<fake::view>;
    using view_two = fake::pattern_t<fake::view_t<"iiyo">>;
    static_assert(std::same_as<view_one, view_two>);
    static_assert(std::same_as<view_one::type<'y', 'a', 'j', 'u'>, fake::view_t<"yaju">>);
    
    static_assert(fake::gene_c<tuple_one>);
    static_assert(fake::gene_c<array_one>);
    static_assert(fake::gene_c<view_one>);
}
```

## `fake::sfinae`

### 简介

一个通过继承实现的标签类型，支持优先级化的重载解析，常用于在 constexpr 上下文中模拟 SFINAE。
更高的`_Priority`值从较低的值派生，使得它们在重载解析中更特化。

### 语法

```c++
template<std::size_t _Priority>
struct sfinae : sfinae<_Priority - 1> {};

template<>
struct sfinae<std::size_t{}> {};
```

### 示例

```c++
#include <type_traits>
#include "is_valid.h"

int main()
{
    using base1 = decltype([](fake::sfinae<1>, ...) { return false; });
    using base2 = decltype([]<typename _T>(fake::sfinae<2>, _T, std::type_identity_t<_T>) { return true; });
    
    struct select final : base1, base2
    {
        using base1::operator();
        using base2::operator();
    };
    
    static constexpr select convertible;
    
    static_assert(convertible(fake::sfinae<2>{}, 114, 514.0) == true);
    static_assert(convertible(fake::sfinae<2>{}, 1919, "810") == false);
}
```

## `fake::index_v`

### 简介

为给定的`std::size_t`值创建`std::integral_constant<std::size_t, _value>`实例的变量模板。

### 语法

```c++
template<std::size_t _value>
constexpr auto index_v = std::integral_constant<std::size_t, _value>{};
```

### 示例

```c++
#include "is_valid.h"

int main()
{
    static_assert(fake::index_v<114uz> == 114uz);
    static_assert(fake::index_v<514uz>.value == 514uz);
}
```

## `fake::pack_index_t`

### 简介

从参数包中提取指定索引处的类型。

### 语法

```c++
template<std::size_t _Index, typename... _Types>
using pack_index_t = typename arg_pack<_Index, _Types...>::type;
```

### 示例

```c++
#include <tuple>
#include "is_valid.h"

int main()
{
    static constexpr auto result = []<typename... _T>(std::tuple<_T...>, auto _index) {
        return fake::pack_v<fake::pack_index_t<_index, _T...>>;
    }(std::make_tuple(114, 514.0, "1919"), fake::index_v<1>);
    
    static_assert(std::same_as<fake::take_t<result>, double>);
}
```

## `fake::element_index_t`

### 简介

从实例化的模板（如`std::variant`）中提取指定索引处的类型。

### 语法

```c++
template<std::size_t _Index, typename _Type>
using element_index_t = typename element<_Index, _Type>::type;
```

### 示例

```c++
#include <variant>
#include "is_valid.h"

int main()
{
    static_assert(std::same_as<fake::element_index_t<1, std::variant<int, double, std::nullptr_t>>, double>);
}
```

## `fake::element_size_v`

### 简介

获取实例化模板（如`std::variant`）中类型参数的个数。

### 语法

```c++
template<typename _Type>
constexpr std::size_t element_size_v = element<0, std::remove_cvref_t<_Type>>::size;
```

### 示例

```c++
#include <variant>
#include "is_valid.h"

int main()
{
    static_assert(fake::element_size_v<std::variant<int, double, std::nullptr_t>> == 3);
}
```
