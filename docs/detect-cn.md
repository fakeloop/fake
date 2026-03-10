# `detect.h`

- 使用编译期谓词和组合器进行类型检测和转换的框架。

## 概述

`detect.h` 提供了一系列工具，用于创建和组合 **探测器（detectors）** —— 这些是操作于类型包装器（`fake::pack_c` 或 `fake::pazz_c`）上的编译期谓词。
探测器可以通过逻辑运算符（`&&`、`||`、`!`、`==`、`!=`、`^`）和变换运算符（`<<=`）进行组合，以构建复杂的类型查询。
类型转换通过 **shift** 运算符实现，这些运算符在将类型传递给探测器之前对包装的类型进行修改。

## 核心概念

### `fake::creed_c`

- **简介**: 一个概念，用于标识一个类型为探测器（可应用于类型包装器的谓词）。
- **语法**:

```c++
template<typename _Type>
concept creed_c = std::derived_from<std::remove_cvref_t<_Type>, fake::detect::op::creed>;
```

### `fake::shift_c`

- **简介**: 一个概念，用于标识一个类型为变换（shift）操作，可应用于类型包装器。
- **语法**:

```c++
template<typename _Type>
concept shift_c = std::derived_from<std::remove_cvref_t<_Type>, fake::detect::op::shift>;
```

### `fake::pack_c` 和 `fake::pazz_c`

- **简介**: 类型包装器，分别持有一个类型（`fake::pack_t`）或一个类型包的值（`fake::pazz_t`）。它们是探测器的输入。
- **语法**（定义于 `is_valid.h`）:

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

- **简介**: 从 `pack_c` 或 `pazz_c` 包装器中提取底层类型。
- **语法**（定义于 `is_valid.h`）:

```c++
template<auto _arg>
using take_t = typename take<_arg>::type;
```

## 预定义探测器

### 主要类型类别

#### `fake::void_d`

- **简介**: 检测包装的类型是否为 `void`。
- **语法**:

```c++
inline constexpr fake::detect::traits::void_d void_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::void_d(fake::pack_v<void>));
    static_assert(!fake::void_d(fake::pack_v<int>));
}
```

#### `fake::null_pointer_d`

- **简介**: 检测包装的类型是否为 `std::nullptr_t`。
- **语法**:

```c++
inline constexpr fake::detect::traits::null_pointer_d null_pointer_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::null_pointer_d(fake::pack_v<std::nullptr_t>));
    static_assert(!fake::null_pointer_d(fake::pack_v<int>));
}
```

#### `fake::integral_d`

- **简介**: 检测包装的类型是否为整型。
- **语法**:

```c++
inline constexpr fake::detect::traits::integral_d integral_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::integral_d(fake::pack_v<int>));
    static_assert(!fake::integral_d(fake::pack_v<float>));
}
```

#### `fake::floating_point_d`

- **简介**: 检测包装的类型是否为浮点型。
- **语法**:

```c++
inline constexpr fake::detect::traits::floating_point_d floating_point_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::floating_point_d(fake::pack_v<double>));
    static_assert(!fake::floating_point_d(fake::pack_v<int>));
}
```

#### `fake::array_d`

- **简介**: 检测包装的类型是否为数组类型（有界或无界）。
- **语法**:

```c++
inline constexpr fake::detect::traits::array_d array_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::array_d(fake::pack_v<int[]>));
    static_assert(!fake::array_d(fake::pack_v<int>));
}
```

#### `fake::enum_d`

- **简介**: 检测包装的类型是否为枚举。
- **语法**:

```c++
inline constexpr fake::detect::traits::enum_d enum_d;
```

- **示例**:

```c++
#include "detect.h"
enum struct enum_e{foo, bar};
int main() {
    static_assert(fake::enum_d(fake::pack_v<enum_e>));
    static_assert(!fake::enum_d(fake::pack_v<int>));
}
```

#### `fake::union_d`

- **简介**: 检测包装的类型是否为联合体。
- **语法**:

```c++
inline constexpr fake::detect::traits::union_d union_d;
```

- **示例**:

```c++
#include "detect.h"
union un{int foo; float bar;};
int main() {
    static_assert(fake::union_d(fake::pack_v<un>));
    static_assert(!fake::union_d(fake::pack_v<int>));
}
```

#### `fake::class_d`

- **简介**: 检测包装的类型是否为类（或结构体）。
- **语法**:

```c++
inline constexpr fake::detect::traits::class_d class_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{};
int main() {
    static_assert(fake::class_d(fake::pack_v<foo>));
    static_assert(!fake::class_d(fake::pack_v<int>));
}
```

#### `fake::function_d`

- **简介**: 检测包装的类型是否为函数类型。
- **语法**:

```c++
inline constexpr fake::detect::traits::function_d function_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::function_d(fake::pack_v<decltype(main)>));
    static_assert(!fake::function_d(fake::pack_v<int>));
}
```

#### `fake::pointer_d`

- **简介**: 检测包装的类型是否为指针。
- **语法**:

```c++
inline constexpr fake::detect::traits::pointer_d pointer_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::pointer_d(fake::pack_v<int*>));
    static_assert(!fake::pointer_d(fake::pack_v<int>));
}
```

#### `fake::lvalue_reference_d`

- **简介**: 检测包装的类型是否为左值引用。
- **语法**:

```c++
inline constexpr fake::detect::traits::lvalue_reference_d lvalue_reference_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::lvalue_reference_d(fake::pack_v<int&>));
    static_assert(!fake::lvalue_reference_d(fake::pack_v<int&&>));
}
```

#### `fake::rvalue_reference_d`

- **简介**: 检测包装的类型是否为右值引用。
- **语法**:

```c++
inline constexpr fake::detect::traits::rvalue_reference_d rvalue_reference_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::rvalue_reference_d(fake::pack_v<int&&>));
    static_assert(!fake::rvalue_reference_d(fake::pack_v<int&>));
}
```

#### `fake::member_object_pointer_d`

- **简介**: 检测包装的类型是否为指向数据成员的指针。
- **语法**:

```c++
inline constexpr fake::detect::traits::member_object_pointer_d member_object_pointer_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{int bar;};
int main() {
    static_assert(fake::member_object_pointer_d(fake::pack_v<decltype(&foo::bar)>));
    static_assert(!fake::member_object_pointer_d(fake::pack_v<int*>));
}
```

#### `fake::member_function_pointer_d`

- **简介**: 检测包装的类型是否为指向成员函数的指针。
- **语法**:

```c++
inline constexpr fake::detect::traits::member_function_pointer_d member_function_pointer_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{int bar; int fun(){return 114514;};};
int main() {
    static_assert(fake::member_function_pointer_d(fake::pack_v<decltype(&foo::fun)>));
    static_assert(!fake::member_function_pointer_d(fake::pack_v<decltype(&foo::bar)>));
}
```

### Fake 类型类别

这些探测器用于识别 `fake` 库中定义的类型。

#### `fake::null_d`

- **简介**: 检测包装的类型是否为 `fake::null_t`。
- **语法**:

```c++
inline constexpr fake::creed_c auto null_d = fake::just_d<[]<fake::like<fake::null_t>>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::null_d(fake::pack_v<fake::null_t>));
    static_assert(!fake::null_d(fake::pack_v<int>));
}
```

#### `fake::view_d`

- **简介**: 检测包装的类型是否为 `fake::view` 的实例。
- **语法**:

```c++
inline constexpr fake::creed_c auto view_d = fake::just_d<[]<fake::view_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
#include "view.h"
int main() {
    static_assert(fake::view_d(fake::pack_v<fake::view_t<"foo">>));
    static_assert(!fake::view_d(fake::pack_v<int>));
}
```

#### `fake::flat_d`

- **简介**: 检测包装的类型是否为 `fake::flat` 的实例。
- **语法**:

```c++
inline constexpr fake::creed_c auto flat_d = fake::just_d<[]<fake::flat_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
#include "flat.h"
int main() {
    static_assert(fake::flat_d(fake::pack_v<fake::flat<int, float, double>>));
    static_assert(!fake::flat_d(fake::pack_v<int>));
}
```

#### `fake::query_d`

- **简介**: 检测包装的类型是否为 `fake::query` 的实例。
- **语法**:

```c++
inline constexpr fake::creed_c auto query_d = fake::just_d<[]<fake::query_c>(){}>;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为 `fake::pack_t`（type package）。
- **语法**:

```c++
inline constexpr fake::creed_c auto pack_d = fake::just_d<[]<fake::pack_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
#include "is_valid.h"
int main() {
    static_assert(fake::pack_d(fake::pack_v<fake::pack_t<int>>));
    static_assert(!fake::pack_d(fake::pack_v<int>));
}
```

#### `fake::pazz_d`

- **简介**: 检测包装的类型是否为 `fake::pazz_t`（package mezzanine）。
- **语法**:

```c++
inline constexpr fake::creed_c auto pazz_d = fake::just_d<[]<fake::pazz_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::pazz_d(fake::pack_v<fake::pazz_t<int>>));
    static_assert(!fake::pazz_d(fake::pack_v<int>));
}
```

#### `fake::mezz_d`

- **简介**: 检测包装的类型是否为 `fake::mezz_t`（value mezzanine）。
- **语法**:

```c++
inline constexpr fake::creed_c auto mezz_d = fake::just_d<[]<fake::mezz_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::mezz_d(fake::pack_v<fake::mezz_t<114514>>));
    static_assert(!fake::mezz_d(fake::pack_v<int>));
}
```

#### `fake::gene_d`

- **简介**: 检测包装的类型是否为泛型模式持有者（例如 `fake::generic`、`fake::array_like`、`fake::view_like`）。
- **语法**:

```c++
inline constexpr fake::creed_c auto gene_d = fake::just_d<[]<fake::gene_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    using pattern = fake::pattern_t<std::variant<int, float, double>>;
    static_assert(fake::gene_d(fake::pack_v<pattern>));
    static_assert(!fake::gene_d(fake::pack_v<int>));
}
```

#### `fake::debug_d`

- **简介**: 检测包装的类型是否为 `fake::debug`。
- **语法**:

```c++
inline constexpr fake::creed_c auto debug_d = fake::just_d<[]<fake::debug_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::debug_d(fake::pack_v<fake::debug>));
    static_assert(!fake::debug_d(fake::pack_v<int>));
}
```

#### `fake::char_array_d`

- **简介**: 检测包装的类型是否为 `fake::char_array`。
- **语法**:

```c++
inline constexpr fake::creed_c auto char_array_d = fake::just_d<[]<fake::char_array_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::char_array_d(fake::pack_v<fake::char_array<1919>>));
    static_assert(!fake::char_array_d(fake::pack_v<int>));
}
```

#### `fake::constant_d`

- **简介**: 检测包装的类型是否为 `std::integral_constant`。
- **语法**:

```c++
inline constexpr fake::creed_c auto constant_d = fake::just_d<[]<fake::constant_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::constant_d(fake::pack_v<std::integral_constant<uint32_t, 364364>>));
    static_assert(!fake::constant_d(fake::pack_v<int>));
}
```

#### `fake::pair_d`

- **简介**: 检测包装的类型是否为 `std::pair`。
- **语法**:

```c++
inline constexpr fake::creed_c auto pair_d = fake::just_d<[]<fake::pair_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::pair_d(fake::pack_v<std::pair<int, double>>));
    static_assert(!fake::pair_d(fake::pack_v<int>));
}
```

#### `fake::tuple_d`

- **简介**: 检测包装的类型是否为 `std::tuple`。
- **语法**:

```c++
inline constexpr fake::creed_c auto tuple_d = fake::just_d<[]<fake::tuple_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::tuple_d(fake::pack_v<std::tuple<int, double, float>>));
    static_assert(!fake::tuple_d(fake::pack_v<int>));
}
```

#### `fake::combine_d`

- **简介**: 检测包装的类型是否为 `std::pair` 或 `std::tuple`。
- **语法**:

```c++
inline constexpr fake::creed_c auto combine_d = fake::just_d<[]<fake::combine_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::combine_d(fake::pack_v<std::pair<int, double>>));
    static_assert(fake::combine_d(fake::pack_v<std::tuple<int, double, float>>));
    static_assert(!fake::combine_d(fake::pack_v<int>));
}
```

#### `fake::std_array_d`

- **简介**: 检测包装的类型是否为 `std::array`。
- **语法**:

```c++
inline constexpr fake::creed_c auto std_array_d = fake::just_d<[]<fake::std_array_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::std_array_d(fake::pack_v<std::array<int, 810>>));
    static_assert(!fake::std_array_d(fake::pack_v<int>));
}
```

#### `fake::container_d`

- **简介**: 检测包装的类型是否满足类似容器的要求（拥有 `begin`、`end`、`size`、`value_type`）。
- **语法**:

```c++
inline constexpr fake::creed_c auto container_d = fake::just_d<[]<fake::container_c>(){}>;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::container_d(fake::pack_v<std::vector<float>>));
    static_assert(!fake::container_d(fake::pack_v<int>));
}
```

#### `fake::optional_d`

- **简介**: 检测包装的类型是否为 `std::optional`。
- **语法**:

```c++
inline constexpr fake::creed_c auto optional_d = fake::just_d<[]<fake::optional_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::optional_d(fake::pack_v<std::optional<double>>));
    static_assert(!fake::optional_d(fake::pack_v<int>));
}
```

#### `fake::variant_d`

- **简介**: 检测包装的类型是否为 `std::variant`。
- **语法**:

```c++
inline constexpr fake::creed_c auto variant_d = fake::just_d<[]<fake::variant_c>(){}>;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::variant_d(fake::pack_v<std::variant<float, double, int>>));
    static_assert(!fake::variant_d(fake::pack_v<int>));
}
```

### 复合类型类别

#### `fake::fundamental_d`

- **简介**: 检测包装的类型是否为基本类型（算术类型、void 或 nullptr_t）。
- **语法**:

```c++
inline constexpr fake::detect::traits::fundamental_d fundamental_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{};
int main() {
    static_assert(fake::fundamental_d(fake::pack_v<double>));
    static_assert(!fake::fundamental_d(fake::pack_v<foo>));
}
```

#### `fake::arithmetic_d`

- **简介**: 检测包装的类型是否为算术类型（整型或浮点型）。
- **语法**:

```c++
inline constexpr fake::detect::traits::arithmetic_d arithmetic_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{};
int main() {
    static_assert(fake::arithmetic_d(fake::pack_v<int64_t>));
    static_assert(!fake::arithmetic_d(fake::pack_v<foo>));
}
```

#### `fake::scalar_d`

- **简介**: 检测包装的类型是否为标量类型（算术、枚举、指针、成员指针或 nullptr_t）。
- **语法**:

```c++
inline constexpr fake::detect::traits::scalar_d scalar_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为对象类型（非函数、非引用、非 void）。
- **语法**:

```c++
inline constexpr fake::detect::traits::object_d object_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为复合类型（数组、函数、指针、引用、类、联合、枚举或成员指针）。
- **语法**:

```c++
inline constexpr fake::detect::traits::compound_d compound_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为引用（左值或右值）。
- **语法**:

```c++
inline constexpr fake::detect::traits::reference_d reference_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::reference_d(fake::pack_v<int&&>));
    static_assert(fake::reference_d(fake::pack_v<int&>));
    static_assert(!fake::reference_d(fake::pack_v<int>));
}
```

#### `fake::member_pointer_d`

- **简介**: 检测包装的类型是否为成员指针（数据或函数）。
- **语法**:

```c++
inline constexpr fake::detect::traits::member_pointer_d member_pointer_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{int bar;};
int main() {
    static_assert(fake::member_pointer_d(fake::pack_v<decltype(&foo::bar)>));
    static_assert(!fake::member_pointer_d(fake::pack_v<foo>));
}
```

### 类型属性

#### `fake::const_d`

- **简介**: 检测包装的类型是否具有 `const` 限定。
- **语法**:

```c++
inline constexpr fake::detect::traits::const_d const_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::const_d(fake::pack_v<const int>));
    static_assert(!fake::const_d(fake::pack_v<int>));
}
```

#### `fake::volatile_d`

- **简介**: 检测包装的类型是否具有 `volatile` 限定。
- **语法**:

```c++
inline constexpr fake::detect::traits::volatile_d volatile_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::volatile_d(fake::pack_v<volatile int>));
    static_assert(!fake::volatile_d(fake::pack_v<int>));
}
```

#### `fake::trivially_copyable_d`

- **简介**: 检测包装的类型是否为可平凡复制。
- **语法**:

```c++
inline constexpr fake::detect::traits::trivially_copyable_d trivially_copyable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否具有标准布局。
- **语法**:

```c++
inline constexpr fake::detect::traits::standard_layout_d standard_layout_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否具有唯一对象表示。
- **语法**:

```c++
inline constexpr fake::detect::traits::unique_object_representations_d unique_object_representations_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为空类（无非静态数据成员）。
- **语法**:

```c++
inline constexpr fake::detect::traits::empty_d empty_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为多态类型（至少有一个虚函数）。
- **语法**:

```c++
inline constexpr fake::detect::traits::polymorphic_d polymorphic_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为抽象类型（至少有一个纯虚函数）。
- **语法**:

```c++
inline constexpr fake::detect::traits::abstract_d abstract_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否标记为 `final`。
- **语法**:

```c++
inline constexpr fake::detect::traits::final_d final_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为聚合体。
- **语法**:

```c++
inline constexpr fake::detect::traits::aggregate_d aggregate_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为有符号算术类型。
- **语法**:

```c++
inline constexpr fake::detect::traits::signed_d signed_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::signed_d(fake::pack_v<int32_t>));
    static_assert(!fake::signed_d(fake::pack_v<uint32_t>));
}
```

#### `fake::unsigned_d`

- **简介**: 检测包装的类型是否为无符号算术类型。
- **语法**:

```c++
inline constexpr fake::detect::traits::unsigned_d unsigned_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::unsigned_d(fake::pack_v<uint8_t>));
    static_assert(!fake::unsigned_d(fake::pack_v<int8_t>));
}
```

#### `fake::bounded_array_d`

- **简介**: 检测包装的类型是否为有界数组（已知边界）。
- **语法**:

```c++
inline constexpr fake::detect::traits::bounded_array_d bounded_array_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::bounded_array_d(fake::pack_v<int[114]>));
    static_assert(!fake::bounded_array_d(fake::pack_v<int[]>));
}
```

#### `fake::unbounded_array_d`

- **简介**: 检测包装的类型是否为无界数组（未知边界）。
- **语法**:

```c++
inline constexpr fake::detect::traits::unbounded_array_d unbounded_array_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::unbounded_array_d(fake::pack_v<int[]>));
    static_assert(!fake::unbounded_array_d(fake::pack_v<int[514]>));
}
```

### 支持的操作

#### `fake::constructible_d<Args...>`

- **简介**: 检测包装的类型是否可以从给定的参数类型构造。
- **语法**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::constructible_d<_Args...> constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以从给定的参数类型平凡构造。
- **语法**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::trivially_constructible_d<_Args...> trivially_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以从给定的参数类型无异常构造。
- **语法**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::nothrow_constructible_d<_Args...> nothrow_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可默认构造。
- **语法**:

```c++
inline constexpr fake::detect::traits::default_constructible_d default_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可平凡默认构造。
- **语法**:

```c++
inline constexpr fake::detect::traits::trivially_default_constructible_d trivially_default_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为无异常默认构造。
- **语法**:

```c++
inline constexpr fake::detect::traits::nothrow_default_constructible_d nothrow_default_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可拷贝构造。
- **语法**:

```c++
inline constexpr fake::detect::traits::copy_constructible_d copy_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可平凡拷贝构造。
- **语法**:

```c++
inline constexpr fake::detect::traits::trivially_copy_constructible_d trivially_copy_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为无异常拷贝构造。
- **语法**:

```c++
inline constexpr fake::detect::traits::nothrow_copy_constructible_d nothrow_copy_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可移动构造。
- **语法**:

```c++
inline constexpr fake::detect::traits::move_constructible_d move_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可平凡移动构造。
- **语法**:

```c++
inline constexpr fake::detect::traits::trivially_move_constructible_d trivially_move_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为无异常移动构造。
- **语法**:

```c++
inline constexpr fake::detect::traits::nothrow_move_constructible_d nothrow_move_constructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以接受类型 `T` 的值的赋值。
- **语法**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::assignable_d<_Arg> assignable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以平凡地接受类型 `T` 的值的赋值。
- **语法**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::trivially_assignable_d<_Arg> trivially_assignable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以无异常地接受类型 `T` 的值的赋值。
- **语法**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::nothrow_assignable_d<_Arg> nothrow_assignable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可拷贝赋值。
- **语法**:

```c++
inline constexpr fake::detect::traits::copy_assignable_d copy_assignable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可平凡拷贝赋值。
- **语法**:

```c++
inline constexpr fake::detect::traits::trivially_copy_assignable_d trivially_copy_assignable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为无异常拷贝赋值。
- **语法**:

```c++
inline constexpr fake::detect::traits::nothrow_copy_assignable_d nothrow_copy_assignable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可移动赋值。
- **语法**:

```c++
inline constexpr fake::detect::traits::move_assignable_d move_assignable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可平凡移动赋值。
- **语法**:

```c++
inline constexpr fake::detect::traits::trivially_move_assignable_d trivially_move_assignable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为无异常移动赋值。
- **语法**:

```c++
inline constexpr fake::detect::traits::nothrow_move_assignable_d nothrow_move_assignable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可析构。
- **语法**:

```c++
inline constexpr fake::detect::traits::destructible_d destructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为可平凡析构。
- **语法**:

```c++
inline constexpr fake::detect::traits::trivially_destructible_d trivially_destructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为无异常析构。
- **语法**:

```c++
inline constexpr fake::detect::traits::nothrow_destructible_d nothrow_destructible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否拥有虚析构函数。
- **语法**:

```c++
inline constexpr fake::detect::traits::virtual_destructor_d virtual_destructor_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以与类型 `T` 的值进行交换。
- **语法**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::swappable_with_d<_Arg> swappable_with_d;
```

- **示例**:

```c++
#include <string>
#include "detect.h"
int main() {
    static_assert(fake::swappable_with_d<std::string&>(fake::pack_v<std::string&>));
    static_assert(!fake::swappable_with_d<int>(fake::pack_v<std::string&>));
}
```

#### `fake::swappable_d`

- **简介**: 检测包装的类型是否为可交换（与自身）。
- **语法**:

```c++
inline constexpr fake::detect::traits::swappable_d swappable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以与类型 `T` 的值进行无异常交换。
- **语法**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::nothrow_swappable_with_d<_Arg> nothrow_swappable_with_d;
```

- **示例**:

```c++
#include <string>
#include "detect.h"
int main() {
    static_assert(fake::nothrow_swappable_with_d<std::string&>(fake::pack_v<std::string&>));
    static_assert(!fake::nothrow_swappable_with_d<std::string>(fake::pack_v<std::string>));
}
```

#### `fake::nothrow_swappable_d`

- **简介**: 检测包装的类型是否为无异常可交换。
- **语法**:

```c++
inline constexpr fake::detect::traits::nothrow_swappable_d nothrow_swappable_d;
```

- **示例**:

```c++
#include <iostream>
#include <memory>
#include "detect.h"
int main() {
    static_assert(fake::nothrow_swappable_d(fake::pack_v<std::shared_ptr<int>>));
    static_assert(!fake::nothrow_swappable_d(fake::pack_v<std::istream>));
}
```

### 类型关系

#### `fake::base_of_d<Derived>`

- **简介**: 检测包装的类型是否为 `Derived` 的基类。
- **语法**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::base_of_d<_Arg> base_of_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以从 `From` 转换而来。
- **语法**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::convertible_d<_Arg> convertible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以从 `From` 无异常转换而来。
- **语法**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::nothrow_convertible_d<_Arg> nothrow_convertible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否与 `T` 布局兼容。
- **语法**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::layout_compatible_d<_Arg> layout_compatible_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为 `Derived` 的指针可互转换基类。
- **语法**:

```c++
template<typename _Arg> inline constexpr fake::detect::traits::pointer_interconvertible_base_of_d<_Arg> pointer_interconvertible_base_of_d;
```

- **示例**:

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

- **简介**: 检测包装的类型（可调用对象）是否可以使用给定的参数类型进行调用。
- **语法**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::invocable_d<_Args...> invocable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以使用给定的参数进行调用，并且其结果可转换为 `R`。
- **语法**:

```c++
template<typename _R, typename... _Args> inline constexpr fake::detect::traits::invocable_r_d<_R, _Args...> invocable_r_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以使用给定的参数类型无异常调用。
- **语法**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::nothrow_invocable_d<_Args...> nothrow_invocable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以使用给定的参数无异常调用，并且其结果可转换为 `R`。
- **语法**:

```c++
template<typename _R, typename... _Args> inline constexpr fake::detect::traits::nothrow_invocable_r_d<_R, _Args...> nothrow_invocable_r_d;
```

- **示例**:

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

### 核心语言概念

#### `fake::same_d<T>`

- **简介**: 检测包装的类型是否与 `T` 完全相同。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::same_d<_Type> same_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::same_d<float>(fake::pack_v<float>));
    static_assert(!fake::same_d<float>(fake::pack_v<int>));
}
```

#### `fake::derived_from_d<Base>`

- **简介**: 检测包装的类型是否派生自 `Base`。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::derived_from_d<_Type> derived_from_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否为有符号整型。
- **语法**:

```c++
inline constexpr fake::detect::traits::signed_integral_d signed_integral_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::signed_integral_d(fake::pack_v<int32_t>));
    static_assert(!fake::signed_integral_d(fake::pack_v<uint32_t>));
}
```

#### `fake::unsigned_integral_d`

- **简介**: 检测包装的类型是否为无符号整型。
- **语法**:

```c++
inline constexpr fake::detect::traits::unsigned_integral_d unsigned_integral_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::unsigned_integral_d(fake::pack_v<uint32_t>));
    static_assert(!fake::unsigned_integral_d(fake::pack_v<int32_t>));
}
```

#### `fake::convertible_to_d<To>`

- **简介**: 检测包装的类型是否可转换为 `To`。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::convertible_to_d<_Type> convertible_to_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否可以从 `From` 转换而来。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::convertible_from_d<_Type> convertible_from_d;
```

- **示例**:

```c++
#include "detect.h"
struct base{};
struct derive : base{};
int main() {
    static_assert(fake::convertible_from_d<derive>(fake::pack_v<base>));
    static_assert(!fake::convertible_from_d<base>(fake::pack_v<derive>));
}
```

### 比较概念

#### `fake::equality_comparable_d`

- **简介**: 检测包装的类型是否支持相等比较（`==` 和 `!=`）。
- **语法**:

```c++
inline constexpr fake::detect::traits::equality_comparable_d equality_comparable_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::equality_comparable_d(fake::pack_v<float>));
    static_assert(!fake::equality_comparable_d(fake::pack_v<foo>));
}
```

#### `fake::equality_comparable_with_d<T>`

- **简介**: 检测包装的类型是否可以与类型 `T` 进行相等比较。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::equality_comparable_with_d<_Type> equality_comparable_with_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::equality_comparable_with_d<float>(fake::pack_v<int>));
    static_assert(!fake::equality_comparable_with_d<foo>(fake::pack_v<int>));
}
```

#### `fake::totally_ordered_d`

- **简介**: 检测包装的类型是否为全序类型（支持 `<`、`<=`、`>`、`>=`）。
- **语法**:

```c++
inline constexpr fake::detect::traits::totally_ordered_d totally_ordered_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::totally_ordered_d(fake::pack_v<float>));
    static_assert(!fake::totally_ordered_d(fake::pack_v<foo>));
}
```

#### `fake::totally_ordered_with_d<T>`

- **简介**: 检测包装的类型是否可以与类型 `T` 进行全序比较。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::totally_ordered_with_d<_Type> totally_ordered_with_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::totally_ordered_with_d<int>(fake::pack_v<float>));
    static_assert(!fake::totally_ordered_with_d<int>(fake::pack_v<foo>));
}
```

#### `fake::three_way_comparable_d`

- **简介**: 检测包装的类型是否支持三路比较（`<=>`）。
- **语法**:

```c++
inline constexpr fake::detect::traits::three_way_comparable_d three_way_comparable_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::three_way_comparable_d(fake::pack_v<float>));
    static_assert(!fake::three_way_comparable_d(fake::pack_v<foo>));
}
```

#### `fake::three_way_comparable_with_d<T>`

- **简介**: 检测包装的类型是否可以与类型 `T` 进行三路比较。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::three_way_comparable_with_d<_Type> three_way_comparable_with_d;
```

- **示例**:

```c++
#include "detect.h"
struct foo{int x; double y;};
int main() {
    static_assert(fake::three_way_comparable_with_d<int>(fake::pack_v<float>));
    static_assert(!fake::three_way_comparable_with_d<int>(fake::pack_v<foo>));
}
```

### 对象概念

#### `fake::movable_d`

- **简介**: 检测包装的类型是否满足 `std::movable` 概念。
- **语法**:

```c++
inline constexpr fake::detect::traits::movable_d movable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否满足 `std::copyable` 概念。
- **语法**:

```c++
inline constexpr fake::detect::traits::copyable_d copyable_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否满足 `std::semiregular` 概念。
- **语法**:

```c++
inline constexpr fake::detect::traits::semiregular_d semiregular_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否满足 `std::regular` 概念（semiregular + equality_comparable）。
- **语法**:

```c++
inline constexpr fake::detect::traits::regular_d regular_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
struct bar{bar(const bar&) = default; bar(bar&&) = delete;};
int main() {
    static_assert(fake::regular_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::regular_d(fake::pack_v<bar>));
}
```

### 范围概念

#### `fake::range_d`

- **简介**: 检测包装的类型是否满足 `std::ranges::range`。
- **语法**:

```c++
inline constexpr fake::detect::traits::range_d range_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::range_d(fake::pack_v<int>));
}
```

#### `fake::borrowed_range_d`

- **简介**: 检测包装的类型是否满足 `std::ranges::borrowed_range`。
- **语法**:

```c++
inline constexpr fake::detect::traits::borrowed_range_d borrowed_range_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否满足 `std::ranges::sized_range`。
- **语法**:

```c++
inline constexpr fake::detect::traits::sized_range_d sized_range_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::sized_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::sized_range_d(fake::pack_v<int>));
}
```

#### `fake::views_d`

- **简介**: 检测包装的类型是否满足 `std::ranges::view`。
- **语法**:

```c++
inline constexpr fake::detect::traits::views_d views_d;
```

- **示例**:

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

- **简介**: 检测包装的类型是否满足 `std::ranges::input_range`。
- **语法**:

```c++
inline constexpr fake::detect::traits::input_range_d input_range_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::input_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::input_range_d(fake::pack_v<int>));
}
```

#### `fake::output_range_d<Value>`

- **简介**: 检测包装的类型是否满足 `std::ranges::output_range`，其值类型为 `Value`。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::output_range_d<_Type> output_range_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::output_range_d<int>(fake::pack_v<std::vector<int>>));
    static_assert(!fake::output_range_d<int>(fake::pack_v<int>));
}
```

#### `fake::forward_range_d`

- **简介**: 检测包装的类型是否满足 `std::ranges::forward_range`。
- **语法**:

```c++
inline constexpr fake::detect::traits::forward_range_d forward_range_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::forward_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::forward_range_d(fake::pack_v<int>));
}
```

#### `fake::bidirectional_range_d`

- **简介**: 检测包装的类型是否满足 `std::ranges::bidirectional_range`。
- **语法**:

```c++
inline constexpr fake::detect::traits::bidirectional_range_d bidirectional_range_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::bidirectional_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::bidirectional_range_d(fake::pack_v<int>));
}
```

#### `fake::random_access_range_d`

- **简介**: 检测包装的类型是否满足 `std::ranges::random_access_range`。
- **语法**:

```c++
inline constexpr fake::detect::traits::random_access_range_d random_access_range_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::random_access_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::random_access_range_d(fake::pack_v<int>));
}
```

#### `fake::contiguous_range_d`

- **简介**: 检测包装的类型是否满足 `std::ranges::contiguous_range`。
- **语法**:

```c++
inline constexpr fake::detect::traits::contiguous_range_d contiguous_range_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::contiguous_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::contiguous_range_d(fake::pack_v<int>));
}
```

#### `fake::common_range_d`

- **简介**: 检测包装的类型是否满足 `std::ranges::common_range`。
- **语法**:

```c++
inline constexpr fake::detect::traits::common_range_d common_range_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::common_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::common_range_d(fake::pack_v<int>));
}
```

#### `fake::viewable_range_d`

- **简介**: 检测包装的类型是否满足 `std::ranges::viewable_range`。
- **语法**:

```c++
inline constexpr fake::detect::traits::viewable_range_d viewable_range_d;
```

- **示例**:

```c++
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::viewable_range_d(fake::pack_v<std::vector<int>>));
    static_assert(!fake::viewable_range_d(fake::pack_v<int>));
}
```

### Fake 概念

#### `fake::hold_d<T>`

- **简介**: 检测包装的类型是否 **hold** 类型 `T`，即去除引用后保留 cv 限定符与 `T` 匹配。通常用于检查类型是否匹配某个目标，而不考虑引用。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::hold_d<_Type> hold_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::hold_d<int>(fake::pack_v<int&>));
    static_assert(fake::hold_d<const int>(fake::pack_v<const int&>));
    static_assert(!fake::hold_d<int>(fake::pack_v<const int&>));
}
```

#### `fake::like_d<T>`

- **简介**: 检测包装的类型是否 **like** `T`，即去除 cvref 限定符后与 `T` 相同，或通过 `mezz` 包装器匹配自定义概念。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::like_d<_Type> like_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::like_d<int>(fake::pack_v<const int&>));
    static_assert(!fake::like_d<int>(fake::pack_v<const double&>));
}
```

#### `fake::meets_d<Ts...>`

- **简介**: 检测包装的类型是否正好是包 `Ts...` 中的某一个类型。
- **语法**:

```c++
template<typename... _Type> inline constexpr fake::detect::traits::meets_d<_Type...> meets_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::meets_d<int&, double>(fake::pack_v<int&>));
    static_assert(!fake::meets_d<int&, double>(fake::pack_v<int>));
}
```

#### `fake::holds_d<Ts...>`

- **简介**: 检测包装的类型是否 **hold** 包 `Ts...` 中的某一个类型（使用 `fake::hold` 概念）。
- **语法**:

```c++
template<typename... _Type> inline constexpr fake::detect::traits::holds_d<_Type...> holds_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::holds_d<const int, double>(fake::pack_v<const int&>));
    static_assert(!fake::holds_d<const int, double>(fake::pack_v<int&>));
}
```

#### `fake::likes_d<Ts...>`

- **简介**: 检测包装的类型是否 **like** 包 `Ts...` 中的某一个类型（使用 `fake::like` 概念）。
- **语法**:

```c++
template<typename... _Type> inline constexpr fake::detect::traits::likes_d<_Type...> likes_d;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(fake::likes_d<int, double>(fake::pack_v<const double&>));
    static_assert(!fake::likes_d<int, double>(fake::pack_v<const float&>));
}
```

#### `fake::trait_d<Template>`

- **简介**: 检测包装的类型是否为模板 `Template` 的实例。
- **语法**:

```c++
template<template<typename...> typename _Tamplate> inline constexpr fake::detect::traits::trait_d<_Tamplate> trait_d;
```

- **示例**:

```c++
#include <optional>
#include <vector>
#include "detect.h"
int main() {
    static_assert(fake::trait_d<std::vector>(fake::pack_v<std::vector<double>>));
    static_assert(!fake::trait_d<std::vector>(fake::pack_v<std::optional<double>>));
}
```

## 类型变换（Shift 运算符）

Shift 运算符（`<<=`）在将类型传递给探测器之前对其应用类型变换。它们被定义为 `fake::shift_c` 对象。

### 自定义变换

#### `fake::just_p<lambda>`

- **简介**: 从一个 lambda 创建一个变换， 可以接受额外的参数。如果 lambda 无法用给定的参数调用，则尝试将其用作一元运算（例如 `[](auto _pack){ return ...; }`）.
- **语法**:

```c++
template<auto _lambda> inline constexpr fake::detect::traits::just_p<_lambda> just_p;
```

- **示例**:

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

### 容器成员类型提取

#### `fake::key_p`

- **简介**: 将包装的类型转换为其 `key_type`（用于关联容器）。
- **语法**:

```c++
inline constexpr fake::detect::traits::key_p key_p;
```

- **示例**:

```c++
#include <unordered_map>
#include "detect.h"
int main() {
    static_assert((fake::key_p <<= fake::same_d<int>)(fake::pack_v<std::unordered_map<int, double>>));
    static_assert(!(fake::key_p <<= fake::same_d<int>)(fake::pack_v<int>));
}
```

#### `fake::mapped_p`

- **简介**: 将包装的类型转换为其 `mapped_type`。
- **语法**:

```c++
inline constexpr fake::detect::traits::mapped_p mapped_p;
```

- **示例**:

```c++
#include <unordered_map>
#include "detect.h"
int main() {
    static_assert((fake::mapped_p <<= fake::same_d<double>)(fake::pack_v<std::unordered_map<int, double>>));
    static_assert(!(fake::mapped_p <<= fake::same_d<int>)(fake::pack_v<int>));
}
```

#### `fake::first_p`

- **简介**: 将 `std::pair` 转换为其 `first_type`。
- **语法**:

```c++
inline constexpr fake::detect::traits::first_p first_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::first_p <<= fake::same_d<int>)(fake::pack_v<std::pair<int, double>>));
    static_assert(!(fake::first_p <<= fake::same_d<double>)(fake::pack_v<std::pair<int, double>>));
}
```

#### `fake::second_p`

- **简介**: 将 `std::pair` 转换为其 `second_type`。
- **语法**:

```c++
inline constexpr fake::detect::traits::second_p second_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::second_p <<= fake::same_d<double>)(fake::pack_v<std::pair<int, double>>));
    static_assert(!(fake::second_p <<= fake::same_d<double>)(fake::pack_v<double>));
}
```

#### `fake::element_p`

- **简介**: 将类似智能指针的类型转换为其 `element_type`。
- **语法**:

```c++
inline constexpr fake::detect::traits::element_p element_p;
```

- **示例**:

```c++
#include <memory>
#include "detect.h"
int main() {
    static_assert((fake::element_p <<= fake::same_d<int>)(fake::pack_v<std::unique_ptr<int>>));
    static_assert(!(fake::element_p <<= fake::same_d<double>)(fake::pack_v<double>));
}
```

#### `fake::type_p`

- **简介**: 将具有嵌套 `type` 别名（例如 `std::remove_const<const int>`）的类型转换为其 `type`。
- **语法**:

```c++
inline constexpr fake::detect::traits::type_p type_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::type_p <<= fake::same_d<int>)(fake::pack_v<std::remove_const<const int>>));
    static_assert(!(fake::type_p <<= fake::same_d<int>)(fake::pack_v<int>));
}
```

#### `fake::index_p<N>`

- **简介**: 将类似元组的类型转换为其索引 `N` 处的元素。
- **语法**:

```c++
template<std::size_t _index> inline constexpr fake::detect::traits::index_p<_index> index_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::index_p<2> <<= fake::same_d<double>)(fake::pack_v<std::tuple<int, float, double, char>>));
    static_assert(!(fake::index_p<2> <<= fake::same_d<int>)(fake::pack_v<int>));
}
```

#### `fake::mimic_p<Target>`

- **简介**: 将类型（如 `std::variant<A,B>`）转换为具有相同模板参数的 `Target` 实例（例如 `std::tuple<A,B>`）。
- **语法**:

```c++
template<template<typename...> typename _Target> inline constexpr fake::detect::traits::mimic_p<_Target> mimic_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::mimic_p<std::tuple> <<= fake::same_d<std::tuple<int, float>>)(fake::pack_v<std::variant<int, float>>));
    static_assert(!(fake::mimic_p<std::tuple> <<= fake::same_d<int>)(fake::pack_v<int>));
}
```

### CV 限定符修改

#### `fake::remove_cv_p`

- **简介**: 移除顶层 `const` 和 `volatile` 限定符。
- **语法**:

```c++
inline constexpr fake::detect::traits::remove_cv_p remove_cv_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_cv_p <<= fake::same_d<int>)(fake::pack_v<const volatile int>));
    static_assert(!(fake::remove_cv_p <<= fake::same_d<double>)(fake::pack_v<int>));
}
```

#### `fake::remove_const_p`

- **简介**: 移除顶层 `const`。
- **语法**:

```c++
inline constexpr fake::detect::traits::remove_const_p remove_const_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_const_p <<= fake::same_d<volatile int>)(fake::pack_v<const volatile int>));
    static_assert(!(fake::remove_const_p <<= fake::same_d<int>)(fake::pack_v<volatile int>));
}
```

#### `fake::remove_volatile_p`

- **简介**: 移除顶层 `volatile`。
- **语法**:

```c++
inline constexpr fake::detect::traits::remove_volatile_p remove_volatile_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_volatile_p <<= fake::same_d<const int>)(fake::pack_v<const volatile int>));
    static_assert(!(fake::remove_volatile_p <<= fake::same_d<int>)(fake::pack_v<const int>));
}
```

#### `fake::add_cv_p`

- **简介**: 添加顶层 `const` 和 `volatile`。
- **语法**:

```c++
inline constexpr fake::detect::traits::add_cv_p add_cv_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::add_cv_p <<= fake::same_d<const volatile int>)(fake::pack_v<int>));
    static_assert(!(fake::add_cv_p <<= fake::same_d<int>)(fake::pack_v<const volatile int>));
}
```

#### `fake::add_const_p`

- **简介**: 添加顶层 `const`。
- **语法**:

```c++
inline constexpr fake::detect::traits::add_const_p add_const_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::add_const_p <<= fake::same_d<const int>)(fake::pack_v<int>));
    static_assert(!(fake::add_const_p <<= fake::same_d<int>)(fake::pack_v<const int>));
}
```

#### `fake::add_volatile_p`

- **简介**: 添加顶层 `volatile`。
- **语法**:

```c++
inline constexpr fake::detect::traits::add_volatile_p add_volatile_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::add_volatile_p <<= fake::same_d<volatile int>)(fake::pack_v<int>));
    static_assert(!(fake::add_volatile_p <<= fake::same_d<int>)(fake::pack_v<volatile int>));
}
```

### 引用修改

#### `fake::remove_reference_p`

- **简介**: 移除任何引用（左值或右值）。
- **语法**:

```c++
inline constexpr fake::detect::traits::remove_reference_p remove_reference_p;
```

- **示例**:

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

- **简介**: 仅移除左值引用；保持右值引用不变。
- **语法**:

```c++
inline constexpr fake::detect::traits::remove_lvalue_reference_p remove_lvalue_reference_p;
```

- **示例**:

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

- **简介**: 仅移除右值引用；保持左值引用不变。
- **语法**:

```c++
inline constexpr fake::detect::traits::remove_rvalue_reference_p remove_rvalue_reference_p;
```

- **示例**:

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

- **简介**: 添加左值引用（如果还不是左值引用）。
- **语法**:

```c++
inline constexpr fake::detect::traits::add_lvalue_reference_p add_lvalue_reference_p;
```

- **示例**:

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

- **简介**: 添加右值引用（如果还不是引用）。
- **语法**:

```c++
inline constexpr fake::detect::traits::add_rvalue_reference_p add_rvalue_reference_p;
```

- **示例**:

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

### 其他类型变换

#### `fake::make_signed_p`

- **简介**: 将整型转换为其有符号版本。
- **语法**:

```c++
inline constexpr fake::detect::traits::make_signed_p make_signed_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::make_signed_p <<= fake::same_d<int32_t>)(fake::pack_v<uint32_t>));
    static_assert(!(fake::make_signed_p <<= fake::same_d<uint32_t>)(fake::pack_v<int32_t>));
}
```

#### `fake::make_unsigned_p`

- **简介**: 将整型转换为其无符号版本。
- **语法**:

```c++
inline constexpr fake::detect::traits::make_unsigned_p make_unsigned_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::make_unsigned_p <<= fake::same_d<uint32_t>)(fake::pack_v<int32_t>));
    static_assert(!(fake::make_unsigned_p <<= fake::same_d<int32_t>)(fake::pack_v<uint32_t>));
}
```

#### `fake::remove_extent_p`

- **简介**: 从数组类型中移除一层维度（例如 `int[][N]` -> `int[N]`）。
- **语法**:

```c++
inline constexpr fake::detect::traits::remove_extent_p remove_extent_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_extent_p <<= fake::same_d<int[514]>)(fake::pack_v<int[114][514]>));
    static_assert(!(fake::remove_extent_p <<= fake::same_d<int>)(fake::pack_v<double[514]>));
}
```

#### `fake::remove_all_extents_p`

- **简介**: 移除数组类型的所有维度（例如 `int[][N]` -> `int`）。
- **语法**:

```c++
inline constexpr fake::detect::traits::remove_all_extents_p remove_all_extents_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_all_extents_p <<= fake::same_d<int>)(fake::pack_v<int[114][514]>));
    static_assert(!(fake::remove_all_extents_p <<= fake::same_d<int>)(fake::pack_v<double[514]>));
}
```

#### `fake::remove_pointer_p`

- **简介**: 移除一层指针。
- **语法**:

```c++
inline constexpr fake::detect::traits::remove_pointer_p remove_pointer_p;
```

- **示例**:

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

- **简介**: 添加一层指针。
- **语法**:

```c++
inline constexpr fake::detect::traits::add_pointer_p add_pointer_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::add_pointer_p <<= fake::same_d<int*>)(fake::pack_v<int>));
    static_assert((fake::add_pointer_p <<= fake::same_d<const int**>)(fake::pack_v<const int*>));
    static_assert(!(fake::add_pointer_p <<= fake::same_d<const int*>)(fake::pack_v<int>));
}
```

#### `fake::decay_p`

- **简介**: 应用标准退化变换（数组到指针、函数到指针、cv 移除）。
- **语法**:

```c++
inline constexpr fake::detect::traits::decay_p decay_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::decay_p <<= fake::same_d<int*>)(fake::pack_v<int(&)[3]>));
    static_assert(!(fake::decay_p <<= fake::same_d<int&>)(fake::pack_v<int&&>));
}
```

#### `fake::bare_p`

- **简介**: 递归地剥离所有指针和 cvref 限定符，获得底层类型。
- **语法**:

```c++
inline constexpr fake::detect::traits::bare_p bare_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::bare_p <<= fake::same_d<int[3]>)(fake::pack_v<int(*const*&)[3]>));
    static_assert(!(fake::bare_p <<= fake::same_d<int>)(fake::pack_v<double*&>));
}
```

#### `fake::remove_cvref_p`

- **简介**: 同时移除 cv 限定符和引用。
- **语法**:

```c++
inline constexpr fake::detect::traits::remove_cvref_p remove_cvref_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::remove_cvref_p <<= fake::same_d<int>)(fake::pack_v<const volatile int&&>));
    static_assert(!(fake::remove_cvref_p <<= fake::same_d<double>)(fake::pack_v<const volatile int&&>));
}
```

#### `fake::common_type_p<Args...>`

- **简介**: 将包装的类型转换为该类型与 `Args...` 的公共类型（使用 `std::common_type`）。
- **语法**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::common_type_p<_Args...> common_type_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::common_type_p<int> <<= fake::same_d<double>)(fake::pack_v<double>));
    static_assert(!(fake::common_type_p<int> <<= fake::same_d<int>)(fake::pack_v<double>));
}
```

#### `fake::common_reference_p<Args...>`

- **简介**: 将包装的类型转换为该类型与 `Args...` 的公共引用类型。
- **语法**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::common_reference_p<_Args...> common_reference_p;
```

- **示例**:

```c++
#include "detect.h"
struct base{}; struct derive : base{};
int main() {
    static_assert((fake::common_reference_p<base> <<= fake::same_d<base>)(fake::pack_v<derive>));
    static_assert(!(fake::common_reference_p<derive> <<= fake::same_d<derive>)(fake::pack_v<base>));
}
```

#### `fake::underlying_type_p`

- **简介**: 将枚举转换为其底层类型。
- **语法**:

```c++
inline constexpr fake::detect::traits::underlying_type_p underlying_type_p;
```

- **示例**:

```c++
#include "detect.h"
enum struct enum_e : int16_t{foo, bar};
int main() {
    static_assert((fake::underlying_type_p <<= fake::same_d<int16_t>)(fake::pack_v<enum_e>));
    static_assert(!(fake::underlying_type_p <<= fake::same_d<uint16_t>)(fake::pack_v<enum_e>));
}
```

#### `fake::invoke_result_p<Args...>`

- **简介**: 将可调用类型转换为其使用 `Args...` 调用时的返回类型。
- **语法**:

```c++
template<typename... _Args> inline constexpr fake::detect::traits::invoke_result_p<_Args...> invoke_result_p;
```

- **示例**:

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

- **简介**: 将实例化的类型转换为其模式持有者（例如 `std::tuple<int>` -> `fake::generic<std::tuple>`）。
- **语法**:

```c++
inline constexpr fake::detect::traits::pattern_p pattern_p;
```

- **示例**:

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

- **简介**: 将类型包装为 `fake::pack_t`。
- **语法**:

```c++
inline constexpr fake::detect::traits::pack_p pack_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::pack_p <<= fake::same_d<fake::pack_t<int>>)(fake::pack_v<int>));
    static_assert(!(fake::pack_p <<= fake::same_d<fake::pack_t<int>>)(fake::pack_v<double>));
}
```

#### `fake::pazz_p`

- **简介**: 将类型包装为 `fake::pazz_t`。
- **语法**:

```c++
inline constexpr fake::detect::traits::pazz_p pazz_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::pazz_p <<= fake::same_d<fake::pazz_t<double>>)(fake::pack_v<double>));
    static_assert(!(fake::pazz_p <<= fake::same_d<fake::pazz_t<double>>)(fake::pack_v<int>));
}
```

#### `fake::take_p`

- **简介**: 从 `pack_c` 或 `pazz_c` 包装器中提取底层类型。
- **语法**:

```c++
inline constexpr fake::detect::traits::take_p take_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert((fake::take_p <<= fake::same_d<double>)(fake::pack_v<fake::pack_t<double>>));
    static_assert((fake::take_p <<= fake::same_d<double>)(fake::pack_v<fake::pazz_t<double>>));
    static_assert(!(fake::take_p <<= fake::same_d<double>)(fake::pack_v<double>));
}
```

#### `fake::try_take_p`

- **简介**: 如果包装器是 `pack_c` 或 `pazz_c`，则提取底层类型；否则保持类型不变。
- **语法**:

```c++
inline constexpr fake::detect::traits::try_take_p try_take_p;
```

- **示例**:

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

- **简介**: 递归地从嵌套包装器中提取底层类型，直到遇到非包装器类型。
- **语法**:

```c++
inline constexpr fake::detect::traits::take_all_p take_all_p;
```

- **示例**:

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

- **简介**: 将类型 `T` 转换为 `Target<T, Appends...>`。
- **语法**:

```c++
template<template<typename...> typename _Target, typename... _Appends>
inline constexpr fake::detect::traits::apply_p<_Target, _Appends...> apply_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    using result = fake::take_t<fake::apply_p<std::variant, char, double>(fake::pazz_v<int>)>;
    static_assert(std::same_as<result, std::variant<int, char, double>>);
}
```

#### `fake::keep_p`

- **简介**: 保持类型不变。
- **语法**:

```c++
inline constexpr fake::detect::traits::keep_p keep_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(std::same_as<fake::take_t<fake::keep_p(fake::pazz_v<const int* const&>)>, const int* const&>);
}
```

#### `fake::replace_p<Just>`

- **简介**: 将类型转换为 `Just`。
- **语法**:

```c++
template<typename _Type> inline constexpr fake::detect::traits::replace_p<_Type> replace_p;
```

- **示例**:

```c++
#include "detect.h"
int main() {
    static_assert(std::same_as<fake::take_t<fake::replace_p<double>(fake::pazz_v<const int* const&>)>, double>);
}
```

#### `fake::conditional_p<Condition, True, False>`

- **简介**: 如果类型符合 `Condition`, 通过 `True` 转换它； 否则, 通过 `False` 转换它。
- **语法**:

```c++
template<fake::creed_c auto _condition, fake::shift_c auto _true, fake::shift_c auto _false>
inline constexpr fake::detect::traits::conditional_p<_condition, _true, _false> conditional_p;
```

- **示例**:

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

## 组合运算符

探测器可以使用逻辑运算符组合成新的探测器。Shift 运算符可以与探测器通过 `<<=` 组合。

### 逻辑运算符

- `operator&&` : 合取
- `operator||` : 析取
- `operator!` : 否定
- `operator==` : 等价
- `operator!=` 和 `operator^` : 不等价

- **示例**:

```c++
#include "detect.h"
int main() {
    constexpr fake::pazz_c auto e = fake::pazz_v<const float&>;
    auto op = fake::remove_reference_p <<= fake::remove_const_p <<= fake::same_d<float> && !fake::reference_d;
    static_assert(op(e));
}
```

- **等价示例**:

```c++
#include "detect.h"
int main() {
    constexpr fake::pazz_c auto e = fake::pazz_v<const float&>;
    auto no_lref_no_const = fake::lvalue_reference_d == (fake::remove_reference_p <<= fake::const_d);
    static_assert(no_lref_no_const(e));
    static_assert(!no_lref_no_const(fake::pazz_v<const int>));
}
```

### 与 Shift 的组合

- `<<=` : 在将类型传递给探测器之前对其应用 shift 变换。

- **示例**:

```c++
#include "detect.h"
int main() {
    constexpr fake::pazz_c auto e = fake::pazz_v<const float&>;
    auto ep = fake::remove_const_p <<= fake::remove_reference_p <<= fake::same_d<float> || fake::reference_d;
    static_assert(!ep(e)); // result is false
}
```

## 自定义探测器

### `fake::just_d<lambda>`

- **简介**: 从一个 lambda 创建探测器，该 lambda 可以接受额外的参数。如果 lambda 无法用给定的参数调用，则尝试将其用作模板约束（例如 `[]<some_c>{}`）。
- **语法**:

```c++
template<auto _lambda> inline constexpr fake::detect::op::just<_lambda> just_d;
```

- **示例**:

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

- **简介**: 从一个 lambda 创建探测器，该 lambda 必须能够使用给定的参数（包括包装的类型）进行调用。如果 lambda 无法调用，则探测器非良构。
- **语法**:

```c++
template<auto _lambda> inline constexpr fake::detect::op::must<_lambda> must_d;
```

- **示例**:

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

## 概念适配器

### `fake::detect::as<detector>`

- **简介**: 从探测器（`fake::creed_c`）创建一个概念。
- **语法**:

```c++
template<typename _Type, auto _op>
concept as = requires{requires _op(fake::pazz_v<_Type>);} || requires{requires _op(fake::pack_v<_Type>);};
```

- **示例**:

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

**注意**：上述许多示例依赖于 `<vector>`、`<memory>`、`<ranges>` 等头文件。请根据需要使用它们。`fake` 头文件（`detect.h`、`view.h`、`flat.h`、`tuple.h`）应在使用相应类型的地方包含。
