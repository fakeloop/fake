# `view.h`

- Compile-time string view with full constexpr support, based on variadic char templates.
- Provides string manipulation operations (find, replace, comparison, hashing) entirely at compile time.
- Includes utilities for converting between integers and string views, and for obtaining type/value names as views.

## `fake::view`

### brief

- A template class representing a compile-time fixed string as a sequence of `char` template parameters.
- All operations are `constexpr` and usable in constant expressions.

### syntax

```c++
template<char... _Chars>
struct view;
```

### example

```c++
#include "view.h"

int main()
{
    using hello_view = fake::view<'H', 'e', 'l', 'l', 'o'>;
    static_assert(hello_view::size() == 5);
    static_assert(hello_view::front() == 'H');
}
```

## `fake::view_v`

### brief

- A variable template that creates a `fake::view` instance from a string literal.
- The string literal must be passed as a template argument.

### syntax

```c++
template<fake::detail::view::string _Value>
constexpr auto view_v = make_view<_Value>();
```

### example

```c++
#include "view.h"

int main()
{
    constexpr fake::view_c auto foo = fake::view_v<"foo!">;
    static_assert(foo.size() == 4);
}
```

## `fake::view_c`

### brief

- A concept that checks whether a type is an instantiation of `fake::view`.

### syntax

```c++
template<typename _Type>
concept view_c = is_view_v<_Type>;
```

### example

```c++
#include "view.h"

int main()
{
    constexpr auto v = fake::view_v<"hello">;
    static_assert(fake::view_c<decltype(v)>);
    static_assert(!fake::view_c<int>);
}
```

## `fake::literals::operator""_v`

### brief

- User-defined literal operator that creates a `fake::view` from a string literal.
- Placed in namespace `fake::literals`.

### syntax

```c++
template<fake::detail::view::string _Value>
constexpr auto operator""_v() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr fake::view_c auto bar = "bar!"_v;
    static_assert(bar.size() == 4);
    static_assert(bar.front() == 'b');
}
```

## `fake::view_t`

### brief

- A type alias that extracts the `fake::view` type corresponding to a given string literal.

### syntax

```c++
template<fake::detail::view::string _Value>
using view_t = std::remove_cvref_t<decltype(view_v<_Value>)>;
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    using FooView = fake::view_t<"foo!">;
    constexpr FooView foo{};
    static_assert(foo == "foo!"_v);
}
```

## `fake::view::data()`

### brief

- Returns a pointer to a null-terminated character array holding the string.

### syntax

```c++
static constexpr decltype(auto) data() noexcept;
```

### example

```c++
#include "view.h"
#include <string_view>

int main()
{
    constexpr auto foo = fake::view_v<"foo!">;
    constexpr std::string_view sv{foo.data()};
    static_assert(sv == "foo!");
}
```

## `fake::view::utf8()`

### brief

- Returns a pointer to a null-terminated UTF-8 character array (`char8_t`).

### syntax

```c++
static constexpr decltype(auto) utf8() noexcept;
```

### example

```c++
#include "view.h"
#include <string_view>

int main()
{
    constexpr auto foo = fake::view_v<"foo!">;
    constexpr std::u8string_view u8{foo.utf8()};
    static_assert(u8 == u8"foo!");
}
```

## `fake::view::size()` / `fake::view::length()`

### brief

- Returns the number of characters in the view (excluding the null terminator).

### syntax

```c++
static constexpr std::size_t size() noexcept;
static constexpr std::size_t length() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    constexpr auto foo = fake::view_v<"foo!">;
    static_assert(foo.size() == 4);
    static_assert(foo.length() == 4);
}
```

## `fake::view::at()`, `front()`, `back()`

### brief

- Access a character at a given index, or the first/last character.

### syntax

```c++
template<std::size_t _Index>
static constexpr char at() noexcept;

static constexpr char front() noexcept;
static constexpr char back() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    constexpr auto foo = fake::view_v<"foo!">;
    static_assert(foo.at<1>() == 'o');
    static_assert(foo.front() == 'f');
    static_assert(foo.back() == '!');
}
```

## `fake::view::empty()`

### brief

- Checks whether the view is empty (contains zero characters).

### syntax

```c++
static constexpr bool empty() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    constexpr auto foo = fake::view_v<"foo!">;
    constexpr auto empty = fake::view_v<"">;
    static_assert(foo.empty() == false);
    static_assert(empty.empty() == true);
}
```

## `fake::view::hash()`

### brief

- Computes a 64‑bit FNV‑1a hash of the string.

### syntax

```c++
static constexpr unsigned long long hash() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    constexpr auto foo = fake::view_v<"foo!">;
    static_assert(foo.hash() == 15929882213276287266ULL);
}
```

## `fake::view::starts_with()`

### brief

- Checks whether the view starts with a given prefix.
- The prefix can be provided as another `fake::view` or as a string literal (wrapped via `fake::detail::view::string`).

### syntax

```c++
template<fake::view _Value>
static constexpr bool starts_with() noexcept;

template<fake::detail::view::string _Value>
static constexpr bool starts_with() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr auto foo = fake::view_v<"foo!">;
    static_assert(foo.starts_with<"foo">());
    static_assert(foo.starts_with<"foo"_v>());
}
```

## `fake::view::ends_with()`

### brief

- Checks whether the view ends with a given suffix.

### syntax

```c++
template<fake::view _Value>
static constexpr bool ends_with() noexcept;

template<fake::detail::view::string _Value>
static constexpr bool ends_with() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr auto foo = fake::view_v<"foo!">;
    static_assert(foo.ends_with<"oo!">());
    static_assert(foo.ends_with<"oo!"_v>());
}
```

## `fake::view::contains()`

### brief

- Checks whether the view contains a given substring.

### syntax

```c++
template<fake::view _Value>
static constexpr bool contains() noexcept;

template<fake::detail::view::string _Value>
static constexpr bool contains() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr auto foo = fake::view_v<"foo!">;
    static_assert(foo.contains<"oo">());
    static_assert(foo.contains<"oo"_v>());
}
```

## `fake::view::find()`

### brief

- Finds the first occurrence of a substring, starting from an optional index.
- Returns the index or `npos` if not found.

### syntax

```c++
template<fake::view _Value, std::size_t _Index = 0>
static constexpr std::size_t find() noexcept;

template<fake::detail::view::string _Value, std::size_t _Index = 0>
static constexpr std::size_t find() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr auto foo = fake::view_v<"foo!">;
    static_assert(foo.find<"o">() == 1);
    static_assert(foo.find<"o"_v>() == 1);
}
```

## `fake::view::rfind()`

### brief

- Finds the last occurrence of a substring, starting from an optional index (default `npos`).

### syntax

```c++
template<fake::view _Value, std::size_t _Index = npos>
static constexpr std::size_t rfind() noexcept;

template<fake::detail::view::string _Value, std::size_t _Index = npos>
static constexpr std::size_t rfind() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr auto foo = fake::view_v<"foo!">;
    static_assert(foo.rfind<"o">() == 2);
    static_assert(foo.rfind<"o"_v>() == 2);
}
```

## `fake::view::find_first_of()`, `find_last_of()`, `find_first_not_of()`, `find_last_not_of()`

### brief

- Locate the first/last character that matches (or does not match) any character in a given set.

### syntax

```c++
template<fake::view _Value, std::size_t _Index = 0>
static constexpr std::size_t find_first_of() noexcept;
// similar for find_last_of, find_first_not_of, find_last_not_of
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr auto bar = "bar!"_v;
    static_assert(bar.find_first_of<"ar">() == 1);
    static_assert(bar.find_first_of<"ar"_v>() == 1);
    static_assert(bar.find_last_of<"ar">() == 2);
    static_assert(bar.find_last_of<"ar"_v>() == 2);
    static_assert(bar.find_first_not_of<"b!">() == 1);
    static_assert(bar.find_first_not_of<"b!"_v>() == 1);
    static_assert(bar.find_last_not_of<"b!">() == 2);
    static_assert(bar.find_last_not_of<"b!"_v>() == 2);
}
```

## `fake::view::substr()`

### brief

- Returns a new view that is a substring starting at `_Index` with length `_Size` (default until the end).

### syntax

```c++
template<std::size_t _Index, std::size_t _Size = npos>
static constexpr auto substr() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr auto foo = "foo!"_v;
    static_assert(foo.substr<1>() == "oo!"_v);
    static_assert(foo.substr<1, 2>() == "oo"_v);
}
```

## `fake::view::replace()`

### brief

- Replaces all occurrences of a substring `_What` with `_With`, starting from `_Index`.
- Multiple overloads accept either `fake::view` or string literal for both arguments.

### syntax

```c++
template<fake::view _What, fake::view _With, std::size_t _Index = 0>
static constexpr auto replace() noexcept;
// other combinations with fake::detail::view::string
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr auto foo = "foo!"_v;
    static_assert(foo.replace<"o", "oo">() == "foooo!"_v);
    static_assert(foo.replace<"o", "oo"_v>() == "foooo!"_v);
    static_assert(foo.replace<"o"_v, "oo">() == "foooo!"_v);
    static_assert(foo.replace<"o"_v, "oo"_v>() == "foooo!"_v);
    static_assert(foo.replace<"o", "oo", 2>() == "fooo!"_v);
}
```

## `fake::view::append()`

### brief

- Concatenates another view (or string literal) to the current view.

### syntax

```c++
template<fake::view _Value>
static constexpr auto append() noexcept;

template<fake::detail::view::string _Value>
static constexpr auto append() noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr auto foo = "foo!"_v;
    static_assert(foo.append<"!!">() == "foo!!!"_v);
    static_assert(foo.append<"!!"_v>() == "foo!!!"_v);
}
```

## Operators `+`, `==`, `!=`, `<=>`

### brief

- `operator+` concatenates two views into a new view.
- Comparison operators compare the underlying strings lexicographically.

### syntax

```c++
template<char... _Lhs, char... _Rhs>
constexpr auto operator+(view<_Lhs...>, view<_Rhs...>) noexcept;

template<char... _Lhs, char... _Rhs>
constexpr bool operator==(view<_Lhs...>, view<_Rhs...>) noexcept;

template<char... _Lhs, char... _Rhs>
constexpr bool operator!=(view<_Lhs...>, view<_Rhs...>) noexcept;

template<char... _Lhs, char... _Rhs>
constexpr auto operator<=>(view<_Lhs...>, view<_Rhs...>) noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    constexpr auto foo = "foo!"_v;
    constexpr auto bar = "bar!"_v;
    static_assert(foo != bar);
    static_assert(foo + bar == "foo!bar!"_v);
    static_assert(bar < foo);
    static_assert(foo >= bar);
}
```

## `fake::to_view_v`

### brief

- Converts an integral value (or `std::integral_constant`) to a `fake::view` representing its string form.
- Supports arbitrary base between 2 and 36 (default 10).

### syntax

```c++
template<auto _integral, std::size_t _base = 10>
constexpr auto to_view_v = to_view<_integral, _base>();
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    static_assert(fake::to_view_v<114514> == "114514"_v);
    static_assert(fake::to_view_v<114514, 16> == "1BF52"_v);
    static_assert(fake::to_view_v<-114514> == "-114514"_v);
    static_assert(fake::to_view_v<-114514, 16> == "-1BF52"_v);
}
```

## `fake::from_view_v`

### brief

- Converts a `fake::view` (representing an integer in a given base) to an integral value.
- Detects sign from a leading `'-'`.

### syntax

```c++
template<fake::view_c auto _v, std::size_t _base = 10>
consteval auto from_view_v = from_view<_v, _base>();
```

### example

```c++
#include "view.h"

int main()
{
    using namespace fake::literals;
    static_assert(fake::from_view_v<"114514"_v> == 114514);
    static_assert(fake::from_view_v<"1BF52"_v, 16> == 114514);
    static_assert(fake::from_view_v<"-114514"_v> == -114514);
    static_assert(fake::from_view_v<"-1BF52"_v, 16> == -114514);
}
```

## `fake::type_view`

### brief

- Returns a `fake::view` containing the human‑readable name of a type.
- Accepts either a `fake::pack_c` (type package) or an instance of a type.

### syntax

```c++
constexpr auto type_view(fake::pack_c auto _pack) noexcept;
constexpr auto type_view(auto _type) noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    struct local{};
    local dummy;
    
    static_assert(fake::type_view(fake::pack_v<local>) == "main()::local"_v);
    static_assert(fake::type_view(std::as_const(dummy)) == "main()::local"_v);
}
```

## `fake::value_view`

### brief

- Returns a `fake::view` containing the string representation of a compile‑time value.
- The value is wrapped in a `fake::mezz_c` (e.g., via `fake::mezz_v`).

### syntax

```c++
constexpr auto value_view(fake::mezz_c auto _mezz) noexcept;
```

### example

```c++
#include "view.h"

int main()
{
    static_assert(fake::value_view(fake::mezz_v<114514.0>) == "1.14514e+5"_v);
}
```

## `fake::self_view`

### brief

- Returns a `fake::view` containing the name of a lambda, function, or scope where it is invoked.
- Based on `fake::symbol::self`.

### syntax

```c++
constexpr auto self_view(auto &&_lambda) noexcept;
```

### example

```c++
#include "view.h"

struct global
{
    static constexpr fake::view_c auto self = fake::self_view([]{});
    static constexpr auto function() noexcept {
        return fake::self_view([]{});
    }
};

int main()
{
    static_assert(fake::self_view([]{}) == "main()"_v);
    static_assert(global::self == "global"_v);
    static_assert(global::function() == "global::function()"_v);
}
```
