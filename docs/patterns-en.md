# `patterns.h`

- Design patterns and utilities for compile‑time programming: singleton, gueston (cross‑binary reference), intact (move/copy tracking), registry (compile‑time registration), and builder (named parameter builder).

---

## `fake::patterns::singleton`

### brief

A template struct that manages a set of singleton types. Each type is instantiated once, and accessed via `plug<Type>()` or `rash<Type>()` (for extreme situation). The singleton itself is a Meyers’ singleton; its lifetime begins before `main()` and ends after `main()` eagerly if `singleton<...>::instace()` has been instantiated in any compilation unit of the program; otherwise, its lifetime may begin lazily.

### syntax

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

### example

The following two files demonstrate a singleton that holds three classes (`foo`, `bar`, `doo`). The executable (`singleton.exe`) creates the singleton and also loads a dynamic library (`dynamic.dll`) which accesses the same singleton instances via `gueston` (see next section).

#### `singleton.cpp` (executable)

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

#### `dynamic.cpp` (dynamic library)

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

**output:** (from `singleton.exe`)

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

**Compilation (Windows, MinGW):**

*Executable:*

```bat
g++ -c -o "singleton.o" "singleton.cpp" -iquote "<fake_dir>/include/" -std=c++23
g++ -o "singleton.exe" "singleton.o" -lstdc++exp -s -static-libstdc++ -static-libgcc -static -pthread
```

*Dynamic library:*

```bat
g++ -c -o "dynamic.o" "dynamic.cpp" -iquote "<fake_dir>/include/" -std=c++23
g++ -shared -o "dynamic.dll" "dynamic.o" -lstdc++exp -s -static-libstdc++ -static-libgcc -static -pthread
```

> **Note:** The `gueston` demonstrate is Windows‑specific (using `__declspec(dllexport)` and explicit pointer passing). On Linux one can achieve similar cross‑binary symbol resolution with compiler flags like `-rdynamic` or built‑in attributes; the pattern shown here is just one possible implementation.
>
> **Note:** The initialization order of types is corresponding to the order of passed template parameters, unless the initialization order is forcibly changed by calling `singleton<...>::rash()`. If `singleton<...>::plug()` is forcibly invoked in the constructor with dependencies in a different order than given without using `singleton<...>::rash()`, an exception will be thrown.

---

## `fake::patterns::gueston`

### brief

A companion to `singleton` for dynamic libraries. It holds pointers to singleton instances provided by the host executable. The host calls `singleton::inject` with a callback; the library’s `reference` function receives an array of pointers and stores them. Afterwards the library can access the host’s singleton objects via `gueston::plug<Type>()`.

### syntax

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

### example

See the example in the `singleton` section above. The dynamic library uses `gueston` to obtain the same instances created by the executable.

---

## `fake::patterns::intact`

### brief

A small helper class that tracks whether an object is “intact” (i.e., not moved‑from) after copy/move operations. It is parameterised by an enumeration `intact_e` that controls the behaviour:

- `origin`      – copy keeps source intact, destination becomes *non‑intact*.
- `duplicate`   – copy makes source *non‑intact*, destination intact.
- `both`        – copy keeps both intact (source unchanged, destination intact).
- `neither`     – copy makes both *non‑intact*.
- `moveonly`    – copy is deleted; move makes source *non‑intact*, destination intact.

### syntax

```c++
enum struct intact_e : std::size_t { origin, duplicate, both, neither, moveonly };

template<intact_e _policy>
struct intact final {
    constexpr operator bool() const noexcept;   // returns current intact state
    // constructors and assignment operators as per policy
};
```

### example

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

**output:**

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

### brief

A CRTP base that forces instantiation of a static member before `main()`, thereby executing a piece of code at program startup (or DSO load). The delegate (a lambda or function) is called once. This can be used for compile‑time registration.

### syntax

```c++
template<typename _Type, auto _delegate>
struct registry : detail::facet<registry<_Type, _delegate>> {
    using type = _Type;
    static constexpr auto delegate = _delegate;
};
```

The delegate can be:

- a lambda that takes no arguments,
- a lambda that takes a `pazz_v<type>` or `pack_v<type>`,
- a templated lambda with `operator()<type>()`.

### example

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

**output:** (the registry’s delegate runs before `main`)

```plain
invoke before main, message = "compile time message"

------ fake::patterns::registry ------
```

> **Note:** The delegate is invoked during static initialization, before `main()`.

---

## `fake::patterns::builder`

### brief

A named‑parameter builder that accumulates arguments with compile‑time keys (views). It supports default values, type conversions, and constraints via detectors. The final builder can be passed to a function that extracts arguments by key.

Two kinds of parameters:

- `arg<Key, Config, Detector = null_t>` – holds a value directly (or a functor that produces a value on demand).
- `ref<Key, Config, Detector = null_t>` – holds a reference (or a functor that produces a value on demand).

`Config` can be:

- a concrete type (for immediate values),
- a `pack_v<Type>` or `pazz_v<Type>` (indicating that the type is known but the value will be supplied later),
- a `mezz`‑wrapped functor that returns a value (delayed evaluation).

The builder provides `operator[]` to set a parameter, and `forward(key)` to obtain the value (respecting delayed evaluation).

### syntax

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

Helper types:

- `arg<Key, Config, Detector>` – declares a value argument (can be bound to a delayed functor).
- `ref<Key, Config, Detector>` – declares a reference argument (can be bound to a delayed functor).

### example

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

**output:**

```plain
1919, 810, "yaju", 531, "run time"_v
114, 514, "default", 364, "default"_v
```

> **Explanation:** The builder `info` declares five parameters:
>
> - `"i"` – an `arg` with default `114`.
> - `"f"` – a `ref` with default `514.0f` and a detector `convertible_to_d<float>` (so an int like `810` is accepted).
> - `"v"` – a `ref` with default `"default"_v` and detector `view_d` (accepts any `fake::view`).
> - `"m"` – a `ref` with type `double` but no default value; it must be supplied.
> - `"c"` – a `ref` with a functor that returns a `complex` object. The functor is evaluated only when `forward` is called (delayed evaluation).

---

## Dependencies

The examples above require the following headers:

- `"patterns.h"` – always needed.
- `"enum_info.h"` – only for the `fake::enum_info_for_v` in the `intact` example (can be omitted if you replace that line with a manual string).
- `<print>` – C++23 printing (or you can substitute with `<iostream>`).
- `<string>`, `<concepts>`, etc. as needed.

For the singleton/gueston example, Windows headers (`<windows.h>`) are used; adjust for other platforms.
