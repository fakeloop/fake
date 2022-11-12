#ifndef __FAKE_TYPE_ARRAY_H__
#define __FAKE_TYPE_ARRAY_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "traits.h"

namespace fake::meta
{
	
	namespace detail::meta::array
	{
		
		template<std::size_t _index, typename _Type>
		struct bind{};
		
		template<typename, typename...>
		struct broker;
		
		template<std::size_t... _index, typename... _Args>
		struct broker<std::index_sequence<_index...>, _Args...> : bind<_index, _Args>...{};
		
	}
	
	template<typename... _Args>
	struct array : fake::meta::detail::meta::array::broker<std::index_sequence_for<_Args...>, _Args...>{
		static constexpr std::size_t size = sizeof... (_Args);
		
		template<std::size_t _index>
		requires (_index < size)
		static consteval auto at() noexcept{
			return get<_index>(array{});
		}
		
	private:
		template<std::size_t _index, typename _Type>
		static consteval auto get(detail::meta::array::bind<_index, _Type>) noexcept{
			return fake::pack_v<_Type>;
		}
	};
	
	template<typename = void> struct is_array{static constexpr bool value = false;};
	template<typename... _Args> struct is_array<array<_Args...>>{static constexpr bool value = true;};
	template<typename _Type> constexpr bool is_array_v = is_array<std::remove_cvref_t<_Type>>::value;
	template<typename _Type> concept array_c = is_array_v<_Type>;
	
	template<std::size_t _Index, typename _Array>
	requires std::convertible_to<_Array, mimic_t<_Array, array>>
	using array_element_t = typename decltype(std::remove_cvref_t<_Array>::template at<_Index>())::type;
	
	template<array_c...>
	struct array_cat final{};
	
	template<>
	struct array_cat<> final{
		using type = array<>;
	};
	
	template<typename... _Types>
	struct array_cat<array<_Types...>> final{
		using type = array<_Types...>;
	};
	
	template<typename... _Lhs, typename... _Rhs>
	struct array_cat<array<_Lhs...>, array<_Rhs...>> final{
		using type = array<_Lhs..., _Rhs...>;
	};
	
	template<typename... _Lhs, typename... _Rhs, array_c... _Tail>
	struct array_cat<array<_Lhs...>, array<_Rhs...>, _Tail...> final{
		using type = typename array_cat<array<_Lhs..., _Rhs...>, _Tail...>::type;
	};
	
	template<array_c... _Segments>
	using array_cat_t = typename array_cat<_Segments...>::type;
	
	template<array_c _Array>
	inline constexpr std::size_t array_size_v = _Array::size;
	
	template<typename = void>
	struct make{};
	
	template<template<typename...> typename _Template, typename... _Args>
	struct make<_Template<_Args...>>{using type = array<_Args...>;};
	
	template<typename _Type>
	using make_t = typename make<std::remove_cvref_t<_Type>>::type;
	
}

#endif /*__FAKE_TYPE_ARRAY_H__*/ 
