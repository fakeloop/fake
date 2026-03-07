# `is_valid.h`

- basic utilities for meta programming

## `fake::null_t`

### brief

- `null_t` is a tag type which usually represents an invalid result.

### syntax

```c++
struct null_t;
```

### example

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

### brief

- Concept to check if an entity is a `fake::null_t`.

### syntax

```c++
template<auto _value>
concept null_c;
```

### example

```c++
#include "is_valid.h"

int main()
{
    static_assert(fake::null_c<fake::null_t{}>);
}
```

## `fake::trait`

### brief

- Template struct type to generate a type trait of templates with `type template parameters`.

### syntax

```c++
template<template<typename...> typename, typename>
struct trait;
```

### note

this trait could only check template types with parameters of types.
`template template parameters` and `non-type template parameters` are not supported.

#### helper template

```c++
template<template<typename...> typename _Tamplate, typename _Type>
constexpr bool trait_v;
```

### example

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

### brief

- Concept to check if a type is an instance of a specified template.

### syntax

```c++
template<typename _Type, template<typename...> typename _Tamplate>
concept trait_c;
```

### example

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

**output:**

```plain
pair version
tuple version
```

## `fake::trait_auto`

### brief

- Template struct type to generate a type trait of templates with `non-type template parameters`.

### syntax

```c++
template<template<auto...> typename, typename>
struct trait_auto;
```

### note

this trait could only check template types with parameters of entities.
`template template parameters` and `type template parameters` are not supported.

#### helper template

```c++
template<template<auto...> typename _Tamplate, typename _Type>
constexpr bool trait_auto_v;
```

### example

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

### brief

- Concept to check if a type is an instance of a specified template.

### syntax

```c++
template<typename _Type, template<auto...> typename _Tamplate>
concept trait_auto_c;
```

### example

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

**output:**

```plain
pair version
bar version
```

## `fake::to_trait_c`

### brief

- Template concept to turn a constexpr bool value into a concept.

### syntax

```c++
template<bool _Condition>
concept to_trait_c;
```

### example

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

**output:**

```plain
default version
tuple[3] version
default version
```

## `fake::adapt_c`

### brief

- A std `type_traits` adaptor.

### syntax

```c++
template<typename _Type, template<typename> typename _Template>
concept adapt_c = fake::detail::is_valid::trait_adaptor<_Type, _Template>::value;
```

### example

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

### brief

- A template struct that holds a type for easy manipulation and identification.

### syntax

```c++
template<typename _Type>
struct type_package;
```

### example

```c++
#include "is_valid.h"

int main()
{
    using t = fake::type_package<float>;
    
    static_assert(std::same_as<t::type, float>);
}
```

## `fake::pack_t`

### brief

- An alias for `fake::type_package` to simplify type wrapping.

### syntax

```c++
template<typename _Type>
using pack_t = type_package<_Type>;
```

### example

```c++
#include "is_valid.h"

int main()
{
    using t = fake::pack_t<double>;
    
    static_assert(std::same_as<t::type, double>);
}
```

## `fake::pack_v`

### brief

- A helper to create instances of `type_package` with a specific type.

### syntax

```c++
template<typename _Type>
constexpr type_package<_Type> pack_v{};
```

### example

```c++
#include "is_valid.h"

int main()
{
    constexpr fake::pack_c auto t = fake::pack_v<std::tuple<int, float, char>>;
    
    static_assert(std::same_as<fake::take_t<t>, std::tuple<int, float, char>>);
}
```

## `fake::pack_c`

### brief

- A concept that checks if a type is an instance of `type_package`.

### syntax

```c++
template<typename _Type>
concept pack_c = trait_v<type_package, std::remove_cvref_t<_Type>>;
```

### example

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

### brief

- A template that wraps the type of a constexpr value.

### syntax

```c++
template<auto _value>
constexpr type_package<decltype(_value)> wrap_v{};
```

### example

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

### brief

- A template that creates a `std::integral_constant` instance if the provided type is integral.

### syntax

```c++
template<auto _value>
requires std::is_integral_v<std::remove_cvref_t<decltype(_value)>>
constexpr std::integral_constant<std::remove_cvref_t<decltype(_value)>, _value> number{};
```

### example

```c++
#include "is_valid.h"

int main()
{
    static_assert(fake::number<true> == std::bool_constant<true>{});
}
```

This code example demonstrates the usage of `fake::number` by asserting that calling it with `true` results in a `std::bool_constant<true>`, which is an integral constant.

## `fake::value_mezzanine`

### brief

- A template struct used to wrap a compile-time value into a type with a `value` member.

### syntax

```c++
template<auto _Value>
struct value_mezzanine;
```

### example

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

### brief

- A template alias refer to `value_mezzanine`.

### syntax

```c++
template<auto _Value>
using mezz_t = value_mezzanine<_Value>;
```

### example

```c++
#include "is_valid.h"

int main()
{
    using m = fake::mezz_t<114514>;
    static_assert(m::value == 114514);
}
```

## `fake::mezz_v`

### brief

- A constexpr instance of `value_mezzanine`.

### syntax

```c++
template<auto _Value>
constexpr value_mezzanine<_Value> mezz_v{};
```

### example

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

### brief

- A concept that determines if a type can be treated as a mezzanine type, allowing for safer template usage.

### syntax

```c++
template<typename _Type>
concept mezz_c = trait_auto_v<value_mezzanine, std::remove_cvref_t<_Type>>;
```

### example

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

### brief

Utilities to extract a type or a value from a type package (`pack_c`) or a type package mezzanine (`pazz_c`).

### syntax

```c++
template<auto _arg>
using take_t = typename take<_arg>::type;

template<auto _arg>
constexpr take_t<_arg> take_v{};
```

### example

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

### brief

Utilities for capturing and reapplying template patterns.

- `generic` handles templates with type parameters (e.g. `std::tuple`).

- `array_like` handles templates with a type and a size (e.g. `std::array`).

- `view_like` handles templates with a pack of characters (e.g. `fake::view`).

- `pattern_t` extracts the corresponding pattern from an instantiated type.

- `gene_c` is a concept that checks if a type is one of these pattern holders.

### syntax

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

### example

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

### brief

A tag type with inheritance that enables prioritized overload resolution, often used to simulate SFINAE in constexpr contexts.
Higher `_Priority` values derive from lower ones, making them more specialized in overload resolution.

### syntax

```c++
template<std::size_t _Priority>
struct sfinae : sfinae<_Priority - 1> {};

template<>
struct sfinae<std::size_t{}> {};
```

### example

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

### brief

A variable template that creates a `std::integral_constant<std::size_t, _value>` instance for a given `std::size_t` value.

### syntax

```c++
template<std::size_t _value>
constexpr auto index_v = std::integral_constant<std::size_t, _value>{};
```

### example

```c++
#include "is_valid.h"

int main()
{
    static_assert(fake::index_v<114uz> == 114uz);
    static_assert(fake::index_v<514uz>.value == 514uz);
}
```

## `fake::pack_index_t`

### brief

Extracts the type at a given index from a parameter pack.

### syntax

```c++
template<std::size_t _Index, typename... _Types>
using pack_index_t = typename arg_pack<_Index, _Types...>::type;
```

### example

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

### brief

Extracts the type at a given index from an instantiated template (like `std::variant`).

### syntax

```c++
template<std::size_t _Index, typename _Type>
using element_index_t = typename element<_Index, _Type>::type;
```

### example

```c++
#include <variant>
#include "is_valid.h"

int main()
{
    static_assert(std::same_as<fake::element_index_t<1, std::variant<int, double, std::nullptr_t>>, double>);
}
```

## `fake::element_size_v`

### brief

Obtains the number of type parameters in an instantiated template (like `std::variant`).

### syntax

```c++
template<typename _Type>
constexpr std::size_t element_size_v = element<0, std::remove_cvref_t<_Type>>::size;
```

### example

```c++
#include <variant>
#include "is_valid.h"

int main()
{
    static_assert(fake::element_size_v<std::variant<int, double, std::nullptr_t>> == 3);
}
```
