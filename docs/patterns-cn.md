# `patterns.h`

- 用于编译期编程的设计模式与工具：singleton、gueston（跨二进制引用）、intact（移动/拷贝追踪）、registry（编译期注册）以及 builder（具名参数构建器）。

---

## `fake::patterns::singleton`

### 简介

一个模板结构体，管理一组单例类型。每个类型只实例化一次，并通过 `plug<Type>()` 或 `rash<Type>()`（极端情况下使用）访问。单例本身是一个 Meyers 单例；如果程序中任一编译单元实例化了 `singleton<...>::instance()`，其生命周期会在 `main()` 之前开始并在 `main()` 之后结束；否则，其生命周期可能会延迟开始。

### 语法

```c++
template<tool::plug_c... _Plugs>
struct singleton {
    template<tool::member_c<_Plugs...> _Plug>
    [[nodiscard]] static bool ready() noexcept;
    
    template<tool::member_c<_Plugs...> _Plug>
    static _Plug& plug();
    
    template<tool::member_c<_Plugs...> _Plug>
    static _Plug& rash();   // like plug, but forces instantiation even if not yet needed
    
    static constexpr auto instance();   // touches the singleton to ensure it exists
    static void inject(void (*_dynamic)(uint64_t, void*)) noexcept;   // for exe to pass pointers to dll
};
```

### 示例

以下两个文件演示了一个持有三个类（`foo`、`bar`、`doo`）的单例。可执行文件（`singleton.exe`）创建单例，同时加载一个动态库（`dynamic.dll`），该动态库通过 `gueston`（见下一节）访问相同的单例实例。

#### `singleton.cpp`（可执行文件）

```c++
#include <print>
#include "patterns.h"

struct foo; struct bar; struct doo;

using singleton = fake::patterns::singleton<foo, bar, doo>;

struct doo final{
    doo(){std::println("{}", fake::self_view([]{}).data());}
    ~doo(){std::println("{}", fake::self_view([]{}).data());}
    void print(){std::println("{}: {}", data, fake::self_view([]{}).data());}
    std::string data = "doo";
};

struct foo final{
    foo(){std::println("{}", fake::self_view([]{}).data()); singleton::rash<doo>().print();}
    ~foo(){std::println("{}", fake::self_view([]{}).data());}
    void print(){std::println("{}: {}", data, fake::self_view([]{}).data());}
    std::string data = "foo";
};

struct bar final{
    bar(){
        std::println(
            "singleton @ {}, ready<foo>: {}, ready<bar>: {}, ready<doo>: {}",
            fake::self_view([]{}).data(),
            singleton::ready<foo>(),
            singleton::ready<bar>(),
            singleton::ready<doo>()
        );
    }
    ~bar(){std::println("{}", fake::self_view([]{}).data());}
    void print(){std::println("{}: {}", data, fake::self_view([]{}).data());}
    std::string data = "bar";
};

#include <windows.h>

int main(){
    std::println("{}", fake::self_view([]{}).data());
    
    singleton::plug<foo>().print();
    singleton::plug<bar>().print();
    singleton::plug<doo>().print();
    
    singleton::plug<foo>().data = "foo!";
    singleton::plug<bar>().data = "bar!";
    singleton::plug<doo>().data = "doo!";
    
    using reference_t = void (*)(uint64_t _signature, void* _array);
    using run_t = void (*)();
    
    const auto dynamic = LoadLibrary("dynamic.dll");
    
    if(dynamic == nullptr)
        return 1;
    
    const reference_t reference = reinterpret_cast<reference_t>(GetProcAddress(dynamic, "reference"));
    const run_t run = reinterpret_cast<run_t>(GetProcAddress(dynamic, "run"));
    
    if(reference && run)
        singleton::inject(reference), run();
    
    FreeLibrary(dynamic);
    
    singleton::plug<foo>().print();
    singleton::plug<bar>().print();
    singleton::plug<doo>().print();
    
    std::println("~{}", fake::self_view([]{}).data());
}

inline constexpr auto touch = singleton::instance();
```

#### `dynamic.cpp`（动态库）

```c++
#include <print>
#include "patterns.h"

struct foo; struct bar; struct doo;

using singleton = fake::patterns::singleton<foo, bar, doo>;

struct doo final{
    doo(){std::println("{}", fake::self_view([]{}).data());}
    ~doo(){std::println("{}", fake::self_view([]{}).data());}
    void print(){std::println("{}: {}", data, fake::self_view([]{}).data());}
    std::string data = "doo";
};

struct foo final{
    foo(){std::println("{}", fake::self_view([]{}).data()); singleton::rash<doo>().print();}
    ~foo(){std::println("{}", fake::self_view([]{}).data());}
    void print(){std::println("{}: {}", data, fake::self_view([]{}).data());}
    std::string data = "foo";
};

struct bar final{
    bar(){
        std::println(
            "singleton @ {}, ready<foo>: {}, ready<bar>: {}, ready<doo>: {}",
            fake::self_view([]{}).data(),
            singleton::ready<foo>(),
            singleton::ready<bar>(),
            singleton::ready<doo>()
        );
    }
    ~bar(){std::println("{}", fake::self_view([]{}).data());}
    void print(){std::println("{}: {}", data, fake::self_view([]{}).data());}
    std::string data = "bar";
};

using gueston = fake::patterns::gueston<foo, bar, doo>;

extern "C" __declspec(dllexport) void reference(uint64_t _signature, void* _array) noexcept{
    std::println(
        "gueston @ {}, ready<foo>: {}, ready<bar>: {}, ready<doo>: {}",
        fake::self_view([]{}).data(),
        gueston::ready<foo>(),
        gueston::ready<bar>(),
        gueston::ready<doo>()
    );
    
    gueston::reference(_signature, _array);
}

extern "C" __declspec(dllexport) void run() noexcept{
    std::println("{}", fake::self_view([]{}).data());
    
    std::println(
        "singleton @ {}, ready<foo>: {}, ready<bar>: {}, ready<doo>: {}",
        fake::self_view([]{}).data(),
        singleton::ready<foo>(),
        singleton::ready<bar>(),
        singleton::ready<doo>()
    );
    
    std::println(
        "gueston @ {}, ready<foo>: {}, ready<bar>: {}, ready<doo>: {}",
        fake::self_view([]{}).data(),
        gueston::ready<foo>(),
        gueston::ready<bar>(),
        gueston::ready<doo>()
    );
    
    gueston::plug<foo>().print();
    gueston::plug<bar>().print();
    gueston::plug<doo>().print();
    
    gueston::plug<foo>().data = "foo!!";
    gueston::plug<bar>().data = "bar!!";
    gueston::plug<doo>().data = "doo!!";
    
    std::println("~{}", fake::self_view([]{}).data());
}

inline constexpr auto touch = gueston::instance();
```

**输出：**（来自 `singleton.exe`）

```plain
foo::foo()
doo::doo()
doo: doo::print()
singleton @ bar::bar(), ready<foo>: true, ready<bar>: false, ready<doo>: true
main()
foo: foo::print()
bar: bar::print()
doo: doo::print()
gueston @ reference(uint64_t, void*), ready<foo>: false, ready<bar>: false, ready<doo>: false
run()
singleton @ run(), ready<foo>: false, ready<bar>: false, ready<doo>: false
gueston @ run(), ready<foo>: true, ready<bar>: true, ready<doo>: true
foo!: foo::print()
bar!: bar::print()
doo!: doo::print()
~run()
foo!!: foo::print()
bar!!: bar::print()
doo!!: doo::print()
~main()
doo::~doo()
bar::~bar()
foo::~foo()
```

**编译（Windows, MinGW）：**

*可执行文件：*

```bat
g++ -c -o "singleton.o" "singleton.cpp" -iquote "<fake_dir>/include/" -std=c++23
g++ -o "singleton.exe" "singleton.o" -lstdc++exp -s -static-libstdc++ -static-libgcc -static -pthread
```

*动态库：*

```bat
g++ -c -o "dynamic.o" "dynamic.cpp" -iquote "<fake_dir>/include/" -std=c++23
g++ -shared -o "dynamic.dll" "dynamic.o" -lstdc++exp -s -static-libstdc++ -static-libgcc -static -pthread
```

> **注意：** 此处演示的 `gueston` 是 Windows 特定的（使用了 `__declspec(dllexport)` 和显式指针传递）。在 Linux 上可以通过编译选项（如 `-rdynamic`）或内建属性实现类似的跨二进制符号解析；此处展示的模式仅为一种可行实现。
>
> **注意：** 类型的初始化顺序与模板参数的传入顺序一致，除非通过调用 `singleton<...>::rash()` 强行改变初始化顺序。如果在构造函数中不使用 `singleton<...>::rash()` 却以与给定顺序不同的依赖关系强制调用 `singleton<...>::plug()`，则会抛出异常。

---

## `fake::patterns::gueston`

### 简介

用于动态库的 `singleton` 配套组件。它持有由宿主可执行文件提供的单例实例的指针。宿主通过回调调用 `singleton::inject`；库的 `reference` 函数接收指针数组并进行存储。之后库可以通过 `gueston::plug<Type>()` 访问宿主的单例对象。

### 语法

```c++
template<tool::plug_c... _Plugs>
struct gueston {
    template<tool::member_c<_Plugs...> _Plug>
    [[nodiscard]] static bool ready() noexcept;
    
    template<tool::member_c<_Plugs...> _Plug>
    static _Plug& plug();   // may throw if not yet referenced
    
    static constexpr auto instance();
    static void reference(uint64_t _signature, void* _array) noexcept;   // called by host
};
```

### 示例

参见上方 `singleton` 部分中的示例。动态库使用 `gueston` 获取可执行文件创建的相同实例。

---

## `fake::patterns::intact`

### 简介

一个小型辅助类，用于追踪对象在拷贝/移动操作后是否保持“完好”（即未被移走）。它由一个枚举 `intact_e` 参数化地控制其行为：

- `origin`      – 拷贝保持源完好，目标变为*非完好*。
- `duplicate`   – 拷贝使源变为*非完好*，目标完好。
- `both`        – 拷贝保持两者完好（源不变，目标完好）。
- `neither`     – 拷贝使两者都变为*非完好*。
- `moveonly`    – 拷贝被删除；移动使源变为*非完好*，目标完好。

### 语法

```c++
enum struct intact_e : std::size_t { origin, duplicate, both, neither, moveonly };

template<intact_e _policy>
struct intact final {
    constexpr operator bool() const noexcept;   // returns current intact state
    // constructors and assignment operators as per policy
};
```

### 示例

```c++
#include <print>
#include "patterns.h"
#include "enum_info.h"   // for fake::enum_info_for_v (used in example but not essential)

int main()
{
    constexpr auto example = [](fake::mezz_c auto _enum){
        struct object final { fake::patterns::intact<_enum.value> intact; };
        
        std::println("------ {} ------", fake::enum_info_for_v<fake::patterns::intact_e::moveonly>[_enum].data());
        
        object src_copy, dst_copy, src_move, dst_move;
        std::println(
            "src_copy: {}, dst_copy: {}, src_move: {}, dst_move: {}",
            bool(src_copy.intact),
            bool(dst_copy.intact),
            bool(src_move.intact),
            bool(dst_move.intact)
        );
        if constexpr(requires{ dst_copy = src_copy; }){
            dst_copy = src_copy, dst_move = std::move(src_move);
            std::println(
                "src_copy: {}, dst_copy: {}, src_move: {}, dst_move: {}",
                bool(src_copy.intact),
                bool(dst_copy.intact),
                bool(src_move.intact),
                bool(dst_move.intact)
            );
        }
        else{
            dst_move = std::move(src_move);
            std::println("src_move: {}, dst_move: {}", bool(src_move.intact), bool(dst_move.intact));
        }
        
        object origin_copy, origin_move;
        std::println("origin_copy: {}, origin_move: {}", bool(origin_copy.intact), bool(origin_move.intact));
        if constexpr(requires{ object{origin_copy}; }){
            object construct_copy{origin_copy}, construct_move{std::move(origin_move)};
            std::println(
                "origin_copy: {}, construct_copy: {}, origin_move: {}, construct_move: {}",
                bool(origin_copy.intact),
                bool(construct_copy.intact),
                bool(origin_move.intact),
                bool(construct_move.intact)
            );
        }
        else{
            object construct_move{std::move(origin_move)};
            std::println("origin_move: {}, construct_move: {}", bool(origin_move.intact), bool(construct_move.intact));
        }
    };
    
    example(fake::mezz_v<fake::patterns::intact_e::origin>);
    example(fake::mezz_v<fake::patterns::intact_e::duplicate>);
    example(fake::mezz_v<fake::patterns::intact_e::both>);
    example(fake::mezz_v<fake::patterns::intact_e::neither>);
    example(fake::mezz_v<fake::patterns::intact_e::moveonly>);
}
```

**输出：**

```plain
------ fake::patterns::intact_e::origin ------
src_copy: true, dst_copy: true, src_move: true, dst_move: true
src_copy: true, dst_copy: false, src_move: false, dst_move: true
origin_copy: true, origin_move: true
origin_copy: true, construct_copy: false, origin_move: false, construct_move: true
------ fake::patterns::intact_e::duplicate ------
src_copy: true, dst_copy: true, src_move: true, dst_move: true
src_copy: false, dst_copy: true, src_move: false, dst_move: true
origin_copy: true, origin_move: true
origin_copy: false, construct_copy: true, origin_move: false, construct_move: true
------ fake::patterns::intact_e::both ------
src_copy: true, dst_copy: true, src_move: true, dst_move: true
src_copy: true, dst_copy: true, src_move: false, dst_move: true
origin_copy: true, origin_move: true
origin_copy: true, construct_copy: true, origin_move: false, construct_move: true
------ fake::patterns::intact_e::neither ------
src_copy: true, dst_copy: true, src_move: true, dst_move: true
src_copy: false, dst_copy: false, src_move: false, dst_move: true
origin_copy: true, origin_move: true
origin_copy: false, construct_copy: false, origin_move: false, construct_move: true
------ fake::patterns::intact_e::moveonly ------
src_copy: true, dst_copy: true, src_move: true, dst_move: true
src_move: false, dst_move: true
origin_copy: true, origin_move: true
origin_move: false, construct_move: true
```

---

## `fake::patterns::registry`

### 简介

一个 CRTP 基类，强制在 `main()` 之前实例化一个静态成员，从而在程序启动（或 DSO 加载）时执行一段代码。委托（lambda 或函数）仅被调用一次。可用于编译期注册。

### 语法

```c++
template<typename _Type, auto _delegate>
struct registry : detail::facet<registry<_Type, _delegate>> {
    using type = _Type;
    static constexpr auto delegate = _delegate;
};
```

委托可以是：

- 一个不带参数的 lambda，
- 一个接受 `pazz_v<type>` 或 `pack_v<type>` 的 lambda，
- 一个带有 `operator()<type>()` 的模板 lambda。

### 示例

```c++
#include <print>
#include "patterns.h"

int main()
{
    std::println("\n------ fake::patterns::registry ------\n");
}

constexpr auto aspect = [](fake::pazz_c auto _e){
    std::println("invoke before main, message = \"{}\"", fake::take_t<_e>{}.message.data());
};

struct injector final : fake::patterns::registry<injector, aspect>{
    const fake::view_t<"compile time message"> message;
};
```

**输出：**（registry 的委托在 `main` 之前运行）

```plain
invoke before main, message = "compile time message"

------ fake::patterns::registry ------
```

> **注意：** 委托在静态初始化期间被调用，早于 `main()`。

---

## `fake::patterns::builder`

### 简介

一个具名参数构建器，使用编译期键（视图）累积参数。它支持默认值、类型转换以及通过检测器（detector）施加约束。最终的构建器可以传递给一个按键提取参数的函数。

两种参数类型：

- `arg<Key, Config, Detector = null_t>` – 直接持有值（或一个按需生成值的仿函数）。
- `ref<Key, Config, Detector = null_t>` – 持有一个引用（或一个按需生成值的仿函数）。

`Config` 可以是：

- 一个具体类型（用于立即值），
- 一个 `pack_v<Type>` 或 `pazz_v<Type>`（表示类型已知但值将在之后提供），
- 一个用 `mezz` 包装的、返回值的仿函数（延迟求值）。

构建器提供 `operator[]` 来设置参数，并提供 `forward(key)` 来获取值（遵循延迟求值规则）。

### 语法

```c++
template<fake::view_c auto _tag, detail::builder::arg... _args>
struct builder {
    static constexpr bool valid = ...;   // true if all required arguments are satisfied
    template<fake::view_c auto _key>
    constexpr auto operator[](_key) const & -> /* proxy */;
    // ... similar for &, &&, const &&
    
    template<fake::view_c auto _key>
    constexpr auto forward(_key) const &;   // obtain value (respects delay)
    template<fake::view_c auto _key>
    constexpr auto&& forward(_key) &&;
};
```

辅助类型：

- `arg<Key, Config, Detector>` – 声明一个值参数（可以绑定到延迟仿函数）。
- `ref<Key, Config, Detector>` – 声明一个引用参数（可以绑定到延迟仿函数）。

### 示例

```c++
#include <print>
#include "patterns.h"

int main()
{
    using namespace fake::literals;
    
    // Demonstrate a non-structural type as parameter
    struct complex final{
        complex(const std::string _e): data(_e) {}
        [[nodiscard]] const std::string& get() const noexcept { return data; }
    private:
        const std::string data;
    };
    
    static constexpr auto origin = [](int _i, const float &_f, fake::view_c auto _v, double _m, complex _c){
        std::println("{}, {}, \"{}\", {}, \"{}\"_v", _i, _f, _v.data(), _m, _c.get());
        return 114514;
    };
    
    constexpr fake::patterns::builder<
        fake::view_v<"some-tag">,
        fake::patterns::arg{fake::view_v<"i">, 114},
        fake::patterns::ref{fake::view_v<"f">, 514.0f, fake::convertible_to_d<float>},
        fake::patterns::ref{fake::view_v<"v">, "default"_v, fake::view_d},
        fake::patterns::ref{fake::view_v<"m">, fake::pack_v<double>},
        fake::patterns::ref{fake::view_v<"c">, fake::mezz_v<[]{ return complex{"default"}; }>}
    > info{};
    
    static constexpr auto interface = []<fake::patterns::builder_c _Type>(const _Type &_e)
    requires (_Type::tag == "some-tag"_v) && _Type::valid {
        return origin(_e.forward("i"_v), _e.forward("f"_v), _e.forward("v"_v), _e.forward("m"_v), _e.forward("c"_v));
    };
    
    interface(
        info
        ["i"_v](1919) // int
        ["f"_v](810)  // convert to float
        ["v"_v]("yaju"_v) // fake::view_d
        ["m"_v](531.0) // type double, without default value
        ["c"_v](complex{"run time"}) // compile time bound run time default value
    );
    
    interface(info["m"_v](364.0));
    
    // interface(info["i"_v](1919)); // the parameter of type double with key "m"_v must be provided
}
```

**输出：**

```plain
1919, 810, "yaju", 531, "run time"_v
114, 514, "default", 364, "default"_v
```

> **解释：** 构建器 `info` 声明了五个参数：
>
> - `"i"` – 一个 `arg`，默认值为 `114`。
> - `"f"` – 一个 `ref`，默认值为 `514.0f`，并带有检测器 `convertible_to_d<float>`（因此像 `810` 这样的 `int` 会被接受）。
> - `"v"` – 一个 `ref`，默认值为 `"default"_v`，并带有检测器 `view_d`（接受任何 `fake::view`）。
> - `"m"` – 一个 `ref`，类型为 `double` 但没有默认值；必须提供。
> - `"c"` – 一个 `ref`，带有一个返回 `complex` 对象的仿函数。该仿函数仅在调用 `forward` 时才被求值（延迟求值）。

---

## 依赖项

上述示例需要以下头文件：

- `"patterns.h"` – 始终需要。
- `"enum_info.h"` – 仅在 `intact` 示例中用于 `fake::enum_info_for_v`（如果将该行替换为手动字符串则可以省略）。
- `<print>` – C++23 打印（也可以用 `<iostream>` 替代）。
- 根据需要包含 `<string>`、`<concepts>` 等。

对于 singleton/gueston 示例，使用了 Windows 头文件（`<windows.h>`）；其他平台请相应调整。
