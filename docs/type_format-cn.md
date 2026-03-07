# `type_format.h`

- 一个 C++23 纯头文件库，提供编译时可配置的（反）序列化工具

## 概述

`type_format.h` 头文件为任意 C++ 类型提供一组可定制的流运算符（`operator<<` 和 `operator>>`）。
它利用编译期反射（通过 `fake::for_each` 和 `fake::fields`）来自动格式化聚合体、容器、元组、变体以及许多其他类型。
其行为可以通过有状态元编程接口，为每个“配置令牌”（例如 `fancy`、`pretty`、`json`）进行定制。
所有定制都局限于当前翻译单元，并产生唯一的符号，因此不会发生 ODR 违规。

---

## 预定义的格式化器

该库在命名空间 `fake::io` 内提供了一些可直接使用的格式化器。

### `fake::io::fancy`

#### 简介

一种格式化器，输出人类可读的、带缩进的文本，默认启用颜色，并自动推导聚合体的成员名称。
默认的缩进字符是 `|`。

#### 语法

```c++
template<auto _footprint = ...>
inline constexpr auto fancy;
```

#### 示例

```c++
#include <iostream>
#include "type_format.h"

struct Point { int x; int y; };

int main() {
    Point p{10, 20};
    std::cout << fake::io::fancy<>(p) << std::endl;
    // std::cout << fake::io::fancy_w<[]{}>(p) << std::endl; // workaround for compiler bugs in template entities
}
```

**输出（纯文本，省略颜色）：**

```plain
Point : {
|   int32_t x : 10,
|   int32_t y : 20
}
```

---

### `fake::io::pretty`

#### 简介

与 `fancy` 类似，但使用不同的配色方案，并且默认显示成员索引而非推导出的名称。
默认的缩进字符也是 `|`。

#### 语法

```c++
template<auto _footprint = ...>
inline constexpr auto pretty;
```

#### 示例

```c++
#include <iostream>
#include "type_format.h"

struct Point { int x; int y; };

int main() {
    Point p{10, 20};
    std::cout << fake::io::pretty<>(p) << std::endl;
    // std::cout << fake::io::pretty_w<[]{}>(p) << std::endl; // workaround for compiler bugs in template entities
}
```

**输出（纯文本，省略颜色）：**

```plain
Point : {
|   int32_t 0 : 10,
|   int32_t 1 : 20
}
```

---

### `fake::io::plain`

#### 简介

一个极简的格式化器，不输出颜色，使用空格（或制表符）缩进。
成员索引会被显示。

#### 语法

```c++
template<auto _footprint = ...>
inline constexpr auto plain;
```

#### 示例

```c++
#include <iostream>
#include "type_format.h"

struct Point { int x; int y; };

int main() {
    Point p{10, 20};
    std::cout << fake::io::plain<>(p) << std::endl;
    // std::cout << fake::io::plain_w<[]{}>(p) << std::endl; // workaround for compiler bugs in template entities
}
```

**输出：**

```plain
Point : {
    int32_t 0 : 10,
    int32_t 1 : 20
}
```

---

### `fake::io::json`

#### 简介

将对象输出为 JSON 字符串。
对于容器，顶层对象的键等于类型名称。
字符串会被引号包围，对象变成 `{ ... }`，数组变成 `[ ... ]`。

#### 语法

```c++
template<auto _footprint = ...>
inline constexpr auto json;
```

#### 示例

```c++
#include <iostream>
#include <vector>
#include "type_format.h"

struct demo final{
    std::vector<int> v{1, 2, 3};
};

int main() {
    std::vector<int> v{1, 2, 3};
    std::cout << fake::io::json<>(v) << std::endl;
    // std::cout << fake::io::json_w<[]{}>(v) << std::endl; // workaround for compiler bugs in template entities
    
    demo d;
    std::cout << fake::io::json<>(d) << std::endl;
}
```

**输出：**

```plain
{
    "std::vector<int32_t>" : [
        1,
        2,
        3
    ]
}
{
    "demo" : {
        "std::vector<int32_t> 0" : [
            1,
            2,
            3
        ]
    }
}
```

---

### `fake::io::html`

#### 简介

生成一个表示对象的 HTML 片段，使用带颜色的 `<font>` 标签。
适合将美化打印的数据嵌入网页。

#### 语法

```c++
template<auto _footprint = ...>
inline constexpr auto html;
```

#### 示例

```c++
#include <iostream>
#include "type_format.h"

struct Point { int x; int y; };

int main() {
    Point p{10, 20};
    std::cout << fake::io::html<>(p) << std::endl;
    // std::cout << fake::io::html_w<[]{}>(p) << std::endl; // workaround for compiler bugs in template entities
}
```

**输出（已简化，实际输出为单行）：**

```plain
<pre style="background-color:#333; font-weight: bold; padding:1em;"><span style="color:#11A8CD">Point</span> <span style="color:#CCCCCC">:</span> <span style="color:#CCCCCC">{</span></br>    <span style="color:#11A8CD">int32_t</span> <span style="color:#E5E510">0</span> <span style="color:#CCCCCC">:</span> <span style="color:#DB6A6A">10</span><span style="color:#CCCCCC">,</span></br>    <span style="color:#11A8CD">int32_t</span> <span style="color:#E5E510">1</span> <span style="color:#CCCCCC">:</span> <span style="color:#DB6A6A">20</span></br><span style="color:#CCCCCC">}</span></pre>
```

---

### `fake::io::brief`

#### 简介

以单行输出，值之间用空格分隔，不输出任何类型或成员名称。
非常适合简单的序列化，例如打印日志或调试信息。

#### 语法

```c++
template<auto _footprint = ...>
inline constexpr auto brief;
```

#### 示例

```c++
#include <iostream>
#include "type_format.h"

struct Point { int x; int y; };

int main() {
    Point p{10, 20};
    std::cout << fake::io::brief<>(p) << std::endl;
}
```

**输出：**

```plain
{0 : 10, 1 : 20}
```

---

## 主示例

下面的示例演示了在一个复合对象上使用 `fancy` 和 `pretty`，包括对私有成员和枚举格式的定制。
所有配置调用都放在函数或类/结构体内，以满足 `consteval` 配置接口必须在某个作用域内调用的要求。

```c++
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "type_format.h"

// forward declarations of injection functions
namespace inject_ns { consteval void some_inject() noexcept; }

// -------------------------------------------------------------------
// Configuration function – its body is instantiated at compile time,
// which triggers the stateful configuration before any use of the formatters.
// -------------------------------------------------------------------
namespace {
    inline consteval void some_function_to_config() noexcept {
        // configure the key 'fancy' : disable colours, use four spaces for indentation
        fake::custom::format::config::color::disable<[]{}, fake::io::token::fancy>();
        fake::custom::format::config::style::tab<[]{}, fake::io::token::fancy, fake::view_v<"    ">>();
        
        // configure the key 'pretty' : enable json style, disable colours, add hex output for unsigned integers
        fake::custom::format::config::json::enable<[]{}, fake::io::token::pretty>();
        fake::custom::format::config::color::disable<[]{}, fake::io::token::pretty>();
        fake::custom::format::config::transform<[]{}, fake::io::token::pretty, fake::io::with::hex>();
    }
}

// -------------------------------------------------------------------
// Demo classes
// -------------------------------------------------------------------
struct demo {
    struct nested {
        unsigned int foo = 114;
        float bar = 514.0f;
    };
    
    struct owner {
    private:
        std::string foooo = "foooo!";
        std::string baaar = "baaar!";
    };
    
    template<typename _T, typename _R>
    struct custom {
    private:
        _T fooo = "fooo!";
        _R baar = "baar!";
        std::string_view ignore = "ignored member";
    
    private:
        friend consteval void inject_ns::some_inject() noexcept;
    };
    
    enum struct auto_enum { eFoo, eBar, enum_size };
    enum struct custom_enum { eFooo, eBaar };
    
    std::vector<nested> vec{{}, {}, {}, {}, {}};
    owner own;
    custom<std::string, std::string> cus;
    auto_enum auto_e = auto_enum::eFoo;
    custom_enum custom_e = custom_enum::eBaar;
};

// -------------------------------------------------------------------
// Steal private members of demo::owner
// -------------------------------------------------------------------
template<>
struct fake::tool::steal<[]{}, fake::tool::adaptor<demo::owner>, &demo::owner::foooo, &demo::owner::baaar> {
    using fancy_g = fake::cpo::format<fake::io::token::fancy>;
    static_assert(fake::custom::for_each::config::emplace_visitor<[]{}, fancy_g, steal>());
    using pretty_g = fake::cpo::format<fake::io::token::pretty>;
    static_assert(fake::custom::for_each::config::emplace_visitor<[]{}, pretty_g, steal>());
};

// -------------------------------------------------------------------
// Inject custom visitors for demo::custom
// -------------------------------------------------------------------
namespace inject_ns {
    inline consteval void some_inject() noexcept {
        // Custom visitor for demo::custom (any instantiation)
        constexpr auto v = [](auto &&_e, const auto &_f) {
            _f.template operator()<fake::view_t<"fooo">>(_e.fooo);
            _f.template operator()<fake::view_t<"baar">>(_e.baar);
        };
        
        // Generic for key 'fancy'
        using custom_g = fake::pattern_t<demo::custom<char[5], char[5]>>;
        fake::custom::format::config::visitor::emplace<[]{}, fake::io::token::fancy, custom_g>(v);
        
        // Specialized for key 'pretty'
        using specialized_g = demo::custom<std::string, std::string>;
        fake::custom::format::config::visitor::emplace<[]{}, fake::io::token::pretty, specialized_g>(v);
    }
}

int main() {
    // Note: some_function_to_config() and some_inject() are NOT called.
    // Their bodies are consteval and the statements inside are instantiated at compile time,
    // which performs the configuration through stateful metaprogramming.
    
    // configure the key 'pretty' : enable member names, use four spaces for indentation, register the enum 'custom_enum'
    fake::custom::format::config::aggregate::fields::enable<[]{}, fake::io::token::pretty>();
    fake::custom::format::config::style::tab<[]{}, fake::io::token::pretty, fake::view_v<"    ">>();
    fake::custom::format::config::transform<[]{}, fake::io::token::pretty, fake::io::as::custom_enumerate<demo::custom_enum::eBaar>>();
    
    demo value;
    
    std::stringstream ss;
    ss << fake::io::fancy<>(value);
    std::cout << ss.str() << std::endl;
    
    // Modify a value in the output and parse it back with fancy
    // Input and output must be paired, therefore same fancy with same configurations
    ss.str(ss.str().replace(ss.str().find("114", 364), 3, "1919"));
    ss >> fake::io::fancy<>(value);
    
    std::cout << fake::io::pretty<>(value) << std::endl;
}
```

**输出：**

```plain
demo : {
    std::vector<demo::nested> vec : {
        demo::nested : {
            uint32_t foo : 114,
            float bar : 514
        },
        demo::nested : {
            uint32_t foo : 114,
            float bar : 514
        },
        demo::nested : {
            uint32_t foo : 114,
            float bar : 514
        },
        demo::nested : {
            uint32_t foo : 114,
            float bar : 514
        },
        demo::nested : {
            uint32_t foo : 114,
            float bar : 514
        }
    },
    demo::owner own : {
        std::string foooo : "foooo!",
        std::string baaar : "baaar!"
    },
    demo::custom<std::string, std::string> cus : {
        std::string fooo : "fooo!",
        std::string baar : "baar!"
    },
    demo::auto_enum auto_e : "demo::auto_enum::eFoo",
    demo::custom_enum custom_e : 1
}
{
    "demo" : {
        "std::vector<demo::nested> vec" : [
            {
                "uint32_t foo" : 114 (0x00000072),
                "float bar" : 514
            },
            {
                "uint32_t foo" : 114 (0x00000072),
                "float bar" : 514
            },
            {
                "uint32_t foo" : 114 (0x00000072),
                "float bar" : 514
            },
            {
                "uint32_t foo" : 1919 (0x0000077F),
                "float bar" : 514
            },
            {
                "uint32_t foo" : 114 (0x00000072),
                "float bar" : 514
            }
        ],
        "demo::owner own" : {
            "std::string foooo" : "foooo!",
            "std::string baaar" : "baaar!"
        },
        "demo::custom<std::string, std::string> cus" : {
            "std::string fooo" : "fooo!",
            "std::string baar" : "baar!"
        },
        "demo::auto_enum auto_e" : 0,
        "demo::custom_enum custom_e" : "demo::custom_enum::eBaar"
    }
}
```

---

## 编译时配置

所有定制都通过**有状态模板元编程**完成，并且必须在**首次使用**任何针对给定配置令牌的格式化器**之前**进行。
定制与使用的顺序遵循编译期实例化顺序，而非运行时顺序。
每个配置接口都是 `fake::custom::format::config` 的一个 `static consteval` 成员。
第一个模板参数总是一个用于标识配置位置的唯一 lambda（通常为 `[]{}`）。
第二个模板参数是目标令牌（例如 `fake::io::token::fancy`）。
额外的参数取决于具体的设置。

### 重要说明：配置如何生效

配置函数（例如示例中的 `some_function_to_config()`）**不需要在运行时被调用**。
因为它们不是模板实体（非 temploid），其函数体会在编译期间（第一阶段——语义分析）被实例化。
在此实例化过程中，`consteval` 配置接口（例如 `fake::custom::format::config::color::disable`）会被实例化。
这个实例化会触发有状态元编程机制，将配置写入当前翻译单元的编译器内部状态。
因此，仅仅存在该函数定义（即使没有调用）就足以激活配置（例如定义在未使用的命名空间中）——前提是该函数本身没有被丢弃。
这种设计是有意的，它确保了配置可以放在头文件或专门的配置单元中，而不需要运行时调用。

不过，为了避免混淆，该库也允许将配置直接放在 `main()` 或任何**被实际调用**的函数中。
两种方式都是正确的；唯一的要求是配置代码的编译必须**早于**相应格式化器的首次使用。

---

### `fake::custom::format::config::visitor::emplace`

#### 简介

为特定类型或模板注册一个自定义访问器。该访问器将被 `fake::for_each` 调用来遍历该类型的成员。

#### 语法

```c++
template<auto _vest, typename _ConfigToken, typename _Matcher>
static consteval auto emplace(auto _visitor) noexcept;
```

- `_vest` – 标识配置位置的唯一 lambda。
- `_ConfigToken` – 该访问器所应用的令牌（例如 `fake::io::token::fancy`）。
- `_Matcher` – 用于匹配目标类型的类型。可以是具体类型、`fake::generic`、`fake::array_like` 或 `fake::view_like`。
- `_visitor` – 一个 constexpr lambda，将被调用并传入 `(auto &&obj, const auto &functor)`。

#### 示例

```c++
#include "type_format.h"

int main() {
    // Custom visitor for std::pair (already provided by default, but shown for illustration)
    constexpr auto pair_visitor = [](auto &&_e, const auto &_f) {
        _f.template operator()<fake::view_t<"first">>(_e.first);
        _f.template operator()<fake::view_t<"second">>(_e.second);
    };
    
    // Register it for the "fancy" token
    fake::custom::format::config::visitor::emplace<[]{}, fake::io::token::fancy, std::pair<int,int>>(pair_visitor);
}
```

---

### `fake::custom::format::config::transform`

#### 简介

用自定义的流处理器替换某个类型的默认输出/输入。该流处理器必须满足 `fake::io::streamer` 模板的要求（即提供静态的 `ostream` 和 `istream` lambda）。

#### 语法

```c++
template<auto _vest, typename _ConfigToken, typename _Streamer>
static consteval auto transform() noexcept;
```

#### 示例

```c++
#include <iostream>
#include "type_format.h"

// Streamer that prints an unsigned integer as hex only
struct hex_only {
    static constexpr auto ostream = [](std::ostream &os, unsigned int v) {
        os << std::hex << v << std::dec;
    };
    static constexpr auto istream = [](std::istream &is, unsigned int &v) {
        is >> std::hex >> v >> std::dec;
    };
};

int main() {
    // Apply it to unsigned int for the "pretty" token
    using streamer = fake::io::streamer<hex_only::ostream, hex_only::istream>;
    fake::custom::format::config::transform<[]{}, fake::io::token::pretty, streamer>();
    
    std::cout << fake::io::pretty<>(114514) << std::endl;
}
```

**输出（纯文本，省略颜色）：**

```plain
int32_t : 1bf52
```

---

### `fake::custom::format::config::wrap::enable` / `disable`

#### 简介

为给定令牌开启或关闭包装在最外层的字符串。

#### 语法

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### 示例

```c++
#include "type_format.h"

int main() {
    // Disable colours for the "html" token
    fake::custom::format::config::wrap::disable<[]{}, fake::io::token::html>();
}
```

---

### `fake::custom::format::config::style::wrapper`

#### 简介

为给定令牌设置包装在最外层的字符串。

#### 语法

```c++
template<auto _vest, typename _ConfigToken, auto _wrapper>
static consteval auto _wrapper() noexcept;
```

- `_wrapper` – 包含一对 `fake::view` 字面量的 `fake::mate`，例如 `fake::mate<fake::view_t<"{">, fake::view_t<"}">>{}`。

#### 示例

```c++
#include "type_format.h"

int main() {
    // Use a white background colour for '<pre>' with the "html" token
    using wrapper = fake::mate<
        fake::view_t<"<pre style=\"background-color:#EEE; font-weight: bold; padding:1em;\">">,
        fake::view_t<"</pre>">
    >;
    fake::custom::format::config::style::wrapper<[]{}, token::html, wrapper{}>();
}
```

---

### `fake::custom::format::config::style::tab`

#### 简介

为给定令牌设置缩进字符串。该字符串不得包含会干扰格式解析的字符（具体限制请参阅源码）。

#### 语法

```c++
template<auto _vest, typename _ConfigToken, auto _tab>
static consteval auto tab() noexcept;
```

- `_tab` – 一个 `fake::view` 字面量，例如 `fake::view_v<"    ">`。

#### 示例

```c++
#include "type_format.h"

int main() {
    // Use four spaces for indentation with the "fancy" token
    fake::custom::format::config::style::tab<[]{}, fake::io::token::fancy, fake::view_v<"    ">>();
}
```

---

### `fake::custom::format::config::style::endl`

#### 简介

为给定令牌设置行结束字符串（默认为 `"\n"`）。

#### 语法

```c++
template<auto _vest, typename _ConfigToken, auto _endl>
static consteval auto endl() noexcept;
```

#### 示例

```c++
#include "type_format.h"

int main() {
    // Use HTML line break for the "html" token
    fake::custom::format::config::style::endl<[]{}, fake::io::token::html, fake::view_v<"</br>">>();
}
```

---

### `fake::custom::format::config::color::enable` / `disable`

#### 简介

启用或禁用给定令牌的 ANSI 颜色。

#### 语法

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### 示例

```c++
#include "type_format.h"

int main() {
    // Disable colours for the "fancy" token
    fake::custom::format::config::color::disable<[]{}, fake::io::token::fancy>();
}
```

---

### `fake::custom::format::config::color::map`

#### 简介

用用户自定义的 `fake::format::color` 结构体替换默认的颜色映射（哪些部分使用哪种颜色）。

#### 语法

```c++
template<auto _vest, typename _ConfigToken, format::color _color>
static consteval auto map() noexcept;
```

- `_color` – 一个 `fake::custom::format::color` 实例，包含 `indent`、`type`、`member` 等成员。

#### 示例

```c++
#include "type_format.h"

int main() {
    constexpr fake::custom::format::color myColors{
        .indent = fake::colors::white,
        .type   = fake::colors::cyan,
        .member = fake::colors::yellow,
        .value  = fake::colors::red,
        .colon  = fake::colors::white,
        .quote  = fake::colors::red,
        .comma  = fake::colors::white,
        .bracket= fake::colors::blue
    };
    
    fake::custom::format::config::color::map<[]{}, fake::io::token::fancy, myColors>();
}
```

---

### `fake::custom::format::config::color::decorator`

#### 简介

完全替换颜色机制，使用自定义的装饰器类。
该装饰器必须提供一个静态的 `template<fake::colors _color> auto color(auto &)`，用于将颜色应用于给定的对象。

#### 语法

```c++
template<auto _vest, typename _ConfigToken, typename _Decorator>
static consteval auto decorator() noexcept;
```

#### 示例

请参阅内置的 HTML 装饰器 `fake::io::detail::inject::preset_formatter::colorization`。

---

### `fake::custom::format::config::type::enable` / `disable`

#### 简介

控制是否在成员名称之前打印类型名称。

#### 语法

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### 示例

```c++
#include "type_format.h"

int main() {
    // Suppress type names for the "brief" token (already done by default)
    fake::custom::format::config::type::disable<[]{}, fake::io::token::brief>();
}
```

---

### `fake::custom::format::config::member::enable` / `disable`

#### 简介

控制是否在值之前或对象的开括号之前打印成员名称（或索引）。

#### 语法

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### 示例

```c++
#include "type_format.h"

int main() {
    // Print member names (or indices) for the "pretty" token (already done by default)
    fake::custom::format::config::member::enable<[]{}, fake::io::token::pretty>();
}
```

---

### `fake::custom::format::config::json::enable` / `disable`

#### 简介

为给定令牌开启或关闭 JSON 风格的输出。启用后，字符串会被引号包围，对象变成 `{ ... }`，容器变成 `[ ... ]`（顶层带有类型名称作为键）。

#### 语法

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### 示例

```c++
#include "type_format.h"

int main() {
    // Enable JSON output for the "fancy" token
    fake::custom::format::config::json::enable<[]{}, fake::io::token::fancy>();
}
```

---

### `fake::custom::format::config::json::register_container`

#### 简介

声明给定的类型（或由 `fake::mezz` 谓词匹配的一类类型）应被视为 JSON 容器（即使用 `[`/`]` 而不是 `{`/`}`）。
对于大多数 STL 容器，这个操作是自动完成的，但可以扩展。

#### 语法

```c++
template<auto _vest, typename _ConfigToken, fake::mezz_c _Filter>
static consteval auto register_container() noexcept;
```

- `_Filter` – 一个 mezzanine，包装了一个接受 `type_package` 的函数对象，用于匹配容器类型。例如，`fake::mezz_t<[]<typename T>(fake::type_package<T>) requires is_my_container_v<T>{}>`。

#### 示例

```c++
#include <iostream>
#include <sstream>
#include "type_format.h"

template<typename _T>
struct my_container final{
    using value_type = _T;
    my_container(value_type _e): data{_e}{}
    void emplace_back(value_type _e){data = _e;}
    
private:
    friend int main();
    _T data;
};

int main() {
    constexpr auto v = [](auto &&_e, const auto &_f){_f.template operator()<fake::view_t<"data">>(_e.data);};
    using custom_g = fake::pattern_t<my_container<char>>;
    fake::custom::format::config::visitor::emplace<[]{}, fake::io::token::json, custom_g>(v);
    
    constexpr auto my_container_matcher = []<std::same_as<my_container<int>> T>(fake::type_package<T>){};
    
    fake::custom::format::config::json::register_container<[]{}, fake::io::token::json, fake::mezz_t<my_container_matcher>>();
    
    my_container value{114514};
    
    std::stringstream ss;
    
    ss << fake::io::json<>(value);
    std::cout << ss.str() << std::endl;
    
    // Modify a value in the output and parse it back with json
    ss.str(ss.str().replace(ss.str().find("114514"), 6, "1919810"));
    
    ss >> fake::io::json<>(value);
    
    std::cout << fake::io::json<>(value) << std::endl;
}
```

**输出：**

```plain
{
    "my_container<int32_t>" : [
        114514
    ]
}
{
    "my_container<int32_t>" : [
        1919810
    ]
}
```

---

### `fake::custom::format::config::bracket_pair_colorization::enable` / `disable`

#### 简介

启用或禁用基于嵌套深度的括号对颜色化。启用后，每一层嵌套都会获得不同的颜色（在一个调色板中循环）。

#### 语法

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### 示例

```c++
#include "type_format.h"

int main() {
    // Enable nested brace colourization for the "fancy" token (already done by default)
    fake::custom::format::config::bracket_pair_colorization::enable<[]{}, fake::io::token::fancy>();
}
```

---

### `fake::custom::format::config::member_name_colorization::enable` / `disable`

#### 简介

启用或禁用基于嵌套深度的成员名称颜色化。每一层嵌套的成员名称获得相同的颜色，而不同的嵌套层获得不同的颜色（在一个较小的调色板中循环）。

#### 语法

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### 示例

```c++
#include "type_format.h"

int main() {
    // Enable member name colourization for the "pretty" token (already done by default)
    fake::custom::format::config::member_name_colorization::enable<[]{}, fake::io::token::pretty>();
}
```

---

### `fake::custom::format::config::aggregate::fields::enable` / `disable`

#### 简介

控制库是否尝试自动推导聚合体的成员名称（使用 `fake::fields` 机制）。
如果禁用，成员将通过索引引用（`int 0`，`int 1`，……）。

#### 语法

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### 示例

```c++
#include "type_format.h"

int main() {
    // Enable automatic field name deduction for the "pretty" token (enabled by default for fancy, disabled for pretty)
    fake::custom::format::config::aggregate::fields::enable<[]{}, fake::io::token::pretty>();
}
```

---

## 额外的定制点

### `fake::tool::steal`

#### 简介

一个可以特化的模板，用于让格式化器访问类的私有成员。
特化必须在首次使用该类的格式化器之前进行。
在特化内部，你可以将这些成员注册为针对所需令牌的访问器。

#### 语法

```c++
template<auto _vest, typename _Adaptor, auto... _members>
struct steal;
```

- `_vest` – 标识特化位置的唯一 lambda。
- `_Adaptor` – 通常是 `fake::tool::adaptor<ClassName>`。
- `_members` – 指向你想要暴露的私有数据成员的指针。

#### 示例

```c++
#include "type_format.h"

class Secret {
    int hidden = 42;
    std::string secret = "p455w0rd";
};

// Specialisation (must be at global scope)
template<>
struct fake::tool::steal<[]{}, fake::tool::adaptor<Secret>, &Secret::hidden, &Secret::secret> {
    using fancy_g = fake::cpo::format<fake::io::token::fancy>;
    static_assert(fake::custom::for_each::config::emplace_visitor<[]{}, fancy_g, steal>());
    // also register for other tokens if needed
};

int main(){}
```

---

### `fake::io::as::enumerate` 和 `fake::io::as::custom_enumerate`

#### 简介

流转换器，将枚举输出为其名称字符串（前提是枚举提供了 `enum_size` 哨兵，或者传递了最大枚举值）。
`enumerate` 适用于具有名为 `enum_size` 的成员（即枚举总数）的枚举。
`custom_enumerate<EnumValue>` 适用于任何枚举；你需要传递最大的枚举值（或任意值）来生成直到该值的查找表。

#### 语法

```c++
inline constexpr auto enumerate;
template<auto _enum> inline constexpr auto custom_enumerate;
```

#### 示例

```c++
#include <iostream>
#include "type_format.h"

enum class Color { Red, Green, Blue, enum_size };   // provides enum_size
enum class State { Init, Running, Stopped };

int main() {
    Color c = Color::Green;
    State s = State::Running;
    
    std::cout << fake::io::fancy<>(c) << std::endl;          // prints "Color::Green"
    std::cout << fake::io::fancy<>(s) << std::endl;          // prints integer (no name)
    
    // Configure State to be printed as name using custom_enumerate<State::Stopped>
    // Use 'token::pretty' since 'token::fancy' is invoked, so its custom interfaces are blocked
    fake::custom::format::config::transform<[]{}, fake::io::token::pretty, fake::io::as::custom_enumerate<State::Stopped>>();
    
    std::cout << fake::io::pretty<>(s) << std::endl;          // now prints "State::Running"
}
```

**输出（纯文本，省略颜色）：**

```plain
Color : "Color::Green"
State : 1
State : "State::Running"
```

---

## 重要说明

- 所有配置必须**在同一个翻译单元内**、且**早于**（**编译时早于，而不是运行时早于**）相应格式化器的首次使用。
  在首次实例化之后，配置接口将变得不可用（编译时错误），以防止 ODR 违规。

- 用作第一个参数的唯一 lambda `[]{}` 只是一个标记；它不需要是一个真实的函数。
  不同的 lambda 会创建不同的配置位置。

- `_footprint` 机制确保每个配置历史产生唯一的符号，因此不同 TU 中的不同配置不会冲突。

- **配置函数不需要被调用**——因为它们被标记为 `consteval`，其函数体会在编译期间被实例化。
  编译函数定义这一行为（即编译器看到并进行语义检查）会实例化 `consteval` 接口，从而触发有状态元编程写入。
  这意味着你可以安全地将配置代码放在头文件或单独的命名空间中，而无需在运行时调用它们。

- 尽管如此，将配置直接放在 `main()` 或任何实际执行的函数中也是允许的（而且通常更清晰）。两种方法效果相同。

- 更多细节，请查看 `type_format.h` 的源代码以及 `fake::io::detail::inject::preset_formatter` 中预定义的配置。
