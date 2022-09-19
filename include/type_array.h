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

#include "is_valid.h"

namespace fake::meta
{
	
	template<typename... _Args>
	struct array{
		static constexpr std::size_t size = sizeof... (_Args);
		
		template<std::size_t _Index>
		static constexpr auto at() noexcept{
			constexpr bool valid = _Index < size;
			static_assert(
				valid,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::array::at()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			if constexpr(valid)
				return get<0, _Index, args>();
		}
		
	private:
		template<std::size_t _Current, std::size_t _Target, typename _Tail>
		static constexpr auto get() noexcept{
			if constexpr(_Current < _Target)
				return get<_Current + 1, _Target, typename _Tail::tail>();
			else
				return fake::pack_v<typename _Tail::type>;
		}
		
		template<std::size_t, typename...>
		struct impl;
		
		template<std::size_t _Index, typename _Type, typename... _Tail>
		struct impl<_Index, _Type, _Tail...>{
			static constexpr std::size_t index = _Index;
			using type = _Type;
			using tail = impl<_Index + 1, _Tail...>;
		};
		
		template<std::size_t _Index>
		struct impl<_Index>{};
		
		using args = impl<0, _Args...>;
	};
	
	template<typename = void> struct is_array{static constexpr bool value = false;};
	template<typename... _Args> struct is_array<array<_Args...>>{static constexpr bool value = true;};
	template<typename _Type> constexpr bool is_array_v = is_array<std::remove_cvref_t<_Type>>::value;
	template<typename _Type> concept array_c = is_array_v<_Type>;
	
	template<std::size_t _Index, typename _Array>
	using array_element = typename decltype(std::remove_cvref_t<_Array>::template at<_Index>())::type;
	
	template<array_c, array_c>
	struct array_cat final{};
	
	template<typename... _Lhs, typename... _Rhs>
	struct array_cat<array<_Lhs...>, array<_Rhs...>> final{
		using type = array<_Lhs..., _Rhs...>;
	};
	
	template<array_c _Lhs, array_c _Rhs>
	using array_cat_t = typename array_cat<_Lhs, _Rhs>::type;
	
}

#endif /*__FAKE_TYPE_ARRAY_H__*/ 
