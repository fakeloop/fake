# `functor_info.h`

- Provides static reflection for various callable types (functors) in C++.
- Extracts information such as return type, parameter types, function type, number of parameters, and additional qualifiers (for member functions) or placeholder mapping (for `std::bind` expressions).
- Also provides a concept `fake::functor_c` to check whether a type is supported by `fake::functor_info`.

## `fake::functor_c`

### brief

- Concept that checks if a type is recognized as a callable object by `fake::functor_info`.

### syntax

```c++
template<typename _Type>
concept functor_c = is_functor_v<_Type>;
```

### example

```c++
#include "functor_info.h"

int main()
{
    auto lambda = [](int){ return 0; };
    static_assert(fake::functor_c<decltype(lambda)>);
    static_assert(fake::functor_c<void(*)()>);
    static_assert(fake::functor_c<int(&)(double)>);
    struct S { void f(); };
    static_assert(fake::functor_c<decltype(&S::f)>);
}
```

## `fake::functor_info`

### brief

- Primary template for extracting information from a callable type. Specializations exist for lambdas, `std::bind` expressions, member function pointers, free functions, function pointers, and function references.

### common members (available in all specializations)

| Member            | Description                                          |
|-------------------|------------------------------------------------------|
| `name`            | A string literal describing the kind of functor.     |
| `size`            | Number of parameters of the **effective** call.      |
| `func`            | Function type of the **effective** call.             |
| `retn`            | Return type of the **effective** call.               |
| `tuple`           | `std::tuple` of the **effective** parameter types.   |
| `args<Idx>`       | Type of the `Idx`-th **effective** parameter.        |

### members for member function pointers

- `object`          – The class type the member function belongs to.
- `constant`        – `true` if the member function is `const`-qualified.
- `volatility`      – `true` if the member function is `volatile`-qualified.
- `exception`       – `true` if the member function is **not** `noexcept`.
- `lvalue_reference`– `true` if the member function has `&` ref‑qualifier.
- `rvalue_reference`– `true` if the member function has `&&` ref‑qualifier.

### members for `std::bind` expressions

- `rec_*` (e.g. `rec_size`, `rec_func`, `rec_tuple`, `rec_args<Idx>`) – Information about the **original** callable being bound.
- `bind_*` (e.g. `bind_size`, `bind_func`, `bind_tuple`, `bind_args<Idx>`) – Information about the **binding arguments** (including placeholders).
- The **effective** call (`size`, `func`, `tuple`, `args<Idx>`) is the callable type obtained after placeholder substitution (i.e. what you actually invoke when you call the bind object).

## Example: Lambda object (functor_object)

**brief**: Demonstrates extracting information from a lambda.

**example**:

```c++
#include <string_view>
#include <tuple>
#include "functor_info.h"

int main()
{
    auto lambda = [](double&, char, float) { return 114514; };
    
    static_assert(fake::functor_c<decltype(lambda)>);
    
    using info = fake::functor_info<decltype(lambda)>;
    
    static_assert(std::string_view{info::name} == "functor_object");
    static_assert(info::size == 3);
    static_assert(std::same_as<info::func, int(double&, char, float)>);
    static_assert(std::same_as<info::retn, int>);
    static_assert(std::same_as<info::tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<info::args<1>, char>);
}
```

## Example: `std::bind` expression

**brief**: Shows how `functor_info` extracts information from a `std::bind` object, including the original functor, the bound arguments, and the effective call signature after placeholder mapping.

**example**:

```c++
#include <string_view>
#include <tuple>
#include <functional>
#include "functor_info.h"

int main()
{
    auto lambda = [](double&, char, float) { return 114514; };
    auto bind = std::bind(lambda, std::placeholders::_2, std::placeholders::_1, 114514.0f);
    
    static_assert(fake::functor_c<decltype(bind)>);
    using info = fake::functor_info<decltype(bind)>;
    
    static_assert(std::string_view{info::name} == "bind_expression");
    
    // Original functor info (the lambda)
    static_assert(info::rec_size == 3);
    static_assert(std::same_as<info::rec_func, int(double&, char, float)>);
    static_assert(std::same_as<info::rec_retn, int>);
    static_assert(std::same_as<info::rec_tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<info::rec_args<1>, char>);
    
    using placeholders_1 = std::remove_cvref_t<decltype(std::placeholders::_1)>;
    using placeholders_2 = std::remove_cvref_t<decltype(std::placeholders::_2)>;
    
    // Binding arguments info (as passed to std::bind)
    static_assert(info::bind_size == 3);
    static_assert(std::same_as<info::bind_func, int(placeholders_2, placeholders_1, float)>);
    static_assert(std::same_as<info::bind_retn, int>);
    static_assert(std::same_as<info::bind_tuple, std::tuple<placeholders_2, placeholders_1, float>>);
    static_assert(std::same_as<info::bind_args<2>, float>);
    
    double d = 1919.0;
    bind('@', d);   // after placeholder substitution, effective call takes (char, double&)
    
    // Effective call info (after placeholder mapping)
    static_assert(info::size == 2);
    static_assert(std::same_as<info::func, int(char, double&)>);
    static_assert(std::same_as<info::retn, int>);
    static_assert(std::same_as<info::tuple, std::tuple<char, double&>>);
    static_assert(std::same_as<info::args<1>, double&>);
}
```

## Example: Member function pointers (various qualifiers)

**brief**: Extracts information from pointers to member functions, including const, volatile, ref-qualifiers and noexcept.

**example**:

```c++
#include <string_view>
#include <tuple>
#include "functor_info.h"

int main()
{
    struct object {
        int foo(double&, char, float) noexcept { return 114514; }
        int bar(double&, char, float) const volatile && { return 114514; }
    };
    
    static_assert(fake::functor_c<decltype(&object::foo)>);
    static_assert(fake::functor_c<decltype(&object::bar)>);
    
    using foo = fake::functor_info<decltype(&object::foo)>;
    using bar = fake::functor_info<decltype(&object::bar)>;
    
    // foo: noexcept member function
    static_assert(foo::size == 3);
    static_assert(std::same_as<foo::object, object>);
    static_assert(std::same_as<foo::func, int(double&, char, float)>);
    static_assert(std::same_as<foo::retn, int>);
    static_assert(std::same_as<foo::tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<foo::args<1>, char>);
    
    static_assert(std::string_view{foo::name} == "member_function_pointer_noexcept");
    static_assert(foo::constant == false);
    static_assert(foo::volatility == false);
    static_assert(foo::exception == false);          // noexcept => exception == false
    static_assert(foo::lvalue_reference == false);
    static_assert(foo::rvalue_reference == false);
    
    // bar: const volatile &&, noexcept not specified (so exception == true)
    static_assert(std::string_view{bar::name} == "member_function_pointer_const_volatile_rvalue");
    static_assert(bar::constant == true);
    static_assert(bar::volatility == true);
    static_assert(bar::exception == true);           // not noexcept
    static_assert(bar::lvalue_reference == false);
    static_assert(bar::rvalue_reference == true);
}
```

## Example: Free function type

**brief**: Information about a plain function type (not a pointer or reference). Here we obtain a function type by converting a lambda to a function pointer and then removing the pointer.

**example**:

```c++
#include <string_view>
#include <tuple>
#include "functor_info.h"

int main()
{
    auto lambda = [](double&, char, float) { return 114514; };
    
    using func_t = std::remove_pointer_t<decltype(+lambda)>; // int(double&, char, float)
    static_assert(fake::functor_c<func_t>);
    
    using info = fake::functor_info<func_t>;
    
    static_assert(std::string_view{info::name} == "global_function");
    static_assert(info::size == 3);
    static_assert(std::same_as<info::func, int(double&, char, float)>);
    static_assert(std::same_as<info::retn, int>);
    static_assert(std::same_as<info::tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<info::args<1>, char>);
}
```

## Example: Function pointer

**brief**: Information about a function pointer.

**example**:

```c++
#include <string_view>
#include <tuple>
#include "functor_info.h"

int main()
{
    auto lambda = [](double&, char, float) { return 114514; };
    auto fptr = +lambda;   // int(*)(double&, char, float)
    
    static_assert(fake::functor_c<decltype(fptr)>);
    
    using info = fake::functor_info<decltype(fptr)>;
    
    static_assert(std::string_view{info::name} == "global_function_pointer");
    static_assert(info::size == 3);
    static_assert(std::same_as<info::func, int(double&, char, float)>);
    static_assert(std::same_as<info::retn, int>);
    static_assert(std::same_as<info::tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<info::args<1>, char>);
}
```

## Example: Function reference

**brief**: Information about a function reference (obtained by dereferencing a function pointer).

**example**:

```c++
#include <string_view>
#include <tuple>
#include "functor_info.h"

int main()
{
    auto lambda = [](double&, char, float) { return 114514; };
    auto& fref = *+lambda;   // int(&)(double&, char, float)
    
    static_assert(fake::functor_c<decltype(fref)>);
    
    using info = fake::functor_info<decltype(fref)>;
    
    static_assert(std::string_view{info::name} == "global_function_reference");
    static_assert(info::size == 3);
    static_assert(std::same_as<info::func, int(double&, char, float)>);
    static_assert(std::same_as<info::retn, int>);
    static_assert(std::same_as<info::tuple, std::tuple<double&, char, float>>);
    static_assert(std::same_as<info::args<1>, char>);
}
```
