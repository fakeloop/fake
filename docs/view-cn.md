# `view.h`

- 基于可变参数字符模板的编译期字符串视图，完全支持 constexpr。
- 提供完全在编译期完成的字符串操作（查找、替换、比较、哈希）。
- 包含整数与字符串视图之间相互转换的工具，以及获取类型/值的名称作为视图的实用功能。

## `fake::view`

### 简介

- 一个模板类，将编译期固定字符串表示为 `char` 模板参数序列。
- 所有操作均为 `constexpr` 且可在常量表达式中使用。

### 语法

```c++
template<char... _Chars>
struct view;
```

### 示例

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

### 简介

- 一个变量模板，从字符串字面量创建 `fake::view` 实例。
- 字符串字面量必须作为模板参数传入。

### 语法

```c++
template<fake::detail::view::string _Value>
constexpr auto view_v = make_view<_Value>();
```

### 示例

```c++
#include "view.h"

int main()
{
    constexpr fake::view_c auto foo = fake::view_v<"foo!">;
    static_assert(foo.size() == 4);
}
```

## `fake::view_c`

### 简介

- 一个概念，检查类型是否为 `fake::view` 的实例化。

### 语法

```c++
template<typename _Type>
concept view_c = is_view_v<_Type>;
```

### 示例

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

### 简介

- 用户定义字面量操作符，从字符串字面量创建 `fake::view`。
- 位于命名空间 `fake::literals` 中。

### 语法

```c++
template<fake::detail::view::string _Value>
constexpr auto operator""_v() noexcept;
```

### 示例

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

### 简介

- 一个类型别名，提取与给定字符串字面量对应的 `fake::view` 类型。

### 语法

```c++
template<fake::detail::view::string _Value>
using view_t = std::remove_cvref_t<decltype(view_v<_Value>)>;
```

### 示例

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

### 简介

- 返回指向以空字符结尾的字符数组的指针，该数组持有字符串。

### 语法

```c++
static constexpr decltype(auto) data() noexcept;
```

### 示例

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

### 简介

- 返回指向以空字符结尾的 UTF-8 字符数组 (`char8_t`) 的指针。

### 语法

```c++
static constexpr decltype(auto) utf8() noexcept;
```

### 示例

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

### 简介

- 返回视图中的字符数（不包括空终止符）。

### 语法

```c++
static constexpr std::size_t size() noexcept;
static constexpr std::size_t length() noexcept;
```

### 示例

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

### 简介

- 访问指定索引处的字符，或第一个/最后一个字符。

### 语法

```c++
template<std::size_t _Index>
static constexpr char at() noexcept;

static constexpr char front() noexcept;
static constexpr char back() noexcept;
```

### 示例

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

### 简介

- 检查视图是否为空（包含零个字符）。

### 语法

```c++
static constexpr bool empty() noexcept;
```

### 示例

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

### 简介

- 计算字符串的 64 位 FNV‑1a 哈希值。

### 语法

```c++
static constexpr unsigned long long hash() noexcept;
```

### 示例

```c++
#include "view.h"

int main()
{
    constexpr auto foo = fake::view_v<"foo!">;
    static_assert(foo.hash() == 15929882213276287266ULL);
}
```

## `fake::view::starts_with()`

### 简介

- 检查视图是否以给定前缀开头。
- 前缀可以作为另一个 `fake::view` 提供，也可以作为字符串字面量（通过 `fake::detail::view::string` 包装）。

### 语法

```c++
template<fake::view _Value>
static constexpr bool starts_with() noexcept;

template<fake::detail::view::string _Value>
static constexpr bool starts_with() noexcept;
```

### 示例

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

### 简介

- 检查视图是否以给定后缀结尾。

### 语法

```c++
template<fake::view _Value>
static constexpr bool ends_with() noexcept;

template<fake::detail::view::string _Value>
static constexpr bool ends_with() noexcept;
```

### 示例

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

### 简介

- 检查视图是否包含给定子串。

### 语法

```c++
template<fake::view _Value>
static constexpr bool contains() noexcept;

template<fake::detail::view::string _Value>
static constexpr bool contains() noexcept;
```

### 示例

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

### 简介

- 查找子串第一次出现的位置，可以从指定索引开始。
- 返回索引或 `npos`（若未找到）。

### 语法

```c++
template<fake::view _Value, std::size_t _Index = 0>
static constexpr std::size_t find() noexcept;

template<fake::detail::view::string _Value, std::size_t _Index = 0>
static constexpr std::size_t find() noexcept;
```

### 示例

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

### 简介

- 查找子串最后一次出现的位置，可以从指定索引开始（默认 `npos`）。

### 语法

```c++
template<fake::view _Value, std::size_t _Index = npos>
static constexpr std::size_t rfind() noexcept;

template<fake::detail::view::string _Value, std::size_t _Index = npos>
static constexpr std::size_t rfind() noexcept;
```

### 示例

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

### 简介

- 定位第一个/最后一个与给定字符集合中任一字符匹配（或不匹配）的字符。

### 语法

```c++
template<fake::view _Value, std::size_t _Index = 0>
static constexpr std::size_t find_first_of() noexcept;
// 类似地还有 find_last_of, find_first_not_of, find_last_not_of
```

### 示例

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

### 简介

- 返回一个新的视图，该视图是从 `_Index` 开始、长度为 `_Size`（默认到末尾）的子串。

### 语法

```c++
template<std::size_t _Index, std::size_t _Size = npos>
static constexpr auto substr() noexcept;
```

### 示例

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

### 简介

- 将从 `_Index` 开始的所有子串 `_What` 替换为 `_With`。
- 多个重载可接受 `fake::view` 或字符串字面量作为两个参数。

### 语法

```c++
template<fake::view _What, fake::view _With, std::size_t _Index = 0>
static constexpr auto replace() noexcept;
// 其他与 fake::detail::view::string 的组合
```

### 示例

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

### 简介

- 将另一个视图（或字符串字面量）拼接到当前视图之后。

### 语法

```c++
template<fake::view _Value>
static constexpr auto append() noexcept;

template<fake::detail::view::string _Value>
static constexpr auto append() noexcept;
```

### 示例

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

## 运算符 `+`, `==`, `!=`, `<=>`

### 简介

- `operator+` 将两个视图拼接成一个新视图。
- 比较运算符按字典序比较底层字符串。

### 语法

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

### 示例

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

### 简介

- 将整数值（或 `std::integral_constant`）转换为表示其字符串形式的 `fake::view`。
- 支持 2 到 36 之间的任意进制（默认为 10）。

### 语法

```c++
template<auto _integral, std::size_t _base = 10>
constexpr auto to_view_v = to_view<_integral, _base>();
```

### 示例

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

### 简介

- 将 `fake::view`（表示给定进制下的整数）转换为整数值。
- 通过前导 `'-'` 检测符号。

### 语法

```c++
template<fake::view_c auto _v, std::size_t _base = 10>
consteval auto from_view_v = from_view<_v, _base>();
```

### 示例

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

### 简介

- 返回一个包含类型可读名称的 `fake::view`。
- 可接受 `fake::pack_c`（类型包）或某个类型的实例。

### 语法

```c++
constexpr auto type_view(fake::pack_c auto _pack) noexcept;
constexpr auto type_view(auto _type) noexcept;
```

### 示例

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

### 简介

- 返回一个包含编译期值的字符串表示形式的 `fake::view`。
- 该值被包装在 `fake::mezz_c` 中（例如通过 `fake::mezz_v`）。

### 语法

```c++
constexpr auto value_view(fake::mezz_c auto _mezz) noexcept;
```

### 示例

```c++
#include "view.h"

int main()
{
    static_assert(fake::value_view(fake::mezz_v<114514.0>) == "1.14514e+5"_v);
}
```

## `fake::self_view`

### 简介

- 返回一个包含 lambda、函数或调用作用域名称的 `fake::view`。
- 基于 `fake::symbol::self`。

### 语法

```c++
constexpr auto self_view(auto &&_lambda) noexcept;
```

### 示例

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
