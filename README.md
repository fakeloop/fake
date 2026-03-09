# `fake` – metaprogramming library / 元编程库

[![License: WTFPL](https://img.shields.io/badge/License-WTFPL-brightgreen.svg)](LICENSE)

`fake` is a header‑only library for C++23 that provides a rich set of tools for compile‑time programming, asynchronous DAG execution, type reflection, formatting, and more.

`fake` 是一个仅头文件的 C++23 库，提供了丰富的编译期编程工具，包括异步 DAG 执行、类型反射、格式化等功能。

## Features / 特性

**`acyclic.h`**

>Build and execute asynchronous directed acyclic graphs (DAGs) using C++20 coroutines. Compile‑time topology checks, token‑based scheduling, exception handling, and aspect‑oriented observation.
>
>使用 C++20 协程构建和执行异步有向无环图（DAG）。编译期拓扑检查、基于 token 的调度、异常处理和面向切面观测。

**`delegate.h`**

>Versatile multicast/singlecast delegates with static or runtime signatures. Parent/child composition, and compile‑time aspect injection.
>
>多功能多播/单播委托，支持静态或运行时签名。父子组合以及编译期切面注入。

**`detect.h`**

>Type detection and transformation framework using compile‑time predicates and combinators (`&&`, `||`, `<<=`, etc.). Includes a huge collection of predefined detectors and transformations.
>
>使用编译期谓词和组合符（`&&`、`||`、`<<=` 等）进行类型检测和变换的框架。包含大量预定义的探测器和变换。

**`enum_info.h`**

>Bidirectional mapping between enumeration values and their names at compile time.
>
>编译期枚举值与名称的双向映射。

**`flat.h`**

>Compile‑time heterogeneous containers: `mate` (pair‑like), `flat` (indexed tuple), `query` (key‑value map). Optimized storage with empty‑base and static `mezz` keys.
>
>编译期异构容器：`mate`（类似 pair）、`flat`（索引元组）、`query`（键值映射）。通过空基类和静态 `mezz` 键优化存储。

**`functor_info.h`**

>Static reflection for callable types (lambdas, `std::bind`, member function pointers, free functions, …).
>
>可调用类型（lambda、`std::bind`、成员函数指针、自由函数等）的静态反射。

**`is_valid.h`**

>Basic metaprogramming utilities: `null_t`, `pack_t`/`pazz_t`, `mezz_t`, `take_t`, pattern holders (`generic`, `array_like`, `view_like`), and more.
>
>基础元编程工具：`null_t`、`pack_t`/`pazz_t`、`mezz_t`、`take_t`、模式持有者（`generic`、`array_like`、`view_like`）等。

**`patterns.h`**

>Design patterns for compile‑time: singleton, gueston (cross‑binary reference), intact (move/copy tracking), registry (compile‑time registration), builder (named parameters).
>
>编译期设计模式：单例（singleton）、跨二进制引用（gueston）、移动/拷贝追踪（intact）、编译期注册（registry）、具名参数构建器（builder）。

**`traits.h`**

>Advanced type traits: pointer‑sized integers, reference modifications, common base/derive, variant/tuple introspection, `mimic_t`/`gemma_t`, expressive concepts (`hold`/`like`/`meets`), and more.
>
>高级类型 traits：指针大小的整数、引用修改、公共基类/派生类、variant/tuple 内省、`mimic_t`/`gemma_t`、表达性概念（`hold`/`like`/`meets`）等。

**`tuple.h`**

>Metaprogramming utilities for `std::tuple`: search, filter, transform, concatenate, zip, fold, and more.
>
>用于 `std::tuple` 的元编程工具：搜索、过滤、变换、拼接、压缩、折叠等。

**`type_format.h`**

>Compile‑time configurable (de)serialization for arbitrary C++ types. Predefined formatters: `fancy`, `pretty`, `plain`, `json`, `html`, `brief`. Extensible via stateful metaprogramming.
>
>编译期可配置的（反）序列化工具，适用于任意 C++ 类型。预定义的格式化器：`fancy`、`pretty`、`plain`、`json`、`html`、`brief`。可通过有状态元编程扩展。

**`utility.h`**

>Runtime utilities: string/number conversion, stream matching, terminal colors, hexadecimal conversion, compile‑time string signatures (`signet`), scope guards.
>
>运行时实用工具：字符串/数字转换、流匹配、终端颜色、十六进制转换、编译期字符串签名（`signet`）、作用域守卫。

**`view.h`**

>Compile‑time string view with full `constexpr` support. String manipulation (find, replace, compare, hashing) entirely at compile time. Also provides `type_view`, `value_view`, `self_view`.
>
>完全支持 `constexpr` 的编译期字符串视图。完全在编译期完成的字符串操作（查找、替换、比较、哈希）。还提供 `type_view`、`value_view`、`self_view`。

## Requirements / 要求

- Compiler / 编译器:
  >GCC 14.2.0 or later (required for full C++23 support, especially `consteval local static`).
  >
  >GCC 14.2.0 或更高版本（需要完整支持 C++23，特别是 `consteval local static`）。
- Standard / 标准:
  >C++23 (some parts use C++20 coroutines).
  >
  >C++23（部分功能使用了 C++20 协程）。

The library is header‑only; no linking required except for threading support (if you use `acyclic.h` with multi‑threaded schedulers, you may need `-pthread`).

纯头文件库；无需链接，除非使用多线程调度器（例如 `acyclic.h` 可能需要 `-pthread`）。

## Quick Start / 快速开始

1. **Clone the repository** / **克隆仓库**

    ```sh
    git clone https://github.com/fakeloop/fake.git
    ```

2. **Include the headers** / **包含头文件**

    >Add `fake/include/` to your compiler’s include path. Then include the desired header, e.g.:
    >
    >将 `fake/include/` 添加到编译器的包含路径中。然后包含所需的头文件，例如：

    ```cpp
    #include "type_format.h"
    ```

3. **Try the demo** / **尝试示例**

    >The repository contains a demo under `fake/demo/io/`. Build it with the provided scripts:
    >
    >仓库中的 `fake/demo/io/` 目录下有一个演示程序。使用提供的脚本编译：

    **linux:**

    ```sh
    cd fake/demo/io
    ./make.sh
    ./build/demo.out
    ```

    **windows:**

    ```bat
    cd fake\demo\io
    make.bat
    build\demo.exe
    ```

    **fake/demo/io/io.cpp:**

    ```c++
    #include <iostream>
    #include <vector>
    #include <unordered_set>
    #include <unordered_map>

    #include "type_format.h"

    struct foo{
        int x;
        double y;
        std::unordered_set<int> s;
        std::unordered_map<std::string, int> m;
    };

    template<typename _Type>
    struct bar{
        std::tuple<foo, std::string, std::string> t;
        std::vector<std::string_view> v;
        _Type z;
        _Type w;
        char s[2] = "S";
        char n[2] = "N";
        char e[0];
        char m[3] = "NN";
        char l[0];
    };

    struct foobar{
    public:
        template<std::convertible_to<bar<std::string>> _Cookie>
        foobar(_Cookie &&_cookie): cookie(std::forward<_Cookie>(_cookie)){}
        
    private:
        int password = 114514;
        std::string secret = "embarrassing fact";
        bar<std::string> cookie;
    };

    // steal private members. 
    template<>
    struct fake::tool::steal<[]{}, fake::tool::adaptor<foobar>, &foobar::password, &foobar::secret, &foobar::cookie>{
        // register meta-implementations for token-based-cpo 'fake::for_each<fake::cpo::format<...>>' at compile-time. 
        using token = fake::cpo::format<fake::io::token::pretty>;
        static_assert(fake::custom::for_each::config::emplace_visitor<[]{}, token, steal>());
    };

    int main(int _argc, char* _argv[])
    {
        using namespace std::string_literals;
        
        bar<std::string> var{
            {{114, 514.0, {1919, 893}, {{"ya", 8}, {"ju", 10}}}, "MGR", "YUH"},
            {"SZ", "JOKER"},
            "DIYUSI"s,
            "NEL"s
        };
        
        foobar lost_owner{std::move(var)};
        
        std::cout << fake::io::pretty<>(lost_owner) << std::endl;
    }
    ```

    >The demo prints a colored representation of a complex object and demonstrates round‑trip parsing.
    >
    >该演示会打印一个复杂对象的彩色表示，并展示往返解析。

    [![output terminal](demo/io/output.png?raw=true)](demo/io/output.png)

4. **Use in your project** / **在你的项目中使用**

    >Just include the headers and start using the features. All configuration is done at compile time.
    >
    >只需包含头文件即可使用所有功能。所有配置都在编译期完成。

## Documentation / 文档

Each header comes with extensive comments and usage examples. The following summary links to the corresponding documentation sections (see the source files for details).

每个头文件都附有详细的注释和使用示例。以下链接指向对应的文档章节（详细内容请查看源文件）。

- `acyclic.h` [Chinese](docs/acyclic-cn.md) – [English](docs/acyclic-en.md)
- `delegate.h` [Chinese](docs/delegate-cn.md) – [English](docs/delegate-en.md)
- `detect.h` [Chinese](docs/detect-cn.md) – [English](docs/detect-en.md)
- `enum_info.h` [Chinese](docs/enum_info-cn.md) – [English](docs/enum_info-en.md)
- `flat.h` [Chinese](docs/flat-cn.md) – [English](docs/flat-en.md)
- `functor_info.h` [Chinese](docs/functor_info-cn.md) – [English](docs/functor_info-en.md)
- `is_valid.h` [Chinese](docs/is_valid-cn.md) – [English](docs/is_valid-en.md)
- `patterns.h` [Chinese](docs/patterns-cn.md) – [English](docs/patterns-en.md)
- `traits.h` [Chinese](docs/traits-cn.md) – [English](docs/traits-en.md)
- `tuple.h` [Chinese](docs/tuple-cn.md) – [English](docs/tuple-en.md)
- `type_format.h` [Chinese](docs/type_format-cn.md) – [English](docs/type_format-en.md)
- `utility.h` [Chinese](docs/utility-cn.md) – [English](docs/utility-en.md)
- `view.h` [Chinese](docs/view-cn.md) – [English](docs/view-en.md)

## License / 许可证

This library is provided under the **WTFPL**.
You can copy, modify, distribute, and use the code for any purpose, without any warranty. See the [LICENSE](LICENSE) for details.

本库使用 **WTFPL** 许可证。
你可以复制、修改、分发和使用本代码用于任何目的，无任何担保。详见 [LICENSE](LICENSE)。
