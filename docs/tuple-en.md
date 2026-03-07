# `tuple.h`

- provides metaprogramming utilities for `std::tuple`

---

## `fake::tuple::make_t`

### brief

- Converts a template instantiation (e.g., `std::variant`) into a `std::tuple` with the same template arguments.

### syntax

```c++
template<typename _Type>
using make_t = typename make<std::remove_cvref_t<_Type>>::type;
```

### example

```c++
#include <variant>
#include "tuple.h"

int main()
{
    using origin = std::variant<int, char, const float&, const int*&, double, const int*, char*>;
    using mimic = fake::mimic_t<origin, std::tuple>; // defined in <fake/traits.h>
    using tuple = fake::tuple::make_t<origin>;
    
    static_assert(std::same_as<mimic, std::tuple<int, char, const float&, const int*&, double, const int*, char*>>);
    static_assert(std::same_as<mimic, tuple>);
}
```

---

## `fake::tuple::match_index_v`

### brief

- Finds the first index of a tuple element that satisfies a given detector (concept). Returns an `std::integral_constant<std::size_t, index>` if found, otherwise `fake::null_t`.

### syntax

```c++
template<fake::tuple_c _Tuple, auto _functor>
constexpr auto match_index_v = typename match_index<_Tuple, _functor>::type{};
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    static constexpr fake::creed_c auto ptref_d = (fake::remove_reference_p <<= fake::pointer_d) && fake::reference_d;
    static constexpr fake::creed_c auto ctref_d = fake::convertible_to_d<const int*>;
    static constexpr fake::creed_c auto rvref_d = fake::rvalue_reference_d;
    
    static_assert(fake::tuple::match_index_v<type, ptref_d> == 3);
    static_assert(fake::tuple::match_index_v<type, ctref_d> == 3);
    static_assert(fake::null_c<fake::tuple::match_index_v<type, rvref_d>>);
}
```

---

## `fake::tuple::first_index_v`

### brief

- Alias for `match_index_v` (finds the first index).

### syntax

```c++
template<fake::tuple_c _Tuple, auto _functor>
constexpr auto first_index_v = typename match_index<_Tuple, _functor, true>::type{};
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    static constexpr fake::creed_c auto ptref_d = (fake::remove_reference_p <<= fake::pointer_d) && fake::reference_d;
    static constexpr fake::creed_c auto ctref_d = fake::convertible_to_d<const int*>;
    static constexpr fake::creed_c auto rvref_d = fake::rvalue_reference_d;
    
    static_assert(fake::tuple::first_index_v<type, ptref_d> == 3);
    static_assert(fake::tuple::first_index_v<type, ctref_d> == 3);
    static_assert(fake::null_c<fake::tuple::first_index_v<type, rvref_d>>);
}
```

---

## `fake::tuple::last_index_v`

### brief

- Finds the last index of a tuple element that satisfies a given detector.

### syntax

```c++
template<fake::tuple_c _Tuple, auto _functor>
constexpr auto last_index_v = typename match_index<_Tuple, _functor, false>::type{};
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    static constexpr fake::creed_c auto ptref_d = (fake::remove_reference_p <<= fake::pointer_d) && fake::reference_d;
    static constexpr fake::creed_c auto ctref_d = fake::convertible_to_d<const int*>;
    static constexpr fake::creed_c auto rvref_d = fake::rvalue_reference_d;
    
    static_assert(fake::tuple::last_index_v<type, ptref_d> == 3);
    static_assert(fake::tuple::last_index_v<type, ctref_d> == 5);
    static_assert(fake::null_c<fake::tuple::last_index_v<type, rvref_d>>);
}
```

---

## `fake::tuple::indices_v`

### brief

- Returns a `std::array<std::size_t, N>` containing all indices that satisfy a given detector.

### syntax

```c++
template<fake::tuple_c _Tuple, auto _functor>
constexpr auto indices_v = indices<_Tuple, _functor>::value;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    static constexpr fake::creed_c auto ctref_d = fake::convertible_to_d<const int*>;
    
    static_assert(fake::tuple::indices_v<type, ctref_d> == std::array{3uz, 5uz});
}
```

---

## `fake::tuple::find_if_t`

### brief

- Finds the first type in the tuple that satisfies a given detector. Returns the type if found, otherwise `fake::null_t`.

### syntax

```c++
template<fake::tuple_c _Tuple, auto _functor>
using find_if_t = typename find_if<_Tuple, _functor>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    static constexpr fake::creed_c auto ctref_d = fake::convertible_to_d<const int*>;
    static constexpr fake::creed_c auto rvref_d = fake::rvalue_reference_d;
    
    static_assert(std::same_as<fake::tuple::find_if_t<type, ctref_d>, const int*&>);
    static_assert(std::same_as<fake::tuple::find_if_t<type, rvref_d>, fake::null_t>);
}
```

---

## `fake::tuple::contains_v`

### brief

- Checks whether any element of the tuple satisfies a given detector.

### syntax

```c++
template<fake::tuple_c _Tuple, auto _functor>
constexpr bool contains_v = contains<_Tuple, _functor>::value;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    static constexpr fake::creed_c auto ctref_d = fake::convertible_to_d<const int*>;
    static constexpr fake::creed_c auto rvref_d = fake::rvalue_reference_d;
    
    static_assert(fake::tuple::contains_v<type, ctref_d> == true);
    static_assert(fake::tuple::contains_v<type, rvref_d> == false);
}
```

---

## `fake::tuple::contains_type_v`

### brief

- Checks whether a specific type exists in the tuple.

### syntax

```c++
template<fake::tuple_c _Tuple, typename _Type>
constexpr bool contains_type_v = contains_type<_Tuple, _Type>::value;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    static_assert(fake::tuple::contains_type_v<type, const int*&> == true);
    static_assert(fake::tuple::contains_type_v<type, float*> == false);
}
```

---

## `fake::tuple::scale_t`

### brief

- Applies a transformation that can replace each element with a pack of types (e.g., expand or erase), then concatenates the results into a new tuple.

### syntax

```c++
template<fake::tuple_c _Tuple, auto _functor>
using scale_t = typename scale<_Tuple, _functor>::type;
```

### note

- `_functor` should return a `fake::pack_c` wrapper of a tuple (to transform one type or insert multiple types), or `fake::pack_v<std::tuple<>>` to erase.

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    static constexpr fake::shift_c auto transform = fake::conditional_p<
        fake::convertible_to_d<const int*>,
        fake::replace_p<std::tuple<>>, // erase type
        fake::add_const_p <<= fake::apply_p<std::tuple> // add const
    >;
    
    using expect = std::tuple<const int, const char, const float&, const double, char* const>;
    
    static_assert(std::same_as<fake::tuple::scale_t<type, transform>, expect>);
}
```

---

## `fake::tuple::transform_t`

### brief

- Applies a unary type transformation to each element of the tuple, producing a new tuple of the same length.

### syntax

```c++
template<fake::tuple_c _Tuple, auto _functor>
using transform_t = typename transform<_Tuple, _functor>::type;
```

### note

- `_functor` should accept a `fake::pack_c<element>` and return a `fake::pack_c<new_type>`.

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    using expect = std::tuple<int, char, float, int, double, int, char>;
    
    static_assert(std::same_as<fake::tuple::transform_t<type, fake::bare_p>, expect>);
}
```

---

## `fake::tuple::select_if_t`

### brief

- Selects elements that satisfy a given detector, producing a new tuple containing only those elements.

### syntax

```c++
template<fake::tuple_c _Tuple, auto _functor>
using select_if_t = typename select_if<_Tuple, _functor>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    using expect = std::tuple<int, char, double, const int*, char*>;
    
    static_assert(std::same_as<fake::tuple::select_if_t<type, !fake::reference_d>, expect>);
}
```

---

## `fake::tuple::erase_if_t`

### brief

- Erases elements that satisfy a given detector, producing a new tuple with the remaining elements.

### syntax

```c++
template<fake::tuple_c _Tuple, auto _functor>
using erase_if_t = typename erase_if<_Tuple, _functor>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    using expect = std::tuple<const float&, const int*&>;
    
    static_assert(std::same_as<fake::tuple::erase_if_t<type, !fake::reference_d>, expect>);
}
```

---

## `fake::tuple::concat_t`

### brief

- Concatenates multiple tuples into one.

### syntax

```c++
template<fake::tuple_c... _Tuples>
using concat_t = typename concat<_Tuples...>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using prefix = std::tuple<int, char, const float&>;
    using infix = std::tuple<const int*&, double>;
    using suffix = std::tuple<const int*, char*>;
    
    using expect = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    static_assert(std::same_as<fake::tuple::concat_t<prefix, infix, suffix>, expect>);
}
```

---

## `fake::tuple::emplace_t`

### brief

- Inserts a type at a specified index into a tuple.

### syntax

```c++
template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
using emplace_t = typename emplace<_Tuple, _Index, _Element>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, double>;
    
    static_assert(std::same_as<fake::tuple::emplace_t<type, 1, void>, std::tuple<int, void, char, double>>);
}
```

---

## `fake::tuple::emplace_front_t`

### brief

- Inserts a type at the front of a tuple.

### syntax

```c++
template<fake::tuple_c _Tuple, typename _Element>
using emplace_front_t = emplace_t<_Tuple, 0, _Element>;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, double>;
    
    static_assert(std::same_as<fake::tuple::emplace_front_t<type, void>, std::tuple<void, int, char, double>>);
}
```

---

## `fake::tuple::emplace_back_t`

### brief

- Inserts a type at the back of a tuple.

### syntax

```c++
template<fake::tuple_c _Tuple, typename _Element>
using emplace_back_t = emplace_t<_Tuple, std::tuple_size_v<_Tuple>, _Element>;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, double>;
    
    static_assert(std::same_as<fake::tuple::emplace_back_t<type, void>, std::tuple<int, char, double, void>>);
}
```

---

## `fake::tuple::erase_t`

### brief

- Erases the element at a specified index.

### syntax

```c++
template<fake::tuple_c _Tuple, std::size_t _Index>
using erase_t = typename erase<_Tuple, _Index>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, double>;
    
    static_assert(std::same_as<fake::tuple::erase_t<type, 1>, std::tuple<int, double>>);
}
```

---

## `fake::tuple::erase_front_t`

### brief

- Erases the first element.

### syntax

```c++
template<fake::tuple_c _Tuple>
using erase_front_t = erase_t<_Tuple, 0>;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, double>;
    
    static_assert(std::same_as<fake::tuple::erase_front_t<type>, std::tuple<char, double>>);
}
```

---

## `fake::tuple::erase_back_t`

### brief

- Erases the last element.

### syntax

```c++
template<fake::tuple_c _Tuple>
using erase_back_t = erase_t<_Tuple, std::tuple_size_v<_Tuple> - 1>;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, double>;
    
    static_assert(std::same_as<fake::tuple::erase_back_t<type>, std::tuple<int, char>>);
}
```

---

## `fake::tuple::subtuple_t`

### brief

- Extracts a contiguous sub-tuple starting at `_Index` of length `_Length`.

### syntax

```c++
template<fake::tuple_c _Tuple, std::size_t _Index, std::size_t _Length>
using subtuple_t = typename subtuple<_Tuple, _Index, _Length>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, const float&, const int*&, double, const int*, char*>;
    
    static_assert(std::same_as<fake::tuple::subtuple_t<type, 2, 3>, std::tuple<const float&, const int*&, double>>);
}
```

---

## `fake::tuple::swap_t`

### brief

- Swaps two elements at given indices.

### syntax

```c++
template<fake::tuple_c _Tuple, std::size_t _Lhs, std::size_t _Rhs>
using swap_t = typename swap<_Tuple, _Lhs, _Rhs>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, double, const int*, char*>;
    using expect = std::tuple<int, char*, double, const int*, char>;
    
    static_assert(std::same_as<fake::tuple::swap_t<type, 1, 4>, expect>);
}
```

---

## `fake::tuple::rebind_t`

### brief

- Replaces the type at a given index with another type.

### syntax

```c++
template<fake::tuple_c _Tuple, std::size_t _index, typename _Type>
using rebind_t = typename rebind<_Tuple, _index, _Type>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, double, const int*, char*>;
    using expect = std::tuple<int, char, void, const int*, char*>;
    
    static_assert(std::same_as<fake::tuple::rebind_t<type, 2, void>, expect>);
}
```

---

## `fake::tuple::shrink_t`

### brief

- Removes duplicate types from a tuple, preserving the order of first occurrence.

### syntax

```c++
template<fake::tuple_c _Tuple>
using shrink_t = typename shrink<_Tuple>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, int, int, char, int, short, double, int, float, double>;
    using expect = std::tuple<int, char, short, double, float>;
    
    static_assert(std::same_as<fake::tuple::shrink_t<type>, expect>);
}
```

---

## `fake::tuple::unique_t`

### brief

- Removes duplicate types from a tuple (alternative implementation, same result as `shrink_t`).

### syntax

```c++
template<fake::tuple_c _Tuple>
using unique_t = typename unique<_Tuple>::type;
```

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<int, char, int, int, char, int, short, double, int, float, double>;
    using expect = std::tuple<int, char, short, double, float>;
    
    static_assert(std::same_as<fake::tuple::unique_t<type>, expect>);
}
```

---

## `fake::tuple::for_each_t`

### brief

- Folds over the tuple elements with a state, allowing early exit.

### syntax

```c++
template<fake::tuple_c _Tuple, typename _Init, auto _functor>
using for_each_t = typename for_each<_Tuple, _Init, _functor>::type;
```

### note

- `_functor` should accept `(state, index, element)` and return a new state or `fake::null_t` to stop.

### example

```c++
#include "tuple.h"

int main()
{
    using type = std::tuple<fake::mezz_t<114>, fake::mezz_t<514>, fake::mezz_t<1919>, fake::mezz_t<810>>;
    
    static constexpr auto lambda = [](fake::pack_c auto _state, fake::mezz_c auto _index, fake::pack_c auto _each){
        using state = fake::take_t<decltype(_state){}>;
        using each = fake::take_t<decltype(_each){}>;
        
        if constexpr(each::value > 1000)
            return fake::null_t{}; // exit the loop early
        else
            return fake::pack_v<fake::mezz_t<state::value * 1000 + each::value>>;
    };
    
    static_assert(fake::tuple::for_each_t<type, fake::mezz_t<0>, lambda>::value == 114514);
}
```

---

## `fake::tuple::zip_t`

### brief

- Zips multiple tuples together, producing a tuple of tuples where each inner tuple contains the corresponding elements from each input tuple.

### syntax

```c++
template<fake::tuple_c... _Tuples>
using zip_t = typename zip<_Tuples...>::type;
```

### note

- The length of the result is the minimum length of the input tuples.

### example

```c++
#include "tuple.h"

int main()
{
    using first = std::tuple<int, char, float, double, short>;
    using second = std::tuple<int*, char*, float*, double*>;
    using third = std::tuple<int&, char&, float&, double&>;
    
    using expect = std::tuple<
        std::tuple<int, int*, int&>,
        std::tuple<char, char*, char&>,
        std::tuple<float, float*, float&>,
        std::tuple<double, double*, double&>
    >;
    
    static_assert(std::same_as<fake::tuple::zip_t<first, second, third>, expect>);
}
```
