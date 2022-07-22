#ifndef __FAKE_VIEW_H__
#define __FAKE_VIEW_H__

#include <type_traits>
#include <algorithm>

#include "traits.h"
#include "symbol.h"

namespace fake
{
	
	namespace detail::view
	{
		
		template<std::size_t _Size>
		struct string{
			constexpr string(const char (&_str)[_Size]) noexcept{std::ranges::copy(_str, buffer);};
			explicit constexpr string(const char* _str) noexcept{std::ranges::copy_n(_str, _Size, buffer);};
			
			constexpr std::size_t size() const noexcept{return _Size;}
			constexpr std::size_t length() const noexcept{return _Size;}
			
			template<std::size_t _Index>
			requires (_Index < _Size)
			constexpr char at() const noexcept{return buffer[_Index];}
			
			constexpr decltype(auto) data() const noexcept{return (buffer);}
			
			char buffer[_Size]{};
		};
		
		template<typename = void>
		struct is_string{static constexpr bool value = false;};
		
		template<std::size_t _Size>
		struct is_string<fake::detail::view::string<_Size>>{static constexpr bool value = true;};
		
		template<typename _Type>
		constexpr bool is_string_v = is_string<std::remove_cvref_t<_Type>>::value;
		
		template<typename _Type>
		concept string_c = is_string_v<_Type>;
		
	}
	
	template<char... _Chars>
	struct view{
		constexpr view() noexcept{}
		
		static constexpr std::size_t size() noexcept{return sizeof...(_Chars);}
		static constexpr std::size_t length() noexcept{return sizeof...(_Chars);}
		
		template<std::size_t _Index>
		requires (_Index <= sizeof...(_Chars))
		static constexpr char at() noexcept{
			constexpr char local[]{_Chars..., '\0'};
			return local[_Index];
		}
		
		template<std::size_t _Index, std::size_t _Size>
		requires (_Index <= sizeof...(_Chars))
		static constexpr auto substr() noexcept{
			constexpr char local[]{_Chars..., '\0'};
			return []<std::size_t... _I>(std::index_sequence<_I...>){
				return view<local[_Index + _I]...>{};
			}(std::make_index_sequence<std::min(_Size, sizeof...(_Chars) - _Index)>());
		}
		
		static constexpr decltype(auto) data() noexcept{return (buffer);}
		
	private:
		static constexpr char buffer[]{_Chars..., '\0'};
	};
	
	template<char... _Lhs, char... _Rhs>
	constexpr auto operator+(view<_Lhs...>, view<_Rhs...>) noexcept{
		return view<_Lhs..., _Rhs...>{};
	}
	
	template<typename = void>
	struct is_view{static constexpr bool value = false;};
	
	template<char... _Chars>
	struct is_view<fake::view<_Chars...>>{static constexpr bool value = true;};
	
	template<typename _Type>
	constexpr bool is_view_v = is_view<std::remove_cvref_t<_Type>>::value;
	
	template<typename _Type>
	concept view_c = is_view_v<_Type>;
	
	template<fake::detail::view::string _Value>
	requires (_Value.size() > 0)
	constexpr auto make_view() noexcept{
		return []<std::size_t... _Index>(std::index_sequence<_Index...>){
			return fake::view<_Value.template at<_Index>()...>{};
		}(std::make_index_sequence<_Value.size() - 1>{});
	}
	
	template<fake::detail::view::string _Value>
	requires view_c<decltype(make_view<_Value>())>
	constexpr auto view_v = make_view<_Value>();
	
	template<fake::detail::view::string _Value>
	requires view_c<decltype(view_v<_Value>)>
	using view_t = std::remove_cvref_t<decltype(view_v<_Value>)>;
	
	template<auto _integral, std::size_t _base = 10>
	requires fake::to_trait_c<std::is_integral_v<decltype(_integral)>> || fake::constant_c<decltype(_integral)>
	constexpr auto to_view(){
		if constexpr(_integral < 0)
			return view_v<"-"> + to_view<-_integral, _base>();
		else if constexpr(_integral < _base)
			return view<_integral < 10 ? '0' + char(_integral) : 'A' + char(_integral - 10)>{};
		else
			return to_view<_integral / _base, _base>() + to_view<_integral % _base, _base>();
	}
	
	template<auto _integral, std::size_t _base = 10>
	requires requires{{to_view<_integral, _base>()} -> view_c;}
	constexpr auto to_view_v = to_view<_integral, _base>();
	
	constexpr auto type_view(pack_c auto _pack) noexcept{
		constexpr auto symbol = fake::symbol::string_view<fake::take_t<_pack>>();
		return fake::view_v<fake::detail::view::string<symbol.size() + 1>{symbol.data()}>;
	}
	
	constexpr auto type_view(auto _pack) noexcept{
		constexpr auto symbol = fake::symbol::string_view<decltype(_pack)>();
		return fake::view_v<fake::detail::view::string<symbol.size() + 1>{symbol.data()}>;
	}
	
	constexpr auto self_view(auto &&_lambda) noexcept{
		constexpr auto symbol = fake::symbol::self(std::remove_cvref_t<decltype(_lambda)>{});
		return fake::view_v<fake::detail::view::string<symbol.size() + 1>{symbol.data()}>;
	}
	
	namespace literals
	{
		
		template<fake::detail::view::string _Value>
		requires (_Value.size() > 0)
		constexpr auto operator"" _v() noexcept{
			return []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return fake::view<_Value.template at<_Index>()...>{};
			}(std::make_index_sequence<_Value.size() - 1>{});
		}
		
	}
	
}

#endif /*__FAKE_VIEW_H__*/ 
