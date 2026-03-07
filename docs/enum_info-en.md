# `enum_info.h`

- Compile-time utilities for working with enumerations, providing bidirectional mapping between enumerators and their names.

## `fake::enum_info` – basic creation

### brief

- `enum_info` is a template that captures a set of enumerators of a given enumeration type. It can be created either from a single enumerator (using `enum_info_for_v`) or from an explicit list of enumerators (using `enum_info_v`).

### syntax

```c++
template<auto... _enums>
requires std::is_enum_v<fake::merge_t<decltype(_enums)...>>
struct enum_info;

template<auto _enum>
constexpr fake::enum_info enum_info_for_v;

template<auto... _enums>
inline constexpr enum_info<_enums...> enum_info_v;
```

### example

```c++
#include "enum_info.h"

namespace ns
{
    enum struct enumerate { none, foo, bar, fuz, buz, yoo, yar };
    enum struct bitsfield { none = 0, foo = 1, bar = 2, fuz = 4, buz = 8, yoo = 16, yar = 32 };
}

int main()
{
    // Create an enum_info object containing all enumerators of ns::enumerate
    // that are <= ns::enumerate::yar (here it includes all from 0 to yar)
    // Do NOT pass an enumerate like 0x100000 here, since that would generate
    // 1048576 template instantiations
    constexpr fake::enum_info info = fake::enum_info_for_v<ns::enumerate::yar>;
    
    // Create an enum_info object containing an explicit list of enumerators
    constexpr fake::enum_info bits = fake::enum_info_v<
        ns::bitsfield::none,
        ns::bitsfield::foo,
        ns::bitsfield::bar,
        ns::bitsfield::fuz,
        ns::bitsfield::buz,
        ns::bitsfield::yoo,
        ns::bitsfield::yar
    >;
    
    // Both objects are now ready for name/value queries.
    static_assert(info.size == 7); // all enumerators from none to yar
    static_assert(bits.size == 7); // explicit list
}
```

## `fake::enum_info::operator[]` – lookup enumerator by name (compile‑time view)

### brief

- Retrieve the enumerator value corresponding to a fully qualified name given as a `fake::view` compile‑time string (e.g. `"ns::enumerate::foo"_v`). Returns `std::nullopt` if the name is not found.

### syntax

```c++
template<fake::view_c auto _name>
consteval std::optional<type> operator[](fake::view_c auto _name) const noexcept;
```

### example

```c++
#include "enum_info.h"
#include "view.h"        // for _v literal
using namespace fake::literals;

namespace ns
{
    enum struct enumerate { none, foo, bar, fuz, buz, yoo, yar };
    enum struct bitsfield { none = 0, foo = 1, bar = 2, fuz = 4, buz = 8, yoo = 16, yar = 32 };
}

int main()
{
    constexpr fake::enum_info info = fake::enum_info_for_v<ns::enumerate::yar>;
    constexpr fake::enum_info bits = fake::enum_info_v<
        ns::bitsfield::none,
        ns::bitsfield::foo,
        ns::bitsfield::bar,
        ns::bitsfield::fuz,
        ns::bitsfield::buz,
        ns::bitsfield::yoo,
        ns::bitsfield::yar
    >;
    
    static_assert(info["ns::enumerate::foo"_v] == ns::enumerate::foo);
    static_assert(bits["ns::bitsfield::foo"_v] == ns::bitsfield::foo);
    static_assert(info["wrong key"_v] == std::nullopt);
    static_assert(bits["wrong key"_v] == std::nullopt);
}
```

## `fake::enum_info::operator[]` – lookup enumerator by name (runtime string)

### brief

- Retrieve the enumerator value corresponding to a fully qualified name given as a runtime string (`std::string_view`, `const char*`, or `std::string`). Returns `std::nullopt` if the name is not found.

### syntax

```c++
constexpr std::optional<type> operator[](std::string_view _name) const noexcept;
constexpr std::optional<type> operator[](const char* _name) const noexcept;  // via implicit conversion
```

### example

```c++
#include "enum_info.h"

namespace ns
{
    enum struct enumerate { none, foo, bar, fuz, buz, yoo, yar };
    enum struct bitsfield { none = 0, foo = 1, bar = 2, fuz = 4, buz = 8, yoo = 16, yar = 32 };
}

int main()
{
    constexpr fake::enum_info info = fake::enum_info_for_v<ns::enumerate::yar>;
    constexpr fake::enum_info bits = fake::enum_info_v<
        ns::bitsfield::none,
        ns::bitsfield::foo,
        ns::bitsfield::bar,
        ns::bitsfield::fuz,
        ns::bitsfield::buz,
        ns::bitsfield::yoo,
        ns::bitsfield::yar
    >;
    
    static_assert(info["ns::enumerate::foo"] == ns::enumerate::foo);
    static_assert(bits["ns::bitsfield::foo"] == ns::bitsfield::foo);
    static_assert(info["wrong key"] == std::nullopt);
    static_assert(bits["wrong key"] == std::nullopt);
}
```

## `fake::enum_info::operator[]` – lookup name by enumerator value (compile‑time mezzanine)

### brief

- Obtain the fully qualified name of an enumerator given as a `fake::mezz_v` wrapper. Returns an empty view if the value is not part of the enumeration set.

### syntax

```c++
template<type _enum>
consteval auto operator[](fake::value_mezzanine<_enum>) const noexcept;
```

### example

```c++
#include "enum_info.h"
#include "view.h"
using namespace fake::literals;

namespace ns
{
    enum struct enumerate { none, foo, bar, fuz, buz, yoo, yar };
    enum struct bitsfield { none = 0, foo = 1, bar = 2, fuz = 4, buz = 8, yoo = 16, yar = 32 };
}

int main()
{
    constexpr fake::enum_info info = fake::enum_info_for_v<ns::enumerate::yar>;
    constexpr fake::enum_info bits = fake::enum_info_v<
        ns::bitsfield::none,
        ns::bitsfield::foo,
        ns::bitsfield::bar,
        ns::bitsfield::fuz,
        ns::bitsfield::buz,
        ns::bitsfield::yoo,
        ns::bitsfield::yar
    >;
    
    static_assert(info[fake::mezz_v<ns::enumerate::foo>] == "ns::enumerate::foo"_v);
    static_assert(bits[fake::mezz_v<ns::bitsfield::foo>] == "ns::bitsfield::foo"_v);
    static_assert(info[fake::mezz_v<static_cast<ns::enumerate>(114514)>] == ""_v);
    static_assert(bits[fake::mezz_v<static_cast<ns::bitsfield>(114514)>] == ""_v);
}
```

## `fake::enum_info::operator[]` – lookup name by enumerator value (runtime)

### brief

- Obtain the fully qualified name of an enumerator given as a runtime enumeration value. Returns an empty string view if the value is not part of the enumeration set.

### syntax

```c++
constexpr std::string_view operator[](type _enum) const noexcept;
```

### example

```c++
#include "enum_info.h"

namespace ns
{
    enum struct enumerate { none, foo, bar, fuz, buz, yoo, yar };
    enum struct bitsfield { none = 0, foo = 1, bar = 2, fuz = 4, buz = 8, yoo = 16, yar = 32 };
}

int main()
{
    constexpr fake::enum_info info = fake::enum_info_for_v<ns::enumerate::yar>;
    constexpr fake::enum_info bits = fake::enum_info_v<
        ns::bitsfield::none,
        ns::bitsfield::foo,
        ns::bitsfield::bar,
        ns::bitsfield::fuz,
        ns::bitsfield::buz,
        ns::bitsfield::yoo,
        ns::bitsfield::yar
    >;
    
    static_assert(info[ns::enumerate::foo] == "ns::enumerate::foo");
    static_assert(bits[ns::bitsfield::foo] == "ns::bitsfield::foo");
    static_assert(info[static_cast<ns::enumerate>(114514)] == "");
    static_assert(bits[static_cast<ns::bitsfield>(114514)] == "");
}
```

## `fake::enum_info::core` – lookup enumerator by unqualified name

### brief

- Retrieve the enumerator value corresponding to an unqualified name (without the `ns::` prefix). Accepts `std::string_view`, `std::string`, or `const char*`. Returns `std::nullopt` if the name is not found.

### syntax

```c++
constexpr std::optional<type> core(std::string_view _name) const noexcept;
constexpr std::optional<type> core(const std::string &_name) const noexcept;
template<std::size_t _size>
constexpr std::optional<type> core(const char (&_name)[_size]) const noexcept;
```

### example

```c++
#include "enum_info.h"
#include <string>

namespace ns
{
    enum struct enumerate { none, foo, bar, fuz, buz, yoo, yar };
    enum struct bitsfield { none = 0, foo = 1, bar = 2, fuz = 4, buz = 8, yoo = 16, yar = 32 };
}

int main()
{
    constexpr fake::enum_info info = fake::enum_info_for_v<ns::enumerate::yar>;
    constexpr fake::enum_info bits = fake::enum_info_v<
        ns::bitsfield::none,
        ns::bitsfield::foo,
        ns::bitsfield::bar,
        ns::bitsfield::fuz,
        ns::bitsfield::buz,
        ns::bitsfield::yoo,
        ns::bitsfield::yar
    >;
    
    // std::string_view
    static_assert(info.core(std::string_view{"foo"}) == ns::enumerate::foo);
    static_assert(bits.core(std::string_view{"foo"}) == ns::bitsfield::foo);
    static_assert(info.core(std::string_view{"wrong key"}) == std::nullopt);
    static_assert(bits.core(std::string_view{"wrong key"}) == std::nullopt);
    
    // std::string
    static_assert(info.core(std::string{"foo"}) == ns::enumerate::foo);
    static_assert(bits.core(std::string{"foo"}) == ns::bitsfield::foo);
    static_assert(info.core(std::string{"wrong key"}) == std::nullopt);
    static_assert(bits.core(std::string{"wrong key"}) == std::nullopt);
    
    // const char*
    static_assert(info.core("foo") == ns::enumerate::foo);
    static_assert(bits.core("foo") == ns::bitsfield::foo);
    static_assert(info.core("wrong key") == std::nullopt);
    static_assert(bits.core("wrong key") == std::nullopt);
}
```

## `fake::enum_info::core` – lookup unqualified name by enumerator value

### brief

- Obtain the unqualified name (without namespace prefix) of an enumerator given its value. Returns an empty string if the value is not part of the enumeration set.

### syntax

```c++
constexpr std::string_view core(type _enum) const noexcept;
```

### example

```c++
#include "enum_info.h"

namespace ns
{
    enum struct enumerate { none, foo, bar, fuz, buz, yoo, yar };
    enum struct bitsfield { none = 0, foo = 1, bar = 2, fuz = 4, buz = 8, yoo = 16, yar = 32 };
}

int main()
{
    constexpr fake::enum_info info = fake::enum_info_for_v<ns::enumerate::yar>;
    constexpr fake::enum_info bits = fake::enum_info_v<
        ns::bitsfield::none,
        ns::bitsfield::foo,
        ns::bitsfield::bar,
        ns::bitsfield::fuz,
        ns::bitsfield::buz,
        ns::bitsfield::yoo,
        ns::bitsfield::yar
    >;
    
    static_assert(info.core(ns::enumerate::foo) == "foo");
    static_assert(bits.core(ns::bitsfield::foo) == "foo");
    static_assert(info.core(static_cast<ns::enumerate>(114514)) == "");
    static_assert(bits.core(static_cast<ns::bitsfield>(114514)) == "");
}
```

## Additional members: `size`, `enums`, `names`

### brief

- The `enum_info` template also provides compile‑time access to the underlying sequence of enumerators and their names via static members:
  - `static constexpr std::size_t size` – number of enumerators.
  - `using enums` – a `std::tuple` of `fake::mezz_t` for each enumerator value.
  - `using names` – a `std::tuple` of `fake::view` for each fully qualified name.

### example

```c++
#include "enum_info.h"
#include <type_traits>
#include <tuple>

namespace ns
{
    enum struct enumerate { none, foo, bar };
}

int main()
{
    constexpr fake::enum_info info = fake::enum_info_v<ns::enumerate::none, ns::enumerate::foo, ns::enumerate::bar>;
    
    static_assert(info.size == 3);
    
    using Enums = decltype(info)::enums; // std::tuple<fake::mezz_t<ns::enumerate::none>, ...>
    using Names = decltype(info)::names; // std::tuple<fake::view_t<"ns::enumerate::none">, ...>
    
    static_assert(std::tuple_size_v<Enums> == 3);
    static_assert(std::tuple_size_v<Names> == 3);
    static_assert(std::is_same_v<std::tuple_element_t<0, Enums>, fake::mezz_t<ns::enumerate::none>>);
    static_assert(std::is_same_v<std::tuple_element_t<0, Names>, fake::view_t<"ns::enumerate::none">>);
}
```
