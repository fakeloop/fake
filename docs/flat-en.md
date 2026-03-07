# `flat.h`

- Provides compile-time heterogeneous containers: `mate` (pair-like), `flat` (indexed tuple), and `query` (key-value map).
- Implements empty-base optimization and static storage for `mezz` keys to minimize size.

---

## `fake::mate`

### brief

A pair-like template that stores two values. Multiple specializations optimize storage:

- If the first type is a `mezz` (compile-time value), it is stored as a static member, saving space.
- If either type is empty, `[[no_unique_address]]` is applied.
- If either type is `void`, the corresponding member is omitted.

### syntax

```c++
template<typename First, typename Second>
struct mate;

// Specializations exist for various combinations of mezz, empty, and void types.
```

### example

```c++
#include "flat.h"
#include "view.h"

int main()
{
    using namespace fake::literals;
    
    // Empty types (like views) yield size 1 when paired together
    static_assert(std::is_empty_v<decltype("foo"_v)> && std::is_empty_v<decltype("bar"_v)>);
    static_assert(sizeof(fake::mate{"foo"_v, "bar"_v}) == 1);      // both empty → size 1
    
    // One empty, one non‑empty → size of the non‑empty member (8)
    static_assert(sizeof(fake::mate{"foo"_v, 114514uz}) == 8);
    
    // Both non‑empty → sum of sizes (8+8=16 on 64‑bit)
    static_assert(sizeof(fake::mate{114, 514uz}) == 16);
    
    // If the first type is a mezz, its instance is static and shared
    constexpr fake::mate<fake::mezz_t<114514>, std::size_t> foo{1919810uz}, bar{1919810uz};
    static_assert(sizeof(foo) == 8);                                   // mezz key is static, only second member stored
    static_assert(std::addressof(foo.first) == std::addressof(bar.first)); // same static mezz object
    static_assert(std::addressof(foo.second) != std::addressof(bar.second)); // distinct second members
    
    // Structured binding works (tuple-like)
    constexpr auto result = [](auto _e){
        const auto &[first, second] = _e;
        return std::tuple{first, second};
    }(foo);
    
    static_assert(std::same_as<std::tuple_element_t<0uz, decltype(result)>, const fake::mezz_t<114514>>);
    static_assert(std::get<0>(result).value == 114514 && std::get<1>(result) == 1919810uz);
}
```

---

## `fake::flat`

### brief

A compile‑time heterogeneous sequence. It stores its elements as `mate<mezz_t<Index>, T>` bases, allowing **compile-time dispatched** `O(1)` access by index or by type.
`flat` satisfies the `flat_c` concept and works with structured bindings, `std::tuple_size`, and `std::tuple_element`.
Under the premise that all its elements are structural, `flat` itself is also structural.

### syntax

```c++
template<typename... Args>
struct flat : detail::flat::broker<std::index_sequence_for<Args...>, Args...> {
    static constexpr std::size_t size = sizeof...(Args);
    
    template<std::size_t Index>
    static consteval auto type(std::integral_constant<std::size_t, Index>) noexcept -> pack_c;
    
    template<std::size_t Index>
    constexpr auto& value() & noexcept;
    
    template<typename Type>
    constexpr auto& value() & noexcept;
    
    // Omit the interface version composed of const and && here
    /* ... */
};

template<typename T>
concept flat_c = /* ... */;

template<typename... Args>
constexpr auto make_flat(Args&&... args) -> flat</* decayed types */>;
```

### example

```c++
#include "flat.h"
#include "view.h"

int main()
{
    using namespace fake::literals;
    
    constexpr fake::flat_c auto flat = fake::make_flat(fake::mezz_v<114514>, 1919uz, 810.0);
    
    // `flat` itself is a structural type (mezz holds its value)
    static_assert(fake::mezz_c<fake::mezz_t<flat>>);
    
    // Access element type by index
    static_assert(std::same_as<fake::take_t<flat.type(fake::index_v<0>)>, const fake::mezz_t<114514>>);
    
    // Access value by index or by type
    static_assert(flat.value<0>().value == 114514);
    static_assert(flat.value<double>() == 810.0);
    static_assert(fake::get<1>(flat) == 1919uz);
    static_assert(fake::get<std::size_t>(flat) == 1919uz);
    
    // Size information
    static_assert(flat.size == 3);
    static_assert(fake::flat_size_v<decltype(flat)> == flat.size);
    static_assert(std::tuple_size_v<decltype(flat)> == flat.size);
    
    // Structured binding
    static_assert([](auto &&_e){ const auto &[a, b, c] = _e; return c; }(flat) == 810.0);
}
```

---

## `fake::flat_cat`

### brief

Concatenates multiple `flat` objects into a single `flat`.

### syntax

```c++
template<flat_based_c... Segments>
using flat_cat_result_t = /* concatenated flat type */;

constexpr auto flat_cat(flat_based_c auto&&... flats) -> flat_cat_result_t</* decayed types */>;
```

### example

```c++
#include "flat.h"
#include "view.h"

int main()
{
    using namespace fake::literals;
    
    constexpr auto flat = fake::make_flat(fake::mezz_v<114514>, 1919uz, 810.0);
    constexpr auto tail = fake::make_flat('@', 364.0);
    constexpr auto concat = fake::flat_cat(flat, tail);
    
    using flat_t = std::remove_cvref_t<decltype(flat)>;
    using tail_t = std::remove_cvref_t<decltype(tail)>;
    using concat_t = std::remove_cvref_t<decltype(concat)>;
    
    static_assert(std::same_as<fake::flat_cat_result_t<flat_t, tail_t>, concat_t>);
}
```

---

## `fake::subflat`

### brief

Extracts a contiguous subsequence of a `flat` as a new `flat`.

### syntax

```c++
template<std::size_t Index, std::size_t Size = std::size_t(-1)>
constexpr auto subflat(flat_based_c auto&& f);

template<flat_based_c Type, std::size_t Index, std::size_t Size = std::size_t(-1)>
using subflat_t = decltype(subflat<Index, Size>(std::declval<Type>()));
```

### example

```c++
#include "flat.h"
#include "view.h"

int main()
{
    using namespace fake::literals;
    
    constexpr auto flat = fake::make_flat(fake::mezz_v<114514>, 1919uz, 810.0);
    constexpr auto tail = fake::make_flat('@', 364.0);
    constexpr auto concat = fake::flat_cat(flat, tail);
    
    constexpr auto subflat = fake::subflat<1, 3>(concat);
    using subflat_t = std::remove_cvref_t<decltype(subflat)>;
    using concat_t = std::remove_cvref_t<decltype(concat)>;
    
    static_assert(std::same_as<fake::subflat_t<concat_t, 1, 3>, subflat_t>);
    static_assert(fake::get<2>(subflat) == '@');
}
```

---

## `fake::tie`

### brief

Creates a `flat` of references to the given arguments, similar to `std::tie`.

### syntax

```c++
constexpr auto tie(auto&... args) noexcept -> flat<decltype(args)...>;
```

### example

```c++
#include "flat.h"
#include "view.h"

int main()
{
    using namespace fake::literals;
    
    constexpr auto tail = fake::make_flat('@', 364.0);
    // tie creates a flat of references; here it captures references to the two elements of tail
    static_assert(fake::tie(fake::get<1>(tail), fake::get<0>(tail)).value<1>() == '@');
}
```

---

## `fake::query`

### brief

A compile‑time key‑value map. Each entry is a `mate<Key, Value>`. Keys can be any type (including `view` strings, `mezz`, or custom types).
Lookup is **compile-time dispatched** `O(1)`  via `operator[]`, `value<Key>()`, or `get<Key>()`.
`query` satisfies the `query_c` concept and works with structured bindings, `std::tuple_size`, and `std::tuple_element`.
Under the premise that all its elements are structural, `query` itself is also structural.

### syntax

```c++
template<mate_c... Mates>
struct query : Mates... {
    static constexpr std::size_t size = sizeof...(Mates);
    using first_types  = flat<typename Mates::first_type...>;
    using second_types = flat<typename Mates::second_type...>;
    
    template<typename Key>
    static consteval auto type() noexcept -> pack_c;   // yields the value type for Key
    
    template<typename Key>
    static consteval auto mate() noexcept -> pack_c;   // yields the whole mate for Key
    
    template<typename Key>
    static consteval std::size_t index() noexcept;     // index of Key
    
    template<typename Key>
    constexpr auto& value() & noexcept;                 // returns the mate for Key
    
    template<typename Key>
    constexpr auto& operator[](Key) & noexcept;         // returns the value for Key
    
    // Omit the interface version composed of const and && here
    /* ... */
};

template<typename T>
concept query_c = /* ... */;

template<typename Query, typename Key>
concept query_contains = /* ... */;
```

### example

```c++
#include "flat.h"
#include "view.h"

struct w {};

int main()
{
    using namespace fake::literals;
    
    constexpr fake::query_c auto query = fake::query{
        fake::mate{"x"_v, 114},
        fake::mate{"y"_v, 514.0f},
        fake::mate{"z"_v, 1919.0},
        fake::mate{w{}, "yaju"_v}
    };
    
    constexpr fake::query_c auto append = fake::query{
        fake::mate{"r"_v, w{}},
        fake::mate{"t"_v, 810uz}
    };
    
    // `query` itself is a structural type
    static_assert(fake::mezz_c<fake::mezz_t<query>>);
    
    // Type lookup by key
    static_assert(std::same_as<fake::take_t<query.type<w>()>, fake::view_t<"yaju">>);
    static_assert(std::same_as<fake::take_t<query.mate<w>()>, fake::mate<w, fake::view_t<"yaju">>>);
    
    // Index of a key
    static_assert(query.index<w>() == 3);
    
    // Access mate by key type
    static_assert(query.value<fake::view_t<"y">>() == fake::mate{"y"_v, 514.0f});
    
    // operator[] returns the value directly
    static_assert(query["x"_v] == 114 && query[w{}] == "yaju"_v);
    
    // Access by index or by key or by key type (through get)
    static_assert(fake::get<1>(query) == fake::mate{"y"_v, 514.0f});
    static_assert(fake::get<"y"_v>(query) == fake::mate{"y"_v, 514.0f});
    static_assert(fake::get<fake::view_t<"y">>(query) == fake::mate{"y"_v, 514.0f});
    
    // Size
    static_assert(fake::query_size_v<decltype(query)> == 4);
    
    // Structured binding
    static_assert([](auto &&_e){ const auto &[a, b, c, d] = _e; return c; }(query) == fake::mate{"z"_v, 1919.0});
    
    // query_contains concept
    static_assert(fake::query_contains<decltype(query), w>);
    static_assert(fake::query_contains<decltype(query), int> == false);
    
    // Concatenation (type only)
    using expect_t = fake::query<
        fake::mate<fake::view_t<"x">, int>,
        fake::mate<fake::view_t<"y">, float>,
        fake::mate<fake::view_t<"z">, double>,
        fake::mate<w, fake::view_t<"yaju">>,
        fake::mate<fake::view_t<"r">, w>,
        fake::mate<fake::view_t<"t">, std::size_t>
    >;
    using cat_t = fake::query_cat_result_t<std::remove_cvref_t<decltype(query)>, std::remove_cvref_t<decltype(append)>>;
    static_assert(std::same_as<expect_t, cat_t>);
}
```

---

## `fake::query_rebind`

### brief

Replaces the value type associated with a given key in a `query` type.

### syntax

```c++
template<query_c Query, auto Key, typename NewValue>
using query_rebind_t = /* new query type with updated value for Key */;
```

### example

```c++
#include "flat.h"
#include "view.h"

int main()
{
    using namespace fake::literals;
    
    using lhs = fake::query<
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ lhs">>,
        fake::mate<fake::view_t<"bar">, fake::view_t<"bar @ lhs">>,
        fake::mate<fake::view_t<"yoo">, fake::view_t<"yoo @ lhs">>
    >;
    
    // Rebind key "bar" to a new value
    using rebind = fake::query_rebind_t<lhs, "bar"_v, fake::view_t<"bar!">>;
    using expect_rebind = fake::query<
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ lhs">>,
        fake::mate<fake::view_t<"bar">, fake::view_t<"bar!">>,
        fake::mate<fake::view_t<"yoo">, fake::view_t<"yoo @ lhs">>
    >;
    static_assert(std::same_as<rebind, expect_rebind>);
}
```

---

## `fake::query_merge`

### brief

Merges two or more `query` types, keeping entries from the leftmost `query` when keys conflict.

### syntax

```c++
template<query_c... Queries>
using query_merge_t = /* merged query type */;
```

### example

```c++
#include "flat.h"
#include "view.h"

int main()
{
    using namespace fake::literals;
    
    using lhs = fake::query<
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ lhs">>,
        fake::mate<fake::view_t<"bar">, fake::view_t<"bar @ lhs">>,
        fake::mate<fake::view_t<"yoo">, fake::view_t<"yoo @ lhs">>
    >;
    
    using rhs = fake::query<
        fake::mate<fake::view_t<"fuz">, fake::view_t<"fuz @ rhs">>,
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ rhs">>,
        fake::mate<fake::view_t<"buz">, fake::view_t<"buz @ rhs">>
    >;
    
    using merge = fake::query_merge_t<lhs, rhs>;
    using expect_merge = fake::query<
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ lhs">>,   // lhs wins
        fake::mate<fake::view_t<"bar">, fake::view_t<"bar @ lhs">>,
        fake::mate<fake::view_t<"yoo">, fake::view_t<"yoo @ lhs">>,
        fake::mate<fake::view_t<"fuz">, fake::view_t<"fuz @ rhs">>,   // new from rhs
        fake::mate<fake::view_t<"buz">, fake::view_t<"buz @ rhs">>
    >;
    static_assert(std::same_as<merge, expect_merge>);
}
```

---

## `fake::query_diff`

### brief

Removes from the first `query` all keys that appear in the second `query`.

### syntax

```c++
template<query_c Lhs, query_c Rhs>
using query_diff_t = /* Lhs without keys present in Rhs */;
```

### example

```c++
#include "flat.h"
#include "view.h"

int main()
{
    using namespace fake::literals;
    
    using lhs = fake::query<
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ lhs">>,
        fake::mate<fake::view_t<"bar">, fake::view_t<"bar @ lhs">>,
        fake::mate<fake::view_t<"yoo">, fake::view_t<"yoo @ lhs">>
    >;
    
    using rhs = fake::query<
        fake::mate<fake::view_t<"fuz">, fake::view_t<"fuz @ rhs">>,
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ rhs">>,
        fake::mate<fake::view_t<"buz">, fake::view_t<"buz @ rhs">>
    >;
    
    // Keys shared with rhs are removed from lhs
    using diff_lr = fake::query_diff_t<lhs, rhs>;
    using expect_diff_lr = fake::query<
        fake::mate<fake::view_t<"bar">, fake::view_t<"bar @ lhs">>,
        fake::mate<fake::view_t<"yoo">, fake::view_t<"yoo @ lhs">>
    >;
    static_assert(std::same_as<diff_lr, expect_diff_lr>);
    
    // Similarly, keys shared with lhs are removed from rhs
    using diff_rl = fake::query_diff_t<rhs, lhs>;
    using expect_diff_rl = fake::query<
        fake::mate<fake::view_t<"fuz">, fake::view_t<"fuz @ rhs">>,
        fake::mate<fake::view_t<"buz">, fake::view_t<"buz @ rhs">>
    >;
    static_assert(std::same_as<diff_rl, expect_diff_rl>);
}
```

---

## `fake::query_roll`

### brief

Updates the first `query` with entries from the following `query`. Keys present in the following `query` replace the corresponding entries in the first. This is essentially a fold operation of `merge(diff(Lhs, Rhs), Rhs)`.

### syntax

```c++
template<query_c... Queries>
using query_roll_t = /* updated query type */;
```

### example

```c++
#include "flat.h"
#include "view.h"

int main()
{
    using namespace fake::literals;
    
    using lhs = fake::query<
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ lhs">>,
        fake::mate<fake::view_t<"bar">, fake::view_t<"bar @ lhs">>,
        fake::mate<fake::view_t<"yoo">, fake::view_t<"yoo @ lhs">>
    >;
    
    using rhs = fake::query<
        fake::mate<fake::view_t<"fuz">, fake::view_t<"fuz @ rhs">>,
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ rhs">>,
        fake::mate<fake::view_t<"buz">, fake::view_t<"buz @ rhs">>
    >;
    
    // Roll lhs with rhs: keep lhs entries not in rhs, then add all rhs entries
    using roll_lr = fake::query_roll_t<lhs, rhs>;
    using expect_roll_lr = fake::query<
        fake::mate<fake::view_t<"bar">, fake::view_t<"bar @ lhs">>, // old (not in rhs)
        fake::mate<fake::view_t<"yoo">, fake::view_t<"yoo @ lhs">>, // old
        fake::mate<fake::view_t<"fuz">, fake::view_t<"fuz @ rhs">>, // new
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ rhs">>, // new (replaces lhs)
        fake::mate<fake::view_t<"buz">, fake::view_t<"buz @ rhs">>  // new
    >;
    static_assert(std::same_as<roll_lr, expect_roll_lr>);
    
    // Roll rhs with lhs: keep rhs entries not in lhs, then add all lhs entries
    using roll_rl = fake::query_roll_t<rhs, lhs>;
    using expect_roll_rl = fake::query<
        fake::mate<fake::view_t<"fuz">, fake::view_t<"fuz @ rhs">>, // old
        fake::mate<fake::view_t<"buz">, fake::view_t<"buz @ rhs">>, // old
        fake::mate<fake::view_t<"foo">, fake::view_t<"foo @ lhs">>, // new (replaces rhs)
        fake::mate<fake::view_t<"bar">, fake::view_t<"bar @ lhs">>, // new
        fake::mate<fake::view_t<"yoo">, fake::view_t<"yoo @ lhs">>  // new
    >;
    static_assert(std::same_as<roll_rl, expect_roll_rl>);
}
```
