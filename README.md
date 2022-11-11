# `fake` C++ metaprogramming library

## ***brief:***

`fake` provides a set of tools of `metaprogramming`, `stateful metaprogramming` and `concurrency adapter`

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

**parameter for the `stateful metaprogramming`:**

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

`compile and launch the "demo/io/io.cpp"`

#### ***示例:***

`编译并运行 "demo/io/io.cpp"`

```sh
cd ~
mkdir my_workspace
cd my_workspace
git clone https://github.com/fakeloop/fake
cd fake/demo/io
g++ io.cpp -std=c++20 -iquote "../../include/" -Wno-non-template-friend -o io.out
./io.out
```

- ***"demo/io/io.cpp"***

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
        {{114, 514.0, {1919, 893},
        {{"ya", 8},
        {"ju", 10}}}, "MGR", "YUH"},
        {"SZ", "JOKER"},
        "DIYUSI"s,
        "NEL"s
    };
    
    foobar lost_owner{std::move(var)};
    
    std::cout << fake::io::pretty<>(lost_owner) << std::endl;
}

```

***colorized output in terminal:***

***着色后的控制台输出:***

<div style="font-family:'Courier New';font-size:14px;font-weight:bolder;color:#CCCCCC;background-color:#1E1E1E;padding:12px;">
<font color="#11A8CD">foobar</font><font color="#CCCCCC"> : </font><font color="#CCCCCC">{</font></br><font color="#878787">| &nbsp; </font><font color="#11A8CD">int</font> <font color="#E5E510">password</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">114514</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; </font><font color="#11A8CD">std::string</font> <font color="#E5E510">secret</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>embarrassing fact<font color="#DB6A6A">"</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; </font><font color="#11A8CD">bar&lt;std::string&gt;</font> <font color="#E5E510">cookie</font><font color="#CCCCCC"> : </font><font color="#4E8ED3">{</font></br><font color="#878787">| &nbsp; | &nbsp; </font><font color="#11A8CD">std::tuple&lt;foo, std::string, std::string&gt;</font> <font color="#0DBC79">0</font><font color="#CCCCCC"> : </font><font color="#E5E510">{</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">foo</font> <font color="#C965C9">0</font><font color="#CCCCCC"> : </font><font color="#11A8CD">{</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">int</font> <font color="#CCCCCC">0</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">114</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">double</font> <font color="#CCCCCC">1</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">514</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">std::unordered_set&lt;int&gt;</font> <font color="#CCCCCC">2</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">{</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">int</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">893</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">int</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">1919</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#DB6A6A">}</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">std::unordered_map&lt;std::string, int&gt;</font> <font color="#CCCCCC">3</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">{</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">std::pair&lt;const std::string, int&gt;</font><font color="#CCCCCC"> : </font><font color="#0DBC79">{</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">std::string</font> <font color="#0DBC79">first</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>ju<font color="#DB6A6A">"</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">int</font> <font color="#0DBC79">second</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">10</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#0DBC79">}</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">std::pair&lt;const std::string, int&gt;</font><font color="#CCCCCC"> : </font><font color="#0DBC79">{</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">std::string</font> <font color="#0DBC79">first</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>ya<font color="#DB6A6A">"</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">int</font> <font color="#0DBC79">second</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">8</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#0DBC79">}</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; | &nbsp; </font><font color="#DB6A6A">}</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">}</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">std::string</font> <font color="#C965C9">1</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>MGR<font color="#DB6A6A">"</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">std::string</font> <font color="#C965C9">2</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>YUH<font color="#DB6A6A">"</font></br><font color="#878787">| &nbsp; | &nbsp; </font><font color="#E5E510">}</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; </font><font color="#11A8CD">std::vector&lt;std::string_view&gt;</font> <font color="#0DBC79">1</font><font color="#CCCCCC"> : </font><font color="#E5E510">{</font></br><font color="#878787">| &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">std::string_view</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>SZ<font color="#DB6A6A">"</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; | &nbsp; </font><font color="#11A8CD">std::string_view</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>JOKER<font color="#DB6A6A">"</font></br><font color="#878787">| &nbsp; | &nbsp; </font><font color="#E5E510">}</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; </font><font color="#11A8CD">std::string</font> <font color="#0DBC79">2</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>DIYUSI<font color="#DB6A6A">"</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; </font><font color="#11A8CD">std::string</font> <font color="#0DBC79">3</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>NEL<font color="#DB6A6A">"</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; </font><font color="#11A8CD">char [2]</font> <font color="#0DBC79">4</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>S<font color="#DB6A6A">"</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; </font><font color="#11A8CD">char [2]</font> <font color="#0DBC79">5</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>N<font color="#DB6A6A">"</font><font color="#CCCCCC">,</br></font><font color="#878787">| &nbsp; | &nbsp; </font><font color="#11A8CD">char [3]</font> <font color="#0DBC79">6</font><font color="#CCCCCC"> : </font><font color="#DB6A6A">"</font>NN<font color="#DB6A6A">"</font></br><font color="#878787">| &nbsp; </font><font color="#4E8ED3">}</font></br><font color="#CCCCCC">}</font>
</div>

---

***warning:***

the `stateful metaprogramming` is some sort of **black magic**, which has always been ***controversial***.

The C++ standard may drop support for this feature in the future, although the issue is currently in an open state.

the following headers contain **black magic**: `include/type_map.h` `include/meta.h` `include/type_name.h` `include/type_info.h` `include/type_stream.h` `include/type_format.h`.

***警告:***

`有状态元编程` 是一种 **黑魔法**, 一直以来都存在 ***争议***。

C++ 标准可能会在未来放弃对该特性的支持, 虽然目前该议题处于开放状态。

这些头文件包含 **black magic**: `include/type_map.h` `include/meta.h` `include/type_name.h` `include/type_info.h` `include/type_stream.h` `include/type_format.h`。

---

## `metaprogramming`

## `元编程`

***version:***

the `metaprogramming` subsystem of `fake` is interface-stable currently.

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

## `stateful metaprogramming`

## `有状态元编程`

***version:***

the `stateful metaprogramming` subsystem of `fake` is interface-stable currently.

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

**注意: 编译器只会为某个函数或类型进行一次语法树生成, 因此在编写并多次调用编译期函数时请小心。如果您希望运行一个编译期程序, 您永远应该在类似 `template<auto> auto demo()` 的函数内部编写这些编译期代码, 并且通过 `demo<[]{}>()` 调用它们。每次您所调用的都将是 `demo()` 模板函数的一个不同特化, 这是由于每个 `[]{}` 都拥有不同的类型。编译器永远无法跳过 `demo()` 函数的语法树生成过程, 因为要推导出返回值类型 `auto` 就必须编译函数体定义。**

*本头文件 `"meta.h"` 中定义的模板函数和模板成员函数可以接收一个伪参数 `[]{}`, 目的是强制编译器重新进行特化。如要在其他模板上下文中调用这些模板函数, 在使用 `gcc-12.x.x` 时, 模板参数 `[]{}` 是必须的, 但使用 `gcc-11.x.x` 时不是必须的。其通常为首个模板参数, 默认接收 `template<auto = refresh(tool::token{}, key{}), ...>`。*

***example:***

*demo see `"demo/meta/"` `"demo/format/"` `"demo/print/"`*

`"demo/format/"` and `"demo/print/"` are comprehensive examples to demonstrate the generic techniques of the `stateful metaprogramming`

***示例:***

*示例见 `"demo/meta/"` `"demo/format/"` `"demo/print/"`*

`"demo/format/"` 和 `"demo/print/"` 是用于演示 `有状态元编程` 通用技巧的综合示例

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

`"demo/print/":`

```sh
./make.sh
```

---

### `"type_format.h"`

***brief:***

the `fake::io` is a customizable `object formatter` with several preset format schemes `fake::io::plain<>` `fake::io::pretty<>` `fake::io::json<>` `fake::io::html<>`

the `standard template container libraries` should always be included before the `"type_format.h"` library

the `standard template container libraries` are `<vector>` `<list>` `<forward_list>` `<deque>` `<set>` `<unordered_set>` `<map>` `<unordered_map>`

***简介:***

`fake::io` 是一个可定制的 `对象格式化器`, 其提供了一些预设格式方案 `fake::io::plain<>` `fake::io::pretty<>` `fake::io::json<>` `fake::io::html<>`

应确保在包含 `标准模板容器库` 后再包含 `"type_format.h"` 库

`标准模板容器库` 包括 `<vector>` `<list>` `<forward_list>` `<deque>` `<set>` `<unordered_set>` `<map>` `<unordered_map>`

***usage:***

***使用方法:***

```c++
// #include <vector.h>
// #include <list.h>
// ...

#include "type_format.h"
```

***example:***

*demo see `"demo/io/"`*

***示例:***

*示例见 `"demo/io/"`*

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
