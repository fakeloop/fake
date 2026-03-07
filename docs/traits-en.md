# `traits.h`

- Advanced type traits and utilities for template metaprogramming.

---

## `fake::aligned_signed`, `fake::aligned_unsigned`

### brief

Type aliases that provide a signed/unsigned integer type with the same size as a pointer on the current platform.
Useful for pointer-sized integer operations.

### syntax

```c++
template<typename = void>
using aligned_signed = typename aligned_int<void>::signed_type;

template<typename = void>
using aligned_unsigned = typename aligned_int<void>::unsigned_type;
```

### example

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

### brief

A variable template that yields the number of bits of a given type (defaults to `void*`), i.e. the system’s pointer bit width.

### syntax

```c++
template<typename _T = void*>
constexpr std::size_t system_bit = sizeof(_T) << 3;
```

### example

```c++
#include "traits.h"

int main()
{
    static_assert(fake::system_bit<> == 64);   // 8 bytes * 8 = 64 bits
}
```

---

## `fake::signed_bytes`, `fake::unsigned_bytes`, `fake::signed_bits`, `fake::unsigned_bits`

### brief

Type aliases that map a byte count or bit count to the corresponding fixed‑width integer type.
The byte count must be a power of two between 1 and 8.
The bit count must be a multiple of 8 and a power of two, and the corresponding byte count must be valid.

### syntax

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

### example

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

## `fake::remove_lvalue_reference_t`, `fake::remove_rvalue_reference_t`

### brief

Utilities that remove only lvalue references or only rvalue references, leaving the other kind untouched.

### syntax

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

### example

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

### brief

Adds `const` to a type, preserving reference semantics: if the input is a reference, the result is a reference to `const` (lvalue or rvalue as appropriate).

### syntax

```c++
template<typename _Type>
using add_const_t = /* see implementation */;
```

### example

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

### brief

A concept that detects whether a type is a specialization of `std::integral_constant`.

### syntax

```c++
template<typename _Type>
concept constant_c = is_constant_v<_Type>;
```

### example

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

### brief

A variable template that performs logical AND of a pack of `bool` values.
It yields `true` if all values are `true`. Works with an arbitrary number of arguments.

### syntax

```c++
template<bool... _args>
constexpr bool conjunction_v = /* implementation */;
```

### example

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

### brief

If all types in a pack are identical, `merge_t` yields that common type; otherwise it is ill‑formed (SFINAE‑friendly).

### syntax

```c++
template<typename... _Types>
using merge_t = typename merge<_Types...>::type;
```

### example

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

## `fake::common_base_t`, `fake::common_derive_t`

### brief

- `common_base_t` finds a common unambiguous base class among a set of types. The first type is taken as the candidate base; subsequent types must be derived from it.
- `common_derive_t` finds a common most derived type among a set of types. The first type is taken as the candidate most derived; subsequent types must be base classes of it.

### syntax

```c++
template<typename... _Types>
using common_base_t = typename common_base<_Types...>::type;

template<typename... _Types>
using common_derive_t = typename common_derive<_Types...>::type;
```

### example

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

### brief

A simplified `std::visit` that accepts a variant and a set of lambdas. It combines the lambdas into a single overload set and dispatches.

### syntax

```c++
template<typename _Instance, typename... _Lambda>
constexpr auto visit(_Instance &&_instance, _Lambda&& ..._lambda);
```

### example

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

### brief

A variable template that creates a predicate checking whether a set of arguments can be accepted by any of the given lambdas (stored as `mezz` values).
The lambdas are tried in order; if one matches (i.e. the call is valid), `filter<...>(args...)` returns `true`; otherwise `false`.

### syntax

```c++
template<auto... _mezz>
inline constexpr auto filter = [](auto &&..._args) consteval -> bool { /* ... */ };
```

### example

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

## `fake::pair_c`, `fake::tuple_c`, `fake::combine_c`, `fake::aggregate_c`, `fake::std_array_c`

### brief

Concepts that identify specific kinds of types:

- `pair_c` : a specialization of `std::pair`
- `tuple_c` : a specialization of `std::tuple`
- `combine_c` : either a `pair` or a `tuple`
- `aggregate_c` : any aggregate type
- `std_array_c` : a specialization of `std::array`

### syntax

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

### example

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

## `fake::array_c`, `fake::array_value_t`, `fake::array_size_v`

### brief

Utilities for working with built‑in arrays (including zero‑length and incomplete arrays):

- `array_c` : concept that matches any array type.
- `array_value_t` : yields the element type of an array.
- `array_size_v` : yields the size of an array (0 for zero‑length arrays, ill‑formed for incomplete arrays).

### syntax

```c++
template<typename _Type>
concept array_c = is_array_v<_Type>;

template<array_c _Array>
using array_value_t = typename array_value<_Array>::type;

template<array_c _Array>
constexpr std::size_t array_size_v = array_size<_Array>::value;
```

### example

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

## `fake::optional_c`, `fake::variant_c`

### brief

Concepts that identify specializations of `std::optional` and `std::variant`.

### syntax

```c++
template<typename _Type>
concept optional_c = is_optional_v<_Type>;

template<typename _Type>
concept variant_c = is_variant_v<_Type>;
```

### example

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

### brief

A traits struct that extracts information from a `std::variant`:

- `types` : a `std::tuple` of the alternative types.
- `size`  : the number of alternatives.

### syntax

```c++
template<typename = void>
struct variant_info;

template<template<typename...> typename _Variant, typename... _Types>
struct variant_info<_Variant<_Types...>> { /* ... */ };
```

### example

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

### brief

A traits struct that inspects an instantiation of a class template. It provides:

- `value` : whether the type is a template instantiation.
- `is_array_like` / `is_view_like` : flags for array‑like (type + size) and view‑like (character pack) templates.
- `args` : a `std::tuple` of the type arguments (for classic templates).
- `arg` / `array_size` : for array‑like templates.
- `size` : number of template arguments.
- `type` : a template alias to reconstruct the template with different arguments.

### syntax

```c++
template<typename = void>
struct template_info;

// Specializations for classic templates, array‑like templates, and view‑like templates.
```

### example

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

### brief

Recursively removes all pointers, references, and cv‑qualifiers from a type, yielding the underlying “bare” type.

### syntax

```c++
template<typename _Type>
using bare_t = typename bare<_Type>::type;
```

### example

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

### brief

A concept that identifies a `std::reference_wrapper`.

### syntax

```c++
template<typename _Type>
concept ref_c = is_ref_v<_Type>;
```

### example

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

## `fake::remove_ref_t`, `fake::remove_ref_v`

### brief

- `remove_ref_t` : extracts the underlying type from a `std::reference_wrapper` or a plain reference.
- `remove_ref_v` : a function that returns a (lvalue) reference to the underlying object, stripping away the wrapper if present.
  *Note:* For rvalue arguments that are not a `reference_wrapper`, the function triggers a static assertion to prevent dangling references.

### syntax

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

### example

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

### brief

A concept that checks whether a template can be instantiated with a given set of type arguments.
If `_Template<_Type...>` is a valid type, the concept holds.

### syntax

```c++
template<template<typename...> typename _Template, typename... _Type>
concept deducible_c = requires{ typename _Template<_Type...>; };
```

### example

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

### brief

A concept that applies a predicate (stored as a `mezz` value) to each index in a sequence of size `_size`.
The predicate is a callable that takes an `index` argument and returns a boolean.
If the predicate returns `true` for every index, the concept holds.

### syntax

```c++
template<std::size_t _size, auto _concept>
concept concept_sequence_c = concept_sequence_v<_size, _concept>;
```

### example

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

### brief

A concept that checks whether a functor can be called with the elements of a tuple (as separate arguments).

### syntax

```c++
template<typename _Functor, typename _Tuple>
concept applicable_c = /* ... */;
```

### example

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

### brief

A type alias that yields the result type of applying a functor to a tuple.

### syntax

```c++
template<typename _Functor, typename _Tuple>
using apply_result_t = decltype(std::apply(std::declval<_Functor>(), std::declval<_Tuple>()));
```

### example

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

### brief

A variable template that selects one of two values based on a boolean condition.
Works with any value that can be stored in a variable template (including types wrapped as values, e.g. `fake::mezz_v`).

### syntax

```c++
template<std::convertible_to<bool> auto _Condition, auto _True, auto _False>
constexpr auto conditional_v = conditional<bool(_Condition), _True, _False>::value;
```

### example

```c++
#include "traits.h"

int main()
{
    static_assert(std::same_as<decltype(fake::conditional_v<false, 114, 514.0>), const double>);
}
```

---

## `fake::mimic_t`, `fake::gemma_t`

### brief

- `mimic_t` : takes an instantiated template (e.g. `std::tuple<int, double>`) and another template (e.g. `std::variant`) and produces an instantiation of the second template with the same type arguments.
- `gemma_t` : generates a template instantiation by repeating a given type a specified number of times.

### syntax

```c++
template<typename _From, template<typename...> typename _To>
using mimic_t = typename mimic<std::remove_cvref_t<_From>, _To>::type;

template<std::size_t _size, typename _Arg, template<typename...> typename _Template>
using gemma_t = typename gemma<std::make_index_sequence<_size>, std::remove_cvref_t<_Arg>, _Template>::type;
```

### example

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

## `fake::meets`, `fake::hold`, `fake::holds`, `fake::like`, `fake::likes`

### brief

Concepts for expressive type matching:

- `meets<_Type, _Targets...>` : `_Type` is exactly one of the `_Targets` (matches cv‑ref).
- `hold<_Type, _Target>` : either `_Type` and `_Target` refer to the same type after removing references, or `_Target` is a `mezz` value whose call operator (taking a type template argument) evaluates to `true`.
- `holds<_Type, _Targets...>` : `hold` for multiple targets (OR).
- `like<_Type, _Target>` : similar to `hold`, but compares after removing cv‑ref from both sides.
- `likes<_Type, _Targets...>` : `like` for multiple targets (OR).

### syntax

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

### example

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
