# `type_format.h`

- a compile-time configurable (de)serialization utility for C++23

## Overview

The `type_format.h` header provides a set of customizable stream operators (`operator<<` and `operator>>`) for arbitrary C++ types.
It uses compile-time reflection (via `fake::for_each` and `fake::fields`) to automatically format aggregates, containers, tuples, variants, and many other types.
The behaviour can be tailored per “configuration token” (e.g. `fancy`, `pretty`, `json`) through stateful metaprogramming interfaces.
All customizations are local to the translation unit and produce unique symbols, therefore no ODR violations can occur.

---

## Predefined formatters

The library provides several ready‑to‑use formatters inside the namespace `fake::io`.

### `fake::io::fancy`

#### brief

A formatter that outputs human‑readable, indented text with colourization (by default) and automatically deduces member names for aggregates.
The indentation character is `|` (by default).

#### syntax

```c++
template<auto _footprint = ...>
inline constexpr auto fancy;
```

#### example

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

**output (colours omitted in plain text):**

```plain
Point : {
|   int32_t x : 10,
|   int32_t y : 20
}
```

---

### `fake::io::pretty`

#### brief

Similar to `fancy`, but uses a different colour scheme and by default shows member indices instead of deduced names.
The indentation character is also `|`.

#### syntax

```c++
template<auto _footprint = ...>
inline constexpr auto pretty;
```

#### example

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

**output (colours omitted in plain text):**

```plain
Point : {
|   int32_t 0 : 10,
|   int32_t 1 : 20
}
```

---

### `fake::io::plain`

#### brief

A minimal formatter that outputs no colours and uses spaces (or tabs) for indentation.
Member indices are shown.

#### syntax

```c++
template<auto _footprint = ...>
inline constexpr auto plain;
```

#### example

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

**output:**

```plain
Point : {
    int32_t 0 : 10,
    int32_t 1 : 20
}
```

---

### `fake::io::json`

#### brief

Outputs the object as a JSON string.
For containers, the top‑level object has a key equal to the type name.
Strings are quoted, objects become `{ ... }`, arrays become `[ ... ]`.

#### syntax

```c++
template<auto _footprint = ...>
inline constexpr auto json;
```

#### example

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

**output:**

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

#### brief

Produces an HTML fragment that represents the object with coloured `<font>` tags.
Useful for embedding pretty‑printed data in web pages.

#### syntax

```c++
template<auto _footprint = ...>
inline constexpr auto html;
```

#### example

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

**output (abbreviated, actual output is a single line):**

```plain
<pre style="background-color:#333; font-weight: bold; padding:1em;"><span style="color:#11A8CD">Point</span> <span style="color:#CCCCCC">:</span> <span style="color:#CCCCCC">{</span></br>    <span style="color:#11A8CD">int32_t</span> <span style="color:#E5E510">0</span> <span style="color:#CCCCCC">:</span> <span style="color:#DB6A6A">10</span><span style="color:#CCCCCC">,</span></br>    <span style="color:#11A8CD">int32_t</span> <span style="color:#E5E510">1</span> <span style="color:#CCCCCC">:</span> <span style="color:#DB6A6A">20</span></br><span style="color:#CCCCCC">}</span></pre>
```

---

### `fake::io::brief`

#### brief

Outputs in a single line, separated by spaces, without any type or member names.
Ideal for simple serialization such as printing logs or debugging information.

#### syntax

```c++
template<auto _footprint = ...>
inline constexpr auto brief;
```

#### example

```c++
#include <iostream>
#include "type_format.h"

struct Point { int x; int y; };

int main() {
    Point p{10, 20};
    std::cout << fake::io::brief<>(p) << std::endl;
    // std::cout << fake::io::brief_w<[]{}>(p) << std::endl; // workaround for compiler bugs in template entities
}
```

**output:**

```plain
{0 : 10, 1 : 20}
```

---

## Main example

The following example demonstrates the use of `fancy` and `pretty` on a composite object, including customization for private members and enumeration formatting.
All configuration calls are placed inside functions or class/struct to satisfy the requirement that `consteval` configuration interfaces must be invoked inside a scope.

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

**output:**

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

## Compile‑time configuration

All customization is done through **stateful template metaprogramming** and must be performed **before the first use** of any formatter for the given configuration token.
The order of customization and usage follows the instantiation order of compile-time, not the runtime order.
Each configuration interface is a `static consteval` member of `fake::custom::format::config`.
The first template argument is always a unique lambda that identifies the configuration unit (usually `[]{}`).
The second template argument is the target token (e.g. `fake::io::token::fancy`).
Additional arguments depend on the specific setting.

### Important: How configuration becomes active

Configuration functions (like `some_function_to_config()` in the example) are **not required to be called** at runtime.
Because they are not template entities (non-temploid), their bodies are instantiated during compilation (phase 1 – semantic analysis).
During that evaluation, the `consteval` configuration interfaces (e.g., `fake::custom::format::config::color::disable`) are instantiated.
This instantiation triggers the stateful metaprogramming machinery, which writes the configuration into the compiler's internal state for the current translation unit.
Thus, simply having the function definition (even without a call) is enough to activate the configuration (e.g., by being defined in an unused namespace) – provided the function itself is not discarded.
This behaviour is intentional and ensures that configuration can be placed in header files or dedicated configuration units without needing a runtime call.

Nevertheless, to avoid confusion, the library also allows configuration to be placed directly inside `main()` or any other function that **is** called.
Both ways are correct; the only requirement is that the configuration code is compiled **before** the first use of the corresponding formatter.

---

### `fake::custom::format::config::visitor::emplace`

#### brief

Registers a custom visitor for a specific type or template. The visitor will be called by `fake::for_each` to iterate over the members of that type.

#### syntax

```c++
template<auto _vest, typename _ConfigToken, typename _Matcher>
static consteval auto emplace(auto _visitor) noexcept;
```

- `_vest` – a unique lambda to identify the configuration site.
- `_ConfigToken` – the token for which the visitor applies (e.g. `fake::io::token::fancy`).
- `_Matcher` – a type used to match the target type(s). Can be a concrete type, a `fake::generic`, `fake::array_like`, or `fake::view_like`.
- `_visitor` – a constexpr lambda that will be called with `(auto &&obj, const auto &functor)`.

#### example

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

#### brief

Replaces the default output/input of a type with a custom streamer. The streamer must satisfy the `fake::io::streamer` template (i.e., provide static `ostream` and `istream` lambdas).

#### syntax

```c++
template<auto _vest, typename _ConfigToken, typename _Streamer>
static consteval auto transform() noexcept;
```

#### example

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

**output (colours omitted in plain text):**

```plain
int32_t : 1bf52
```

---

### `fake::custom::format::config::wrap::enable` / `disable`

#### brief

Enables or disables the outermost wrapper strings for the given token.

#### syntax

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### example

```c++
#include "type_format.h"

int main() {
    // Disable colours for the "html" token
    fake::custom::format::config::wrap::disable<[]{}, fake::io::token::html>();
}
```

---

### `fake::custom::format::config::style::wrapper`

#### brief

Sets the outermost wrapper strings for a given token.

#### syntax

```c++
template<auto _vest, typename _ConfigToken, auto _wrapper>
static consteval auto _wrapper() noexcept;
```

- `_wrapper` – a `fake::mate` contains a pair of `fake::view` literals, e.g. `fake::mate<fake::view_t<"{">, fake::view_t<"}">>{}`.

#### example

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

#### brief

Sets the indentation string for a given token. The string must not contain characters that would interfere with the parsing of the format (see the source for the precise restriction).

#### syntax

```c++
template<auto _vest, typename _ConfigToken, auto _tab>
static consteval auto tab() noexcept;
```

- `_tab` – a `fake::view` literal, e.g. `fake::view_v<"    ">`.

#### example

```c++
#include "type_format.h"

int main() {
    // Use four spaces for indentation with the "fancy" token
    fake::custom::format::config::style::tab<[]{}, fake::io::token::fancy, fake::view_v<"    ">>();
}
```

---

### `fake::custom::format::config::style::endl`

#### brief

Sets the end‑of‑line string for a given token (default is `"\n"`).

#### syntax

```c++
template<auto _vest, typename _ConfigToken, auto _endl>
static consteval auto endl() noexcept;
```

#### example

```c++
#include "type_format.h"

int main() {
    // Use HTML line break for the "html" token
    fake::custom::format::config::style::endl<[]{}, fake::io::token::html, fake::view_v<"</br>">>();
}
```

---

### `fake::custom::format::config::color::enable` / `disable`

#### brief

Enables or disables ANSI colourization for the given token.

#### syntax

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### example

```c++
#include "type_format.h"

int main() {
    // Disable colours for the "fancy" token
    fake::custom::format::config::color::disable<[]{}, fake::io::token::fancy>();
}
```

---

### `fake::custom::format::config::color::map`

#### brief

Replaces the default colour mapping (which parts get which colour) with a user‑defined `fake::format::color` struct.

#### syntax

```c++
template<auto _vest, typename _ConfigToken, format::color _color>
static consteval auto map() noexcept;
```

- `_color` – an instance of `fake::custom::format::color` containing members like `indent`, `type`, `member`, etc.

#### example

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

#### brief

Replaces the colourization mechanism entirely with a custom decorator class.
The decorator must provide a static `template<fake::colors _color> auto color(auto &)` that applies the colour to the given object.

#### syntax

```c++
template<auto _vest, typename _ConfigToken, typename _Decorator>
static consteval auto decorator() noexcept;
```

#### example

See the built‑in HTML decorator in `fake::io::detail::inject::preset_formatter::colorization`.

---

### `fake::custom::format::config::type::enable` / `disable`

#### brief

Controls whether the type name is printed before the member name.

#### syntax

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### example

```c++
#include "type_format.h"

int main() {
    // Suppress type names for the "brief" token (already done by default)
    fake::custom::format::config::type::disable<[]{}, fake::io::token::brief>();
}
```

---

### `fake::custom::format::config::member::enable` / `disable`

#### brief

Controls whether the member name (or index) is printed before the value or the opening brace (for objects).

#### syntax

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### example

```c++
#include "type_format.h"

int main() {
    // Print member names (or indices) for the "pretty" token (already done by default)
    fake::custom::format::config::member::enable<[]{}, fake::io::token::pretty>();
}
```

---

### `fake::custom::format::config::json::enable` / `disable`

#### brief

Turns JSON‑style output on or off for the given token. When enabled, strings are quoted, objects become `{ ... }` and containers become `[ ... ]` (with a type‑name key at the top level).

#### syntax

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### example

```c++
#include "type_format.h"

int main() {
    // Enable JSON output for the "fancy" token
    fake::custom::format::config::json::enable<[]{}, fake::io::token::fancy>();
}
```

---

### `fake::custom::format::config::json::register_container`

#### brief

Declares that a given type (or a whole family of types matched by a `fake::mezz` predicate) should be treated as a JSON container (i.e., use `[`/`]` instead of `{`/`}`).
This is automatically done for most STL containers, but can be extended.

#### syntax

```c++
template<auto _vest, typename _ConfigToken, fake::mezz_c _Filter>
static consteval auto register_container() noexcept;
```

- `_Filter` – a mezzanine wraps a functor that accepts a `type_package` for types that are containers. For example, `fake::mezz_t<[]<typename T>(fake::type_package<T>) requires is_my_container_v<T>{}>`.

#### example

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

**output:**

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

#### brief

Enables or disables the colourization of matching braces based on nesting depth. When enabled, each nesting level gets a different colour (cycling through a palette).

#### syntax

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### example

```c++
#include "type_format.h"

int main() {
    // Enable nested brace colourization for the "fancy" token (already done by default)
    fake::custom::format::config::bracket_pair_colorization::enable<[]{}, fake::io::token::fancy>();
}
```

---

### `fake::custom::format::config::member_name_colorization::enable` / `disable`

#### brief

Enables or disables colourization of member names based on their nesting depth. Each successive member gets a same colour and each nesting level gets a different colour (cycling through a smaller palette).

#### syntax

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### example

```c++
#include "type_format.h"

int main() {
    // Enable member name colourization for the "pretty" token (already done by default)
    fake::custom::format::config::member_name_colorization::enable<[]{}, fake::io::token::pretty>();
}
```

---

### `fake::custom::format::config::aggregate::fields::enable` / `disable`

#### brief

Controls whether the library attempts to automatically deduce the names of aggregate members (using the `fake::fields` machinery).
If disabled, members are referred to by index (`int 0`, `int 1`, ...).

#### syntax

```c++
template<auto _vest, typename _ConfigToken>
static consteval auto enable() noexcept;

template<auto _vest, typename _ConfigToken>
static consteval auto disable() noexcept;
```

#### example

```c++
#include "type_format.h"

int main() {
    // Enable automatic field name deduction for the "pretty" token (enabled by default for fancy, disabled for pretty)
    fake::custom::format::config::aggregate::fields::enable<[]{}, fake::io::token::pretty>();
}
```

---

## Additional customization points

### `fake::tool::steal`

#### brief

A template that can be specialised to grant the formatter access to private members of a class.
The specialisation must be placed before any use of the formatter for that class.
Inside the specialisation you can register the members as a visitor for the desired tokens.

#### syntax

```c++
template<auto _vest, typename _Adaptor, auto... _members>
struct steal;
```

- `_vest` – a unique lambda to identify the specialisation.
- `_Adaptor` – typically `fake::tool::adaptor<ClassName>`.
- `_members` – pointers to the private data members you want to expose.

#### example

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

### `fake::io::as::enumerate` and `fake::io::as::custom_enumerate`

#### brief

Stream transformers that output an enumeration as its name string (if the enumeration provides a sentinel `enum_size` or a maximum enumerator is passed).
`enumerate` works for enumerations that have a member named `enum_size` (the total number of enumerators).
`custom_enumerate<EnumValue>` works for any enumeration; you pass the largest enumerator value (or any value) to generate the lookup table up to that value.

#### syntax

```c++
inline constexpr auto enumerate;
template<auto _enum> inline constexpr auto custom_enumerate;
```

#### example

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

**output (colours omitted in plain text):**

```plain
Color : "Color::Green"
State : 1
State : "State::Running"
```

---

## Important notes

- All configuration must be performed **in the same translation unit** and **before** (**compile before, not run before**) the first use of the corresponding formatter.
  After the first instantiation, the configuration interfaces become unavailable (compile‑time error) to prevent ODR violations.

- The unique lambda `[]{}` used as the first argument is only a marker; it does not need to be a real function.
  Different lambdas create distinct configuration sites.

- The `_footprint` mechanism ensures that each configuration history produces a unique symbol, so different configurations in different TUs do not clash.

- **Configuration functions do not need to be called** – because they are `consteval`, their bodies are instantiated during compilation.
  The act of compiling the function definition (i.e., the compiler seeing and semantically checking it) instantiates the `consteval` interfaces, which triggers the stateful metaprogramming writes.
  This means you can safely place configuration code in a header or in a separate namespace without ever invoking it at runtime.

- Nevertheless, it is also allowed (and often clearer) to put configuration directly inside `main()` or any other function that is actually executed. Both approaches work identically.

- For further details, inspect the source code of `type_format.h` and the predefined configurations inside `fake::io::detail::inject::preset_formatter`.
