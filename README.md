# `fake` C++ meta-programming library

## ***brief:***

`fake` provides a set of tools of `meta-programming`, `stateful meta-programming` and `concurrency adapter`

`fake` is a header-only library

## ***简介:***

`fake` 提供了一套 `元编程`, `有状态元编程` 和 `并发适配器` 工具

`fake` 是一个纯头文件库

---

## ***compile:***

all codes passed the compilation test on `gcc-11.2.0` `gcc-11.2.1` `gcc-12.2.0`.

## ***编译:***

所有代码已于 `gcc-11.2.0` `gcc-11.2.1` `gcc-12.2.0` 通过编译测试。

---

### addtional parameters for `g++`

### 额外的 `g++` 参数

**`always` required:**

**`必需` 参数:**

```sh
-std=c++20 -iquote "fake/include"
```

- *the `fake/include` is the directory of the headers in the `repository` you cloned.*
- *`fake/include` 是 `仓库` 克隆目录内的头文件所在目录。*

---

**parameter for the `stateful meta-programming`:**

**`有状态元编程` 所需参数:**

```sh
-Wno-non-template-friend
```

---

**parameter for the `concurrency adapter`:**

**`并发适配器` 所需参数:**

```sh
-fcoroutines
```

---

#### ***example:***

`compile and launch the "demo/tuple/tuple.cpp"`

#### ***示例:***

`编译并运行 "demo/tuple/tuple.cpp"`

```sh
cd ~
mkdir my_workspace
cd my_workspace
git clone https://github.com/fakeloop/fake
cd fake/demo/tuple
g++ tuple.cpp -std=c++20 -iquote "../../include/" -o tuple.out
./tuple.out
```

***warning:***

the `stateful meta-programming` is some sort of **black magic**, which has always been ***controversial***.

The C++ standard may drop support for this feature in the future, although the issue is currently in an open state.

***警告:***

`有状态元编程` 是一种 **黑魔法**, 一直以来都存在 ***争议***。

C++ 标准可能会在未来放弃对该特性的支持，虽然目前该议题处于开放状态。

---

## `meta-programming`

## `元编程`

***version:***

the `meta-programming` subsystem of `fake` is interface-stable currently.

***版本:***

`fake` `元编程` 子系统目前接口稳定。

---

<!---- >

### `"is_valid.h"`

---

### `"traits.h"`

---

<!---->

### `"symbol.h"`

***brief:***

compile time convert `type` to `constexpr std::string_view` or `constexpr fake::symbol::view`

***简介:***

编译期将 `类型` 转换为 `constexpr std::string_view` 或 `constexpr fake::symbol::view`

***usage:***

***使用方法:***

```c++
#include <iostream>
#include "symbol.h"

int main(int _argc, char* _argv[]){
    std::cout << fake::symbol::string_view<decltype(main)>() << std::endl;
    return 0;
}
```

***output:***

***输出:***

```plain
int(int, char**)
```

---

<!---- >

### `"functor_info.h"`

---

<!---->

### `"tuple.h"`

***brief:***

compile time `lambda` functor algorithm tool kit for `std::tuple`

***简介:***

用于 `std::tuple` 的 `lambda` 算子编译期算法工具集

***usage:***

***使用方法:***

```c++
#include "tuple.h"
```

***example:***

*demo see `"demo/tuple/tuple.cpp"`*

***示例:***

*示例见 `"demo/tuple/tuple.cpp"`*

---

### `"view.h"`

***brief:***

`std::view` provides a pure compile time string template implementation with literals

***简介:***

`std::view` 提供了一个带字面量的纯编译期字符串模板实现

***usage:***

***使用方法:***

```c++
#include <iostream>
#include "view.h"

int main(int _argc, char* _argv[]){
    using namespace fake::literals;
    constexpr fake::view origin = "hello view"_v;
    constexpr auto dupli = decltype(origin){}; // make a new copy just from the type. 
    constexpr auto& data = dupli.substr<6>().data();
    static_assert(std::same_as<const char(&)[5], decltype(data)>);
    std::cout << data << std::endl;
    return 0;
}
```

***output:***

***输出:***

```plain
view
```

***example:***

*demo see `"demo/view/view.cpp"`*

***示例:***

*示例见 `"demo/view/view.cpp"`*

---

## `stateful meta-programming`

## `有状态元编程`

***version:***

the `stateful meta-programming` subsystem of `fake` is interface-stable currently.

***版本:***

`fake` `有状态元编程` 子系统目前接口稳定。

---

### `"meta.h"`

***brief:***

compile time variant and containers implemented via the **black magic**

please use only after fully understanding the risks involved

***简介:***

通过**黑魔法**实现的编译期变量与容器

请在使用前充分了解相关风险

***usage:***

***使用方法:***

```c++
#include "meta.h"

int main(int _argc, char* _argv[])
{
    constexpr fake::meta::unordered_map um = []{};
    
    struct mgr{}; struct yuh{};
    struct diyusi{}; struct nel{};
    struct nyn{}; struct icg{};
    struct sz{}; struct joker{};
    
    um.emplace<[]{}>(fake::pack_v<mgr>, fake::pack_v<yuh>);
    um.emplace<[]{}>(fake::pack_v<diyusi>, fake::pack_v<nel>);
    um.emplace<[]{}>(fake::pack_v<nyn>, fake::pack_v<icg>);
    um.emplace<[]{}>(fake::pack_v<sz>, fake::pack_v<joker>);
    
    static_assert(
        std::is_same_v<
            decltype(um.data([]{})),
            fake::meta::array<
                std::pair<mgr, yuh>,
                std::pair<diyusi, nel>,
                std::pair<nyn, icg>,
                std::pair<sz, joker>
            >
        >
    );
    
    struct kofji{};
    
    um.emplace<[]{}>(fake::pack_v<nyn>, fake::pack_v<kofji>);
    
    static_assert(
        std::is_same_v<
            decltype(um.data([]{})),
            fake::meta::array<
                std::pair<mgr, yuh>,
                std::pair<diyusi, nel>,
                std::pair<nyn, kofji>,
                std::pair<sz, joker>
            >
        >
    );
    
    return 0;
}
```

**Node: The compiler would only do one AST generation for a function or type, so be careful when writing some compile-time functions, as well as when invoking them for multiple times. If you expect to run a compile-time program, you should always write the compile-time code in some functions like `template<auto> auto demo()`, and invoke them by writing `demo<[]{}>()`. The function you invoked each time would be a different specialization of the template function `demo()`, since every `[]{}` has a different type. The compiler could never skip the AST generation process of the function `demo()`, since the return type `auto` could only be deduced by compiling the definition of the function body.**

*The template functions and template member functions defined in this header `"meta.h"` may take a dummy parameter `[]{}` in order to force the compiler to generate a new specialization. The template parameter `[]{}` is necessary for `gcc-12.x.x`, but not necessary for `gcc-11.x.x` when invoking these template functions in another template context. It is usually the first parameter of the template, taking something like `template<auto = refresh(tool::token{}, key{}), ...>` by default.*

**注意: 编译器只会为某个函数或类型进行一次语法树生成, 因此在编写并多次调用编译期函数时请小心。如果您希望运行一个编译期程序，您永远应该在类似 `template<auto> auto demo()` 的函数内部编写这些编译期代码, 并且通过 `demo<[]{}>()` 调用它们。每次您所调用的都将是 `demo()` 模板函数的一个不同特化, 这是由于每个 `[]{}` 都拥有不同的类型。编译器永远无法跳过 `demo()` 函数的语法树生成过程, 因为要推导出返回值类型 `auto` 就必须编译函数体定义。**

*本头文件 `"meta.h"` 中定义的模板函数和模板成员函数可以接收一个伪参数 `[]{}`, 目的是强制编译器重新进行特化。如要在其他模板上下文中调用这些模板函数, 在使用 `gcc-12.x.x` 时, 模板参数 `[]{}` 是必须的, 但使用 `gcc-11.x.x` 时不是必须的。其通常为首个模板参数, 默认接收 `template<auto = refresh(tool::token{}, key{}), ...>`。*

***example:***

*demo see `"demo/meta/"` & `"demo/format/"`*

`"demo/format/"` is a comprehensive example to demonstrate the generic techniques of the `stateful meta-programming`

***示例:***

*示例见 `"demo/meta/"` & `"demo/format/"`*

`"demo/format/"` 是用于演示 `有状态元编程` 通用技巧的综合示例

***compile:***

***编译:***

`"demo/meta/":`

```sh
g++ meta.cpp -o meta.out -iquote "../../include" -std=c++20 -Wno-non-template-friend
```

`"demo/format/":`

```sh
./make.sh
```

---

## `concurrency adapter`

## `并发适配器`

***version:***

the `concurrency adapter` subsystem of `fake` is **NOT** interface-stable yet.

***版本:***

`fake` `并发适配器` 子系统目前接口 **尚未** 稳定。

---

### `"acyclic.h"`

***brief:***

compile time `directed acyclic graph` execution flow adapter, adapting delegations by `compile time type representation`

***简介:***

编译期 `有向无环图` 执行流适配器, 依照 `编译期类型表示` 适配委托过程

***usage:***

***使用方法:***

```c++
#include "acyclic.h"
```

***example:***

*demo see `"demo/acyclic/"`*

***示例:***

*示例见 `"demo/acyclic/"`*
