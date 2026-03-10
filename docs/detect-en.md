# `detect.h`

- A framework for type detection and transformation using compile-time predicates and combinators.

## Overview

`detect.h` provides a set of utilities to create and combine **detectors** – compile-time predicates that operate on type wrappers (`fake::pack_c` or `fake::pazz_c`).
Detectors can be combined with logical operators (`&&`, `||`, `!`, `==`, `!=`, `^`) and transformation operators (`<<=`) to build complex type queries.
Type transformations are implemented as **shift** operators that modify the wrapped type before applying a detector.

## Core Concepts

### `fake::creed_c`

- **Brief**: A concept that identifies a type as a detector (a predicate that can be applied to a type wrapper).
- **Syntax**:

```c++
template<typename _Type>
concept creed_c = std::derived_from<std::remove_cvref_t<_Type>, fake::detect::op::creed>;
```

### `fake::shift_c`

- **Brief**: A concept that identifies a type as a transformation (shift) that can be applied to a type wrapper.
- **Syntax**:

```c++
template<typename _Type>
concept shift_c = std::derived_from<std::remove_cvref_t<_Type>, fake::detect::op::shift>;
```

### `fake::pack_c` and `fake::pazz_c`

- **Brief**: Type wrappers that hold a type (`fake::pack_t`) or a value of a type package (`fake::pazz_t`). They are the inputs to detectors.
- **Syntax** (defined in `is_valid.h`):

```c++
template<typename _Type>
using pack_t = type_package<_Type>;
template<typename _Type>
using pazz_t = package_mezzanine<_Type>;
template<typename _Type>
concept pack_c = trait_v<type_package, std::remove_cvref_t<_Type>>;
template<typename _Type>
concept pazz_c = trait_v<package_mezzanine, std::remove_cvref_t<_Type>>;
```

### `fake::take_t`

- **Brief**: Extracts the underlying type from a `pack_c` or `pazz_c` wrapper.
- **Syntax** (defined in `is_valid.h`):

```c++
template<auto _arg>
using take_t = typename take<_arg>::type;
```

## Predefined Detectors

### Primary Type Categories

#### `fake::void_d`

- **Brief**: Detects if the wrapped type is `void`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::void_d void_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::void_d(fake::pack_v<void>));
    static_assert(!fake::void_d(fake::pack_v<int>));
}
```

#### `fake::null_pointer_d`

- **Brief**: Detects if the wrapped type is `std::nullptr_t`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::null_pointer_d null_pointer_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::null_pointer_d(fake::pack_v<std::nullptr_t>));
    static_assert(!fake::null_pointer_d(fake::pack_v<int>));
}
```

#### `fake::integral_d`

- **Brief**: Detects if the wrapped type is an integral type.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::integral_d integral_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::integral_d(fake::pack_v<int>));
    static_assert(!fake::integral_d(fake::pack_v<float>));
}
```

#### `fake::floating_point_d`

- **Brief**: Detects if the wrapped type is a floating-point type.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::floating_point_d floating_point_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::floating_point_d(fake::pack_v<double>));
    static_assert(!fake::floating_point_d(fake::pack_v<int>));
}
```

#### `fake::array_d`

- **Brief**: Detects if the wrapped type is an array type (bounded or unbounded).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::array_d array_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::array_d(fake::pack_v<int[]>));
    static_assert(!fake::array_d(fake::pack_v<int>));
}
```

#### `fake::enum_d`

- **Brief**: Detects if the wrapped type is an enumeration.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::enum_d enum_d;
```

- **Example**:

```c++
#include "detect.h"
enum struct enum_e{foo, bar};
int main() {
    static_assert(fake::enum_d(fake::pack_v<enum_e>));
    static_assert(!fake::enum_d(fake::pack_v<int>));
}
```

#### `fake::union_d`

- **Brief**: Detects if the wrapped type is a union.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::union_d union_d;
```

- **Example**:

```c++
#include "detect.h"
union un{int foo; float bar;};
int main() {
    static_assert(fake::union_d(fake::pack_v<un>));
    static_assert(!fake::union_d(fake::pack_v<int>));
}
```

#### `fake::class_d`

- **Brief**: Detects if the wrapped type is a class (or struct).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::class_d class_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{};
int main() {
    static_assert(fake::class_d(fake::pack_v<foo>));
    static_assert(!fake::class_d(fake::pack_v<int>));
}
```

#### `fake::function_d`

- **Brief**: Detects if the wrapped type is a function type.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::function_d function_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::function_d(fake::pack_v<decltype(main)>));
    static_assert(!fake::function_d(fake::pack_v<int>));
}
```

#### `fake::pointer_d`

- **Brief**: Detects if the wrapped type is a pointer.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::pointer_d pointer_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::pointer_d(fake::pack_v<int*>));
    static_assert(!fake::pointer_d(fake::pack_v<int>));
}
```

#### `fake::lvalue_reference_d`

- **Brief**: Detects if the wrapped type is an lvalue reference.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::lvalue_reference_d lvalue_reference_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::lvalue_reference_d(fake::pack_v<int&>));
    static_assert(!fake::lvalue_reference_d(fake::pack_v<int&&>));
}
```

#### `fake::rvalue_reference_d`

- **Brief**: Detects if the wrapped type is an rvalue reference.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::rvalue_reference_d rvalue_reference_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::rvalue_reference_d(fake::pack_v<int&&>));
    static_assert(!fake::rvalue_reference_d(fake::pack_v<int&>));
}
```

#### `fake::member_object_pointer_d`

- **Brief**: Detects if the wrapped type is a pointer to data member.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::member_object_pointer_d member_object_pointer_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int bar;};
int main() {
    static_assert(fake::member_object_pointer_d(fake::pack_v<decltype(&foo::bar)>));
    static_assert(!fake::member_object_pointer_d(fake::pack_v<int*>));
}
```

#### `fake::member_function_pointer_d`

- **Brief**: Detects if the wrapped type is a pointer to member function.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::member_function_pointer_d member_function_pointer_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int bar; int fun(){return 114514;};};
int main() {
    static_assert(fake::member_function_pointer_d(fake::pack_v<decltype(&foo::fun)>));
    static_assert(!fake::member_function_pointer_d(fake::pack_v<decltype(&foo::bar)>));
}
```

### Fake Type Categories

These detectors identify types defined in the `fake` library.

#### `fake::null_d`

- **Brief**: Detects if the wrapped type is `fake::null_t`.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto null_d = fake::just_d<[]<fake::like<fake::null_t>>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::null_d(fake::pack_v<fake::null_t>));
    static_assert(!fake::null_d(fake::pack_v<int>));
}
```

#### `fake::view_d`

- **Brief**: Detects if the wrapped type is a `fake::view` instantiation.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto view_d = fake::just_d<[]<fake::view_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
#include "view.h"
int main() {
    static_assert(fake::view_d(fake::pack_v<fake::view_t<"foo">>));
    static_assert(!fake::view_d(fake::pack_v<int>));
}
```

#### `fake::flat_d`

- **Brief**: Detects if the wrapped type is a `fake::flat` instantiation.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto flat_d = fake::just_d<[]<fake::flat_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
#include "flat.h"
int main() {
    static_assert(fake::flat_d(fake::pack_v<fake::flat<int, float, double>>));
    static_assert(!fake::flat_d(fake::pack_v<int>));
}
```

#### `fake::query_d`

- **Brief**: Detects if the wrapped type is a `fake::query` instantiation.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto query_d = fake::just_d<[]<fake::query_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
#include "flat.h"
int main() {
    using query_t = fake::query<fake::mate<int, float>, fake::mate<short, double>>;
    static_assert(fake::query_d(fake::pack_v<query_t>));
    static_assert(!fake::query_d(fake::pack_v<int>));
}
```

#### `fake::pack_d`

- **Brief**: Detects if the wrapped type is a `fake::pack_t` (type package).
- **Syntax**:

```c++
inline constexpr fake::creed_c auto pack_d = fake::just_d<[]<fake::pack_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
#include "is_valid.h"
int main() {
    static_assert(fake::pack_d(fake::pack_v<fake::pack_t<int>>));
    static_assert(!fake::pack_d(fake::pack_v<int>));
}
```

#### `fake::pazz_d`

- **Brief**: Detects if the wrapped type is a `fake::pazz_t` (package mezzanine).
- **Syntax**:

```c++
inline constexpr fake::creed_c auto pazz_d = fake::just_d<[]<fake::pazz_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::pazz_d(fake::pack_v<fake::pazz_t<int>>));
    static_assert(!fake::pazz_d(fake::pack_v<int>));
}
```

#### `fake::mezz_d`

- **Brief**: Detects if the wrapped type is a `fake::mezz_t` (value mezzanine).
- **Syntax**:

```c++
inline constexpr fake::creed_c auto mezz_d = fake::just_d<[]<fake::mezz_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::mezz_d(fake::pack_v<fake::mezz_t<114514>>));
    static_assert(!fake::mezz_d(fake::pack_v<int>));
}
```

#### `fake::gene_d`

- **Brief**: Detects if the wrapped type is a generic pattern holder (e.g., `fake::generic`, `fake::array_like`, `fake::view_like`).
- **Syntax**:

```c++
inline constexpr fake::creed_c auto gene_d = fake::just_d<[]<fake::gene_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    using pattern = fake::pattern_t<std::variant<int, float, double>>;
    static_assert(fake::gene_d(fake::pack_v<pattern>));
    static_assert(!fake::gene_d(fake::pack_v<int>));
}
```

#### `fake::debug_d`

- **Brief**: Detects if the wrapped type is `fake::debug`.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto debug_d = fake::just_d<[]<fake::debug_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::debug_d(fake::pack_v<fake::debug>));
    static_assert(!fake::debug_d(fake::pack_v<int>));
}
```

#### `fake::char_array_d`

- **Brief**: Detects if the wrapped type is a `fake::char_array`.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto char_array_d = fake::just_d<[]<fake::char_array_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::char_array_d(fake::pack_v<fake::char_array<1919>>));
    static_assert(!fake::char_array_d(fake::pack_v<int>));
}
```

#### `fake::constant_d`

- **Brief**: Detects if the wrapped type is a `std::integral_constant`.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto constant_d = fake::just_d<[]<fake::constant_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::constant_d(fake::pack_v<std::integral_constant<uint32_t, 364364>>));
    static_assert(!fake::constant_d(fake::pack_v<int>));
}
```

#### `fake::pair_d`

- **Brief**: Detects if the wrapped type is a `std::pair`.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto pair_d = fake::just_d<[]<fake::pair_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::pair_d(fake::pack_v<std::pair<int, double>>));
    static_assert(!fake::pair_d(fake::pack_v<int>));
}
```

#### `fake::tuple_d`

- **Brief**: Detects if the wrapped type is a `std::tuple`.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto tuple_d = fake::just_d<[]<fake::tuple_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::tuple_d(fake::pack_v<std::tuple<int, double, float>>));
    static_assert(!fake::tuple_d(fake::pack_v<int>));
}
```

#### `fake::combine_d`

- **Brief**: Detects if the wrapped type is a `std::pair` or `std::tuple`.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto combine_d = fake::just_d<[]<fake::combine_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::combine_d(fake::pack_v<std::pair<int, double>>));
    static_assert(fake::combine_d(fake::pack_v<std::tuple<int, double, float>>));
    static_assert(!fake::combine_d(fake::pack_v<int>));
}
```

#### `fake::std_array_d`

- **Brief**: Detects if the wrapped type is a `std::array`.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto std_array_d = fake::just_d<[]<fake::std_array_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::std_array_d(fake::pack_v<std::array<int, 810>>));
    static_assert(!fake::std_array_d(fake::pack_v<int>));
}
```

#### `fake::container_d`

- **Brief**: Detects if the wrapped type satisfies container-like requirements (has `begin`, `end`, `size`, `value_type`).
- **Syntax**:

```c++
inline constexpr fake::creed_c auto container_d = fake::just_d<[]<fake::container_c>(){}>;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::container_d(fake::pack_v<std::vector<float>>));
    static_assert(!fake::container_d(fake::pack_v<int>));
}
```

#### `fake::optional_d`

- **Brief**: Detects if the wrapped type is a `std::optional`.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto optional_d = fake::just_d<[]<fake::optional_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::optional_d(fake::pack_v<std::optional<double>>));
    static_assert(!fake::optional_d(fake::pack_v<int>));
}
```

#### `fake::variant_d`

- **Brief**: Detects if the wrapped type is a `std::variant`.
- **Syntax**:

```c++
inline constexpr fake::creed_c auto variant_d = fake::just_d<[]<fake::variant_c>(){}>;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::variant_d(fake::pack_v<std::variant<float, double, int>>));
    static_assert(!fake::variant_d(fake::pack_v<int>));
}
```

### Composite Type Categories

#### `fake::fundamental_d`

- **Brief**: Detects if the wrapped type is a fundamental type (arithmetic, void, or nullptr_t).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::fundamental_d fundamental_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{};
int main() {
    static_assert(fake::fundamental_d(fake::pack_v<double>));
    static_assert(!fake::fundamental_d(fake::pack_v<foo>));
}
```

#### `fake::arithmetic_d`

- **Brief**: Detects if the wrapped type is an arithmetic type (integral or floating-point).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::arithmetic_d arithmetic_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{};
int main() {
    static_assert(fake::arithmetic_d(fake::pack_v<int64_t>));
    static_assert(!fake::arithmetic_d(fake::pack_v<foo>));
}
```

#### `fake::scalar_d`

- **Brief**: Detects if the wrapped type is a scalar type (arithmetic, enum, pointer, member pointer, or nullptr_t).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::scalar_d scalar_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{};
int main() {
    static_assert(fake::scalar_d(fake::pack_v<float>));
    static_assert(fake::scalar_d(fake::pack_v<int>));
    static_assert(!fake::scalar_d(fake::pack_v<foo>));
}
```

#### `fake::object_d`

- **Brief**: Detects if the wrapped type is an object type (not a function, reference, or void).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::object_d object_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{};
int main() {
    static_assert(fake::object_d(fake::pack_v<foo>));
    static_assert(fake::object_d(fake::pack_v<int>));
    static_assert(fake::object_d(fake::pack_v<int*>));
    static_assert(!fake::object_d(fake::pack_v<decltype(main)>));
    static_assert(!fake::object_d(fake::pack_v<foo&>));
    static_assert(!fake::object_d(fake::pack_v<void>));
}
```

#### `fake::compound_d`

- **Brief**: Detects if the wrapped type is a compound type (array, function, pointer, reference, class, union, enum, or member pointer).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::compound_d compound_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::compound_d(fake::pack_v<int&>));
    static_assert(fake::compound_d(fake::pack_v<int*&>));
    static_assert(fake::compound_d(fake::pack_v<int[]>));
    static_assert(!fake::compound_d(fake::pack_v<int>));
}
```

#### `fake::reference_d`

- **Brief**: Detects if the wrapped type is a reference (lvalue or rvalue).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::reference_d reference_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::reference_d(fake::pack_v<int&&>));
    static_assert(fake::reference_d(fake::pack_v<int&>));
    static_assert(!fake::reference_d(fake::pack_v<int>));
}
```

#### `fake::member_pointer_d`

- **Brief**: Detects if the wrapped type is a pointer to member (data or function).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::member_pointer_d member_pointer_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int bar;};
int main() {
    static_assert(fake::member_pointer_d(fake::pack_v<decltype(&foo::bar)>));
    static_assert(!fake::member_pointer_d(fake::pack_v<foo>));
}
```

### Type Properties

#### `fake::const_d`

- **Brief**: Detects if the wrapped type is `const`-qualified.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::const_d const_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::const_d(fake::pack_v<const int>));
    static_assert(!fake::const_d(fake::pack_v<int>));
}
```

#### `fake::volatile_d`

- **Brief**: Detects if the wrapped type is `volatile`-qualified.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::volatile_d volatile_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::volatile_d(fake::pack_v<volatile int>));
    static_assert(!fake::volatile_d(fake::pack_v<int>));
}
```

#### `fake::trivially_copyable_d`

- **Brief**: Detects if the wrapped type is trivially copyable.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::trivially_copyable_d trivially_copyable_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::trivially_copyable_d(fake::pack_v<foo>));
    static_assert(!fake::trivially_copyable_d(fake::pack_v<std::vector<int>>));
}
```

#### `fake::standard_layout_d`

- **Brief**: Detects if the wrapped type has standard layout.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::standard_layout_d standard_layout_d;
```

- **Example**:

```c++
#include <iostream>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::standard_layout_d(fake::pack_v<foo>));
    static_assert(!fake::standard_layout_d(fake::pack_v<std::istream>));
}
```

#### `fake::unique_object_representations_d`

- **Brief**: Detects if the wrapped type has unique object representations.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::unique_object_representations_d unique_object_representations_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int bar; double* yoo;};
struct bar final{int32_t foo; uint32_t ufoo;};
struct empty{};
int main() {
    static_assert(fake::unique_object_representations_d(fake::pack_v<char>));
    static_assert(fake::unique_object_representations_d(fake::pack_v<int>));
    static_assert(fake::unique_object_representations_d(fake::pack_v<bar>));
    static_assert(!fake::unique_object_representations_d(fake::pack_v<float>));
    static_assert(!fake::unique_object_representations_d(fake::pack_v<foo>));
    static_assert(!fake::unique_object_representations_d(fake::pack_v<empty>));
}
```

#### `fake::empty_d`

- **Brief**: Detects if the wrapped type is an empty class (no non-static data members).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::empty_d empty_d;
```

- **Example**:

```c++
#include "detect.h"
struct empty{};
struct foo{int bar;};
int main() {
    static_assert(fake::empty_d(fake::pack_v<empty>));
    static_assert(!fake::empty_d(fake::pack_v<foo>));
    static_assert(!fake::empty_d(fake::pack_v<int>));
}
```

#### `fake::polymorphic_d`

- **Brief**: Detects if the wrapped type is polymorphic (has at least one virtual function).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::polymorphic_d polymorphic_d;
```

- **Example**:

```c++
#include <string>
#include <iostream>
#include "detect.h"
int main() {
    static_assert(fake::polymorphic_d(fake::pack_v<std::istream>));
    static_assert(!fake::polymorphic_d(fake::pack_v<std::string>));
}
```

#### `fake::abstract_d`

- **Brief**: Detects if the wrapped type is abstract (has at least one pure virtual function).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::abstract_d abstract_d;
```

- **Example**:

```c++
#include "detect.h"
struct abstract{virtual int foo() = 0;};
struct foo{};
int main() {
    static_assert(fake::abstract_d(fake::pack_v<abstract>));
    static_assert(!fake::abstract_d(fake::pack_v<foo>));
}
```

#### `fake::final_d`

- **Brief**: Detects if the wrapped type is marked `final`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::final_d final_d;
```

- **Example**:

```c++
#include "detect.h"
struct bar final{};
struct foo{};
int main() {
    static_assert(fake::final_d(fake::pack_v<bar>));
    static_assert(!fake::final_d(fake::pack_v<foo>));
}
```

#### `fake::aggregate_d`

- **Brief**: Detects if the wrapped type is an aggregate.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::aggregate_d aggregate_d;
```

- **Example**:

```c++
#include <string>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::aggregate_d(fake::pack_v<foo>));
    static_assert(!fake::aggregate_d(fake::pack_v<std::string>));
}
```

#### `fake::signed_d`

- **Brief**: Detects if the wrapped type is a signed arithmetic type.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::signed_d signed_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::signed_d(fake::pack_v<int32_t>));
    static_assert(!fake::signed_d(fake::pack_v<uint32_t>));
}
```

#### `fake::unsigned_d`

- **Brief**: Detects if the wrapped type is an unsigned arithmetic type.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::unsigned_d unsigned_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::unsigned_d(fake::pack_v<uint8_t>));
    static_assert(!fake::unsigned_d(fake::pack_v<int8_t>));
}
```

#### `fake::bounded_array_d`

- **Brief**: Detects if the wrapped type is an array with a known bound.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::bounded_array_d bounded_array_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::bounded_array_d(fake::pack_v<int[114]>));
    static_assert(!fake::bounded_array_d(fake::pack_v<int[]>));
}
```

#### `fake::unbounded_array_d`

- **Brief**: Detects if the wrapped type is an array with an unknown bound.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::unbounded_array_d unbounded_array_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::unbounded_array_d(fake::pack_v<int[]>));
    static_assert(!fake::unbounded_array_d(fake::pack_v<int[514]>));
}
```

### Supported Operations

#### `fake::constructible_d<Args...>`

- **Brief**: Detects if the wrapped type is constructible from the given argument types.
- **Syntax**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::constructible_d<_Args...> constructible_d;
```

- **Example**:

```c++
#include <string_view>
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::constructible_d<std::size_t>(fake::pack_v<std::vector<int>>));
    static_assert(!fake::constructible_d<int>(fake::pack_v<std::string_view>));
}
```

#### `fake::trivially_constructible_d<Args...>`

- **Brief**: Detects if the wrapped type is trivially constructible from the given argument types.
- **Syntax**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::trivially_constructible_d<_Args...> trivially_constructible_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::trivially_constructible_d<int>(fake::pack_v<foo>));
    static_assert(!fake::trivially_constructible_d<std::size_t>(fake::pack_v<std::vector<int>>));
}
```

#### `fake::nothrow_constructible_d<Args...>`

- **Brief**: Detects if the wrapped type is nothrow constructible from the given argument types.
- **Syntax**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::nothrow_constructible_d<_Args...> nothrow_constructible_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::nothrow_constructible_d<int>(fake::pack_v<foo>));
    static_assert(!fake::nothrow_constructible_d<std::size_t>(fake::pack_v<std::vector<int>>));
}
```

#### `fake::default_constructible_d`

- **Brief**: Detects if the wrapped type is default constructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::default_constructible_d default_constructible_d;
```

- **Example**:

```c++
#include <functional>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::default_constructible_d(fake::pack_v<foo>));
    static_assert(!fake::default_constructible_d(fake::pack_v<std::reference_wrapper<int>>));
}
```

#### `fake::trivially_default_constructible_d`

- **Brief**: Detects if the wrapped type is trivially default constructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::trivially_default_constructible_d trivially_default_constructible_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::trivially_default_constructible_d(fake::pack_v<foo>));
    static_assert(!fake::trivially_default_constructible_d(fake::pack_v<std::vector<int>>));
}
```

#### `fake::nothrow_default_constructible_d`

- **Brief**: Detects if the wrapped type is nothrow default constructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::nothrow_default_constructible_d nothrow_default_constructible_d;
```

- **Example**:

```c++
#include <iostream>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::nothrow_default_constructible_d(fake::pack_v<foo>));
    static_assert(!fake::nothrow_default_constructible_d(fake::pack_v<std::istream>));
}
```

#### `fake::copy_constructible_d`

- **Brief**: Detects if the wrapped type is copy constructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::copy_constructible_d copy_constructible_d;
```

- **Example**:

```c++
#include <memory>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::copy_constructible_d(fake::pack_v<foo>));
    static_assert(!fake::copy_constructible_d(fake::pack_v<std::unique_ptr<int>>));
}
```

#### `fake::trivially_copy_constructible_d`

- **Brief**: Detects if the wrapped type is trivially copy constructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::trivially_copy_constructible_d trivially_copy_constructible_d;
```

- **Example**:

```c++
#include <memory>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::trivially_copy_constructible_d(fake::pack_v<foo>));
    static_assert(!fake::trivially_copy_constructible_d(fake::pack_v<std::shared_ptr<int>>));
}
```

#### `fake::nothrow_copy_constructible_d`

- **Brief**: Detects if the wrapped type is nothrow copy constructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::nothrow_copy_constructible_d nothrow_copy_constructible_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::nothrow_copy_constructible_d(fake::pack_v<foo>));
    static_assert(!fake::nothrow_copy_constructible_d(fake::pack_v<std::vector<int>>));
}
```

#### `fake::move_constructible_d`

- **Brief**: Detects if the wrapped type is move constructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::move_constructible_d move_constructible_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int bar; double* yoo;};
struct bar{bar(const bar&) = default; bar(bar&&) = delete;};
int main() {
    static_assert(fake::move_constructible_d(fake::pack_v<foo>));
    static_assert(!fake::move_constructible_d(fake::pack_v<bar>));
}
```

#### `fake::trivially_move_constructible_d`

- **Brief**: Detects if the wrapped type is trivially move constructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::trivially_move_constructible_d trivially_move_constructible_d;
```

- **Example**:

```c++
#include <memory>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::trivially_move_constructible_d(fake::pack_v<foo>));
    static_assert(!fake::trivially_move_constructible_d(fake::pack_v<std::shared_ptr<int>>));
}
```

#### `fake::nothrow_move_constructible_d`

- **Brief**: Detects if the wrapped type is nothrow move constructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::nothrow_move_constructible_d nothrow_move_constructible_d;
```

- **Example**:

```c++
#include <iostream>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::nothrow_move_constructible_d(fake::pack_v<foo>));
    static_assert(!fake::nothrow_move_constructible_d(fake::pack_v<std::ostream>));
}
```

#### `fake::assignable_d<T>`

- **Brief**: Detects if the wrapped type can be assigned from a value of type `T`.
- **Syntax**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::assignable_d<_Arg> assignable_d;
```

- **Example**:

```c++
#include <string>
#include <string_view>
#include "detect.h"
int main() {
    static_assert(fake::assignable_d<std::string>(fake::pack_v<std::string_view>));
    static_assert(!fake::assignable_d<std::string_view>(fake::pack_v<std::ostream>));
}
```

#### `fake::trivially_assignable_d<T>`

- **Brief**: Detects if the wrapped type can be trivially assigned from a value of type `T`.
- **Syntax**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::trivially_assignable_d<_Arg> trivially_assignable_d;
```

- **Example**:

```c++
#include <memory>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::trivially_assignable_d<foo>(fake::pack_v<foo>));
    static_assert(!fake::trivially_assignable_d<foo*>(fake::pack_v<std::shared_ptr<foo>>));
}
```

#### `fake::nothrow_assignable_d<T>`

- **Brief**: Detects if the wrapped type can be nothrow assigned from a value of type `T`.
- **Syntax**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::nothrow_assignable_d<_Arg> nothrow_assignable_d;
```

- **Example**:

```c++
#include <iostream>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::nothrow_assignable_d<foo>(fake::pack_v<foo>));
    static_assert(!fake::nothrow_assignable_d<std::ostream>(fake::pack_v<std::ostream>));
}
```

#### `fake::copy_assignable_d`

- **Brief**: Detects if the wrapped type is copy assignable.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::copy_assignable_d copy_assignable_d;
```

- **Example**:

```c++
#include <memory>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::copy_assignable_d(fake::pack_v<foo>));
    static_assert(!fake::copy_assignable_d(fake::pack_v<std::unique_ptr<int>>));
}
```

#### `fake::trivially_copy_assignable_d`

- **Brief**: Detects if the wrapped type is trivially copy assignable.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::trivially_copy_assignable_d trivially_copy_assignable_d;
```

- **Example**:

```c++
#include <memory>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::trivially_copy_assignable_d(fake::pack_v<foo>));
    static_assert(!fake::trivially_copy_assignable_d(fake::pack_v<std::shared_ptr<int>>));
}
```

#### `fake::nothrow_copy_assignable_d`

- **Brief**: Detects if the wrapped type is nothrow copy assignable.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::nothrow_copy_assignable_d nothrow_copy_assignable_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::nothrow_copy_assignable_d(fake::pack_v<foo>));
    static_assert(!fake::nothrow_copy_assignable_d(fake::pack_v<std::vector<int>>));
}
```

#### `fake::move_assignable_d`

- **Brief**: Detects if the wrapped type is move assignable.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::move_assignable_d move_assignable_d;
```

- **Example**:

```c++
#include <memory>
#include "detect.h"
struct bar{bar(const bar&) = default; bar(bar&&) = delete;};
int main() {
    static_assert(fake::move_assignable_d(fake::pack_v<std::unique_ptr<int>>));
    static_assert(!fake::move_assignable_d(fake::pack_v<bar>));
}
```

#### `fake::trivially_move_assignable_d`

- **Brief**: Detects if the wrapped type is trivially move assignable.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::trivially_move_assignable_d trivially_move_assignable_d;
```

- **Example**:

```c++
#include <memory>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::trivially_move_assignable_d(fake::pack_v<foo>));
    static_assert(!fake::trivially_move_assignable_d(fake::pack_v<std::shared_ptr<int>>));
}
```

#### `fake::nothrow_move_assignable_d`

- **Brief**: Detects if the wrapped type is nothrow move assignable.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::nothrow_move_assignable_d nothrow_move_assignable_d;
```

- **Example**:

```c++
#include <iostream>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::nothrow_move_assignable_d(fake::pack_v<foo>));
    static_assert(!fake::nothrow_move_assignable_d(fake::pack_v<std::ostream>));
}
```

#### `fake::destructible_d`

- **Brief**: Detects if the wrapped type is destructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::destructible_d destructible_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int bar; double* yoo;};
struct yoo{~yoo() = delete;};
int main() {
    static_assert(fake::destructible_d(fake::pack_v<foo>));
    static_assert(!fake::destructible_d(fake::pack_v<yoo>));
}
```

#### `fake::trivially_destructible_d`

- **Brief**: Detects if the wrapped type is trivially destructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::trivially_destructible_d trivially_destructible_d;
```

- **Example**:

```c++
#include <string>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::trivially_destructible_d(fake::pack_v<foo>));
    static_assert(!fake::trivially_destructible_d(fake::pack_v<std::string>));
}
```

#### `fake::nothrow_destructible_d`

- **Brief**: Detects if the wrapped type is nothrow destructible.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::nothrow_destructible_d nothrow_destructible_d;
```

- **Example**:

```c++
#include <string>
#include "detect.h"
struct tor{~tor() noexcept(false){};};
int main() {
    static_assert(fake::nothrow_destructible_d(fake::pack_v<std::string>));
    static_assert(!fake::nothrow_destructible_d(fake::pack_v<tor>));
}
```

#### `fake::virtual_destructor_d`

- **Brief**: Detects if the wrapped type has a virtual destructor.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::virtual_destructor_d virtual_destructor_d;
```

- **Example**:

```c++
#include <iostream>
#include "detect.h"
struct foo{int bar; double* yoo;};
int main() {
    static_assert(fake::virtual_destructor_d(fake::pack_v<std::istream>));
    static_assert(!fake::virtual_destructor_d(fake::pack_v<foo>));
}
```

#### `fake::swappable_with_d<T>`

- **Brief**: Detects if the wrapped type can be swapped with a value of type `T`.
- **Syntax**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::swappable_with_d<_Arg> swappable_with_d;
```

- **Example**:

```c++
#include <string>
#include "detect.h"
int main() {
    static_assert(fake::swappable_with_d<std::string&>(fake::pack_v<std::string&>));
    static_assert(!fake::swappable_with_d<int>(fake::pack_v<std::string&>));
}
```

#### `fake::swappable_d`

- **Brief**: Detects if the wrapped type is swappable (with itself).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::swappable_d swappable_d;
```

- **Example**:

```c++
#include <string>
#include "detect.h"
struct bar{bar(const bar&) = default; bar(bar&&) = delete;};
int main() {
    static_assert(fake::swappable_d(fake::pack_v<std::string>));
    static_assert(!fake::swappable_d(fake::pack_v<bar>));
}
```

#### `fake::nothrow_swappable_with_d<T>`

- **Brief**: Detects if the wrapped type can be nothrow swapped with a value of type `T`.
- **Syntax**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::nothrow_swappable_with_d<_Arg> nothrow_swappable_with_d;
```

- **Example**:

```c++
#include <string>
#include "detect.h"
int main() {
    static_assert(fake::nothrow_swappable_with_d<std::string&>(fake::pack_v<std::string&>));
    static_assert(!fake::nothrow_swappable_with_d<std::string>(fake::pack_v<std::string>));
}
```

#### `fake::nothrow_swappable_d`

- **Brief**: Detects if the wrapped type is nothrow swappable.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::nothrow_swappable_d nothrow_swappable_d;
```

- **Example**:

```c++
#include <iostream>
#include <memory>
#include "detect.h"
int main() {
    static_assert(fake::nothrow_swappable_d(fake::pack_v<std::shared_ptr<int>>));
    static_assert(!fake::nothrow_swappable_d(fake::pack_v<std::istream>));
}
```

### Type Relationships

#### `fake::base_of_d<Derived>`

- **Brief**: Detects if the wrapped type is a base class of `Derived`.
- **Syntax**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::base_of_d<_Arg> base_of_d;
```

- **Example**:

```c++
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::base_of_d<derive>(fake::pack_v<base>));
    static_assert(!fake::base_of_d<base>(fake::pack_v<derive>));
}
```

#### `fake::convertible_d<From>`

- **Brief**: Detects if the wrapped type is convertible from `From`.
- **Syntax**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::convertible_d<_Arg> convertible_d;
```

- **Example**:

```c++
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::convertible_d<base>(fake::pack_v<derive>));
    static_assert(!fake::convertible_d<derive>(fake::pack_v<base>));
}
```

#### `fake::nothrow_convertible_d<From>`

- **Brief**: Detects if the wrapped type is nothrow convertible from `From`.
- **Syntax**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::nothrow_convertible_d<_Arg> nothrow_convertible_d;
```

- **Example**:

```c++
#include <iostream>
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::nothrow_convertible_d<base>(fake::pack_v<derive>));
    static_assert(!fake::nothrow_convertible_d<std::istream>(fake::pack_v<std::istream>));
}
```

#### `fake::layout_compatible_d<T>`

- **Brief**: Detects if the wrapped type is layout-compatible with `T`.
- **Syntax**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::layout_compatible_d<_Arg> layout_compatible_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int x; double y;};
struct bar{const int x; volatile double y;};
int main() {
    static_assert(fake::layout_compatible_d<foo>(fake::pack_v<bar>));
    static_assert(!fake::layout_compatible_d<foo[2]>(fake::pack_v<bar[2]>));
}
```

#### `fake::pointer_interconvertible_base_of_d<Derived>`

- **Brief**: Detects if the wrapped type is a pointer-interconvertible base of `Derived`.
- **Syntax**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::pointer_interconvertible_base_of_d<_Arg> pointer_interconvertible_base_of_d;
```

- **Example**:

```c++
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::pointer_interconvertible_base_of_d<derive>(fake::pack_v<base>));
    static_assert(!fake::pointer_interconvertible_base_of_d<base>(fake::pack_v<derive>));
}
```

#### `fake::invocable_d<Args...>`

- **Brief**: Detects if the wrapped type (a callable) can be invoked with the given argument types.
- **Syntax**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::invocable_d<_Args...> invocable_d;
```

- **Example**:

```c++
#include <functional>
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::invocable_d<double, double>(fake::pack_v<std::plus<void>>));
    // Also works with detectors as callables:
    static_assert(fake::invocable_d<fake::pack_t<base>>(fake::pack_v<decltype(fake::const_d)>));
    static_assert(!fake::invocable_d<base>(fake::pack_v<decltype(fake::const_d)>));
}
```

#### `fake::invocable_r_d<R, Args...>`

- **Brief**: Detects if the wrapped type can be invoked with the given arguments and the result is convertible to `R`.
- **Syntax**:

```c++
template<typename _R, typename... _Args> inline constexpr fake::detect::traits::invocable_r_d<_R, _Args...> invocable_r_d;
```

- **Example**:

```c++
#include <functional>
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::invocable_r_d<double, double, int>(fake::pack_v<std::plus<void>>));
    static_assert(fake::invocable_r_d<bool, fake::pack_t<base>>(fake::pack_v<decltype(fake::const_d)>));
    static_assert(!fake::invocable_r_d<double, double, base>(fake::pack_v<std::plus<void>>));
    static_assert(!fake::invocable_r_d<bool, base>(fake::pack_v<decltype(fake::const_d)>));
}
```

#### `fake::nothrow_invocable_d<Args...>`

- **Brief**: Detects if the wrapped type can be invoked nothrow with the given argument types.
- **Syntax**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::nothrow_invocable_d<_Args...> nothrow_invocable_d;
```

- **Example**:

```c++
#include <functional>
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::nothrow_invocable_d<double, int>(fake::pack_v<std::plus<void>>));
    static_assert(!fake::nothrow_invocable_d<double, base>(fake::pack_v<std::plus<void>>));
}
```

#### `fake::nothrow_invocable_r_d<R, Args...>`

- **Brief**: Detects if the wrapped type can be invoked nothrow with the given arguments and the result is convertible to `R`.
- **Syntax**:

```c++
template<typename _R, typename... _Args> inline constexpr fake::detect::traits::nothrow_invocable_r_d<_R, _Args...> nothrow_invocable_r_d;
```

- **Example**:

```c++
#include <functional>
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::nothrow_invocable_r_d<double, double, int>(fake::pack_v<std::plus<void>>));
    static_assert(!fake::nothrow_invocable_r_d<double, double, base>(fake::pack_v<std::plus<void>>));
}
```

### Core Language Concepts

#### `fake::same_d<T>`

- **Brief**: Detects if the wrapped type is exactly the same as `T`.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::same_d<_Type> same_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::same_d<float>(fake::pack_v<float>));
    static_assert(!fake::same_d<float>(fake::pack_v<int>));
}
```

#### `fake::derived_from_d<Base>`

- **Brief**: Detects if the wrapped type is derived from `Base`.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::derived_from_d<_Type> derived_from_d;
```

- **Example**:

```c++
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::derived_from_d<base>(fake::pack_v<derive>));
    static_assert(!fake::derived_from_d<derive>(fake::pack_v<base>));
}
```

#### `fake::signed_integral_d`

- **Brief**: Detects if the wrapped type is a signed integral type.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::signed_integral_d signed_integral_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::signed_integral_d(fake::pack_v<int32_t>));
    static_assert(!fake::signed_integral_d(fake::pack_v<uint32_t>));
}
```

#### `fake::unsigned_integral_d`

- **Brief**: Detects if the wrapped type is an unsigned integral type.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::unsigned_integral_d unsigned_integral_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::unsigned_integral_d(fake::pack_v<uint32_t>));
    static_assert(!fake::unsigned_integral_d(fake::pack_v<int32_t>));
}
```

#### `fake::convertible_to_d<To>`

- **Brief**: Detects if the wrapped type is convertible to `To`.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::convertible_to_d<_Type> convertible_to_d;
```

- **Example**:

```c++
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::convertible_to_d<base>(fake::pack_v<derive>));
    static_assert(!fake::convertible_to_d<derive>(fake::pack_v<base>));
}
```

#### `fake::convertible_from_d<From>`

- **Brief**: Detects if the wrapped type can be converted from `From`.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::convertible_from_d<_Type> convertible_from_d;
```

- **Example**:

```c++
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::convertible_from_d<derive>(fake::pack_v<base>));
    static_assert(!fake::convertible_from_d<base>(fake::pack_v<derive>));
}
```

### Comparison Concepts

#### `fake::equality_comparable_d`

- **Brief**: Detects if the wrapped type supports equality comparisons (`==` and `!=`).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::equality_comparable_d equality_comparable_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::equality_comparable_d(fake::pack_v<float>));
    static_assert(!fake::equality_comparable_d(fake::pack_v<foo>));
}
```

#### `fake::equality_comparable_with_d<T>`

- **Brief**: Detects if the wrapped type can be compared for equality with type `T`.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::equality_comparable_with_d<_Type> equality_comparable_with_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::equality_comparable_with_d<float>(fake::pack_v<int>));
    static_assert(!fake::equality_comparable_with_d<foo>(fake::pack_v<int>));
}
```

#### `fake::totally_ordered_d`

- **Brief**: Detects if the wrapped type is totally ordered (supports `<`, `<=`, `>`, `>=`).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::totally_ordered_d totally_ordered_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::totally_ordered_d(fake::pack_v<float>));
    static_assert(!fake::totally_ordered_d(fake::pack_v<foo>));
}
```

#### `fake::totally_ordered_with_d<T>`

- **Brief**: Detects if the wrapped type can be totally ordered with type `T`.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::totally_ordered_with_d<_Type> totally_ordered_with_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::totally_ordered_with_d<int>(fake::pack_v<float>));
    static_assert(!fake::totally_ordered_with_d<int>(fake::pack_v<foo>));
}
```

#### `fake::three_way_comparable_d`

- **Brief**: Detects if the wrapped type supports three-way comparison (`<=>`).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::three_way_comparable_d three_way_comparable_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::three_way_comparable_d(fake::pack_v<float>));
    static_assert(!fake::three_way_comparable_d(fake::pack_v<foo>));
}
```

#### `fake::three_way_comparable_with_d<T>`

- **Brief**: Detects if the wrapped type can be three-way compared with type `T`.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::three_way_comparable_with_d<_Type> three_way_comparable_with_d;
```

- **Example**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::three_way_comparable_with_d<int>(fake::pack_v<float>));
    static_assert(!fake::three_way_comparable_with_d<int>(fake::pack_v<foo>));
}
```

### Object Concepts

#### `fake::movable_d`

- **Brief**: Detects if the wrapped type satisfies the `std::movable` concept.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::movable_d movable_d;
```

- **Example**:

```c++
#include <string>
#include "detect.h"
struct bar{bar(const bar&) = default; bar(bar&&) = delete;};
int main() {
    static_assert(fake::movable_d(fake::pack_v<std::string>));
    static_assert(!fake::movable_d(fake::pack_v<bar>));
}
```

#### `fake::copyable_d`

- **Brief**: Detects if the wrapped type satisfies the `std::copyable` concept.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::copyable_d copyable_d;
```

- **Example**:

```c++
#include <string>
#include <memory>
#include "detect.h"
int main() {
    static_assert(fake::copyable_d(fake::pack_v<std::string>));
    static_assert(!fake::copyable_d(fake::pack_v<std::unique_ptr<int>>));
}
```

#### `fake::semiregular_d`

- **Brief**: Detects if the wrapped type satisfies the `std::semiregular` concept.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::semiregular_d semiregular_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
struct bar{bar(const bar&) = default; bar(bar&&) = delete;};
int main() {
    static_assert(fake::semiregular_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::semiregular_d(fake::pack_v<bar>));
}
```

#### `fake::regular_d`

- **Brief**: Detects if the wrapped type satisfies the `std::regular` concept (semiregular + equality_comparable).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::regular_d regular_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
struct bar{bar(const bar&) = default; bar(bar&&) = delete;};
int main() {
    static_assert(fake::regular_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::regular_d(fake::pack_v<bar>));
}
```

### Range Concepts

#### `fake::range_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::range`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::range_d range_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::range_d(fake::pack_v<int>));
}
```

#### `fake::borrowed_range_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::borrowed_range`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::borrowed_range_d borrowed_range_d;
```

- **Example**:

```c++
#include <ranges>
#include <vector>
#include "detect.h"
int main() {
    std::vector<int> v{1,2,3};
    using sub = decltype(std::ranges::subrange(v.begin(), v.begin()+2));
    static_assert(fake::borrowed_range_d(fake::pack_v<sub>));
    static_assert(!fake::borrowed_range_d(fake::pack_v<int>));
}
```

#### `fake::sized_range_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::sized_range`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::sized_range_d sized_range_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::sized_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::sized_range_d(fake::pack_v<int>));
}
```

#### `fake::views_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::view`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::views_d views_d;
```

- **Example**:

```c++
#include <ranges>
#include <vector>
#include "detect.h"
int main() {
    auto v = std::vector<int>{1,2,3} | std::views::take(2);
    static_assert(fake::views_d(fake::pack_v<decltype(v)>));
    static_assert(!fake::views_d(fake::pack_v<int>));
}
```

#### `fake::input_range_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::input_range`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::input_range_d input_range_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::input_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::input_range_d(fake::pack_v<int>));
}
```

#### `fake::output_range_d<Value>`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::output_range` with value type `Value`.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::output_range_d<_Type> output_range_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::output_range_d<int>(fake::pack_v<std::vector<int>>));
    static_assert(!fake::output_range_d<int>(fake::pack_v<int>));
}
```

#### `fake::forward_range_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::forward_range`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::forward_range_d forward_range_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::forward_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::forward_range_d(fake::pack_v<int>));
}
```

#### `fake::bidirectional_range_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::bidirectional_range`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::bidirectional_range_d bidirectional_range_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::bidirectional_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::bidirectional_range_d(fake::pack_v<int>));
}
```

#### `fake::random_access_range_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::random_access_range`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::random_access_range_d random_access_range_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::random_access_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::random_access_range_d(fake::pack_v<int>));
}
```

#### `fake::contiguous_range_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::contiguous_range`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::contiguous_range_d contiguous_range_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::contiguous_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::contiguous_range_d(fake::pack_v<int>));
}
```

#### `fake::common_range_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::common_range`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::common_range_d common_range_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::common_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::common_range_d(fake::pack_v<int>));
}
```

#### `fake::viewable_range_d`

- **Brief**: Detects if the wrapped type satisfies `std::ranges::viewable_range`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::viewable_range_d viewable_range_d;
```

- **Example**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::viewable_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::viewable_range_d(fake::pack_v<int>));
}
```

### Fake Concepts

#### `fake::hold_d<T>`

- **Brief**: Detects if the wrapped type **hold** the type `T` after removing reference but preserving cv-qualifiers. Typically used to check if a type matches a target without considering references.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::hold_d<_Type> hold_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::hold_d<int>(fake::pack_v<int&>));
    static_assert(fake::hold_d<const int>(fake::pack_v<const int&>));
    static_assert(!fake::hold_d<int>(fake::pack_v<const int&>));
}
```

#### `fake::like_d<T>`

- **Brief**: Detects if the wrapped type is **like** `T`, meaning it is the same after removing cvref-qualifiers, or matches a custom concept provided via a `mezz` wrapper.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::like_d<_Type> like_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::like_d<int>(fake::pack_v<const int&>));
    static_assert(!fake::like_d<int>(fake::pack_v<const double&>));
}
```

#### `fake::meets_d<Ts...>`

- **Brief**: Detects if the wrapped type is exactly one of the types in the pack `Ts...`.
- **Syntax**:

```c++
template<typename... _Type> inline constexpr fake::detect::traits::meets_d<_Type...> meets_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::meets_d<int&, double>(fake::pack_v<int&>));
    static_assert(!fake::meets_d<int&, double>(fake::pack_v<int>));
}
```

#### `fake::holds_d<Ts...>`

- **Brief**: Detects if the wrapped type **hold** one of the types in `Ts...` (using `fake::hold` concept).
- **Syntax**:

```c++
template<typename... _Type> inline constexpr fake::detect::traits::holds_d<_Type...> holds_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::holds_d<const int, double>(fake::pack_v<const int&>));
    static_assert(!fake::holds_d<const int, double>(fake::pack_v<int&>));
}
```

#### `fake::likes_d<Ts...>`

- **Brief**: Detects if the wrapped type is **like** one of the types in `Ts...` (using `fake::like` concept).
- **Syntax**:

```c++
template<typename... _Type> inline constexpr fake::detect::traits::likes_d<_Type...> likes_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::likes_d<int, double>(fake::pack_v<const double&>));
    static_assert(!fake::likes_d<int, double>(fake::pack_v<const float&>));
}
```

#### `fake::trait_d<Template>`

- **Brief**: Detects if the wrapped type is an instantiation of the template `Template`.
- **Syntax**:

```c++
template<template<typename...> typename _Tamplate> inline constexpr fake::detect::traits::trait_d<_Tamplate> trait_d;
```

- **Example**:

```c++
#include <optional>
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::trait_d<std::vector>(fake::pack_v<std::vector<double>>));
    static_assert(!fake::trait_d<std::vector>(fake::pack_v<std::optional<double>>));
}
```

## Type Transformations (Shift Operators)

Shift operators (`<<=`) apply a type transformation to a wrapper before passing it to a detector. They are defined as `fake::shift_c` objects.

### Custom Transformation

#### `fake::just_p<lambda>`

- **Brief**: Creates a transformation from a lambda that may accept additional arguments. If the lambda cannot be invoked with the given arguments, it attempts to use it as a unary functor (e.g., `[](auto _pack){ return ...; }`).
- **Syntax**:

```c++
template<auto _lambda> inline constexpr fake::detect::traits::just_p<_lambda> just_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static constexpr fake::shift_c auto storage = fake::just_p<
        [](fake::pazz_c auto _e){
            if constexpr(fake::lvalue_reference_d(_e))
                return (fake::remove_reference_p <<= fake::add_const_p <<= fake::add_lvalue_reference_p)(_e);
            else
                return fake::remove_cvref_p(_e);
        }
    >;
    
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<int>)>, int>);
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<const int>)>, int>);
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<int&>)>, const int&>);
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<const int&>)>, const int&>);
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<int&&>)>, int>);
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<const int&&>)>, int>);
}
```

### Container Member Type Extraction

#### `fake::key_p`

- **Brief**: Transforms the wrapped type to its `key_type` (for associative containers).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::key_p key_p;
```

- **Example**:

```c++
#include <unordered_map>
#include "detect.h"
int main() {
    static_assert((fake::key_p <<= fake::same_d<int>)(fake::pack_v<std::unordered_map<int, double>>));
    static_assert(!(fake::key_p <<= fake::same_d<int>)(fake::pack_v<int>));
}
```

#### `fake::mapped_p`

- **Brief**: Transforms the wrapped type to its `mapped_type`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::mapped_p mapped_p;
```

- **Example**:

```c++
#include <unordered_map>
#include "detect.h"
int main() {
    static_assert((fake::mapped_p <<= fake::same_d<double>)(fake::pack_v<std::unordered_map<int, double>>));
    static_assert(!(fake::mapped_p <<= fake::same_d<int>)(fake::pack_v<int>));
}
```

#### `fake::first_p`

- **Brief**: Transforms a `std::pair` to its `first_type`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::first_p first_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::first_p <<= fake::same_d<int>)(fake::pack_v<std::pair<int, double>>));
    static_assert(!(fake::first_p <<= fake::same_d<double>)(fake::pack_v<std::pair<int, double>>));
}
```

#### `fake::second_p`

- **Brief**: Transforms a `std::pair` to its `second_type`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::second_p second_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::second_p <<= fake::same_d<double>)(fake::pack_v<std::pair<int, double>>));
    static_assert(!(fake::second_p <<= fake::same_d<double>)(fake::pack_v<double>));
}
```

#### `fake::element_p`

- **Brief**: Transforms a smart pointer-like type to its `element_type`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::element_p element_p;
```

- **Example**:

```c++
#include <memory>
#include "detect.h"
int main() {
    static_assert((fake::element_p <<= fake::same_d<int>)(fake::pack_v<std::unique_ptr<int>>));
    static_assert(!(fake::element_p <<= fake::same_d<double>)(fake::pack_v<double>));
}
```

#### `fake::type_p`

- **Brief**: Transforms a type with a nested `type` alias (e.g., `std::remove_const<const int>`) to that `type`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::type_p type_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::type_p <<= fake::same_d<int>)(fake::pack_v<std::remove_const<const int>>));
    static_assert(!(fake::type_p <<= fake::same_d<int>)(fake::pack_v<int>));
}
```

#### `fake::index_p<N>`

- **Brief**: Transforms a tuple-like type to its element at index `N`.
- **Syntax**:

```c++
template<std::size_t _index> inline constexpr fake::detect::traits::index_p<_index> index_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::index_p<2> <<= fake::same_d<double>)(fake::pack_v<std::tuple<int, float, double, char>>));
    static_assert(!(fake::index_p<2> <<= fake::same_d<int>)(fake::pack_v<int>));
}
```

#### `fake::mimic_p<Target>`

- **Brief**: Transforms a type (like `std::variant<A,B>`) to an instantiation of `Target` with the same template arguments (e.g., `std::tuple<A,B>`).
- **Syntax**:

```c++
template<template<typename...> typename _Target> inline constexpr fake::detect::traits::mimic_p<_Target> mimic_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::mimic_p<std::tuple> <<= fake::same_d<std::tuple<int, float>>)(fake::pack_v<std::variant<int, float>>));
    static_assert(!(fake::mimic_p<std::tuple> <<= fake::same_d<int>)(fake::pack_v<int>));
}
```

### CV Qualifier Modifications

#### `fake::remove_cv_p`

- **Brief**: Removes top-level `const` and `volatile` qualifiers.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::remove_cv_p remove_cv_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_cv_p <<= fake::same_d<int>)(fake::pack_v<const volatile int>));
    static_assert(!(fake::remove_cv_p <<= fake::same_d<double>)(fake::pack_v<int>));
}
```

#### `fake::remove_const_p`

- **Brief**: Removes top-level `const`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::remove_const_p remove_const_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_const_p <<= fake::same_d<volatile int>)(fake::pack_v<const volatile int>));
    static_assert(!(fake::remove_const_p <<= fake::same_d<int>)(fake::pack_v<volatile int>));
}
```

#### `fake::remove_volatile_p`

- **Brief**: Removes top-level `volatile`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::remove_volatile_p remove_volatile_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_volatile_p <<= fake::same_d<const int>)(fake::pack_v<const volatile int>));
    static_assert(!(fake::remove_volatile_p <<= fake::same_d<int>)(fake::pack_v<const int>));
}
```

#### `fake::add_cv_p`

- **Brief**: Adds top-level `const` and `volatile`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::add_cv_p add_cv_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::add_cv_p <<= fake::same_d<const volatile int>)(fake::pack_v<int>));
    static_assert(!(fake::add_cv_p <<= fake::same_d<int>)(fake::pack_v<const volatile int>));
}
```

#### `fake::add_const_p`

- **Brief**: Adds top-level `const`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::add_const_p add_const_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::add_const_p <<= fake::same_d<const int>)(fake::pack_v<int>));
    static_assert(!(fake::add_const_p <<= fake::same_d<int>)(fake::pack_v<const int>));
}
```

#### `fake::add_volatile_p`

- **Brief**: Adds top-level `volatile`.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::add_volatile_p add_volatile_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::add_volatile_p <<= fake::same_d<volatile int>)(fake::pack_v<int>));
    static_assert(!(fake::add_volatile_p <<= fake::same_d<int>)(fake::pack_v<volatile int>));
}
```

### Reference Modifications

#### `fake::remove_reference_p`

- **Brief**: Removes any reference (lvalue or rvalue).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::remove_reference_p remove_reference_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_reference_p <<= fake::same_d<int>)(fake::pack_v<int&>));
    static_assert((fake::remove_reference_p <<= fake::same_d<int>)(fake::pack_v<int&&>));
    static_assert((fake::remove_reference_p <<= fake::same_d<const int>)(fake::pack_v<const int&>));
    static_assert((fake::remove_reference_p <<= fake::same_d<const int>)(fake::pack_v<const int&&>));
    static_assert(!(fake::remove_reference_p <<= fake::same_d<const int>)(fake::pack_v<int>));
}
```

#### `fake::remove_lvalue_reference_p`

- **Brief**: Removes lvalue reference only; leaves rvalue references unchanged.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::remove_lvalue_reference_p remove_lvalue_reference_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_lvalue_reference_p <<= fake::same_d<int>)(fake::pack_v<int&>));
    static_assert((fake::remove_lvalue_reference_p <<= fake::same_d<int&&>)(fake::pack_v<int&&>));
    static_assert((fake::remove_lvalue_reference_p <<= fake::same_d<const int>)(fake::pack_v<const int&>));
    static_assert((fake::remove_lvalue_reference_p <<= fake::same_d<const int&&>)(fake::pack_v<const int&&>));
    static_assert(!(fake::remove_lvalue_reference_p <<= fake::same_d<const int>)(fake::pack_v<int>));
}
```

#### `fake::remove_rvalue_reference_p`

- **Brief**: Removes rvalue reference only; leaves lvalue references unchanged.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::remove_rvalue_reference_p remove_rvalue_reference_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_rvalue_reference_p <<= fake::same_d<int&>)(fake::pack_v<int&>));
    static_assert((fake::remove_rvalue_reference_p <<= fake::same_d<int>)(fake::pack_v<int&&>));
    static_assert((fake::remove_rvalue_reference_p <<= fake::same_d<const int&>)(fake::pack_v<const int&>));
    static_assert((fake::remove_rvalue_reference_p <<= fake::same_d<const int>)(fake::pack_v<const int&&>));
    static_assert(!(fake::remove_rvalue_reference_p <<= fake::same_d<const int>)(fake::pack_v<int>));
}
```

#### `fake::add_lvalue_reference_p`

- **Brief**: Adds an lvalue reference (if not already a lvalue reference).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::add_lvalue_reference_p add_lvalue_reference_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::add_lvalue_reference_p <<= fake::same_d<int&>)(fake::pack_v<int>));
    static_assert((fake::add_lvalue_reference_p <<= fake::same_d<int&>)(fake::pack_v<int&&>));
    static_assert((fake::add_lvalue_reference_p <<= fake::same_d<const int&>)(fake::pack_v<const int>));
    static_assert((fake::add_lvalue_reference_p <<= fake::same_d<const int&>)(fake::pack_v<const int&&>));
    static_assert(!(fake::add_lvalue_reference_p <<= fake::same_d<const int>)(fake::pack_v<int>));
}
```

#### `fake::add_rvalue_reference_p`

- **Brief**: Adds an rvalue reference (if not already a reference).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::add_rvalue_reference_p add_rvalue_reference_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::add_rvalue_reference_p <<= fake::same_d<int&&>)(fake::pack_v<int>));
    static_assert((fake::add_rvalue_reference_p <<= fake::same_d<int&>)(fake::pack_v<int&>));
    static_assert((fake::add_rvalue_reference_p <<= fake::same_d<const int&&>)(fake::pack_v<const int>));
    static_assert((fake::add_rvalue_reference_p <<= fake::same_d<const int&>)(fake::pack_v<const int&>));
    static_assert(!(fake::add_rvalue_reference_p <<= fake::same_d<const int>)(fake::pack_v<int>));
}
```

### Other Type Transformations

#### `fake::make_signed_p`

- **Brief**: Transforms an integral type to its signed counterpart.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::make_signed_p make_signed_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::make_signed_p <<= fake::same_d<int32_t>)(fake::pack_v<uint32_t>));
    static_assert(!(fake::make_signed_p <<= fake::same_d<uint32_t>)(fake::pack_v<int32_t>));
}
```

#### `fake::make_unsigned_p`

- **Brief**: Transforms an integral type to its unsigned counterpart.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::make_unsigned_p make_unsigned_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::make_unsigned_p <<= fake::same_d<uint32_t>)(fake::pack_v<int32_t>));
    static_assert(!(fake::make_unsigned_p <<= fake::same_d<int32_t>)(fake::pack_v<uint32_t>));
}
```

#### `fake::remove_extent_p`

- **Brief**: Removes one extent from an array type (e.g., `int[][N]` -> `int[N]`).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::remove_extent_p remove_extent_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_extent_p <<= fake::same_d<int[514]>)(fake::pack_v<int[114][514]>));
    static_assert(!(fake::remove_extent_p <<= fake::same_d<int>)(fake::pack_v<double[514]>));
}
```

#### `fake::remove_all_extents_p`

- **Brief**: Removes all extents from an array type (e.g., `int[][N]` -> `int`).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::remove_all_extents_p remove_all_extents_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_all_extents_p <<= fake::same_d<int>)(fake::pack_v<int[114][514]>));
    static_assert(!(fake::remove_all_extents_p <<= fake::same_d<int>)(fake::pack_v<double[514]>));
}
```

#### `fake::remove_pointer_p`

- **Brief**: Removes a pointer level.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::remove_pointer_p remove_pointer_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_pointer_p <<= fake::same_d<int>)(fake::pack_v<int>)); // no pointer -> unchanged
    static_assert((fake::remove_pointer_p <<= fake::same_d<const int>)(fake::pack_v<const int*>));
    static_assert((fake::remove_pointer_p <<= fake::same_d<int*>)(fake::pack_v<int**>));
    static_assert(!(fake::remove_pointer_p <<= fake::same_d<int*>)(fake::pack_v<const int*>));
}
```

#### `fake::add_pointer_p`

- **Brief**: Adds a pointer level.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::add_pointer_p add_pointer_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::add_pointer_p <<= fake::same_d<int*>)(fake::pack_v<int>));
    static_assert((fake::add_pointer_p <<= fake::same_d<const int**>)(fake::pack_v<const int*>));
    static_assert(!(fake::add_pointer_p <<= fake::same_d<const int*>)(fake::pack_v<int>));
}
```

#### `fake::decay_p`

- **Brief**: Applies the standard decay transformation (array-to-pointer, function-to-pointer, cv removal).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::decay_p decay_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::decay_p <<= fake::same_d<int*>)(fake::pack_v<int(&)[3]>));
    static_assert(!(fake::decay_p <<= fake::same_d<int&>)(fake::pack_v<int&&>));
}
```

#### `fake::bare_p`

- **Brief**: Strips all pointers and cvref qualifiers recursively to get the underlying type.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::bare_p bare_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::bare_p <<= fake::same_d<int[3]>)(fake::pack_v<int(*const*&)[3]>));
    static_assert(!(fake::bare_p <<= fake::same_d<int>)(fake::pack_v<double*&>));
}
```

#### `fake::remove_cvref_p`

- **Brief**: Removes cv-qualifiers and reference simultaneously.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::remove_cvref_p remove_cvref_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_cvref_p <<= fake::same_d<int>)(fake::pack_v<const volatile int&&>));
    static_assert(!(fake::remove_cvref_p <<= fake::same_d<double>)(fake::pack_v<const volatile int&&>));
}
```

#### `fake::common_type_p<Args...>`

- **Brief**: Transforms the wrapped type to the common type of the wrapped type and `Args...` (using `std::common_type`).
- **Syntax**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::common_type_p<_Args...> common_type_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::common_type_p<int> <<= fake::same_d<double>)(fake::pack_v<double>));
    static_assert(!(fake::common_type_p<int> <<= fake::same_d<int>)(fake::pack_v<double>));
}
```

#### `fake::common_reference_p<Args...>`

- **Brief**: Transforms the wrapped type to the common reference type of the wrapped type and `Args...`.
- **Syntax**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::common_reference_p<_Args...> common_reference_p;
```

- **Example**:

```c++
#include "detect.h"
struct base{}; struct derive : base{};
int main() {
    static_assert((fake::common_reference_p<base> <<= fake::same_d<base>)(fake::pack_v<derive>));
    static_assert(!(fake::common_reference_p<derive> <<= fake::same_d<derive>)(fake::pack_v<base>));
}
```

#### `fake::underlying_type_p`

- **Brief**: Transforms an enumeration to its underlying type.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::underlying_type_p underlying_type_p;
```

- **Example**:

```c++
#include "detect.h"
enum struct enum_e : int16_t{foo, bar};
int main() {
    static_assert((fake::underlying_type_p <<= fake::same_d<int16_t>)(fake::pack_v<enum_e>));
    static_assert(!(fake::underlying_type_p <<= fake::same_d<uint16_t>)(fake::pack_v<enum_e>));
}
```

#### `fake::invoke_result_p<Args...>`

- **Brief**: Transforms a callable type to its return type when invoked with `Args...`.
- **Syntax**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::invoke_result_p<_Args...> invoke_result_p;
```

- **Example**:

```c++
#include "detect.h"
struct abstract{virtual int foo(int, double) = 0;};
int main() {
    static_assert((fake::invoke_result_p<> <<= fake::same_d<int>)(fake::pack_v<decltype(main)>));
    static_assert((fake::invoke_result_p<abstract, int, int> <<= fake::same_d<int>)(fake::pack_v<decltype(&abstract::foo)>));
    static_assert(!(fake::invoke_result_p<int, int> <<= fake::same_d<int>)(fake::pack_v<decltype(&abstract::foo)>));
}
```

#### `fake::pattern_p`

- **Brief**: Transforms an instantiated type into its pattern holder (e.g., `std::tuple<int>` -> `fake::generic<std::tuple>`).
- **Syntax**:

```c++
inline constexpr fake::detect::traits::pattern_p pattern_p;
```

- **Example**:

```c++
#include <vector>
#include "view.h"
#include "detect.h"
int main() {
    static_assert((fake::pattern_p <<= fake::same_d<int>)(fake::pack_v<int>));
    static_assert((fake::pattern_p <<= fake::same_d<fake::generic<std::vector>>)(fake::pack_v<std::vector<int>>));
    static_assert((fake::pattern_p <<= fake::same_d<fake::array_like<std::array>>)(fake::pack_v<std::array<int, 114>>));
    static_assert((fake::pattern_p <<= fake::same_d<fake::view_like<fake::view>>)(fake::pack_v<fake::view_t<"yaju">>));
    static_assert(!(fake::pattern_p <<= fake::same_d<int>)(fake::pack_v<double>));
}
```

#### `fake::pack_p`

- **Brief**: Transforms a type into a `fake::pack_t` wrapper.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::pack_p pack_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::pack_p <<= fake::same_d<fake::pack_t<int>>)(fake::pack_v<int>));
    static_assert(!(fake::pack_p <<= fake::same_d<fake::pack_t<int>>)(fake::pack_v<double>));
}
```

#### `fake::pazz_p`

- **Brief**: Transforms a type into a `fake::pazz_t` wrapper.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::pazz_p pazz_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::pazz_p <<= fake::same_d<fake::pazz_t<double>>)(fake::pack_v<double>));
    static_assert(!(fake::pazz_p <<= fake::same_d<fake::pazz_t<double>>)(fake::pack_v<int>));
}
```

#### `fake::take_p`

- **Brief**: Extracts the underlying type from a `pack_c` or `pazz_c` wrapper.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::take_p take_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::take_p <<= fake::same_d<double>)(fake::pack_v<fake::pack_t<double>>));
    static_assert((fake::take_p <<= fake::same_d<double>)(fake::pack_v<fake::pazz_t<double>>));
    static_assert(!(fake::take_p <<= fake::same_d<double>)(fake::pack_v<double>));
}
```

#### `fake::try_take_p`

- **Brief**: Extracts the underlying type from a wrapper if it is a `pack_c` or `pazz_c`; otherwise leaves the type unchanged.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::try_take_p try_take_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::try_take_p <<= fake::same_d<double>)(fake::pack_v<fake::pack_t<double>>));
    static_assert((fake::try_take_p <<= fake::same_d<double>)(fake::pack_v<fake::pazz_t<double>>));
    static_assert((fake::try_take_p <<= fake::same_d<double>)(fake::pack_v<double>));
    static_assert(!(fake::try_take_p <<= fake::same_d<double>)(fake::pack_v<int>));
}
```

#### `fake::take_all_p`

- **Brief**: Recursively extracts the underlying type from nested wrappers until a non-wrapper type is reached.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::take_all_p take_all_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::take_all_p <<= fake::same_d<int>)(fake::pack_v<fake::pack_t<int>>));
    static_assert((fake::take_all_p <<= fake::same_d<int>)(fake::pack_v<fake::pazz_t<int>>));
    static_assert((fake::take_all_p <<= fake::same_d<int>)(fake::pack_v<fake::pack_t<fake::pazz_t<int>>>));
    static_assert((fake::take_all_p <<= fake::same_d<int>)(fake::pack_v<int>));
    static_assert(!(fake::take_all_p <<= fake::same_d<int>)(fake::pack_v<double>));
}
```

#### `fake::apply_p<Target, Appends...>`

- **Brief**: Transform a type `T` into `Target<T, Appends...>`.
- **Syntax**:

```c++
template<template<typename...> typename _Target, typename... _Appends>
inline constexpr fake::detect::traits::apply_p<_Target, _Appends...> apply_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    using result = fake::take_t<fake::apply_p<std::variant, char, double>(fake::pazz_v<int>)>;
    static_assert(std::same_as<result, std::variant<int, char, double>>);
}
```

#### `fake::keep_p`

- **Brief**: Keep a type unchanged.
- **Syntax**:

```c++
inline constexpr fake::detect::traits::keep_p keep_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(std::same_as<fake::take_t<fake::keep_p(fake::pazz_v<const int* const&>)>, const int* const&>);
}
```

#### `fake::replace_p<Just>`

- **Brief**: Transform a type into `Just`.
- **Syntax**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::replace_p<_Type> replace_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static_assert(std::same_as<fake::take_t<fake::replace_p<double>(fake::pazz_v<const int* const&>)>, double>);
}
```

#### `fake::conditional_p<Condition, True, False>`

- **Brief**: If a type satisfy `Condition`, transform the type via `True`; else, transform it via `False`.
- **Syntax**:

```c++
template<fake::creed_c auto _condition, fake::shift_c auto _true, fake::shift_c auto _false>
inline constexpr fake::detect::traits::conditional_p<_condition, _true, _false> conditional_p;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static constexpr fake::shift_c auto storage = fake::conditional_p<
        fake::lvalue_reference_d,
        fake::remove_reference_p <<= fake::add_const_p <<= fake::add_lvalue_reference_p,
        fake::remove_cvref_p
    >;
    
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<int>)>, int>);
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<const int>)>, int>);
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<int&>)>, const int&>);
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<const int&>)>, const int&>);
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<int&&>)>, int>);
    static_assert(std::same_as<fake::take_t<storage(fake::pazz_v<const int&&>)>, int>);
}
```

## Composition Operators

Detectors can be combined using logical operators to form new detectors. Shift operators can be combined with detectors using `<<=`.

### Logical Operators

- `operator&&` : conjunction
- `operator||` : disjunction
- `operator!`: negation
- `operator==` : equivalence
- `operator!=` and `operator^` : nonequivalence

- **Example**:

```c++
#include "detect.h"
int main() {
    constexpr fake::pazz_c auto e = fake::pazz_v<const float&>;
    auto op = fake::remove_reference_p <<= fake::remove_const_p <<= fake::same_d<float> && !fake::reference_d;
    static_assert(op(e));
}
```

- **Example of equivalence**:

```c++
#include "detect.h"
int main() {
    constexpr fake::pazz_c auto e = fake::pazz_v<const float&>;
    auto no_lref_no_const = fake::lvalue_reference_d == (fake::remove_reference_p <<= fake::const_d);
    static_assert(no_lref_no_const(e));
    static_assert(!no_lref_no_const(fake::pazz_v<const int>));
}
```

### Composition with Shift

- `<<=` : applies a shift transformation to the wrapped type before passing to a detector.

- **Example**:

```c++
#include "detect.h"
int main() {
    constexpr fake::pazz_c auto e = fake::pazz_v<const float&>;
    auto ep = fake::remove_const_p <<= fake::remove_reference_p <<= fake::same_d<float> || fake::reference_d;
    static_assert(!ep(e)); // result is false
}
```

## Custom Detectors

### `fake::just_d<lambda>`

- **Brief**: Creates a detector from a lambda that may accept additional arguments. If the lambda cannot be invoked with the given arguments, it attempts to use it as a template constraint (e.g., `[]<some_c>{}`).
- **Syntax**:

```c++
template<auto _lambda> inline constexpr fake::detect::op::just<_lambda> just_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static constexpr fake::creed_c auto array_d = fake::just_d<[]<fake::array_c>(){}>;
    static_assert(array_d(fake::pack_v<int[114]>));
    static_assert(array_d(fake::pack_v<int[114]>, 1919.0f));
    static_assert(!array_d(fake::pack_v<int>));
    static_assert(!array_d(fake::pack_v<int>, 1919.0f));
}
```

### `fake::must_d<lambda>`

- **Brief**: Creates a detector from a lambda that must be invocable with the given arguments (including the wrapped type). If the lambda cannot be invoked, the detector is ill-formed.
- **Syntax**:

```c++
template<auto _lambda> inline constexpr fake::detect::op::must<_lambda> must_d;
```

- **Example**:

```c++
#include "detect.h"
int main() {
    static constexpr fake::creed_c auto array_d = fake::must_d<
    [](auto _p, auto &&..._args){
        // a 'creed_c' expression may combine multiple detectors, therefore '_args' should be accepted even unused
        // '_args' should be ignored here if unused, but 'static_assert(...)' shown for args type illustration
        static_assert((fake::like<decltype(_args), float> && ...));
        return fake::array_c<fake::take_t<decltype(_p){}>>;
    }
    >;
    static_assert(array_d(fake::pack_v<int[114]>));
    static_assert(array_d(fake::pack_v<int[114]>, 1919.0f));
    static_assert(!array_d(fake::pack_v<int>));
    static_assert(!array_d(fake::pack_v<int>, 1919.0f));
}
```

## Concept Adaptor

### `fake::detect::as<detector>`

- **Brief**: Creates a concept from a detector (`fake::creed_c`).
- **Syntax**:

```c++
template<typename _Type, auto _op>
concept as = requires{requires _op(fake::pazz_v<_Type>);} || requires{requires _op(fake::pack_v<_Type>);};
```

- **Example**:

```c++
#include "detect.h"
int main() {
    auto no_lref_no_const = fake::lvalue_reference_d == (fake::remove_reference_p <<= fake::const_d);
    int x = 114514;
    [](fake::detect::as<no_lref_no_const> auto&&){}(std::as_const(x));
    [](fake::detect::as<no_lref_no_const> auto&&){}(std::move(x));
    // [](fake::detect::as<no_lref_no_const> auto&&){}(x); // ill-formed
}
```

---

**Note**: Many examples above rely on headers like `<vector>`, `<memory>`, `<ranges>`, etc. Include them as needed. The `fake` headers (`detect.h`, `view.h`, `flat.h`, `tuple.h`) should be included where the corresponding types are used.
