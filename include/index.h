#ifndef __FAKE_INDEX_H__
#define __FAKE_INDEX_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "push.h"

#include "is_valid.h"

namespace fake::meta
{
	
	namespace detail
	{
		
		template<typename _Type, char... _Char>
		consteval auto parse(){
				struct result_t{
				_Type value;
				bool valid;
			};
			
			constexpr auto parse = []<std::size_t _size>(const char (&_string)[_size]) -> result_t {
				constexpr auto is_bin = [](char _char){
					return _char == '0' || _char == '1';
				};
				constexpr auto is_oct = [](char _char){
					return '0' <= _char && _char <= '7';
				};
				constexpr auto is_dec = [](char _char){
					return '0' <= _char && _char <= '9';
				};
				constexpr auto is_hex = [is_dec](char _char){
					return is_dec(_char) || 'A' <= _char && _char <= 'F' || 'a' <= _char && _char <= 'f';
				};
				constexpr auto is_upper = [](char _char){
					return 'A' <= _char && _char <= 'F';
				};
				
				if(_size == 0)
					return {0, false};
				
				if(_size == 1){
					if(is_dec(_string[0]))
						return {_Type(_string[0] - '0'), true};
					else
						return {0, false};
				}
				
				_Type base = 0;
				std::size_t shift = 0;
				
				if(_string[shift] == '0')
					switch(_string[shift + 1]){
						case 'X': base = 16, shift += 2; break;
						case 'x': base = 16, shift += 2; break;
						case 'b': base = 2, shift += 2; break;
						default: base = 8, shift++; break;
					}
				else
					base = 10;
				
				_Type multi = 1;
				_Type value = 0;
				bool valid = true;
				
				for(int i = shift; i < _size && valid; i++){
					const char num = _string[_size + shift - i - 1];
					bool digit = is_dec(num);
					bool upper = is_upper(num);
					value += (digit ? num - '0' : (upper ? num - 'A' : num - 'a') + 10) * multi;
					multi *= base;
					switch(base){
						case 2: valid = is_bin(num); break;
						case 8: valid = is_oct(num); break;
						case 10: valid = is_dec(num); break;
						case 16: valid = is_hex(num); break;
					};
				}
				
				if(valid)
					return {value, true};
				else
					return {0, false};
			};
			
			constexpr char string[] = {_Char...};
			
			return parse(string);
		}
		
	}
	
	namespace literals
	{
		
		template<char... _Char>
		consteval auto operator""_z(){
			using type_t = std::size_t;
			constexpr auto result = fake::meta::detail::parse<type_t, _Char...>();
			
			static_assert(
				result.valid,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::literals::operator\"\"_z>:\n"
				"\tinvalid digit literal.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			
			return std::integral_constant<type_t, result.value>{};
		}
		
		template<char... _Char>
		consteval auto operator""_m(){
			using type_t = std::size_t;
			constexpr auto result = fake::meta::detail::parse<type_t, _Char...>();
			
			static_assert(
				result.valid,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::literals::operator\"\"_m>:\n"
				"\tinvalid digit literal.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			
			return fake::mezz_v<result.value>;
		}
		
		template<char... _Char>
		consteval auto operator""_i(){
			using type_t = int;
			constexpr auto result = fake::meta::detail::parse<type_t, _Char...>();
			
			static_assert(
				result.valid,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::literals::operator\"\"_i>:\n"
				"\tinvalid digit literal.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			
			return std::integral_constant<type_t, result.value>{};
		}
		
		template<char... _Char>
		consteval auto operator""_ll(){
			using type_t = long long;
			constexpr auto result = fake::meta::detail::parse<type_t, _Char...>();
			
			static_assert(
				result.valid,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::literals::operator\"\"_ll>:\n"
				"\tinvalid digit literal.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			
			return std::integral_constant<type_t, result.value>{};
		}
		
	}
	
}

#include "pop.h"

#endif /*__FAKE_INDEX_H__*/ 
