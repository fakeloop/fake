#ifndef __FAKE_META_DEBUG_ALERT_H__
#define __FAKE_META_DEBUG_ALERT_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include <algorithm>

#include "view.h"

namespace fake::meta
{
	
	template<std::size_t _size>
	struct info final
	{
		consteval info(const char (&_e)[_size]) noexcept{std::ranges::copy_n(_e, _size, data);}
		char data[_size];
	};
	
	template<info _e>
	struct debug final{static_assert(!sizeof(_e) != 0, "[fake::meta::debug]");};
	
	namespace detail::debug
	{
		
		consteval auto match(fake::pack_c auto _pack) noexcept{return fake::type_view(_pack);}
		consteval auto match(fake::view_c auto _pack) noexcept{return _pack;}
		
		template<auto... _args>
		requires ((fake::view_c<decltype(_args)> || fake::pack_c<decltype(_args)>) && ...)
		using cat_t = fake::meta::debug<(fake::view_v<""> + ... + detail::debug::match(_args)).data()>;
		
	}
	
}

namespace fake
{
	
	template<auto... _args>
	inline constexpr fake::meta::detail::debug::cat_t<_args...> alert;
	
}

#endif /*__FAKE_META_DEBUG_ALERT_H__*/ 
