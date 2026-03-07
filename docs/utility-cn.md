# `utility.h`

- 运行时实用工具：字符串/数字转换、流匹配、终端颜色、十六进制转换、编译期字符串签名以及作用域守卫。

---

## `fake::global`

### 简介

强制在静态初始化期间（`main` 之前）执行一个 lambda。
其工作原理是实例化一个变量模板，该模板触发 `registry` 机制。

### 语法

```c++
template<auto _lambda>
inline constexpr fake::detail::global::invoke<_lambda> global;
```

### 示例

```c++
#include <iostream>
#include "utility.h"

int main()
{
    std::cout << "main()" << std::endl;
    fake::global<[]{ std::cout << "global" << std::endl; }>;
}
```

**输出：**

```plain
global
main()
```

---

## `fake::cast`

### 简介

在字符串与算术类型、`char`、`bool`、`std::chrono::duration` 以及 `std::chrono::time_point` 之间进行双向转换。
对于整数和浮点数，可以指定进制、格式和精度。

### 语法

```c++
template<fake::cast_c _Type, auto _config = detail::utility::undefine>
constexpr std::optional<_Type> cast(std::string_view _e) noexcept;

template<auto _config = detail::utility::undefine, auto _precision = detail::utility::undefine>
constexpr std::string cast(fake::cast_c auto _e);

// char / char8_t specialisations
constexpr std::optional<char> cast<char>(std::string_view _e) noexcept;
constexpr std::string cast(char _e);

constexpr std::optional<char8_t> cast<char8_t>(std::string_view _e) noexcept;
constexpr std::string cast(char8_t _e);

// bool specialisation
constexpr std::optional<bool> cast<bool>(std::string_view _e) noexcept;
constexpr std::string cast(bool _e);

// duration and time_point
template<fake::duration_c _Duration>
constexpr std::optional<_Duration> cast(std::string_view _e) noexcept;
template<fake::duration_c _Duration>
constexpr std::string cast(_Duration _e);

template<fake::time_point_c _TimePoint>
constexpr std::optional<_TimePoint> cast(std::string_view _e) noexcept;
template<fake::time_point_c _TimePoint>
constexpr std::string cast(_TimePoint _e);
```

### 示例

一个综合示例，演示了各种重载。
（输出使用了来自 `"type_format.h"` 的 `fake::io::fancy` 进行美化打印。）

```c++
#include <iostream>
#include <limits>
#include <charconv>
#include "utility.h"
#include "type_format.h"

int main()
{
    const auto convert = []<typename _Type>(_Type _e) {
        const std::string content = fake::cast(_e);
        const _Type value = fake::cast<_Type>(content).value();
        std::cout << fake::io::fancy<>(content) << std::endl;
        std::cout << fake::io::fancy<>(value) << std::endl;
    };
    
    std::cout << fake::io::fancy<>(fake::cast<int>("foobar")) << std::endl;
    
    convert(bool{true});
    convert(int8_t{114});
    convert(uint8_t{114});
    convert(char{114});
    convert(char8_t{114});
    convert(int8_t{'\t'});
    convert(uint8_t{'\t'});
    convert(char{'\t'});
    convert(char8_t{'\t'});
    
    std::cout << fake::cast(int8_t{'\t'}) << std::endl;
    std::cout << fake::cast(uint8_t{'\t'}) << std::endl;
    std::cout << fake::cast(char{'\t'}) << std::endl;
    std::cout << fake::cast(char8_t{'\t'}) << std::endl;
    
    convert(int16_t{1919});
    convert(uint16_t{1919});
    convert(int32_t{114514});
    convert(uint32_t{114514ul});
    convert(int64_t{1919810ll});
    convert(uint64_t{1919810ull});
    convert(1919.810f);
    convert(1145141919.810);
    convert((long double){1145141919.810364364531});
    
    const auto print = [](auto _e) {
        std::cout << fake::io::fancy<>(_e.size() > 0x40uz ? _e.substr(0, 0x40uz) + "..." : _e) << std::endl;
    };
    
    print(fake::cast(114514));
    print(fake::cast<std::chars_format::scientific>(114.514f));
    print(fake::cast(std::numeric_limits<long double>::max()));
    print(fake::cast(std::numeric_limits<long double>::min()));
    print(fake::cast<std::chars_format::fixed>(std::numeric_limits<long double>::max()));
    print(fake::cast<std::chars_format::fixed>(std::numeric_limits<long double>::min()));
    print(fake::cast<std::chars_format::fixed, 0x20>(114514.0));
    print(fake::cast<std::chars_format::fixed, 0x20>(std::numeric_limits<long double>::max()));
}
```

**输出（纯文本，省略颜色）：**

```plain
std::optional<int32_t> : {
|   
}
std::string : "true"
bool : 1
std::string : "114"
int8_t : 114
std::string : "114"
uint8_t : 114
std::string : "r"
char : 'r'
std::string : "r"
char8_t : '72'
std::string : "9"
int8_t : 9
std::string : "9"
uint8_t : 9
std::string : "\\t"
char : '\t'
std::string : "\\t"
char8_t : '09'
9
9
\t
\t
std::string : "1919"
int16_t : 1919
std::string : "1919"
uint16_t : 1919
std::string : "114514"
int32_t : 114514
std::string : "114514"
uint32_t : 114514
std::string : "1919810"
int64_t : 1919810
std::string : "1919810"
uint64_t : 1919810
std::string : "1919.81"
float : 1919.81
std::string : "1145141919.81"
double : 1145141919.81
std::string : "1145141919.8103642464"
long double : 1145141919.8103642464
std::string : "114514"
std::string : "1.14514e+02"
std::string : "1.189731495357231765e+4932"
std::string : "3.3621031431120935063e-4932"
std::string : "1189731495357231765021263853030970205169063322294624200440323733..."
std::string : "0.00000000000000000000000000000000000000000000000000000000000000..."
std::string : "114514.00000000000000000000000000000000"
std::string : "1189731495357231765021263853030970205169063322294624200440323733..."
```

---

## `fake::ensure`

### 简介

一个流操纵器，用于检查确切的前缀（在跳过空白之后），如果不存在则抛出 `fake::exception::mismatch`。
可选的第二个参数可用于在匹配之前跳过额外的用户定义内容。

### 语法

```c++
template<fake::view_c _View>
consteval auto ensure(_View) noexcept;

template<fake::view_c _View>
consteval auto ensure(_View, auto _skip) noexcept;
```

### 示例

```c++
#include <iostream>
#include <sstream>
#include "view.h"
#include "utility.h"

using namespace fake::literals;

int main()
{
    static constexpr auto ensure = fake::ensure("<prefix>"_v);
    
    std::istringstream iss;
    iss.str(" \t\n \r  <prefix>\t\n \r  114514");
    
    int value = 0;
    static_cast<std::basic_istream<char>&>(iss) >> ensure >> value;
    std::cout << value << std::endl;
    
    iss.str(" \t\n \r  <prefoo>\t\n \r  114514");
    try {
        static_cast<std::basic_istream<char>&>(iss) >> ensure >> value;
    }
    catch(const fake::exception::mismatch &_e) {
        std::cout << _e.what() << std::endl;
    }
}
```

**输出：**

```plain
114514
match failed: [<prefix>]
```

---

## `fake::color`

### 简介

向输出（或输入）流添加 ANSI 颜色代码。
与 `view` 一起使用时，它返回一个嵌入了颜色代码的新 `view`。
与可流式值一起使用时，它返回一个代理对象，该对象在值的前后插入颜色代码。

### 语法

```c++
enum struct colors : std::size_t { black, red, green, yellow, blue, magenta, cyan, white, undefine };

template<colors _front, colors _back = colors::undefine>
constexpr auto color(const fake::view_c auto &_view) noexcept;

template<colors _front, colors _back = colors::undefine>
constexpr auto color(auto &&_data) requires (!fake::view_c<decltype(_data)>);
```

### 示例

```c++
#include <iostream>
#include "view.h"
#include "utility.h"

using namespace fake::literals;

int main()
{
    static constexpr fake::view_c auto color = fake::color<fake::colors::cyan>("114"_v);
    std::cout << std::string_view{color} << std::endl;
    std::cout << fake::color<fake::colors::yellow>("514") << std::endl;
}
```

**输出（纯文本，省略颜色）：**
（实际控制台输出将显示彩色文本；此处仅打印普通字符）

```plain
114
514
```

---

## `fake::to_hex` / `fake::from_hex`

### 简介

将整数值转换为编译期十六进制字符数组（`to_hex`），并从这样的数组转换回原始值（`from_hex`）。
数组大小是类型字节数的两倍。

### 语法

```c++
template<typename _Number> requires std::is_integral_v<_Number>
constexpr auto to_hex(_Number _number);

template<fake::std_array_c _Array> requires std::same_as<char, typename _Array::value_type>
constexpr auto from_hex(const _Array &_array) -> fake::unsigned_bytes<(std::tuple_size_v<_Array> >> 1)>;
```

### 示例

```c++
#include <iostream>
#include <bit>
#include "utility.h"
#include "type_format.h"

int main()
{
    const auto hex = [](fake::mezz_c auto _e) {
        constexpr fake::std_array_c auto hex = fake::to_hex(_e.value);
        constexpr auto value = fake::from_hex(hex);
        std::cout << fake::io::fancy<>(hex) << std::endl;
        std::cout << fake::io::fancy<>(value) << std::endl;
        std::cout << fake::io::fancy<>(std::bit_cast<decltype(_e.value)>(value)) << std::endl;
    };
    
    hex(fake::mezz_v<uint8_t{114}>);
    hex(fake::mezz_v<int16_t{514}>);
    hex(fake::mezz_v<int32_t{-114514}>);
    hex(fake::mezz_v<uint64_t{1145141919810}>);
}
```

**输出（纯文本，省略颜色）：**

```plain
std::array<char [2]> : {
|   char : '7',
|   char : '2'
}
uint8_t : 114
uint8_t : 114
std::array<char [4]> : {
|   char : '0',
|   char : '2',
|   char : '0',
|   char : '2'
}
uint16_t : 514
int16_t : 514
std::array<char [8]> : {
|   char : 'F',
|   char : 'F',
|   char : 'F',
|   char : 'E',
|   char : '4',
|   char : '0',
|   char : 'A',
|   char : 'E'
}
uint32_t : 4294852782
int32_t : -114514
std::array<char [16]> : {
|   char : '0',
|   char : '0',
|   char : '0',
|   char : '0',
|   char : '0',
|   char : '1',
|   char : '0',
|   char : 'A',
|   char : '9',
|   char : 'F',
|   char : 'C',
|   char : '7',
|   char : '0',
|   char : '0',
|   char : '4',
|   char : '2'
}
uint64_t : 1145141919810
uint64_t : 1145141919810
```

---

## `fake::signet`

### 简介

一个字符串的编译期签名（哈希）。签名是一个 `std::size_t` 值数组，每个值由字符串与固定盐值组合哈希而成。
盐值是在 `fake::detail::signet::salt` 内部定义的一个 `view` 字符串元组。
`signet_t` 派生自 `std::array<std::size_t, N>`，其中 `N` 是盐值元素的数量。

### 语法

```c++
struct signet_t : std::array<std::size_t, N> { ... };

consteval signet_t signet(fake::view_c auto _view) noexcept;
template<std::size_t _size> constexpr signet_t signet(const char (&_view)[_size]) noexcept;
constexpr signet_t signet(std::string_view _view) noexcept;

template<fake::view_c auto _view> inline constexpr signet_t signet_v;
```

### 示例

```c++
#include <iostream>
#include "view.h"
#include "utility.h"
#include "type_format.h"

using namespace fake::literals;

int main()
{
    constexpr fake::signet_t foo = fake::signet("foo"_v);
    constexpr fake::signet_t bar = fake::signet("bar");
    
    static_assert(foo == fake::signet_v<"foo"_v>);
    static_assert(foo > bar);
    
    std::cout << fake::io::fancy<>(foo) << std::endl;
    std::cout << fake::io::fancy<>(bar) << std::endl;
    
    std::cout << fake::io::fancy<>(std::hash<fake::signet_t>{}(foo)) << std::endl;
    std::cout << fake::io::fancy<>(std::hash<fake::signet_t>{}(bar)) << std::endl;
}
```

**输出（纯文本，省略颜色）：**

```plain
fake::signet_t : {
|   std::array<uint64_t [4]> 0 : {
|   |   uint64_t : 16345644267147238257,
|   |   uint64_t : 15588940365837418621,
|   |   uint64_t : 3849800218523126584,
|   |   uint64_t : 742205786577311208
|   }
}
fake::signet_t : {
|   std::array<uint64_t [4]> 0 : {
|   |   uint64_t : 263491208738193704,
|   |   uint64_t : 13228322985560509212,
|   |   uint64_t : 1220939384411075349,
|   |   uint64_t : 16866298616524510245
|   }
}
uint64_t : 15396474305027250218
uint64_t : 9209692740600937066
```

---

## `fake::scope_guard`

### 简介

一个简单的作用域守卫，当它离开作用域时执行一个可调用对象。
它是仅移动的，并使用 `fake::patterns::intact<fake::patterns::intact_e::moveonly>` 来跟踪其“完整”状态（以确保即使被移动也仅执行一次）。

### 语法

```c++
template<std::invocable<> _Lambda>
struct scope_guard {
    [[nodiscard]] scope_guard(_Lambda &&_lambda);
    ~scope_guard();
    // move constructor only, copy and assignment deleted
};
```

### 示例

```c++
#include <iostream>
#include "utility.h"

int main()
{
    fake::scope_guard guard{[]{ std::cout << "guard" << std::endl; }};
    std::cout << "~main()" << std::endl;
}
```

**输出：**

```plain
~main()
guard
```
