# `traits.h`

- 用于模板元编程的高级类型 traits 和工具。

---

## `fake::aligned_signed`, `fake::aligned_unsigned`

### 简介

提供与当前平台指针大小相同的 signed/unsigned 整数类型的类型别名。
可用于指针大小的整数操作。

### 语法

```c++
template<typename = void>
using aligned_signed = typename aligned_int<void>::signed_type;

template<typename = void>
using aligned_unsigned = typename aligned_int<void>::unsigned_type;
```

### 示例

```c++
#include <concepts>
#include "traits.h"

int main()
{
    // On x86_64, pointer size is 8 bytes → aligned_signed is long long.
    static_assert(std::same_as<fake::aligned_signed<>, long long>);
    static_assert(std::same_as<fake::aligned_unsigned<>, unsigned long long>);
}
```

---

## `fake::system_bit`

### 简介

一个变量模板，返回给定类型（默认为 `void*`）的位数，即系统的指针位宽。

### 语法

```c++
template<typename _T = void*>
constexpr std::size_t system_bit = sizeof(_T) << 3;
```

### 示例

```c++
#include "traits.h"

int main()
{
    static_assert(fake::system_bit<> == 64);   // 8 bytes * 8 = 64 bits
}
```

---

## `fake::signed_bytes`、`fake::unsigned_bytes`、`fake::signed_bits`、`fake::unsigned_bits`

### 简介

将字节数或位数映射到对应的定宽整数类型的类型别名。
字节数必须是 1 到 8 之间的 2 的幂。
位数必须是 8 的倍数且为 2 的幂，并且对应的字节数必须有效。

### 语法

```c++
template<std::size_t _bytes>
using signed_bytes = /* int8_t, int16_t, int32_t or int64_t */;

template<std::size_t _bytes>
using unsigned_bytes = std::make_unsigned_t<signed_bytes<_bytes>>;

template<std::size_t _bits>
using signed_bits = signed_bytes<(_bits >> 3)>;

template<std::size_t _bits>
using unsigned_bits = std::make_unsigned_t<signed_bits<_bits>>;
```

### 示例

```c++
#include <concepts>
#include <cstdint>
#include "traits.h"

int main()
{
    using namespace std;
    
    // Valid byte sizes
    static_assert(same_as<fake::signed_bytes<1>, int8_t>);
    static_assert(same_as<fake::signed_bytes<2>, int16_t>);
    static_assert(same_as<fake::signed_bytes<4>, int32_t>);
    static_assert(same_as<fake::signed_bytes<8>, int64_t>);
    
    static_assert(same_as<fake::unsigned_bytes<1>, uint8_t>);
    static_assert(same_as<fake::unsigned_bytes<2>, uint16_t>);
    static_assert(same_as<fake::unsigned_bytes<4>, uint32_t>);
    static_assert(same_as<fake::unsigned_bytes<8>, uint64_t>);
    
    // Valid bit sizes
    static_assert(same_as<fake::signed_bits<8>, int8_t>);
    static_assert(same_as<fake::signed_bits<16>, int16_t>);
    static_assert(same_as<fake::signed_bits<32>, int32_t>);
    static_assert(same_as<fake::signed_bits<64>, int64_t>);
    
    static_assert(same_as<fake::unsigned_bits<8>, uint8_t>);
    static_assert(same_as<fake::unsigned_bits<16>, uint16_t>);
    static_assert(same_as<fake::unsigned_bits<32>, uint32_t>);
    static_assert(same_as<fake::unsigned_bits<64>, uint64_t>);
    
    // Invalid byte size (5) – the alias does not exist
    [](auto _bytes){
        static_assert(requires{ typename fake::signed_bytes<_bytes.value>; } == false);
    }(fake::index_v<5>);
    
    // Invalid bit size (5) – the alias does not exist
    [](auto _bits){
        static_assert(requires{ typename fake::signed_bits<_bits.value>; } == false);
    }(fake::index_v<5>);
}
```

---

## `fake::remove_lvalue_reference_t`、`fake::remove_rvalue_reference_t`

### 简介

仅移除左值引用或仅移除右值引用的工具，保留另一种引用类型不变。

### 语法

```c++
template<typename _Type>
using remove_lvalue_reference_t = std::conditional_t<
    std::is_lvalue_reference_v<_Type>,
    std::remove_reference_t<_Type>,
    _Type
>;

template<typename _Type>
using remove_rvalue_reference_t = std::conditional_t<
    std::is_rvalue_reference_v<_Type>,
    std::remove_reference_t<_Type>,
    _Type
>;
```

### 示例

```c++
#include <concepts>
#include <utility>
#include "traits.h"

int main()
{
    struct foo{};
    
    auto wrap = []<typename _T>(_T &&_e) {
        struct keep_or_ref final {
            fake::remove_rvalue_reference_t<_T> data;
        };
        return keep_or_ref{_e};
    };
    
    foo instance;
    auto ref   = wrap(std::as_const(instance)); // _T = const foo& → data = const foo&
    auto keep  = wrap(std::move(instance));     // _T = foo&&     → data = foo
    
    static_assert(std::same_as<const foo&, decltype(ref.data)>);
    static_assert(std::same_as<foo, decltype(keep.data)>);
}
```

---

## `fake::add_const_t`

### 简介

为类型添加 `const`，同时保留引用语义：如果输入是引用，则结果为指向 `const` 的引用（左值或右值引用，视情况而定）。

### 语法

```c++
template<typename _Type>
using add_const_t = /* see implementation */;
```

### 示例

```c++
#include <concepts>
#include "traits.h"

int main()
{
    static_assert(std::same_as<fake::add_const_t<int&>, const int&>);
    static_assert(std::same_as<fake::add_const_t<int&&>, const int&&>);
    static_assert(std::same_as<fake::add_const_t<int>, const int>);
}
```

---

## `fake::constant_c`

### 简介

一个概念，检测类型是否是 `std::integral_constant` 的特化。

### 语法

```c++
template<typename _Type>
concept constant_c = is_constant_v<_Type>;
```

### 示例

```c++
#include "traits.h"
#include "is_valid.h"   // for fake::index_v

int main()
{
    static_assert(fake::constant_c<decltype(fake::index_v<114514>)>);
}
```

---

## `fake::conjunction_v`

### 简介

一个变量模板，对 `bool` 值包执行逻辑 AND 操作。
如果所有值均为 `true`，则结果为 `true`。可接受任意数量的参数。

### 语法

```c++
template<bool... _args>
constexpr bool conjunction_v = /* implementation */;
```

### 示例

```c++
#include "traits.h"

int main()
{
    static_assert(fake::conjunction_v<true, true, true>);
    static_assert(fake::conjunction_v<true, true, false, true> == false);
}
```

---

## `fake::merge_t`

### 简介

如果包中的所有类型都相同，`merge_t` 将产生该公共类型；否则为非法（适用于 SFINAE）。

### 语法

```c++
template<typename... _Types>
using merge_t = typename merge<_Types...>::type;
```

### 示例

```c++
#include <concepts>
#include "traits.h"

int main()
{
    static_assert(std::same_as<fake::merge_t<int, int, int, int>, int>);
    
    // When types differ, the alias does not exist
    [](auto... _pazz){
        static_assert(requires{ typename fake::merge_t<fake::take_t<_pazz.value>...>; } == false);
    }(fake::pazz_v<int>, fake::pazz_v<char>, fake::pazz_v<int>, fake::pazz_v<int>);
}
```

---

## `fake::common_base_t`、`fake::common_derive_t`

### 简介

- `common_base_t`：在一组类型中查找公共的无歧义基类。第一个类型作为候选基类，后续类型必须派生自它。
- `common_derive_t`：在一组类型中查找公共的最派生类型。第一个类型作为候选最派生类型，后续类型必须是它的基类。

### 语法

```c++
template<typename... _Types>
using common_base_t = typename common_base<_Types...>::type;

template<typename... _Types>
using common_derive_t = typename common_derive<_Types...>::type;
```

### 示例

```c++
#include <concepts>
#include "traits.h"

struct base {};
struct lhs : virtual base {};
struct rhs : virtual base {};
struct derive : lhs, rhs {};

int main()
{
    static_assert(std::same_as<fake::common_base_t<base, lhs, rhs, derive>, base>);
    static_assert(std::same_as<fake::common_derive_t<derive, base, lhs, rhs>, derive>);
    
    // Order matters: base must be first for common_base_t
    [](auto... _pazz){
        static_assert(requires{ typename fake::common_base_t<fake::take_t<_pazz.value>...>; } == false);
    }(fake::pazz_v<lhs>, fake::pazz_v<rhs>, fake::pazz_v<base>, fake::pazz_v<derive>);
    
    // For common_derive_t, the most derived must be first
    [](auto... _pazz){
        static_assert(requires{ typename fake::common_derive_t<fake::take_t<_pazz.value>...>; } == false);
    }(fake::pazz_v<base>, fake::pazz_v<lhs>, fake::pazz_v<rhs>, fake::pazz_v<derive>);
}
```

---

## `fake::visit`

### 简介

一个简化的 `std::visit`，接受一个 variant 和一组 lambda。它将 lambda 组合成一个重载集并分发。

### 语法

```c++
template<typename _Instance, typename... _Lambda>
constexpr auto visit(_Instance &&_instance, _Lambda&& ..._lambda);
```

### 示例

```c++
#include <variant>
#include "traits.h"

int main()
{
    constexpr auto result = fake::visit(
        std::variant<int, double, const char*>{std::in_place_type<double>, 514.0},
        [](std::same_as<int> auto)      { return 0; },
        [](std::same_as<double> auto)   { return 1; },
        [](std::same_as<const char*> auto) { return 2; }
    );
    
    static_assert(result == 1);
}
```

---

## `fake::filter`

### 简介

一个变量模板，创建一个谓词，检查一组参数是否能被任何给定的 lambda（存储为 `mezz` 值）接受。
lambda 按顺序尝试；如果其中一个匹配（即调用有效），则 `filter<...>(args...)` 返回 `true`；否则返回 `false`。

### 语法

```c++
template<auto... _mezz>
inline constexpr auto filter = [](auto &&..._args) consteval -> bool { /* ... */ };
```

### 示例

```c++
#include <concepts>
#include "traits.h"

int main()
{
    constexpr auto one_of = fake::filter<
        fake::mezz_v<[](std::same_as<int> auto){}>,
        fake::mezz_v<[](std::same_as<double> auto){}>
    >;
    
    static_assert(one_of(114));      // int matches
    static_assert(one_of(514.0));    // double matches
    static_assert(one_of("1919") == false); // const char* matches none
}
```

---

## `fake::pair_c`、`fake::tuple_c`、`fake::combine_c`、`fake::aggregate_c`、`fake::std_array_c`

### 简介

识别特定种类类型的概念：

- `pair_c`：`std::pair` 的特化
- `tuple_c`：`std::tuple` 的特化
- `combine_c`：`pair` 或 `tuple`
- `aggregate_c`：任何聚合类型
- `std_array_c`：`std::array` 的特化

### 语法

```c++
template<typename _Type>
concept pair_c = is_pair_v<_Type>;

template<typename _Type>
concept tuple_c = is_tuple_v<_Type>;

template<typename _Type>
concept combine_c = is_combine_v<_Type>;

template<typename _Type>
concept aggregate_c = is_aggregate_v<_Type>;

template<typename _Type>
concept std_array_c = is_std_array_v<_Type>;
```

### 示例

```c++
#include <array>
#include <tuple>
#include <utility>
#include "traits.h"

int main()
{
    static_assert(fake::pair_c<std::pair<int, double>>);
    static_assert(fake::tuple_c<std::tuple<int, double, char>>);
    static_assert(fake::combine_c<std::pair<int, double>>);
    static_assert(fake::combine_c<std::tuple<int, double, char>>);
    static_assert(fake::aggregate_c<std::array<int, 114514>>);
    static_assert(fake::std_array_c<std::array<int, 1919>>);
}
```

---

## `fake::array_c`、`fake::array_value_t`、`fake::array_size_v`

### 简介

用于处理内置数组（包括零长度和不完整数组）的工具：

- `array_c`：匹配任何数组类型的概念。
- `array_value_t`：返回数组的元素类型。
- `array_size_v`：返回数组的大小（零长度数组返回 0，不完整数组非法）。

### 语法

```c++
template<typename _Type>
concept array_c = is_array_v<_Type>;

template<array_c _Array>
using array_value_t = typename array_value<_Array>::type;

template<array_c _Array>
constexpr std::size_t array_size_v = array_size<_Array>::value;
```

### 示例

```c++
#include "traits.h"

int main()
{
    constexpr std::nullptr_t empty[0] = {};
    constexpr std::nullptr_t array[810] = {};
    
    static_assert(fake::array_c<decltype(empty)>);
    static_assert(fake::array_c<decltype(array)>);
    
    static_assert(std::same_as<fake::array_value_t<decltype(empty)>, std::nullptr_t>);
    static_assert(std::same_as<fake::array_value_t<decltype(array)>, std::nullptr_t>);
    
    static_assert(fake::array_size_v<decltype(empty)> == 0);
    static_assert(fake::array_size_v<decltype(array)> == 810);
}
```

---

## `fake::optional_c`、`fake::variant_c`

### 简介

识别 `std::optional` 和 `std::variant` 特化的概念。

### 语法

```c++
template<typename _Type>
concept optional_c = is_optional_v<_Type>;

template<typename _Type>
concept variant_c = is_variant_v<_Type>;
```

### 示例

```c++
#include <optional>
#include <variant>
#include "traits.h"

int main()
{
    static_assert(fake::optional_c<std::optional<double>>);
    static_assert(fake::variant_c<std::variant<int, double, char>>);
}
```

---

## `fake::variant_info`

### 简介

一个 traits 结构，从 `std::variant` 中提取信息：

- `types`：备选类型的 `std::tuple`。
- `size`：备选类型的数量。

### 语法

```c++
template<typename = void>
struct variant_info;

template<template<typename...> typename _Variant, typename... _Types>
struct variant_info<_Variant<_Types...>> { /* ... */ };
```

### 示例

```c++
#include <tuple>
#include <variant>
#include <concepts>
#include "traits.h"

int main()
{
    using V = std::variant<int, double, char>;
    using info = fake::variant_info<V>;
    
    static_assert(std::same_as<info::types, std::tuple<int, double, char>>);
    static_assert(info::size == 3);
}
```

---

## `fake::template_info`

### 简介

一个 traits 结构，用于检查类模板的实例化。它提供：

- `value`：该类型是否为模板实例化。
- `is_array_like` / `is_view_like`：指示是否为 array‑like（类型+大小）或 view‑like（字符包）模板的标志。
- `args`：类型参数的 `std::tuple`（适用于经典模板）。
- `arg` / `array_size`：用于 array‑like 模板。
- `size`：模板参数的数量。
- `type`：一个模板别名，用于使用不同参数重建模板。

### 语法

```c++
template<typename = void>
struct template_info;

// Specializations for classic templates, array‑like templates, and view‑like templates.
```

### 示例

```c++
#include <array>
#include <tuple>
#include <variant>
#include "traits.h"
#include "view.h"   // for fake::view_t

int main()
{
    // Classic template (std::variant)
    using classic_like = fake::template_info<std::variant<int, double, char>>;
    static_assert(classic_like::value);
    static_assert(classic_like::is_array_like == false);
    static_assert(classic_like::is_view_like == false);
    
    // Array‑like template (std::array)
    using array_like = fake::template_info<std::array<int, 810>>;
    static_assert(array_like::value);
    static_assert(array_like::is_array_like == true);
    static_assert(array_like::is_view_like == false);
    
    // View‑like template (fake::view)
    using view_like = fake::template_info<fake::view_t<"foo">>;
    static_assert(view_like::value);
    static_assert(view_like::is_array_like == false);
    static_assert(view_like::is_view_like == true);
    
    // Non‑template or unsupported template (std::bool_constant)
    using unsupported = fake::template_info<std::bool_constant<true>>;
    static_assert(unsupported::value == false);
}
```

---

## `fake::bare_t`

### 简介

递归移除类型的所有指针、引用和 cv‑限定符，得到底层的“裸”类型。

### 语法

```c++
template<typename _Type>
using bare_t = typename bare<_Type>::type;
```

### 示例

```c++
#include <concepts>
#include "traits.h"

int main()
{
    static_assert(std::same_as<fake::bare_t<const int * const ** const &>, int>);
}
```

---

## `fake::ref_c`

### 简介

一个概念，用于识别 `std::reference_wrapper`。

### 语法

```c++
template<typename _Type>
concept ref_c = is_ref_v<_Type>;
```

### 示例

```c++
#include <functional>
#include "traits.h"

int main()
{
    int value = 114514;
    static_assert(fake::ref_c<decltype(std::ref(value))>);
    static_assert(fake::ref_c<decltype(std::ref(std::as_const(value)))>);
}
```

---

## `fake::remove_ref_t`、`fake::remove_ref_v`

### 简介

- `remove_ref_t`：从 `std::reference_wrapper` 或普通引用中提取底层类型。
- `remove_ref_v`：一个函数，返回底层对象的（左值）引用，如果存在 wrapper 则将其剥离。
  *注意：* 对于不是 `reference_wrapper` 的右值参数，该函数会触发静态断言以防止悬垂引用。

### 语法

```c++
template<typename _T>
using remove_ref_t = typename remove_ref<std::remove_cvref_t<_T>>::type;

template<typename _T>
auto remove_ref_v(_T &_value) -> std::add_lvalue_reference_t<remove_ref_t<_T>>;

template<typename _T>
auto remove_ref_v(const _T &_value) -> std::add_lvalue_reference_t<remove_ref_t<_T>>;

template<typename _T>
auto remove_ref_v(_T &&_value) -> std::add_lvalue_reference_t<remove_ref_t<_T>>; // constrained
```

### 示例

```c++
#include <functional>
#include <concepts>
#include "traits.h"

int main()
{
    int value = 114514;
    
    static_assert(std::same_as<fake::remove_ref_t<decltype(std::ref(std::as_const(value)))>, const int>);
    static_assert(std::same_as<decltype(fake::remove_ref_v(std::ref(value))), int&>);
    static_assert(std::same_as<decltype(fake::remove_ref_v(std::ref(std::as_const(value)))), const int&>);
    
    // rvalue usage is only allowed for reference_wrapper
    // auto bad = fake::remove_ref_v(42); // would trigger static_assert
}
```

---

## `fake::deducible_c`

### 简介

一个概念，检查模板是否可以使用给定的类型参数集进行实例化。
如果 `_Template<_Type...>` 是有效类型，则该概念成立。

### 语法

```c++
template<template<typename...> typename _Template, typename... _Type>
concept deducible_c = requires{ typename _Template<_Type...>; };
```

### 示例

```c++
#include <tuple>
#include <vector>
#include "traits.h"

int main()
{
    static_assert(fake::deducible_c<std::tuple, int, char, double>);
    static_assert(fake::deducible_c<std::vector, int, char, double> == false); // vector expects one type
}
```

---

## `fake::concept_sequence_c`

### 简介

一个概念，将一个谓词（存储为 `mezz` 值）应用于大小为 `_size` 的序列中的每个索引。
谓词是一个可调用对象，接受一个 `index` 参数并返回布尔值。
如果谓词对每个索引都返回 `true`，则该概念成立。

### 语法

```c++
template<std::size_t _size, auto _concept>
concept concept_sequence_c = concept_sequence_v<_size, _concept>;
```

### 示例

```c++
#include <tuple>
#include "traits.h"

int main()
{
    using all_const = std::tuple<const int, const char, const double>;
    
    constexpr fake::mezz_c auto all_const_checker = fake::mezz_v<
        [](auto _i) { return std::is_const_v<std::tuple_element_t<_i.value, all_const>>; }
    >;
    
    static_assert(fake::concept_sequence_c<std::tuple_size_v<all_const>, all_const_checker>);
    
    using two_const = std::tuple<const int, char, const double>;
    constexpr fake::mezz_c auto two_const_checker = fake::mezz_v<
        [](auto _i) { return std::is_const_v<std::tuple_element_t<_i.value, two_const>>; }
    >;
    
    static_assert(fake::concept_sequence_c<std::tuple_size_v<two_const>, two_const_checker> == false);
}
```

---

## `fake::applicable_c`

### 简介

一个概念，检查一个函数对象是否可以以 tuple 的元素作为独立参数进行调用。

### 语法

```c++
template<typename _Functor, typename _Tuple>
concept applicable_c = /* ... */;
```

### 示例

```c++
#include <tuple>
#include <concepts>
#include "traits.h"

int main()
{
    auto lambda = [](std::convertible_to<int> auto...) {};
    static_assert(fake::applicable_c<decltype(lambda), std::tuple<int, char, double>>);
    
    auto same_int = [](std::same_as<int> auto...) {};
    static_assert(fake::applicable_c<decltype(same_int), std::tuple<int, double>> == false);
}
```

---

## `fake::apply_result_t`

### 简介

一个类型别名，返回将函数对象应用于 tuple 的结果类型。

### 语法

```c++
template<typename _Functor, typename _Tuple>
using apply_result_t = decltype(std::apply(std::declval<_Functor>(), std::declval<_Tuple>()));
```

### 示例

```c++
#include <tuple>
#include <concepts>
#include "traits.h"

int main()
{
    auto lambda = [](auto...) { return 114514; };
    using result = fake::apply_result_t<decltype(lambda), std::tuple<char, double>>;
    static_assert(std::same_as<result, int>);
}
```

---

## `fake::conditional_v`

### 简介

一个变量模板，根据布尔条件选择两个值之一。
适用于任何可以存储在变量模板中的值（包括包装为值的类型，例如 `fake::mezz_v`）。

### 语法

```c++
template<std::convertible_to<bool> auto _Condition, auto _True, auto _False>
constexpr auto conditional_v = conditional<bool(_Condition), _True, _False>::value;
```

### 示例

```c++
#include "traits.h"

int main()
{
    static_assert(std::same_as<decltype(fake::conditional_v<false, 114, 514.0>), const double>);
}
```

---

## `fake::mimic_t`、`fake::gemma_t`

### 简介

- `mimic_t`：接受一个实例化的模板（例如 `std::tuple<int, double>`）和另一个模板（例如 `std::variant`），生成第二个模板使用相同类型参数的实例。
- `gemma_t`：通过重复指定类型一定次数来生成模板实例。

### 语法

```c++
template<typename _From, template<typename...> typename _To>
using mimic_t = typename mimic<std::remove_cvref_t<_From>, _To>::type;

template<std::size_t _size, typename _Arg, template<typename...> typename _Template>
using gemma_t = typename gemma<std::make_index_sequence<_size>, std::remove_cvref_t<_Arg>, _Template>::type;
```

### 示例

```c++
#include <tuple>
#include <variant>
#include <concepts>
#include "traits.h"

int main()
{
    static_assert(std::same_as<fake::mimic_t<std::tuple<int, double>, std::variant>, std::variant<int, double>>);
    static_assert(std::same_as<fake::gemma_t<4, int, std::tuple>, std::tuple<int, int, int, int>>);
}
```

---

## `fake::meets`、`fake::hold`、`fake::holds`、`fake::like`、`fake::likes`

### 简介

用于表达性类型匹配的概念：

- `meets<_Type, _Targets...>`：`_Type` 恰好是 `_Targets` 中的一个（匹配 cv‑ref）。
- `hold<_Type, _Target>`：要么 `_Type` 和 `_Target` 在移除引用后指向同一类型，要么 `_Target` 是一个 `mezz` 值，其接受类型模板参数的调用运算符求值为 `true`。
- `holds<_Type, _Targets...>`：对多个目标的 `hold`（OR 运算）。
- `like<_Type, _Target>`：类似于 `hold`，但比较前会从两侧移除 cv‑ref。
- `likes<_Type, _Targets...>`：对多个目标的 `like`（OR 运算）。

### 语法

```c++
template<typename _Type, typename _Target>
concept hold = /* ... */;

template<typename _Type, typename... _Targets>
concept holds = (fake::hold<_Type, _Targets> || ...);

template<typename _Type, typename _Target>
concept like = /* ... */;

template<typename _Type, typename... _Targets>
concept likes = (fake::like<_Type, _Targets> || ...);

template<typename _Type, typename... _Targets>
concept meets = (std::same_as<_Type, _Targets> || ...);
```

### 示例

```c++
#include <tuple>
#include <concepts>
#include "traits.h"

int main()
{
    // meets: exact type match, meets requires one of the exact same type (no cv‑ref removal).
    [](fake::meets<int, double> auto...) {}(114.0, 514);
    [](auto ..._e){
        static_assert(requires{ [](fake::meets<int, double> auto...){}(_e...); } == false);
    }(810, "1919");
    
    // hold: after removing reference, types match OR a mezz predicate succeeds.
    static_assert(fake::hold<int, int>);
    static_assert(fake::hold<int&, int>);
    static_assert(fake::hold<int&&, int>);
    static_assert(fake::hold<const int, const int>);
    static_assert(fake::hold<const int&, const int>);
    static_assert(fake::hold<const int&&, const int>);
    static_assert(fake::hold<int&, const int> == false);
    static_assert(fake::hold<const int&&, int> == false);
    
    // like: after removing cv‑ref from both sides, they match.
    static_assert(fake::like<int, int>);
    static_assert(fake::like<int&, int>);
    static_assert(fake::like<int&&, int>);
    static_assert(fake::like<const int, int>);
    static_assert(fake::like<const int&, int>);
    static_assert(fake::like<const int&&, int>);
    
    // Complex example using holds/likes with a mezz predicate.
    constexpr auto lambda = []<typename _T>() requires requires(_T _e) {
        { std::get<0>(_e) } -> fake::like<int>;
    } {
        return true;
    };
    
    double double_value = 114.0;
    auto tuple_value = std::make_tuple(514, "1919");
    auto reverse_value = std::make_tuple("1919", 514);
    
    // holds: the lambda is satisfied for tuple_value (tuple's first element is int)
    // For double, std::get<0> is ill‑formed, so the lambda is not satisfied for double alone, but holds uses OR with double type itself.
    // Actually the example is subtle: `holds<fake::mezz_t<lambda>, double> auto&...` means the pack elements must satisfy `hold` with the mezz or double.
    // `hold` for a mezz checks if the mezz's call operator with the type returns true.
    // So for double, hold<double, mezz> is false (since mezz expects a tuple-like), but hold<double, double> is true (since it's the same type after reference removal).
    // Therefore both double and tuple_value satisfy holds (double matches double, tuple_value matches mezz).
    [](fake::holds<fake::mezz_t<lambda>, double> auto&...){}(double_value, tuple_value);
    
    // lvalue const double does not satisfy holds with double. Actually const double& after removal is const double, not double, so hold<const double, double> is false. Hence fails.
    [](auto &&_e){
        static_assert(requires{ [](fake::holds<fake::mezz_t<lambda>, double> auto&){}(_e); } == false);
    }(std::as_const(double_value));
    
    // reverse_value does not satisfy mezz (first element is const char* not like<int>), and is not double, so fails.
    [](auto &&_e){
        static_assert(requires{ [](fake::holds<fake::mezz_t<lambda>, double> auto&){}(_e); } == false);
    }(reverse_value);
    
    // likes: similar but uses like concept, which removes cv‑ref from both sides.
    [](fake::likes<fake::mezz_t<lambda>, double> auto&...){}(double_value, tuple_value);
    [](fake::likes<fake::mezz_t<lambda>, double> auto&){}(std::as_const(double_value)); // const double& now satisfies like<double, double>
    [](auto &&_e){
        static_assert(requires{ [](fake::likes<fake::mezz_t<lambda>, double> auto&){}(_e); } == false);
    }(reverse_value);
}
```
