#ifndef __FAKE_UTILITY_H__
#define __FAKE_UTILITY_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include <utility>

#include "view.h"

namespace fake
{
	
	enum struct colors : std::size_t{black, red, green, yellow, blue, magenta, cyan, white};
	
	namespace detail::utility
	{
		
		template<colors _front, colors _back = colors::black>
		consteval auto color() noexcept{
			using namespace fake::literals;
			constexpr char front = '0' + static_cast<std::underlying_type_t<colors>>(_front);
			constexpr char back = '0' + static_cast<std::underlying_type_t<colors>>(_back);
			
			return "\e[3"_v + fake::view<front>{} + ";4"_v + fake::view<back>{} + "m"_v;
		}
		
		consteval auto clear() noexcept{return fake::view_v<"\e[0m">;}
		
		template<typename _Type, colors _front, colors _back>
		struct stream final
		{
			using type = std::remove_cvref_t<_Type>;
			
			constexpr stream(const type &_data): data(_data){}
			stream(const stream&) = delete;
			stream& operator=(const stream&) = delete;
			
			friend std::basic_ostream<char>& operator<<(std::basic_ostream<char> &_os, stream _view){
				return _os << color<_front, _back>() << _view.data << clear();
			}
			
		private:
			const type &data;
		};
		
	}
	
	template<colors _front, colors _back = colors::black>
	constexpr auto color(const fake::view_c auto &_view) noexcept{
		return detail::utility::color<_front, _back>() + _view + detail::utility::clear();
	}
	
	template<colors _front, colors _back = colors::black>
	constexpr auto color(std::string_view _sv){return detail::utility::stream<std::string_view, _front, _back>{_sv};}
	
	template<colors _front, colors _back = colors::black>
	constexpr auto color(const auto &_data){return detail::utility::stream<decltype(_data), _front, _back>{_data};}
	
	template<typename _Number>
	requires std::is_integral_v<_Number>
	constexpr auto to_hex(_Number _number){
		return [number = _number]<std::size_t... _Index>(std::index_sequence<_Index...>){
			return std::array{
				[number]{
					constexpr std::size_t index = sizeof...(_Index) - _Index - 1;
					static_assert(sizeof...(_Index) == 8);
					static_assert(index < 8);
					const char bits = (number >> (index << 2)) & 0xF;
					const char mask = (-!(bits & 8)) | (-((bits & 8) && !(bits & 6)));
					
					return char((mask & (bits | '0')) | (~mask & ((bits - 9) | '@'))); //'@''A''B''C'... 
				}()...
			};
		}(std::make_index_sequence<(sizeof(_Number) << 1)>());
	}
	
}

#endif /*__FAKE_UTILITY_H__*/ 
