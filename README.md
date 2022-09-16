# `fake` C++ meta-programming library

## ***brief:***

`fake` provides a set of tools of `meta-programming`<!--, `stateful meta-programming`--> and `concurrency adapter`

`fake` is a header-only library

## ***简介:***

`fake` 提供了一套 `元编程`<!--, `有状态元编程`--> 和 `并发适配器` 工具

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

<!---- >

**parameter for the `stateful meta-programming`:**

**`有状态元编程` 所需参数:**

```sh
-Wno-non-template-friend
```

---

<!---->

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

<!---- >
***warning:***

the `stateful meta-programming` is some sort of **black magic**, which has always been ***controversial***.

***警告:***

`有状态元编程` 是一种 **黑魔法**, 一直以来都存在 ***争议***。

<!---->

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

<!---- >

## `stateful meta-programming`

## `有状态元编程`

---

<!---->

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
