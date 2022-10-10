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
	
	template<colors _front, colors _back = colors::black>
	constexpr decltype(auto) color(const fake::view_c auto &_view) noexcept{
		using namespace fake::literals;
		
		constexpr char front = '0' + static_cast<std::underlying_type_t<colors>>(_front);
		constexpr char back = '0' + static_cast<std::underlying_type_t<colors>>(_back);
		
		return ("\e[3"_v + fake::view<front>{} + ";4"_v + fake::view<back>{} + "m"_v + _view + "\e[0m"_v).data();
	}
	
	template<colors _front, colors _back = colors::black>
	std::string color(const std::string &_str){
		using namespace fake::literals;
		
		constexpr char front = '0' + static_cast<std::underlying_type_t<colors>>(_front);
		constexpr char back = '0' + static_cast<std::underlying_type_t<colors>>(_back);
		
		constexpr std::size_t fix = "\e[30;40m\e[0m"_v.length();
		
		std::string result;
		result.reserve(fix + _str.length() + 1);
		
		result += "\e[3";
		result += front;
		result += ";4";
		result += back;
		result += "m";
		result += _str;
		result += "\e[0m";
		
		return result;
	}
	
	template<colors _front, colors _back = colors::black>
	std::string color(const std::string_view &_sv){return fake::color<_front, _back>(std::string{_sv});}
	
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
