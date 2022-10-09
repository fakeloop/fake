#ifndef __FAKE_META_H__
#define __FAKE_META_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "type_array.h"
#include "type_map.h"
#include "traits.h"
#include "symbol.h"

namespace fake::meta
{
	
	namespace detail
	{
		
		template<typename _Type>
		struct tag{using type = _Type;};
		
	}
	
	namespace detail
	{
		
		template<typename _Type, char... _Char>
		consteval auto parse(){
				struct result_t{
				_Type value;
				bool valid;
			};
			
			constexpr auto parse = []<std::size_t _size>(const char (&_string)[_size]) -> result_t{
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
	
	namespace detail
	{
		
		template<typename _Array, typename _Func, std::size_t... _Index>
		constexpr auto for_each_impl(_Array &&_array, _Func &&_func, std::index_sequence<_Index...>)
		{
			(_func(fake::pack_v<array_element<_Index, _Array>>), ...);
		}
		
		template<typename _Array, typename _Func>
		requires fake::meta::array_c<_Array>
		constexpr auto for_each(_Array &&_array, _Func &&_func)
		{
			for_each_impl(
				std::forward<_Array>(_array),
				std::forward<_Func>(_func),
				std::make_index_sequence<std::remove_cvref_t<_Array>::size>{}
			);
		}
		
	}
	
	template<typename _Array, typename _Func>
	constexpr auto for_each(_Array &&_array, _Func &&_func)
	{
		detail::for_each(std::forward<_Array>(_array), std::forward<_Func>(_func));
	}
	
	template<typename _Type, std::size_t _Size>
	constexpr auto range()
	{
		constexpr auto impl = []<std::size_t ..._Index>(std::index_sequence<_Index...>){
			return array<std::integral_constant<_Type, _Index>...>{};
		};
		
		return impl(std::make_index_sequence<_Size>{});
	}
	
	template<typename _Type, std::size_t _Size>
	constexpr auto range_v = range<_Type, _Size>();
	
	template<typename _Type, typename _Tag>
	struct number
	{
		// constructor. 
		constexpr number() noexcept{}
		template<typename _Constant>
		requires fake::constant_c<_Constant>
		constexpr number(_Constant) noexcept{update<_Constant>([]{});}
		
		// destructor. 
		constexpr number(const number&) = default;
		
	private:
		struct key{};
		
	public:
		// access. 
		template<typename _Lambda>
		constexpr auto operator()(_Lambda) const noexcept{
			return get([]{});
		}
		
		// type conversion. 
		template<auto = refresh(tool::token{}, key{})>
		constexpr operator _Type() const noexcept{
			return decltype(get([]{}))::value;
		}
		
		// assignment. 
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, _value>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, _e([]{})>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator+=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value + _value>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator+=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value + _e([]{})>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator-=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value - _value>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator-=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value - _e([]{})>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator*=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value * _value>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator*=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value * _e([]{})>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator/=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value / _value>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator/=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value / _e([]{})>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator%=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value % _value>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator%=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value % _e([]{})>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator&=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value & _value>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator&=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value & _e([]{})>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator|=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value | _value>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator|=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value | _e([]{})>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator^=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value ^ _value>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator^=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value ^ _e([]{})>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator<<=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, (decltype(get([]{}))::value << _value)>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator<<=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, (decltype(get([]{}))::value << _e([]{}))>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}),_Type _value>
		constexpr auto operator>>=(std::integral_constant<_Type, _value>) const noexcept{
			return update<std::integral_constant<_Type, (decltype(get([]{}))::value >> _value)>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator>>=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return update<std::integral_constant<_Type, (decltype(get([]{}))::value >> _e([]{}))>>([]{});
		}
		
		// increment decrement. 
		template<auto = refresh(tool::token{}, key{})>
		constexpr auto operator++() const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value + 1>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{})>
		constexpr auto operator--() const noexcept{
			return update<std::integral_constant<_Type, decltype(get([]{}))::value - 1>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{})>
		constexpr auto operator++(int) const noexcept{
			constexpr auto result = get([]{});
			update<std::integral_constant<_Type, decltype(result)::value + 1>>([]{});
			return result;
		}
		
		template<auto = refresh(tool::token{}, key{})>
		constexpr auto operator--(int) const noexcept{
			constexpr auto result = get([]{});
			update<std::integral_constant<_Type, decltype(result)::value - 1>>([]{});
			return result;
		}
		
		// arithmetic. 
		template<auto = refresh(tool::token{}, key{})>
		constexpr auto operator+() const noexcept{
			return std::integral_constant<_Type, +decltype(get([]{}))::value>{};
		}
		
		template<auto = refresh(tool::token{}, key{})>
		constexpr auto operator-() const noexcept{
			return std::integral_constant<_Type, -decltype(get([]{}))::value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator+(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value + _value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator+(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value + _e([]{})>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator-(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value - _value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator-(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value - _e([]{})>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator*(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value * _value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator*(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value * _e([]{})>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator/(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value / _value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator/(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value / _e([]{})>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator%(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value % _value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator%(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value % _e([]{})>{};
		}
		
		template<auto = refresh(tool::token{}, key{})>
		constexpr auto operator~() const noexcept{
			return std::integral_constant<_Type, ~decltype(get([]{}))::value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator&(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value & _value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator&(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value & _e([]{})>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator|(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value | _value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator|(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value | _e([]{})>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator^(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value ^ _value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator^(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value ^ _e([]{})>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator<<(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, (decltype(get([]{}))::value << _value)>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator<<(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, (decltype(get([]{}))::value << _e([]{}))>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator>>(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, (decltype(get([]{}))::value >> _value)>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator>>(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, (decltype(get([]{}))::value >> _e([]{}))>{};
		}
		
		// logical. 
		template<auto = refresh(tool::token{}, key{})>
		constexpr auto operator!() const noexcept{
			return std::integral_constant<_Type, !decltype(get([]{}))::value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator&&(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value && _value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator&&(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value && _e([]{})>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator||(std::integral_constant<_Type, _value>) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value || _value>{};
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator||(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return std::integral_constant<_Type, decltype(get([]{}))::value || _e([]{})>{};
		}
		
		// comparison. 
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator==(std::integral_constant<_Type, _value>) const noexcept{
			return decltype(get([]{}))::value == _value;
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator==(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return decltype(get([]{}))::value == _e([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator!=(std::integral_constant<_Type, _value>) const noexcept{
			return decltype(get([]{}))::value != _value;
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator!=(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return decltype(get([]{}))::value != _e([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), _Type _value>
		constexpr auto operator<=>(std::integral_constant<_Type, _value>) const noexcept{
			return decltype(get([]{}))::value <=> _value;
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _ElementTag>
		constexpr auto operator<=>(fake::meta::number<_Type, _ElementTag> _e) const noexcept{
			return decltype(get([]{}))::value <=> _e([]{});
		}
		
	private:
		template<typename _Lambda>
		static consteval std::size_t next(_Lambda&& _lambda) noexcept{
			return fake::type_map::size<key>([]{});
		}
		
		template<typename _Lambda>
		static consteval std::size_t state(_Lambda&& _lambda) noexcept{
			return next([]{}) - 1;
		}
		
		template<typename _Lambda>
		static consteval auto get(_Lambda &&_lambda) noexcept{
			constexpr std::size_t current = state([]{});
			return typename decltype(fake::type_map::read<key, current>([]{}))::type{};
		}
		
		template<typename _Storage, typename _Lambda>
		static consteval auto update(_Lambda&& _lambda) noexcept{
			constexpr std::size_t current = next([]{});
			fake::type_map::write<key, current, _Storage>([]{});
			
			return _Storage{};
		}
	};
	
	template<typename _Constant, auto _lambda = []{}>
	requires fake::constant_c<_Constant>
	number(_Constant) -> number<typename _Constant::value_type, detail::tag<decltype(_lambda)>>;
	
	template<typename _Tag>
	struct variant
	{
		// constructor. 
		constexpr variant() noexcept{}
		template<fake::pack_c _Pack>
		constexpr variant(_Pack) noexcept{update<_Pack>([]{});}
		template<typename _Lambda>
		requires (fake::pack_c<_Lambda> == false)
		constexpr variant(_Lambda&&) noexcept{update<fake::type_package<fake::null_t>>([]{});}
		
	private:
		struct key{};
		
		variant(const variant&) = delete;
		variant(variant&&) = delete;
		constexpr auto operator=(const variant&) = delete;
		constexpr auto operator=(variant&&) = delete;
		
	public:
		// access. 
		template<auto = refresh(tool::token{}, key{}), typename _Lambda>
		constexpr auto operator()(_Lambda&&) const noexcept{
			return get([]{});
		}
		
		// assignment. 
		template<auto = refresh(tool::token{}, key{}), fake::pack_c _Pack>
		constexpr auto assign(_Pack&&) const noexcept{
			return update<std::remove_cvref_t<_Pack>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _Value>
		constexpr auto assign(_Value&&) const noexcept{
			return update<fake::type_package<_Value>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), fake::pack_c _Pack>
		constexpr auto operator=(_Pack&&) const noexcept{
			return update<std::remove_cvref_t<_Pack>>([]{});
		}
		
		template<auto = refresh(tool::token{}, key{}), typename _Value>
		constexpr auto operator=(_Value&&) const noexcept{
			return update<fake::type_package<_Value>>([]{});
		}
		
	private:
		template<typename _Lambda>
		static consteval std::size_t next(_Lambda&& _lambda) noexcept{
			return fake::type_map::size<key>([]{});
		}
		
		template<typename _Lambda>
		static consteval std::size_t state(_Lambda&& _lambda) noexcept{
			return next([]{}) - 1;
		}
		
		template<typename _Lambda>
		static consteval auto get(_Lambda &&_lambda) noexcept{
			constexpr std::size_t current = state([]{});
			return typename decltype(fake::type_map::read<key, current>([]{}))::type{};
		}
		
		template<typename _Package, typename _Lambda>
		static consteval auto update(_Lambda&& _lambda) noexcept{
			constexpr std::size_t current = next([]{});
			fake::type_map::write<key, current, _Package>([]{});
			
			return _Package{};
		}
	};
	
	template<fake::pack_c _Pack, auto _lambda = []{}>
	variant(_Pack) -> variant<detail::tag<decltype(_lambda)>>;
	
	template<typename _Lambda>
	requires (fake::pack_c<_Lambda> == false)
	variant(_Lambda&&) -> variant<detail::tag<_Lambda>>;
	
	template<typename> struct unordered_map;
	
	// tag: vector. 
	template<typename _Tag>
	struct vector final
	{
		using tag_type = typename _Tag::type;
		
		// constructor. 
		constexpr vector() noexcept{}
		
		template<typename _Lambda>
		constexpr vector(_Lambda &&_lambda) noexcept{
			update<storage<std::tuple<>>>([]{});
		}
		
	private:
		struct adl{};
		
	public:
		// element access. 
		template<auto = refresh(tool::token{}, adl{}), typename _Constant>
		requires fake::constant_c<_Constant> && std::same_as<typename _Constant::value_type, std::size_t>
		constexpr auto at(_Constant) const noexcept{
			constexpr std::size_t index_v = _Constant::value;
			constexpr std::size_t current_size = size_impl([]{});
			constexpr bool valid_index = index_v < current_size;
			static_assert(
				valid_index,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::at()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			
			if constexpr(valid_index){
				constexpr auto result = read_impl([]{});
				using package = decltype(result);
				
				using broker_index = std::integral_constant<std::size_t, index_v>;
				
				return broker<broker_index, typename package::type>{};
			}
		}
		
		template<auto = refresh(tool::token{}, adl{}), std::size_t _Index>
		constexpr auto operator[](std::integral_constant<std::size_t, _Index> &&_token) const noexcept{
			constexpr std::size_t current_size = size_impl([]{});
			constexpr bool valid_index = _Index < current_size;
			static_assert(
				valid_index,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::operator[]>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			
			if constexpr(valid_index){
				constexpr auto result = read_impl([]{});
				using package = decltype(result);
				
				using broker_index = std::integral_constant<std::size_t, _Index>;
				
				return broker<broker_index, typename package::type>{};
			}
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _IndexTag>
		constexpr auto operator[](fake::meta::number<std::size_t, _IndexTag> _index) const noexcept{
			constexpr std::size_t current_size = size_impl([]{});
			constexpr bool valid_index = _index([]{}) < current_size;
			static_assert(
				valid_index,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::operator[]>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			
			if constexpr(valid_index){
				constexpr auto result = read_impl([]{});
				using package = decltype(result);
				
				using broker_index = std::integral_constant<std::size_t, _index([]{})>;
				
				return broker<broker_index, typename package::type>{};
			}
		}
		
		template<typename _Lambda>
		constexpr auto front(_Lambda &&_lambda) const noexcept{
			constexpr std::size_t current_size = size_impl([]{});
			static_assert(
				current_size,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::front()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			if constexpr(current_size){
				constexpr auto result = read_impl([]{});
				using package = decltype(result);
				
				using broker_index = std::integral_constant<std::size_t, 0>;
				
				return broker<broker_index, typename package::type>{};
			}
		}
		
		template<typename _Lambda>
		constexpr auto back(_Lambda &&_lambda) const noexcept{
			constexpr std::size_t current_size = size_impl([]{});
			static_assert(
				current_size,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::back()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			if constexpr(current_size){
				constexpr auto result = read_impl([]{});
				using package = decltype(result);
				
				using broker_index = std::integral_constant<std::size_t, current_size - 1>;
				
				return broker<broker_index, typename package::type>{};
			}
		}
		
		template<typename _Lambda>
		constexpr auto data(_Lambda &&_lambda) const noexcept{
			constexpr auto result = read_impl([]{});
			using tuple = typename decltype(result)::type::tuple_t;
			return to_array<tuple>(std::make_index_sequence<std::tuple_size_v<tuple>>{});
		}
		
		// capacity. 
		template<typename _Lambda>
		constexpr bool empty(_Lambda &&_lambda) const noexcept{
			return size_impl([]{}) == 0;
		}
		
		template<typename _Lambda>
		constexpr std::size_t size(_Lambda &&_lambda) const noexcept{
			return size_impl([]{});
		}
		
		// modifier. 
		template<typename _Lambda>
		constexpr auto clear(_Lambda &&_lambda) const noexcept{
			update<storage<std::tuple<>>>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Constant, typename _Package>
		requires
			fake::constant_c<_Constant> &&
			std::same_as<typename _Constant::value_type, std::size_t> &&
			fake::pack_c<_Package>
		constexpr auto insert(_Constant, _Package) const noexcept{
			using type_t = typename _Package::type;
			constexpr std::size_t index_v = _Constant::value;
			constexpr std::size_t current_size = size_impl([]{});
			constexpr bool insert_origin = index_v == 0 && current_size == 0;
			constexpr bool valid_index = index_v < current_size || insert_origin;
			static_assert(
				valid_index,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::insert()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			if constexpr(insert_origin){
				using fresh = std::tuple<type_t>;
				update<storage<fresh>>([]{});
				
				using broker_index = std::integral_constant<std::size_t, 0>;
				
				return broker<broker_index, storage<fresh>>{};
			}
			else if constexpr(valid_index)
			{
				constexpr auto result = read_impl([]{});
				using tuple = typename decltype(result)::type::tuple_t;
				constexpr std::size_t tail = current_size - index_v;
				using prefix = decltype(subtuple<tuple, 0>(std::make_index_sequence<index_v>{}));
				using suffix = decltype(subtuple<tuple, index_v>(std::make_index_sequence<tail>{}));
				using fresh = decltype(
					std::tuple_cat(
						std::declval<typename prefix::type>(),
						std::declval<std::tuple<type_t>>(),
						std::declval<typename suffix::type>()
					)
				);
				update<storage<fresh>>([]{});
				
				using broker_index = std::integral_constant<std::size_t, index_v>;
				
				return broker<broker_index, storage<fresh>>{};
			}
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Constant, typename _Type>
		requires (
			fake::constant_c<_Constant> &&
			std::same_as<typename _Constant::value_type, std::size_t> &&
			fake::pack_c<_Type> == false
		)
		constexpr auto insert(_Constant, _Type&&) const noexcept{
			using type_t = _Type;
			constexpr std::size_t index_v = _Constant::value;
			constexpr std::size_t current_size = size_impl([]{});
			constexpr bool insert_origin = index_v == 0 && current_size == 0;
			constexpr bool valid_index = index_v < current_size || insert_origin;
			static_assert(
				valid_index,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::insert()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			if constexpr(insert_origin){
				using fresh = std::tuple<type_t>;
				update<storage<fresh>>([]{});
				
				using broker_index = std::integral_constant<std::size_t, 0>;
				
				return broker<broker_index, storage<fresh>>{};
			}
			else if constexpr(valid_index)
			{
				constexpr auto result = read_impl([]{});
				using tuple = typename decltype(result)::type::tuple_t;
				constexpr std::size_t tail = current_size - index_v;
				using prefix = decltype(subtuple<tuple, 0>(std::make_index_sequence<index_v>{}));
				using suffix = decltype(subtuple<tuple, index_v>(std::make_index_sequence<tail>{}));
				using fresh = decltype(
					std::tuple_cat(
						std::declval<typename prefix::type>(),
						std::declval<std::tuple<type_t>>(),
						std::declval<typename suffix::type>()
					)
				);
				update<storage<fresh>>([]{});
				
				using broker_index = std::integral_constant<std::size_t, index_v>;
				
				return broker<broker_index, storage<fresh>>{};
			}
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Constant, typename _Package>
		requires
			fake::constant_c<_Constant> &&
			std::same_as<typename _Constant::value_type, std::size_t> &&
			fake::pack_c<_Package>
		constexpr auto emplace(_Constant, _Package) const noexcept{
			constexpr std::size_t index_v = _Constant::value;
			constexpr std::size_t current_size = size_impl([]{});
			constexpr bool emplace_origin = index_v == 0 && current_size == 0;
			constexpr bool valid_index = index_v < current_size || emplace_origin;
			static_assert(
				valid_index,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::emplace()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			if constexpr(valid_index)
				return insert(_Constant{}, _Package{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Constant, typename _Type>
		requires (
			fake::constant_c<_Constant> &&
			std::same_as<typename _Constant::value_type, std::size_t> &&
			fake::pack_c<_Type> == false
		)
		constexpr auto emplace(_Constant, _Type &&_type) const noexcept{
			constexpr std::size_t index_v = _Constant::value;
			constexpr std::size_t current_size = size_impl([]{});
			constexpr bool emplace_origin = index_v == 0 && current_size == 0;
			constexpr bool valid_index = index_v < current_size || emplace_origin;
			static_assert(
				valid_index,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::emplace()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			if constexpr(valid_index)
				return insert(_Constant{}, std::forward<_Type>(_type));
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Constant>
		requires fake::constant_c<_Constant> && std::same_as<typename _Constant::value_type, std::size_t>
		constexpr auto erase(_Constant) const noexcept{
			constexpr std::size_t index_v = _Constant::value;
			constexpr std::size_t current_size = size_impl([]{});
			constexpr bool valid_index = index_v < current_size;
			static_assert(
				valid_index,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::erase()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			if constexpr(valid_index)
			{
				constexpr auto result = read_impl([]{});
				using tuple = typename decltype(result)::type::tuple_t;
				constexpr std::size_t tail = current_size - index_v - 1;
				using prefix = decltype(subtuple<tuple, 0>(std::make_index_sequence<index_v>{}));
				using suffix = decltype(subtuple<tuple, index_v + 1>(std::make_index_sequence<tail>{}));
				using fresh = decltype(
					std::tuple_cat(
						std::declval<typename prefix::type>(),
						std::declval<typename suffix::type>()
					)
				);
				update<storage<fresh>>([]{});
				
				if constexpr(tail){
					using broker_index = std::integral_constant<std::size_t, index_v>;
					return broker<broker_index, storage<fresh>>{};
				}
			}
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Package>
		requires fake::pack_c<_Package>
		constexpr auto push_back(_Package) const noexcept{
			return emplace_back_impl<typename _Package::type>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Type>
		requires (fake::pack_c<_Type> == false)
		constexpr auto push_back(_Type&&) const noexcept{
			return emplace_back_impl<_Type>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Package>
		requires fake::pack_c<_Package>
		constexpr auto emplace_back(_Package) const noexcept{
			return emplace_back_impl<typename _Package::type>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Type>
		requires (fake::pack_c<_Type> == false)
		constexpr auto emplace_back(_Type&&) const noexcept{
			return emplace_back_impl<_Type>([]{});
		}
		
		template<typename _Lambda>
		constexpr auto pop_back(_Lambda &&_lambda) const noexcept{
			constexpr std::size_t current_size = size_impl([]{});
			static_assert(
				current_size,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::pop_back()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			if constexpr(current_size){
				constexpr auto result = read_impl([]{});
				using tuple = typename decltype(result)::type::tuple_t;
				using pack = decltype(subtuple<tuple, 0>(std::make_index_sequence<current_size - 1>{}));
				update<storage<typename pack::type>>([]{});
				return fake::pack_v<std::tuple_element_t<current_size - 1, tuple>>;
			}
			else{
				return fake::pack_v<fake::null_t>;
			}
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Package>
		requires fake::pack_c<_Package>
		constexpr auto push_front(_Package) const noexcept{
			return emplace_front_impl<typename _Package::type>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Type>
		requires (fake::pack_c<_Type> == false)
		constexpr auto push_front(_Type&&) const noexcept{
			return emplace_front_impl<_Type>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Package>
		requires fake::pack_c<_Package>
		constexpr auto emplace_front(_Package) const noexcept{
			return emplace_front_impl<typename _Package::type>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Type>
		requires (fake::pack_c<_Type> == false)
		constexpr auto emplace_front(_Type&&) const noexcept{
			return emplace_front_impl<_Type>([]{});
		}
		
		template<typename _Lambda>
		constexpr auto pop_front(_Lambda &&_lambda) const noexcept{
			constexpr std::size_t current_size = size_impl([]{});
			static_assert(
				current_size,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::meta::vector::pop_front()>:\n"
				"\tout on range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			if constexpr(current_size){
				constexpr auto result = read_impl([]{});
				using tuple = typename decltype(result)::type::tuple_t;
				using pack = decltype(subtuple<tuple, 1>(std::make_index_sequence<current_size - 1>{}));
				update<storage<typename pack::type>>([]{});
				return fake::pack_v<std::tuple_element_t<0, tuple>>;
			}
			else{
				return fake::pack_v<fake::null_t>;
			}
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Constant>
		requires fake::constant_c<_Constant> && std::same_as<typename _Constant::value_type, std::size_t>
		constexpr auto resize(_Constant) const noexcept{
			constexpr std::size_t size_v = _Constant::value;
			constexpr std::size_t current_size = size_impl([]{});
			constexpr auto result = read_impl([]{});
			using tuple = typename decltype(result)::type::tuple_t;
			if constexpr(size_v < current_size){
				using pack = decltype(subtuple<tuple, 0>(std::make_index_sequence<size_v>{}));
				update<storage<typename pack::type>>([]{});
			}
			else if constexpr(current_size < size_v){
				using suffix = decltype(exttuple<tuple>(std::make_index_sequence<size_v - current_size>{}));
				using fresh = decltype(
					std::tuple_cat(
						std::declval<tuple>(),
						std::declval<typename suffix::type>()
					)
				);
				update<storage<fresh>>([]{});
			}
		}
		
	private:
		struct key;
		
		template<typename _Lambda>
		static constexpr std::size_t next(_Lambda&& _lambda) noexcept{
			return fake::type_map::size<key>([]{});
		}
		
		template<typename _Lambda>
		static constexpr std::size_t state(_Lambda&& _lambda) noexcept{
			return next([]{}) - 1;
		}
		
		template<typename _Storage, typename _Lambda>
		static constexpr auto update(_Lambda&& _lambda) noexcept{
			constexpr std::size_t current = next([]{});
			fake::type_map::write<key, current, _Storage>([]{});
			
			return fake::pack_v<_Storage>;
		}
		
		template<typename _Lambda>
		static constexpr auto read_impl(_Lambda &&_lambda) noexcept{
			constexpr std::size_t current = state([]{});
			return fake::type_map::read<key, current>([]{});
		}
		
		template<typename _Tuple, std::size_t... _Index>
		static constexpr auto to_array(std::index_sequence<_Index...>) noexcept{
			return meta::array<std::tuple_element_t<_Index, _Tuple>...>{};
		}
		
		template<typename _Lambda>
		static constexpr std::size_t size_impl(_Lambda &&_lambda) noexcept{
			constexpr auto result = read_impl([]{});
			using package = decltype(result);
			return package::type::size;
		}
		
		template<typename _Type, typename _Lambda>
		static constexpr auto emplace_back_impl(_Lambda &&_lambda) noexcept{
			constexpr auto result = read_impl([]{});
			using package = decltype(result);
			using tuple_t = decltype(
				std::tuple_cat(std::declval<typename package::type::tuple_t>(), std::declval<std::tuple<_Type>>())
			);
			update<storage<tuple_t>>([]{});
			
			using broker_index = std::integral_constant<
				std::size_t,
				std::tuple_size_v<typename package::type::tuple_t>
			>;
			
			return broker<broker_index, storage<tuple_t>>{};
		}
		
		template<typename _Type, typename _Lambda>
		static constexpr auto emplace_front_impl(_Lambda &&_lambda) noexcept{
			constexpr auto result = read_impl([]{});
			using package = decltype(result);
			using tuple_t = decltype(
				std::tuple_cat(std::declval<std::tuple<_Type>>(), std::declval<typename package::type::tuple_t>())
			);
			update<storage<tuple_t>>([]{});
			
			using broker_index = std::integral_constant<std::size_t, 0>;
			
			return broker<broker_index, storage<tuple_t>>{};
		}
		
		template<typename _Tuple, std::size_t _Origin, std::size_t... _Index>
		static constexpr auto subtuple(std::index_sequence<_Index...>) noexcept{
			return fake::pack_v<std::tuple<std::tuple_element_t<_Origin + _Index, _Tuple>...>>;
		}
		
		template<typename _Tuple, std::size_t... _Index>
		static constexpr auto exttuple(std::index_sequence<_Index...>) noexcept{
			return fake::pack_v<std::tuple<fake::type_t<decltype(_Index), fake::null_t>...>>;
		}
		
		// storage. 
		template<typename _Tuple>
		struct storage{
			constexpr storage() noexcept{}
			storage(const storage&) = delete;
			
			static constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			using tuple_t = _Tuple;
		};
		
	private:
		template<typename>
		friend struct fake::meta::vector;
		template<typename>
		friend struct fake::meta::unordered_map;
		
		// broker. 
		template<typename _Constant, typename _Storage>
		requires fake::constant_c<_Constant>
		struct broker{
			constexpr broker() noexcept{}
			broker(const broker&) = delete;
			
			static constexpr std::size_t index_v = _Constant::value;
			using type = std::tuple_element_t<index_v, typename _Storage::tuple_t>;
			
			template<typename _Lambda> constexpr auto operator()(_Lambda&&) const noexcept{return fake::pack_v<type>;}
			
			template<auto _lambda = refresh(tool::token{}, adl{}), typename _Type>
			constexpr auto operator=(fake::type_package<_Type> _package) const noexcept{
				using lambda_t = decltype(_lambda);
				
				constexpr container_t vec{};
				
				using tuple = typename decltype(vec.read_impl(pull<lambda_t>{}))::type::tuple_t;
				
				constexpr auto pack = replace<tuple, _Type>(std::make_index_sequence<std::tuple_size_v<tuple>>{});
				using storage = typename vector::template storage<typename decltype(pack)::type>;
				
				vec.update<storage>(update<lambda_t>{});
				
				return broker<query_t, storage>{};
			}
			
			template<auto _lambda = refresh(tool::token{}, adl{}), typename _Broker>
			requires std::same_as<
				typename _Broker::container_t::template broker<
					typename _Broker::query_t,
					typename _Broker::origin_t
				>,
				_Broker
			>
			constexpr auto operator=(const _Broker&) const noexcept{
				using lambda_t = decltype(_lambda);
				
				constexpr container_t vec{};
				
				using tuple = typename decltype(vec.read_impl(pull<lambda_t>{}))::type::tuple_t;
				using type = typename _Broker::type;
				
				constexpr auto pack = replace<tuple, type>(std::make_index_sequence<std::tuple_size_v<tuple>>{});
				using storage = typename vector::template storage<typename decltype(pack)::type>;
				
				vec.update<storage>(update<lambda_t>{});
				
				return broker<query_t, storage>{};
			}
			
		private:
			using container_t = fake::meta::vector<_Tag>;
			using query_t = _Constant;
			using origin_t = _Storage;
			
			template<typename>
			friend struct fake::meta::vector;
			template<typename>
			friend struct fake::meta::unordered_map;
			
			template<typename _Type> struct instance{};
			template<typename _Type> struct pull{};
			template<typename _Type> struct update{};
			template<typename _Type> struct refresh{};
			struct token;
			
			template<typename _Tuple, typename _Type, std::size_t... _Args>
			static constexpr auto replace(std::index_sequence<_Args...>) noexcept{
				return fake::pack_v<
					std::tuple<
						std::conditional_t<
							index_v == _Args,
							_Type,
							std::tuple_element_t<_Args, _Tuple>
						>...
					>
				>;
			}
		};
	};
	
	template<typename _Lambda>
	vector(_Lambda&&) -> vector<detail::tag<_Lambda>>;
	
	// tag: unordered_set. 
	template<typename _Tag>
	struct unordered_set final
	{
		using tag_type = typename _Tag::type;
		
		// constructor. 
		constexpr unordered_set() noexcept{}
		
		template<typename _Lambda>
		constexpr unordered_set(_Lambda &&_lambda) noexcept{
			node_init([]{});
		}
		
	private:
		struct adl{};
		
	public:
		// element access. 
		template<typename _Lambda>
		constexpr auto data(_Lambda &&_lambda) const noexcept{
			constexpr auto init = node_next([]{});
			using init_t = decltype(init);
			constexpr auto result = data_impl<init_t>([]{});
			using tuple_t = typename decltype(result)::type;
			
			return tuple_to_array<tuple_t>();
		}
		
		// capacity. 
		template<typename _Lambda>
		constexpr bool empty(_Lambda &&_lambda) const noexcept{
			constexpr auto init = node_next([]{});
			using init_t = std::remove_const_t<decltype(init)>;
			
			return std::is_same_v<init_t, node_null>;
		}
		
		template<typename _Lambda>
		constexpr std::size_t size(_Lambda &&_lambda) const noexcept{
			constexpr auto init = node_next([]{});
			using init_t = decltype(init);
			
			return size_impl<init_t>([]{});
		}
		
		// modifier. 
		template<typename _Lambda>
		constexpr auto clear(_Lambda &&_lambda) const noexcept{
			constexpr auto init = node_next([]{});
			using init_t = decltype(init);
			
			return clear_impl<init_t>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Package>
		requires fake::pack_c<_Package>
		constexpr auto insert(_Package) const noexcept{
			using type_t = typename _Package::type;
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<type_t>().hash());
			return emplace_impl<hash, type_t>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Type>
		requires (fake::pack_c<_Type> == false)
		constexpr auto insert(_Type&&) const noexcept{
			using type_t = _Type;
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<type_t>().hash());
			return emplace_impl<hash, type_t>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Package>
		requires fake::pack_c<_Package>
		constexpr auto emplace(_Package) const noexcept{
			using type_t = typename _Package::type;
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<type_t>().hash());
			return emplace_impl<hash, type_t>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Type>
		requires (fake::pack_c<_Type> == false)
		constexpr auto emplace(_Type&&) const noexcept{
			using type_t = _Type;
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<type_t>().hash());
			return emplace_impl<hash, type_t>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Package>
		requires fake::pack_c<_Package>
		constexpr auto erase(_Package) const noexcept{
			using type_t = typename _Package::type;
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<type_t>().hash());
			return erase_impl<hash, type_t>([]{});
		}
		
		// lookup. 
		template<auto = refresh(tool::token{}, adl{}), typename _Package>
		requires fake::pack_c<_Package>
		constexpr auto contains(_Package) const noexcept{
			using type_t = typename _Package::type;
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<type_t>().hash());
			return contains_impl<hash, type_t>([]{});
		}
		
	private:
		// bucket type. 
		template<std::size_t _Hash>
		struct key;
		
		// value_type: node_ptr<_Value, _Valid>. 
		struct node_head;
		// value_type: node_ptr<_Value, _Valid>. 
		struct node_tail;
		
		template<std::size_t _Value>
		struct node_key;
		
		template<std::size_t _Value>
		struct node_ptr{static constexpr std::size_t value = _Value;};
		struct node_null{};
		
		template<typename _Prev, typename _Next>
		struct node_value{
			using prev_t = _Prev;
			using next_t = _Next;
		};
		
		template<typename _Tuple>
		struct storage{
			constexpr storage() noexcept{}
			storage(const storage&) = delete;
			
			static constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			using tuple_t = _Tuple;
		};
		
		// node interface. 
		template<typename _Lambda>
		static constexpr auto node_init(_Lambda&& _lambda) noexcept{
			constexpr std::size_t head_update = fake::type_map::size<node_head>([]{});
			constexpr std::size_t tail_update = fake::type_map::size<node_tail>([]{});
			fake::type_map::write<node_head, head_update, node_null>([]{});
			fake::type_map::write<node_tail, tail_update, node_null>([]{});
		}
		
		template<typename _Ptr = node_head, typename _Lambda>
		static constexpr auto node_next(_Lambda&& _lambda) noexcept{
			if constexpr(std::is_same_v<std::remove_const_t<_Ptr>, node_head>){
				constexpr std::size_t head_update = fake::type_map::size<node_head>([]{});
				constexpr auto head_package = fake::type_map::read<node_head, head_update - 1>([]{});
				using head_value_t = typename decltype(head_package)::type;
				
				return head_value_t{};
			}
			else{
				constexpr std::size_t link_key = _Ptr::value;
				using link_key_t = node_key<link_key>;
				constexpr std::size_t link_update = fake::type_map::size<link_key_t>([]{});
				constexpr auto link_package = fake::type_map::read<link_key_t, link_update - 1>([]{});
				using link_value_t = typename decltype(link_package)::type;
				
				return typename link_value_t::next_t{};
			}
		}
		
		template<std::size_t _Hash, typename _Lambda>
		static constexpr auto node_emplace(_Lambda&& _lambda) noexcept{
			constexpr std::size_t head_update = fake::type_map::size<node_head>([]{});
			constexpr std::size_t tail_update = fake::type_map::size<node_tail>([]{});
			constexpr auto head_package = fake::type_map::read<node_head, head_update - 1>([]{});
			constexpr auto tail_package = fake::type_map::read<node_tail, tail_update - 1>([]{});
			using head_value_t = typename decltype(head_package)::type;
			using tail_value_t = typename decltype(tail_package)::type;
			constexpr bool head_null_ptr = std::is_same_v<head_value_t, node_null>;
			constexpr bool tail_null_ptr = std::is_same_v<tail_value_t, node_null>;
			
			static_assert(head_null_ptr == tail_null_ptr);
			
			using hash_key_t = node_key<_Hash>;
			
			if constexpr(head_null_ptr){
				using node_t = node_value<node_null, node_null>;
				constexpr std::size_t hash_update = fake::type_map::size<hash_key_t>([]{});
				
				if constexpr(hash_update){
					constexpr auto hash_package = fake::type_map::read<hash_key_t, hash_update - 1>([]{});
					using hash_value_t = typename decltype(hash_package)::type;
					constexpr bool valid_node_null = std::is_same_v<hash_value_t, node_null>;
					
					static_assert(valid_node_null);
				}
				
				fake::type_map::write<hash_key_t, hash_update, node_t>([]{});
				fake::type_map::write<node_head, head_update, node_ptr<_Hash>>([]{});
				fake::type_map::write<node_tail, tail_update, node_ptr<_Hash>>([]{});
			}
			else{
				constexpr std::size_t link_key = head_value_t::value;
				using link_key_t = node_key<link_key>;
				constexpr std::size_t link_update = fake::type_map::size<link_key_t>([]{});
				constexpr std::size_t hash_update = fake::type_map::size<hash_key_t>([]{});
				
				static_assert(link_update);
				if constexpr(link_update){
					constexpr auto head_package = fake::type_map::read<link_key_t, link_update - 1>([]{});
					using link_value_t = typename decltype(head_package)::type;
					using next_t = typename link_value_t::next_t;
					using link_t = node_value<node_ptr<_Hash>, next_t>;
					fake::type_map::write<link_key_t, link_update, link_t>([]{});
					
					using head_t = node_value<node_null, node_ptr<link_key>>;
					fake::type_map::write<hash_key_t, hash_update, head_t>([]{});
					
					fake::type_map::write<node_head, head_update, node_ptr<_Hash>>([]{});
				}
			}
		}
		
		template<std::size_t _Hash, typename _Lambda>
		static constexpr auto node_erase(_Lambda&& _lambda) noexcept{
			constexpr std::size_t head_update = fake::type_map::size<node_head>([]{});
			constexpr std::size_t tail_update = fake::type_map::size<node_tail>([]{});
			constexpr auto head_package = fake::type_map::read<node_head, head_update - 1>([]{});
			constexpr auto tail_package = fake::type_map::read<node_tail, tail_update - 1>([]{});
			using head_value_t = typename decltype(head_package)::type;
			using tail_value_t = typename decltype(tail_package)::type;
			constexpr bool head_null_ptr = std::is_same_v<head_value_t, node_null>;
			constexpr bool tail_null_ptr = std::is_same_v<tail_value_t, node_null>;
			
			static_assert(head_null_ptr == tail_null_ptr);
			
			if constexpr(head_null_ptr == false){
				using hash_key_t = node_key<_Hash>;
				using hash_ptr_t = node_ptr<_Hash>;
				constexpr std::size_t hash_update = fake::type_map::size<hash_key_t>([]{});
				
				static_assert(hash_update);
				if constexpr(hash_update){
					constexpr auto hash_package = fake::type_map::read<hash_key_t, hash_update - 1>([]{});
					using hash_value_t = typename decltype(hash_package)::type;
					
					static_assert(std::is_same_v<hash_value_t, node_null> == false);
					
					using prev_t = typename hash_value_t::prev_t;
					using next_t = typename hash_value_t::next_t;
					
					if constexpr(std::is_same_v<prev_t, node_null>){
						fake::type_map::write<node_head, head_update, next_t>([]{});
					}
					else{
						using prev_key_t = node_key<prev_t::value>;
						constexpr std::size_t prev_update = fake::type_map::size<prev_key_t>([]{});
						
						static_assert(prev_update);
						if constexpr(prev_update){
							constexpr auto prev_package = fake::type_map::read<prev_key_t, prev_update - 1>([]{});
							using prev_t = typename decltype(prev_package)::type::prev_t;
							fake::type_map::write<prev_key_t, prev_update, node_value<prev_t, hash_ptr_t>>([]{});
						}
					}
					
					if constexpr(std::is_same_v<next_t, node_null>){
						fake::type_map::write<node_tail, tail_update, prev_t>([]{});
					}
					else{
						using next_key_t = node_key<next_t::value>;
						constexpr std::size_t next_update = fake::type_map::size<next_key_t>([]{});
						
						static_assert(next_update);
						if constexpr(next_update){
							constexpr auto next_package = fake::type_map::read<next_key_t, next_update - 1>([]{});
							using next_t = typename decltype(next_package)::type::next_t;
							fake::type_map::write<next_key_t, next_update, node_value<hash_ptr_t, next_t>>([]{});
						}
					}
					
					fake::type_map::write<hash_key_t, hash_update, node_null>([]{});
				}
			}
		}
		
		// bucket interface. 
		template<std::size_t _Hash, typename _Lambda>
		static constexpr std::size_t bucket_next(_Lambda&& _lambda) noexcept{
			return fake::type_map::size<key<_Hash>>([]{});
		}
		
		template<std::size_t _Hash, typename _Storage, typename _Lambda>
		static constexpr auto bucket_update(_Lambda&& _lambda) noexcept{
			constexpr std::size_t current = bucket_next<_Hash>([]{});
			fake::type_map::write<key<_Hash>, current, _Storage>([]{});
			
			return fake::pack_v<_Storage>;
		}
		
		template<std::size_t _Hash, typename _Lambda>
		static constexpr auto bucket_read(_Lambda &&_lambda) noexcept{
			constexpr std::size_t next = bucket_next<_Hash>([]{});
			
			if constexpr(next)
				return fake::type_map::read<key<_Hash>, next - 1>([]{});
			else
				return fake::pack_v<storage<std::tuple<>>>;
		}
		
		template<std::size_t _Hash, typename _Lambda>
		static constexpr std::size_t bucket_size(_Lambda &&_lambda) noexcept{
			constexpr auto result = bucket_read([]{});
			using package = decltype(result);
			return package::type::size;
		}
		
		// tuple interface. 
		template<typename _Tuple, typename _Target>
		requires fake::tuple_c<_Tuple>
		static constexpr auto tuple_contains() noexcept{
			constexpr auto impl = []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return (std::is_same_v<std::tuple_element_t<_Index, _Tuple>, _Target> || ...);
			};
			return impl(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
		template<typename _Tuple, typename _Type>
		requires fake::tuple_c<_Tuple>
		static constexpr auto tuple_append() noexcept{
			using tuple_t = decltype(std::tuple_cat(std::declval<_Tuple>(), std::declval<std::tuple<_Type>>()));
			return fake::pack_v<tuple_t>;
		}
		
		template<typename _Tuple, typename _Type, std::size_t _Index = 0>
		requires fake::tuple_c<_Tuple>
		static constexpr auto tuple_remove() noexcept{
			constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			if constexpr(_Index < size){
				using current_t = std::tuple_element_t<_Index, _Tuple>;
				constexpr auto package = tuple_remove<_Tuple, _Type, _Index + 1>();
				using tuple_t = typename decltype(package)::type;
				
				if constexpr(std::is_same_v<_Type, current_t>){
					return fake::pack_v<tuple_t>;
				}
				else{
					using type_t = std::tuple<current_t>;
					using cat_t = decltype(std::tuple_cat(std::declval<tuple_t>(), std::declval<type_t>()));
					
					return fake::pack_v<cat_t>;
				}
			}
			else{
				return fake::pack_v<std::tuple<>>;
			}
		}
		
		template<typename _Tuple>
		requires fake::tuple_c<_Tuple>
		static constexpr auto tuple_to_array() noexcept{
			constexpr auto impl = []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return meta::array<std::tuple_element_t<_Index, _Tuple>...>{};
			};
			return impl(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
		// implement interface. 
		template<typename _Ptr, typename _Lambda>
		static constexpr auto data_impl(_Lambda &&_lambda) noexcept{
			using ptr_t = std::remove_const_t<_Ptr>;
			if constexpr(std::is_same_v<ptr_t, node_null>){
				return fake::pack_v<std::tuple<>>;
			}
			else{
				constexpr auto next = node_next<ptr_t>([]{});
				using next_t = decltype(next);
				constexpr auto package = data_impl<next_t>([]{});
				using tuple_t = typename decltype(package)::type;
				constexpr auto bucket = bucket_read<ptr_t::value>([]{});
				using current_t = typename decltype(bucket)::type::tuple_t;
				
				using cat_t = decltype(std::tuple_cat(std::declval<tuple_t>(), std::declval<current_t>()));
				
				return fake::pack_v<cat_t>;
			}
		}
		
		template<typename _Ptr, typename _Lambda>
		static constexpr auto size_impl(_Lambda &&_lambda) noexcept{
			using ptr_t = std::remove_const_t<_Ptr>;
			if constexpr(std::is_same_v<ptr_t, node_null>){
				return std::size_t{};
			}
			else{
				constexpr auto next = node_next<ptr_t>([]{});
				using next_t = decltype(next);
				constexpr auto package = data_impl<next_t>([]{});
				using tuple_t = typename decltype(package)::type;
				constexpr auto bucket = bucket_read<ptr_t::value>([]{});
				using current_t = typename decltype(bucket)::type::tuple_t;
				
				return std::tuple_size_v<tuple_t> + std::tuple_size_v<current_t>;
			}
		}
		
		template<typename _Ptr, typename _Lambda>
		static constexpr auto clear_impl(_Lambda &&_lambda) noexcept{
			using ptr_t = std::remove_const_t<_Ptr>;
			if constexpr(std::is_same_v<ptr_t, node_null>){
				return node_init([]{});
			}
			else{
				constexpr auto next = node_next<ptr_t>([]{});
				using next_t = decltype(next);
				
				using node_key_t = node_key<ptr_t::value>;
				constexpr std::size_t hash_update = fake::type_map::size<node_key_t>([]{});
				fake::type_map::write<node_key_t, hash_update, node_null>([]{});
				
				bucket_update<ptr_t::value, storage<std::tuple<>>>([]{});
				
				return clear_impl<next_t>([]{});
			}
		}
		
		template<std::size_t _Hash, typename _Type, typename _Lambda>
		static constexpr auto emplace_impl(_Lambda &&_lambda) noexcept{
			constexpr auto result = bucket_read<_Hash>([]{});
			
			if constexpr(decltype(result)::type::size == 0)
				node_emplace<_Hash>([]{});
			
			using origin_t = typename decltype(result)::type::tuple_t;
			constexpr bool contains = tuple_contains<origin_t, _Type>();
			
			if constexpr(contains == false){
				constexpr auto append = tuple_append<origin_t, _Type>();
				using tuple_t = typename decltype(append)::type;
				bucket_update<_Hash, storage<tuple_t>>([]{});
			}
			
			return contains == false;
		}
		
		template<std::size_t _Hash, typename _Type, typename _Lambda>
		static constexpr auto erase_impl(_Lambda &&_lambda) noexcept{
			constexpr auto result = bucket_read<_Hash>([]{});
			
			if constexpr(decltype(result)::type::size){
				using origin_t = typename decltype(result)::type::tuple_t;
				constexpr auto remove = tuple_remove<origin_t, _Type>();
				using tuple_t = typename decltype(remove)::type;
				constexpr bool contains = std::tuple_size_v<origin_t> != std::tuple_size_v<tuple_t>;
				
				if constexpr(contains){
					bucket_update<_Hash, storage<tuple_t>>([]{});
					
					if constexpr(std::tuple_size_v<tuple_t> == 0)
						node_erase<_Hash>([]{});
				}
				
				return contains;
			}
			else{
				return false;
			}
		}
		
		template<std::size_t _Hash, typename _Type, typename _Lambda>
		static constexpr auto contains_impl(_Lambda &&_lambda) noexcept{
			constexpr auto result = bucket_read<_Hash>([]{});
			using origin_t = typename decltype(result)::type::tuple_t;
			
			return tuple_contains<origin_t, _Type>();
		}
	};
	
	template<typename _Lambda>
	unordered_set(_Lambda&&) -> unordered_set<detail::tag<_Lambda>>;
	
	// tag: unordered_map. 
	template<typename _Tag>
	struct unordered_map final
	{
		using tag_type = typename _Tag::type;
		
		// constructor. 
		constexpr unordered_map() noexcept{}
		
		template<typename _Lambda>
		constexpr unordered_map(_Lambda &&_lambda) noexcept{
			node_init([]{});
		}
		
	private:
		struct adl{};
		
	public:
		// element access. 
		template<typename _Lambda>
		constexpr auto data(_Lambda &&_lambda) const noexcept{
			constexpr auto init = node_next([]{});
			using init_t = decltype(init);
			constexpr auto result = data_impl<init_t>([]{});
			using tuple_t = typename decltype(result)::type;
			return tuple_to_array<tuple_t>();
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Key>
		requires fake::pack_c<_Key>
		constexpr auto at(_Key) const noexcept{
			using key_type = _Key::type;
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<key_type>().hash());
			return at_impl<hash, key_type>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Package>
		requires fake::pack_c<_Package>
		constexpr auto operator[](_Package) const noexcept{
			using query_key_t = typename _Package::type;
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<query_key_t>().hash());
			using result_t = typename decltype(at_impl<hash, query_key_t>([]{}))::type;
			return broker<query_key_t, result_t>{};
		}
		
		// capacity. 
		template<typename _Lambda>
		constexpr bool empty(_Lambda &&_lambda) const noexcept{
			constexpr auto init = node_next([]{});
			using init_t = std::remove_const_t<decltype(init)>;
			return std::is_same_v<init_t, node_null>;
		}
		
		template<typename _Lambda>
		constexpr std::size_t size(_Lambda &&_lambda) const noexcept{
			constexpr auto init = node_next([]{});
			using init_t = decltype(init);
			return size_impl<init_t>([]{});
		}
		
		// modifier. 
		template<typename _Lambda>
		constexpr auto clear(_Lambda &&_lambda) const noexcept{
			constexpr auto init = node_next([]{});
			using init_t = decltype(init);
			return clear_impl<init_t>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Key, typename _Value>
		constexpr auto insert(_Key&&, _Value&&) const noexcept{
			using key_type = typename std::conditional_t<
				fake::pack_c<_Key>,
				std::remove_cvref_t<_Key>,
				fake::type_package<_Key>
			>::type;
			using value_type = typename std::conditional_t<
				fake::pack_c<_Value>,
				std::remove_cvref_t<_Value>,
				fake::type_package<_Value>
			>::type;
			
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<key_type>().hash());
			return emplace_impl<hash, key_type, value_type>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Key, typename _Value>
		constexpr auto emplace(_Key&&, _Value&&) const noexcept{
			using key_type = typename std::conditional_t<
				fake::pack_c<_Key>,
				std::remove_cvref_t<_Key>,
				fake::type_package<_Key>
			>::type;
			using value_type = typename std::conditional_t<
				fake::pack_c<_Value>,
				std::remove_cvref_t<_Value>,
				fake::type_package<_Value>
			>::type;
			
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<key_type>().hash());
			return emplace_impl<hash, key_type, value_type>([]{});
		}
		
		template<auto = refresh(tool::token{}, adl{}), typename _Key>
		requires fake::pack_c<_Key>
		constexpr auto erase(_Key) const noexcept{
			using key_type = _Key::type;
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<key_type>().hash());
			return erase_impl<hash, key_type>([]{});
		}
		
		// lookup. 
		template<auto = refresh(tool::token{}, adl{}), typename _Key>
		requires fake::pack_c<_Key>
		constexpr auto contains(_Key) const noexcept{
			using key_type = _Key::type;
			constexpr std::size_t hash = std::size_t(fake::symbol::make_view<key_type>().hash());
			return contains_impl<hash, key_type>([]{});
		}
		
	private:
		// bucket type. 
		template<std::size_t _Hash>
		struct key;
		
		// value_type: node_ptr<_Value, _Valid>. 
		struct node_head;
		// value_type: node_ptr<_Value, _Valid>. 
		struct node_tail;
		
		template<std::size_t _Value>
		struct node_key;
		
		template<std::size_t _Value>
		struct node_ptr{static constexpr std::size_t value = _Value;};
		struct node_null{};
		
		template<typename _Prev, typename _Next>
		struct node_value{
			using prev_t = _Prev;
			using next_t = _Next;
		};
		
		template<typename _Tuple>
		struct storage{
			constexpr storage() noexcept{}
			storage(const storage&) = delete;
			
			static constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			using tuple_t = _Tuple;
		};
		
		// node interface. 
		template<typename _Lambda>
		static constexpr auto node_init(_Lambda&& _lambda) noexcept{
			constexpr std::size_t head_update = fake::type_map::size<node_head>([]{});
			constexpr std::size_t tail_update = fake::type_map::size<node_tail>([]{});
			fake::type_map::write<node_head, head_update, node_null>([]{});
			fake::type_map::write<node_tail, tail_update, node_null>([]{});
		}
		
		template<typename _Ptr = node_head, typename _Lambda>
		static constexpr auto node_next(_Lambda&& _lambda) noexcept{
			if constexpr(std::is_same_v<std::remove_const_t<_Ptr>, node_head>){
				constexpr std::size_t head_update = fake::type_map::size<node_head>([]{});
				constexpr auto head_package = fake::type_map::read<node_head, head_update - 1>([]{});
				using head_value_t = typename decltype(head_package)::type;
				
				return head_value_t{};
			}
			else{
				constexpr std::size_t link_key = _Ptr::value;
				using link_key_t = node_key<link_key>;
				constexpr std::size_t link_update = fake::type_map::size<link_key_t>([]{});
				constexpr auto link_package = fake::type_map::read<link_key_t, link_update - 1>([]{});
				using link_value_t = typename decltype(link_package)::type;
				
				return typename link_value_t::next_t{};
			}
		}
		
		template<std::size_t _Hash, typename _Lambda>
		static constexpr auto node_emplace(_Lambda&& _lambda) noexcept{
			constexpr std::size_t head_update = fake::type_map::size<node_head>([]{});
			constexpr std::size_t tail_update = fake::type_map::size<node_tail>([]{});
			constexpr auto head_package = fake::type_map::read<node_head, head_update - 1>([]{});
			constexpr auto tail_package = fake::type_map::read<node_tail, tail_update - 1>([]{});
			using head_value_t = typename decltype(head_package)::type;
			using tail_value_t = typename decltype(tail_package)::type;
			constexpr bool head_null_ptr = std::is_same_v<head_value_t, node_null>;
			constexpr bool tail_null_ptr = std::is_same_v<tail_value_t, node_null>;
			
			static_assert(head_null_ptr == tail_null_ptr);
			
			using hash_key_t = node_key<_Hash>;
			
			if constexpr(head_null_ptr){
				using node_t = node_value<node_null, node_null>;
				constexpr std::size_t hash_update = fake::type_map::size<hash_key_t>([]{});
				
				if constexpr(hash_update){
					constexpr auto hash_package = fake::type_map::read<hash_key_t, hash_update - 1>([]{});
					using hash_value_t = typename decltype(hash_package)::type;
					constexpr bool valid_node_null = std::is_same_v<hash_value_t, node_null>;
					
					static_assert(valid_node_null);
				}
				
				fake::type_map::write<hash_key_t, hash_update, node_t>([]{});
				fake::type_map::write<node_head, head_update, node_ptr<_Hash>>([]{});
				fake::type_map::write<node_tail, tail_update, node_ptr<_Hash>>([]{});
			}
			else{
				constexpr std::size_t link_key = head_value_t::value;
				using link_key_t = node_key<link_key>;
				constexpr std::size_t link_update = fake::type_map::size<link_key_t>([]{});
				constexpr std::size_t hash_update = fake::type_map::size<hash_key_t>([]{});
				
				static_assert(link_update);
				if constexpr(link_update){
					constexpr auto head_package = fake::type_map::read<link_key_t, link_update - 1>([]{});
					using link_value_t = typename decltype(head_package)::type;
					using next_t = typename link_value_t::next_t;
					using link_t = node_value<node_ptr<_Hash>, next_t>;
					fake::type_map::write<link_key_t, link_update, link_t>([]{});
					
					using head_t = node_value<node_null, node_ptr<link_key>>;
					fake::type_map::write<hash_key_t, hash_update, head_t>([]{});
					
					fake::type_map::write<node_head, head_update, node_ptr<_Hash>>([]{});
				}
			}
		}
		
		template<std::size_t _Hash, typename _Lambda>
		static constexpr auto node_erase(_Lambda&& _lambda) noexcept{
			constexpr std::size_t head_update = fake::type_map::size<node_head>([]{});
			constexpr std::size_t tail_update = fake::type_map::size<node_tail>([]{});
			constexpr auto head_package = fake::type_map::read<node_head, head_update - 1>([]{});
			constexpr auto tail_package = fake::type_map::read<node_tail, tail_update - 1>([]{});
			using head_value_t = typename decltype(head_package)::type;
			using tail_value_t = typename decltype(tail_package)::type;
			constexpr bool head_null_ptr = std::is_same_v<head_value_t, node_null>;
			constexpr bool tail_null_ptr = std::is_same_v<tail_value_t, node_null>;
			
			static_assert(head_null_ptr == tail_null_ptr);
			
			if constexpr(head_null_ptr == false){
				using hash_key_t = node_key<_Hash>;
				using hash_ptr_t = node_ptr<_Hash>;
				constexpr std::size_t hash_update = fake::type_map::size<hash_key_t>([]{});
				
				static_assert(hash_update);
				if constexpr(hash_update){
					constexpr auto hash_package = fake::type_map::read<hash_key_t, hash_update - 1>([]{});
					using hash_value_t = typename decltype(hash_package)::type;
					
					static_assert(std::is_same_v<hash_value_t, node_null> == false);
					
					using prev_t = typename hash_value_t::prev_t;
					using next_t = typename hash_value_t::next_t;
					
					if constexpr(std::is_same_v<prev_t, node_null>){
						fake::type_map::write<node_head, head_update, next_t>([]{});
					}
					else{
						using prev_key_t = node_key<prev_t::value>;
						constexpr std::size_t prev_update = fake::type_map::size<prev_key_t>([]{});
						
						static_assert(prev_update);
						if constexpr(prev_update){
							constexpr auto prev_package = fake::type_map::read<prev_key_t, prev_update - 1>([]{});
							using prev_t = typename decltype(prev_package)::type::prev_t;
							fake::type_map::write<prev_key_t, prev_update, node_value<prev_t, hash_ptr_t>>([]{});
						}
					}
					
					if constexpr(std::is_same_v<next_t, node_null>){
						fake::type_map::write<node_tail, tail_update, prev_t>([]{});
					}
					else{
						using next_key_t = node_key<next_t::value>;
						constexpr std::size_t next_update = fake::type_map::size<next_key_t>([]{});
						
						static_assert(next_update);
						if constexpr(next_update){
							constexpr auto next_package = fake::type_map::read<next_key_t, next_update - 1>([]{});
							using next_t = typename decltype(next_package)::type::next_t;
							fake::type_map::write<next_key_t, next_update, node_value<hash_ptr_t, next_t>>([]{});
						}
					}
					
					fake::type_map::write<hash_key_t, hash_update, node_null>([]{});
				}
			}
		}
		
		// bucket interface. 
		template<std::size_t _Hash, typename _Lambda>
		static constexpr std::size_t bucket_next(_Lambda&& _lambda) noexcept{
			return fake::type_map::size<key<_Hash>>([]{});
		}
		
		template<std::size_t _Hash, typename _Storage, typename _Lambda>
		static constexpr auto bucket_update(_Lambda&& _lambda) noexcept{
			constexpr std::size_t current = bucket_next<_Hash>([]{});
			fake::type_map::write<key<_Hash>, current, _Storage>([]{});
			
			return fake::pack_v<_Storage>;
		}
		
		template<std::size_t _Hash, typename _Lambda>
		static constexpr auto bucket_read(_Lambda &&_lambda) noexcept{
			constexpr std::size_t next = bucket_next<_Hash>([]{});
			
			if constexpr(next)
				return fake::type_map::read<key<_Hash>, next - 1>([]{});
			else
				return fake::pack_v<storage<std::tuple<>>>;
		}
		
		template<std::size_t _Hash, typename _Lambda>
		static constexpr std::size_t bucket_size(_Lambda &&_lambda) noexcept{
			constexpr auto result = bucket_read([]{});
			using package = decltype(result);
			return package::type::size;
		}
		
		// tuple interface. 
		template<typename _Tuple, typename _Target, std::size_t _Index = 0>
		requires fake::tuple_c<_Tuple>
		static constexpr auto tuple_query() noexcept{
			constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			
			if constexpr(_Index < size){
				using current_key_t = typename std::tuple_element_t<_Index, _Tuple>::first_type;
				using current_value_t = typename std::tuple_element_t<_Index, _Tuple>::second_type;
				constexpr bool same = std::is_same_v<current_key_t, _Target>;
				
				if constexpr(same)
					return fake::pack_v<current_value_t>;
				else
					return tuple_query<_Tuple, _Target, _Index + 1>();
			}
			else{
				return fake::pack_v<fake::null_t>;
			}
		}
		
		template<typename _Tuple, typename _Target>
		requires fake::tuple_c<_Tuple>
		static constexpr auto tuple_contains() noexcept{
			constexpr auto impl = []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return (
					std::is_same_v<typename std::tuple_element_t<_Index, _Tuple>::first_type, _Target> || ...
				);
			};
			return impl(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
		template<typename _Tuple, typename _Key, typename _Value>
		requires fake::tuple_c<_Tuple>
		static constexpr auto tuple_contains_pair() noexcept{
			using pair_t = std::pair<_Key, _Value>;
			constexpr auto impl = []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return (std::is_same_v<typename std::tuple_element_t<_Index, _Tuple>, pair_t> || ...);
			};
			return impl(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
		template<typename _Tuple, typename _Key, typename _Value>
		requires fake::tuple_c<_Tuple>
		static constexpr auto tuple_append() noexcept{
			using pair_t = std::pair<_Key, _Value>;
			using tuple_t = decltype(std::tuple_cat(std::declval<_Tuple>(), std::declval<std::tuple<pair_t>>()));
			return fake::pack_v<tuple_t>;
		}
		
		template<typename _Tuple, typename _Type, std::size_t _Index = 0>
		requires fake::tuple_c<_Tuple>
		static constexpr auto tuple_remove() noexcept{
			constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			if constexpr(_Index < size){
				using current_t = std::tuple_element_t<_Index, _Tuple>;
				constexpr auto package = tuple_remove<_Tuple, _Type, _Index + 1>();
				using tuple_t = typename decltype(package)::type;
				
				if constexpr(std::is_same_v<_Type, typename current_t::first_type>){
					return fake::pack_v<tuple_t>;
				}
				else{
					using type_t = std::tuple<current_t>;
					using cat_t = decltype(std::tuple_cat(std::declval<tuple_t>(), std::declval<type_t>()));
					
					return fake::pack_v<cat_t>;
				}
			}
			else{
				return fake::pack_v<std::tuple<>>;
			}
		}
		
		template<typename _Tuple>
		requires fake::tuple_c<_Tuple>
		static constexpr auto tuple_to_array() noexcept{
			constexpr auto impl = []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return meta::array<std::tuple_element_t<_Index, _Tuple>...>{};
			};
			return impl(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
		// implement interface. 
		template<typename _Ptr, typename _Lambda>
		static constexpr auto data_impl(_Lambda &&_lambda) noexcept{
			using ptr_t = std::remove_const_t<_Ptr>;
			if constexpr(std::is_same_v<ptr_t, node_null>){
				return fake::pack_v<std::tuple<>>;
			}
			else{
				constexpr auto next = node_next<ptr_t>([]{});
				using next_t = decltype(next);
				constexpr auto package = data_impl<next_t>([]{});
				using tuple_t = typename decltype(package)::type;
				constexpr auto bucket = bucket_read<ptr_t::value>([]{});
				using current_t = typename decltype(bucket)::type::tuple_t;
				
				using cat_t = decltype(std::tuple_cat(std::declval<tuple_t>(), std::declval<current_t>()));
				
				return fake::pack_v<cat_t>;
			}
		}
		
		template<std::size_t _Hash, typename _Type, typename _Lambda>
		static constexpr auto at_impl(_Lambda &&_lambda) noexcept{
			constexpr auto result = bucket_read<_Hash>([]{});
			using origin_t = typename decltype(result)::type::tuple_t;
			
			return tuple_query<origin_t, _Type>();
		}
		
		template<typename _Ptr, typename _Lambda>
		static constexpr auto size_impl(_Lambda &&_lambda) noexcept{
			using ptr_t = std::remove_const_t<_Ptr>;
			if constexpr(std::is_same_v<ptr_t, node_null>){
				return std::size_t{};
			}
			else{
				constexpr auto next = node_next<ptr_t>([]{});
				using next_t = decltype(next);
				constexpr auto package = data_impl<next_t>([]{});
				using tuple_t = typename decltype(package)::type;
				constexpr auto bucket = bucket_read<ptr_t::value>([]{});
				using current_t = typename decltype(bucket)::type::tuple_t;
				
				return std::tuple_size_v<tuple_t> + std::tuple_size_v<current_t>;
			}
		}
		
		template<typename _Ptr, typename _Lambda>
		static constexpr auto clear_impl(_Lambda &&_lambda) noexcept{
			using ptr_t = std::remove_const_t<_Ptr>;
			if constexpr(std::is_same_v<ptr_t, node_null>){
				return node_init([]{});
			}
			else{
				constexpr auto next = node_next<ptr_t>([]{});
				using next_t = decltype(next);
				
				using node_key_t = node_key<ptr_t::value>;
				constexpr std::size_t hash_update = fake::type_map::size<node_key_t>([]{});
				fake::type_map::write<node_key_t, hash_update, node_null>([]{});
				
				bucket_update<ptr_t::value, storage<std::tuple<>>>([]{});
				
				return clear_impl<next_t>([]{});
			}
		}
		
		template<std::size_t _Hash, typename _Key, typename _Value, typename _Lambda>
		static constexpr auto emplace_impl(_Lambda &&_lambda) noexcept{
			constexpr auto result = bucket_read<_Hash>([]{});
			
			if constexpr(decltype(result)::type::size == 0)
				node_emplace<_Hash>([]{});
			
			using origin_t = typename decltype(result)::type::tuple_t;
			constexpr bool contains_key = tuple_contains<origin_t, _Key>();
			constexpr bool contains_pair = tuple_contains_pair<origin_t, _Key, _Value>();
			
			if constexpr(contains_pair == false){
				if constexpr(contains_key){
					constexpr auto remove = tuple_remove<origin_t, _Key>();
					using remove_t = typename decltype(remove)::type;
					
					constexpr auto append = tuple_append<remove_t, _Key, _Value>();
					using tuple_t = typename decltype(append)::type;
					bucket_update<_Hash, storage<tuple_t>>([]{});
				}
				else{
					constexpr auto append = tuple_append<origin_t, _Key, _Value>();
					using tuple_t = typename decltype(append)::type;
					bucket_update<_Hash, storage<tuple_t>>([]{});
				}
			}
			
			return contains_pair == false;
		}
		
		template<std::size_t _Hash, typename _Type, typename _Lambda>
		static constexpr auto erase_impl(_Lambda &&_lambda) noexcept{
			constexpr auto result = bucket_read<_Hash>([]{});
			
			if constexpr(decltype(result)::type::size){
				using origin_t = typename decltype(result)::type::tuple_t;
				constexpr auto remove = tuple_remove<origin_t, _Type>();
				using tuple_t = typename decltype(remove)::type;
				constexpr bool contains = std::tuple_size_v<origin_t> != std::tuple_size_v<tuple_t>;
				
				if constexpr(contains){
					bucket_update<_Hash, storage<tuple_t>>([]{});
					
					if constexpr(std::tuple_size_v<tuple_t> == 0)
						node_erase<_Hash>([]{});
				}
				
				return contains;
			}
			else{
				return false;
			}
		}
		
		template<std::size_t _Hash, typename _Type, typename _Lambda>
		static constexpr auto contains_impl(_Lambda &&_lambda) noexcept{
			constexpr auto result = bucket_read<_Hash>([]{});
			using origin_t = typename decltype(result)::type::tuple_t;
			
			return tuple_contains<origin_t, _Type>();
		}
		
	private:
		template<typename>
		friend struct fake::meta::vector;
		template<typename>
		friend struct fake::meta::unordered_map;
		
		// broker. 
		template<typename _Key, typename _Value>
		struct broker{
			using type = _Value;
			
			constexpr broker() noexcept{}
			broker(const broker&) = delete;
			
			template<typename _Lambda> constexpr auto operator()(_Lambda&&) const noexcept{return fake::pack_v<type>;}
			
			template<auto = refresh(tool::token{}, adl{}), typename _Package>
			requires fake::pack_c<_Package>
			constexpr auto operator=(_Package) const noexcept{
				using value_t = typename _Package::type;
				if constexpr(std::is_same_v<value_t, origin_t> == false){
					constexpr container_t map;
					map.emplace(fake::pack_v<query_t>, fake::pack_v<value_t>);
					
					return broker<query_t, value_t>{};
				}
				else{
					return broker<query_t, origin_t>{};
				}
			}
			
			template<auto = refresh(tool::token{}, adl{}), typename _Broker>
			requires std::same_as<
				typename _Broker::container_t::template broker<
					typename _Broker::query_t,
					typename _Broker::origin_t
				>,
				_Broker
			>
			constexpr auto operator=(const _Broker&) const noexcept{
				using value_t = _Broker::type;
				if constexpr(std::is_same_v<value_t, origin_t> == false){
					constexpr container_t map;
					map.emplace(fake::pack_v<query_t>, fake::pack_v<value_t>);
					
					return broker<query_t, value_t>{};
				}
				else{
					return broker<query_t, origin_t>{};
				}
			}
			
		private:
			using container_t = unordered_map<_Tag>;
			using query_t = _Key;
			using origin_t = _Value;
			
			template<typename>
			friend struct fake::meta::vector;
			template<typename>
			friend struct fake::meta::unordered_map;
		};
	};
	
	template<typename _Lambda>
	unordered_map(_Lambda&&) -> unordered_map<detail::tag<_Lambda>>;
	
}

namespace fake::meta
{
	
	template<typename... _Args>
	struct stack : public array<_Args...>{
	private:
		using array_t = array<_Args...>;
		
	public:
		static constexpr auto size() noexcept{
			return array_t::size > 0;
		}
		
		template<typename = void>
		requires (size())
		static constexpr auto top() noexcept{
			return fake::pack_v<array_element<0, array_t>>;
		}
		
		template<typename _Type>
		static constexpr auto push() noexcept{
			return push_impl<_Type>(std::make_index_sequence<array_t::size>{});
		}
		
		template<typename = void>
		requires (size())
		static constexpr auto pop() noexcept{
			return pop_impl(std::make_index_sequence<array_t::size - 1>{});
		}
		
	private:
		template<typename _Type, std::size_t... _Index>
		static constexpr auto push_impl(std::index_sequence<_Index...>) noexcept ->
			stack<_Type, array_element<_Index, array_t>...>{return {};}
		
		template<std::size_t... _Index>
		static constexpr auto pop_impl(std::index_sequence<_Index...>) noexcept ->
			stack<array_element<_Index + 1, array_t>...>{return {};}
		
		static_assert(
			(fake::meta::is_array_v<_Args> || ...) == false,
			"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			"\nerror<fake::meta::stack>:\n"
			"\ttemplate argument '_Args...' must NOT be type 'fake::meta::stack<...>'.\n"
			"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
		);
	};
	
}

namespace fake::meta
{
	
	template<auto _cond, auto = refresh(tool::token{}, tool::token{})>
	struct while_x
	{
	private:
		struct key{};
		
		template<auto = refresh(tool::token{}, key{}), typename _Body>
		static consteval auto loop(_Body &&_body) noexcept{
			if constexpr(_cond([]{})){
				_body([]{});
				return loop<[]{}>(std::forward<_Body>(_body));
			}
		}
		
	public:
		template<auto = refresh(tool::token{}, key{}), typename _Body>
		consteval while_x(_Body &&_body) noexcept{
			loop<[]{}>(std::forward<_Body>(_body));
		}
		
		consteval operator bool() const noexcept{return true;}
	};
	
	template<auto _init, auto _cond, auto _side, auto = refresh(tool::token{}, tool::token{})>
	struct for_x
	{
	private:
		struct key{};
		
		template<auto = refresh(tool::token{}, key{}), typename _Body>
		static consteval auto loop(_Body &&_body) noexcept{
			if constexpr(_cond([]{})){
				_body([]{});
				_side([]{});
				return loop<[]{}>(std::forward<_Body>(_body));
			}
		}
		
	public:
		template<auto = refresh(tool::token{}, key{}), typename _Body>
		consteval for_x(_Body &&_body) noexcept{
			_init([]{});
			loop<[]{}>(std::forward<_Body>(_body));
		}
		
		consteval operator bool() const noexcept{return true;}
	};
	
	template<typename _Lamdba> void delay(_Lamdba _lambda){_lambda([]{});}
	
}

#endif /*__FAKE_META_H__*/ 
