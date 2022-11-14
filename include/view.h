#ifndef __FAKE_VIEW_H__
#define __FAKE_VIEW_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include <type_traits>
#include <algorithm>
#include <ostream>

#include "traits.h"
#include "symbol.h"

namespace fake
{
	
	namespace detail::view
	{
		
		template<std::size_t _Size>
		struct string{
			constexpr string(const char (&_str)[_Size]) noexcept{std::ranges::copy(_str, buffer);}
			explicit constexpr string(const char* _str) noexcept{std::ranges::copy_n(_str, _Size, buffer);}
			
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
		
		static constexpr std::size_t npos = std::string_view::npos;
		
		template<std::size_t _Index>
		requires (_Index <= sizeof...(_Chars))
		static constexpr char at() noexcept{
			constexpr char local[]{_Chars..., '\0'};
			return local[_Index];
		}
		
		static constexpr char front() noexcept{
			return at<0>();
		}
		
		static constexpr char back() noexcept{
			return at<sizeof...(_Chars) + !sizeof...(_Chars) - 1>();
		}
		
		static constexpr bool empty() noexcept{
			return sizeof...(_Chars) == 0;
		}
		
	private:
		template<fake::detail::view::string _Value>
		requires (_Value.size() > 0)
		static constexpr auto make_view() noexcept{
			return []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return fake::view<_Value.template at<_Index>()...>{};
			}(std::make_index_sequence<_Value.size() - 1>{});
		}
		
	public:
		template<fake::view _Value>
		static constexpr bool starts_with() noexcept{
			constexpr auto prefix_v = _Value;
			constexpr std::string_view self{data()};
			constexpr std::string_view prefix{prefix_v.data()};
			
			return self.starts_with(prefix);
		}
		
		template<fake::detail::view::string _Value>
		requires requires{make_view<_Value>();}
		static constexpr bool starts_with() noexcept{
			return starts_with<make_view<_Value>()>();
		}
		
		template<fake::view _Value>
		static constexpr bool ends_with() noexcept{
			constexpr auto suffix_v = _Value;
			constexpr std::string_view self{data()};
			constexpr std::string_view suffix{suffix_v.data()};
			
			return self.ends_with(suffix);
		}
		
		template<fake::detail::view::string _Value>
		requires requires{make_view<_Value>();}
		static constexpr bool ends_with() noexcept{
			return ends_with<make_view<_Value>()>();
		}
		
		template<fake::view _Value>
		static constexpr bool contains() noexcept{
			constexpr auto element_v = _Value;
			constexpr std::string_view self{data()};
			constexpr std::string_view element{element_v.data()};
			
			return self.find(element) != npos;
		}
		
		template<fake::detail::view::string _Value>
		requires requires{make_view<_Value>();}
		static constexpr bool contains() noexcept{
			return contains<make_view<_Value>()>();
		}
		
		template<fake::view _Value, std::size_t _Index = 0>
		static constexpr std::size_t find() noexcept{
			constexpr auto element_v = _Value;
			constexpr std::string_view self{data()};
			constexpr std::string_view element{element_v.data()};
			
			return self.find(element, _Index);
		}
		
		template<fake::detail::view::string _Value, std::size_t _Index = 0>
		requires requires{make_view<_Value>();}
		static constexpr std::size_t find() noexcept{
			return find<make_view<_Value>(), _Index>();
		}
		
		template<fake::view _Value, std::size_t _Index = npos>
		static constexpr std::size_t rfind() noexcept{
			constexpr auto element_v = _Value;
			constexpr std::string_view self{data()};
			constexpr std::string_view element{element_v.data()};
			
			return self.rfind(element, _Index);
		}
		
		template<fake::detail::view::string _Value, std::size_t _Index = npos>
		requires requires{make_view<_Value>();}
		static constexpr std::size_t rfind() noexcept{
			return rfind<make_view<_Value>(), _Index>();
		}
		
		template<fake::view _Value, std::size_t _Index = 0>
		static constexpr std::size_t find_first_of() noexcept{
			constexpr auto element_v = _Value;
			constexpr std::string_view self{data()};
			constexpr std::string_view element{element_v.data()};
			
			return self.find_first_of(element, _Index);
		}
		
		template<fake::detail::view::string _Value, std::size_t _Index = 0>
		requires requires{make_view<_Value>();}
		static constexpr std::size_t find_first_of() noexcept{
			return find_first_of<make_view<_Value>(), _Index>();
		}
		
		template<fake::view _Value, std::size_t _Index = npos>
		static constexpr std::size_t find_last_of() noexcept{
			constexpr auto element_v = _Value;
			constexpr std::string_view self{data()};
			constexpr std::string_view element{element_v.data()};
			
			return self.find_last_of(element, _Index);
		}
		
		template<fake::detail::view::string _Value, std::size_t _Index = npos>
		requires requires{make_view<_Value>();}
		static constexpr std::size_t find_last_of() noexcept{
			return find_last_of<make_view<_Value>(), _Index>();
		}
		
		template<fake::view _Value, std::size_t _Index = 0>
		static constexpr std::size_t find_first_not_of() noexcept{
			constexpr auto element_v = _Value;
			constexpr std::string_view self{data()};
			constexpr std::string_view element{element_v.data()};
			
			return self.find_first_not_of(element, _Index);
		}
		
		template<fake::detail::view::string _Value, std::size_t _Index = 0>
		requires requires{make_view<_Value>();}
		static constexpr std::size_t find_first_not_of() noexcept{
			return find_first_not_of<make_view<_Value>(), _Index>();
		}
		
		template<fake::view _Value, std::size_t _Index = npos>
		static constexpr std::size_t find_last_not_of() noexcept{
			constexpr auto element_v = _Value;
			constexpr std::string_view self{data()};
			constexpr std::string_view element{element_v.data()};
			
			return self.find_last_not_of(element, _Index);
		}
		
		template<fake::detail::view::string _Value, std::size_t _Index = npos>
		requires requires{make_view<_Value>();}
		static constexpr std::size_t find_last_not_of() noexcept{
			return find_last_not_of<make_view<_Value>(), _Index>();
		}
		
		template<std::size_t _Index, std::size_t _Size = npos>
		requires (_Index <= sizeof...(_Chars))
		static constexpr auto substr() noexcept{
			constexpr char local[]{_Chars..., '\0'};
			return []<std::size_t... _I>(std::index_sequence<_I...>){
				return view<local[_Index + _I]...>{};
			}(std::make_index_sequence<std::min(_Size, sizeof...(_Chars) - _Index)>());
		}
		
	private:
		template<auto>
		struct adapt;
		
		template<fake::view _View>
		struct adapt<_View> final{static constexpr auto value = _View;};
		
		template<fake::detail::view::string _Value>
		struct adapt<_Value> final{static constexpr auto value = make_view<_Value>();};
		
		template<auto _arg>
		static constexpr auto adapt_v = adapt<_arg>::value;
		
		template<char...>
		friend struct fake::view;
		
		template<char... _Rhs>
		static constexpr auto concat(fake::view<_Rhs...>) noexcept{
			return fake::view<_Chars..., _Rhs...>{};
		}
		
	public:
		template<fake::view _What, fake::view _With, std::size_t _Index = 0>
		static constexpr auto replace() noexcept{
			constexpr std::size_t index = find<_What, _Index>();
			if constexpr(index == npos){
				return view{};
			}
			else{
				constexpr fake::view prefix = substr<0, index>();
				constexpr fake::view suffix = substr<index + _What.size()>();
				return prefix.concat(_With).concat(suffix.template replace<_What, _With>());
			}
		}
		
		template<fake::detail::view::string _What, fake::detail::view::string _With, std::size_t _Index = 0>
		requires requires{adapt_v<_What>; adapt_v<_With>;}
		static constexpr auto replace() noexcept{
			return replace<adapt_v<_What>, adapt_v<_With>, _Index>();
		}
		
		template<fake::view _What, fake::detail::view::string _With, std::size_t _Index = 0>
		requires requires{adapt_v<_What>; adapt_v<_With>;}
		static constexpr auto replace() noexcept{
			return replace<_What, adapt_v<_With>, _Index>();
		}
		
		template<fake::detail::view::string _What, fake::view _With, std::size_t _Index = 0>
		requires requires{adapt_v<_What>; adapt_v<_With>;}
		static constexpr auto replace() noexcept{
			return replace<adapt_v<_What>, _With, _Index>();
		}
		
		template<fake::view _Value>
		static constexpr auto append() noexcept{
			return concat(_Value);
		}
		
		template<fake::detail::view::string _Value>
		requires requires{make_view<_Value>();}
		static constexpr auto append() noexcept{
			return append<make_view<_Value>()>();
		}
		
		static constexpr decltype(auto) data() noexcept{return (buffer);}
		
		static constexpr decltype(auto) c_str() noexcept{return (buffer);}
		
	private:
		static constexpr char buffer[]{_Chars..., '\0'};
		
	private:
		friend std::basic_ostream<char>& operator<<(std::basic_ostream<char> &_os, view _view){
			return _os << _view.data();
		}
	};
	
	template<char... _Lhs, char... _Rhs>
	constexpr auto operator+(view<_Lhs...>, view<_Rhs...>) noexcept{
		return view<_Lhs..., _Rhs...>{};
	}
	
	template<char... _Lhs, char... _Rhs>
	constexpr bool operator==(view<_Lhs...> _lhs, view<_Rhs...> _rhs) noexcept{
		constexpr std::string_view lhs{_lhs.data()};
		constexpr std::string_view rhs{_rhs.data()};
		
		return lhs == rhs;
	}
	
	template<char... _Lhs, char... _Rhs>
	constexpr bool operator!=(view<_Lhs...> _lhs, view<_Rhs...> _rhs) noexcept{
		constexpr std::string_view lhs{_lhs.data()};
		constexpr std::string_view rhs{_rhs.data()};
		
		return lhs != rhs;
	}
	
	template<char... _Lhs, char... _Rhs>
	constexpr auto operator<=>(view<_Lhs...> _lhs, view<_Rhs...> _rhs) noexcept{
		constexpr std::string_view lhs{_lhs.data()};
		constexpr std::string_view rhs{_rhs.data()};
		
		return lhs <=> rhs;
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
	
	constexpr auto type_view(auto _type) noexcept{
		constexpr auto symbol = fake::symbol::string_view<decltype(_type)>();
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
