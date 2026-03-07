# `tuple.h`

- 提供用于 `std::tuple` 的元编程工具

---

## `fake::tuple::make_t`

### 简介

- 将一个模板实例（例如 `std::variant`）转换为具有相同模板参数的 `std::tuple`。

### 语法

```c++
template<typename _Type>
using make_t = typename make<std::remove_cvref_t<_Type>>::type;
```

### 示例

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

### 简介

- 查找元组中第一个满足给定探测器（概念）的元素的索引。如果找到，返回一个 `std::integral_constant<std::size_t, index>`，否则返回 `fake::null_t`。

### 语法

```c++
template<fake::tuple_c _Tuple, auto _functor>
constexpr auto match_index_v = typename match_index<_Tuple, _functor>::type{};
```

### 示例

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

### 简介

- `match_index_v` 的别名（查找第一个索引）。

### 语法

```c++
template<fake::tuple_c _Tuple, auto _functor>
constexpr auto first_index_v = typename match_index<_Tuple, _functor, true>::type{};
```

### 示例

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

### 简介

- 查找元组中最后一个满足给定探测器（概念）的元素的索引。

### 语法

```c++
template<fake::tuple_c _Tuple, auto _functor>
constexpr auto last_index_v = typename match_index<_Tuple, _functor, false>::type{};
```

### 示例

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

### 简介

- 返回一个 `std::array<std::size_t, N>`，包含所有满足给定探测器（概念）的索引。

### 语法

```c++
template<fake::tuple_c _Tuple, auto _functor>
constexpr auto indices_v = indices<_Tuple, _functor>::value;
```

### 示例

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

### 简介

- 查找元组中第一个满足给定探测器（概念）的类型。如果找到，返回该类型，否则返回 `fake::null_t`。

### 语法

```c++
template<fake::tuple_c _Tuple, auto _functor>
using find_if_t = typename find_if<_Tuple, _functor>::type;
```

### 示例

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

### 简介

- 检查元组中是否有任何元素满足给定探测器（概念）。

### 语法

```c++
template<fake::tuple_c _Tuple, auto _functor>
constexpr bool contains_v = contains<_Tuple, _functor>::value;
```

### 示例

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

### 简介

- 检查元组中是否存在特定类型。

### 语法

```c++
template<fake::tuple_c _Tuple, typename _Type>
constexpr bool contains_type_v = contains_type<_Tuple, _Type>::value;
```

### 示例

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

### 简介

- 应用一个转换，可以将每个元素替换为一组类型（例如展开或擦除），然后将结果连接成一个新的元组。

### 语法

```c++
template<fake::tuple_c _Tuple, auto _functor>
using scale_t = typename scale<_Tuple, _functor>::type;
```

### 注意

- `_functor` 应该返回一个元组的 `fake::pack_c` 包装（用于将一个类型转换为另一个或多个类型），或者返回 `fake::pack_v<std::tuple<>>` 以擦除该元素。

### 示例

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

### 简介

- 对元组的每个元素应用一元类型转换，生成一个长度相同的新元组。

### 语法

```c++
template<fake::tuple_c _Tuple, auto _functor>
using transform_t = typename transform<_Tuple, _functor>::type;
```

### 注意

- `_functor` 应该接受一个 `fake::pack_c<element>` 并返回一个 `fake::pack_c<new_type>`。

### 示例

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

### 简介

- 选择满足给定探测器（概念）的元素，生成一个仅包含这些元素的新元组。

### 语法

```c++
template<fake::tuple_c _Tuple, auto _functor>
using select_if_t = typename select_if<_Tuple, _functor>::type;
```

### 示例

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

### 简介

- 擦除满足给定探测器（概念）的元素，生成一个包含剩余元素的新元组。

### 语法

```c++
template<fake::tuple_c _Tuple, auto _functor>
using erase_if_t = typename erase_if<_Tuple, _functor>::type;
```

### 示例

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

### 简介

- 将多个元组连接成一个元组。

### 语法

```c++
template<fake::tuple_c... _Tuples>
using concat_t = typename concat<_Tuples...>::type;
```

### 示例

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

### 简介

- 在元组的指定索引处插入一个类型。

### 语法

```c++
template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
using emplace_t = typename emplace<_Tuple, _Index, _Element>::type;
```

### 示例

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

### 简介

- 在元组的前面插入一个类型。

### 语法

```c++
template<fake::tuple_c _Tuple, typename _Element>
using emplace_front_t = emplace_t<_Tuple, 0, _Element>;
```

### 示例

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

### 简介

- 在元组的末尾插入一个类型。

### 语法

```c++
template<fake::tuple_c _Tuple, typename _Element>
using emplace_back_t = emplace_t<_Tuple, std::tuple_size_v<_Tuple>, _Element>;
```

### 示例

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

### 简介

- 擦除指定索引处的元素。

### 语法

```c++
template<fake::tuple_c _Tuple, std::size_t _Index>
using erase_t = typename erase<_Tuple, _Index>::type;
```

### 示例

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

### 简介

- 擦除第一个元素。

### 语法

```c++
template<fake::tuple_c _Tuple>
using erase_front_t = erase_t<_Tuple, 0>;
```

### 示例

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

### 简介

- 擦除最后一个元素。

### 语法

```c++
template<fake::tuple_c _Tuple>
using erase_back_t = erase_t<_Tuple, std::tuple_size_v<_Tuple> - 1>;
```

### 示例

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

### 简介

- 提取从 `_Index` 开始，长度为 `_Length` 的连续子元组。

### 语法

```c++
template<fake::tuple_c _Tuple, std::size_t _Index, std::size_t _Length>
using subtuple_t = typename subtuple<_Tuple, _Index, _Length>::type;
```

### 示例

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

### 简介

- 交换给定索引处的两个元素。

### 语法

```c++
template<fake::tuple_c _Tuple, std::size_t _Lhs, std::size_t _Rhs>
using swap_t = typename swap<_Tuple, _Lhs, _Rhs>::type;
```

### 示例

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

### 简介

- 将给定索引处的类型替换为另一个类型。

### 语法

```c++
template<fake::tuple_c _Tuple, std::size_t _index, typename _Type>
using rebind_t = typename rebind<_Tuple, _index, _Type>::type;
```

### 示例

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

### 简介

- 从元组中移除重复的类型，保留首次出现的顺序。

### 语法

```c++
template<fake::tuple_c _Tuple>
using shrink_t = typename shrink<_Tuple>::type;
```

### 示例

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

### 简介

- 从元组中移除重复的类型（另一种实现，结果与 `shrink_t` 相同）。

### 语法

```c++
template<fake::tuple_c _Tuple>
using unique_t = typename unique<_Tuple>::type;
```

### 示例

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

### 简介

- 使用一个状态对元组元素进行折叠操作，允许提前退出。

### 语法

```c++
template<fake::tuple_c _Tuple, typename _Init, auto _functor>
using for_each_t = typename for_each<_Tuple, _Init, _functor>::type;
```

### 注意

- `_functor` 应该接受 `(state, index, element)` 并返回一个新的状态，或者返回 `fake::null_t` 来停止。

### 示例

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

### 简介

- 将多个元组压缩在一起，生成一个元组的元组，其中每个内部元组包含来自每个输入元组的对应元素。

### 语法

```c++
template<fake::tuple_c... _Tuples>
using zip_t = typename zip<_Tuples...>::type;
```

### 注意

- 结果的长度是输入元组的最小长度。

### 示例

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
