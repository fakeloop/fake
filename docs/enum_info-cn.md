# `enum_info.h`

- 用于处理枚举的编译期工具，提供枚举项与其名称之间的双向映射。

## `fake::enum_info` – 基本创建

### 简介

- `enum_info` 是一个模板，用于捕获给定枚举类型的一组枚举项。它可以通过单个枚举项（使用 `enum_info_for_v`）或显式的枚举项列表（使用 `enum_info_v`）来创建。

### 语法

```c++
template<auto... _enums>
requires std::is_enum_v<fake::merge_t<decltype(_enums)...>>
struct enum_info;

template<auto _enum>
constexpr fake::enum_info enum_info_for_v;

template<auto... _enums>
inline constexpr enum_info<_enums...> enum_info_v;
```

### 示例

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

## `fake::enum_info::operator[]` – 通过名称查找枚举项（编译期视图）

### 简介

- 根据给定的完全限定名称（作为 `fake::view` 编译期字符串，例如 `"ns::enumerate::foo"_v`）返回对应的枚举项值。如果未找到该名称，则返回 `std::nullopt`。

### 语法

```c++
template<fake::view_c auto _name>
consteval std::optional<type> operator[](fake::view_c auto _name) const noexcept;
```

### 示例

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

## `fake::enum_info::operator[]` – 通过名称查找枚举项（运行期字符串）

### 简介

- 根据给定的完全限定名称（作为运行期字符串 `std::string_view`、`const char*` 或 `std::string`）返回对应的枚举项值。如果未找到该名称，则返回 `std::nullopt`。

### 语法

```c++
constexpr std::optional<type> operator[](std::string_view _name) const noexcept;
constexpr std::optional<type> operator[](const char* _name) const noexcept;  // via implicit conversion
```

### 示例

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

## `fake::enum_info::operator[]` – 通过枚举项值查找名称（编译期夹层）

### 简介

- 根据以 `fake::mezz_v` 包装器形式给出的枚举项值，获取其完全限定名称。如果该值不属于当前枚举集合，则返回一个空视图。

### 语法

```c++
template<type _enum>
consteval auto operator[](fake::value_mezzanine<_enum>) const noexcept;
```

### 示例

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

## `fake::enum_info::operator[]` – 通过枚举项值查找名称（运行期）

### 简介

- 根据给定的运行期枚举项值，获取其完全限定名称。如果该值不属于当前枚举集合，则返回一个空字符串视图。

### 语法

```c++
constexpr std::string_view operator[](type _enum) const noexcept;
```

### 示例

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

## `fake::enum_info::core` – 通过非限定名称查找枚举项

### 简介

- 根据给定的非限定名称（不带 `ns::` 前缀）返回对应的枚举项值。接受 `std::string_view`、`std::string` 或 `const char*`。如果未找到该名称，则返回 `std::nullopt`。

### 语法

```c++
constexpr std::optional<type> core(std::string_view _name) const noexcept;
constexpr std::optional<type> core(const std::string &_name) const noexcept;
template<std::size_t _size>
constexpr std::optional<type> core(const char (&_name)[_size]) const noexcept;
```

### 示例

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

## `fake::enum_info::core` – 通过枚举项值查找非限定名称

### 简介

- 根据枚举项的值，获取其非限定名称（不带命名空间前缀）。如果该值不属于当前枚举集合，则返回一个空字符串。

### 语法

```c++
constexpr std::string_view core(type _enum) const noexcept;
```

### 示例

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

## 附加成员：`size`, `enums`, `names`

### 简介

- `enum_info` 模板还通过静态成员提供了对底层枚举项序列及其名称的编译期访问：
  - `static constexpr std::size_t size` – 枚举项的数量。
  - `using enums` – 包含每个枚举项值的 `std::tuple`，元素类型为 `fake::mezz_t`。
  - `using names` – 包含每个完全限定名称的 `std::tuple`，元素类型为 `fake::view`。

### 示例

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
