#ifndef __FAKE_SYMBOL_H__
#define __FAKE_SYMBOL_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include <array>
#include <string_view>

#include "is_valid.h"

namespace fake::symbol
{
	
	struct view;
	
	namespace detail{template<typename _Lambda> constexpr view lambda_view(_Lambda _lambda) noexcept;}
	
	struct view{
		template<std::size_t _Size>
		constexpr view(const char (&_str)[_Size]) noexcept: str{_str}, length{_Size - 1}{}
		constexpr view(const char* _str, std::size_t _size) noexcept: str{_str}, length{_size}{}
		
		constexpr unsigned long long hash() const noexcept{return impl(str, length);}
		constexpr std::string_view to_string_view() const noexcept{return {str, length};}
		
	private:
		template<typename _Type>
		friend constexpr view make_view() noexcept;
		
		template<typename _Lambda>
		friend constexpr view make_self() noexcept;
		
		template<typename _Lambda>
		friend constexpr view detail::lambda_view(_Lambda) noexcept;
		
		template<typename _Type>
		static constexpr view symbol_extractor() noexcept{
			return __PRETTY_FUNCTION__;
		}
		
		static constexpr std::size_t prefix(const char* _svx, const char* _svy, std::size_t _index = 0) noexcept{
			return _svx[_index] == _svy[_index] ? fake::symbol::view::prefix(_svx, _svy, _index + 1) : _index;
		}
		
		static constexpr std::size_t suffix(
			const char* _svx,
			std::size_t _lex,
			const char* _svy,
			std::size_t _ley,
			std::size_t _index = 1
		) noexcept{
			return _lex && _ley && _svx[_lex - _index] == _svy[_ley - _index] ?
				fake::symbol::view::suffix(_svx, _lex, _svy, _ley, _index + 1) : _index - 1;
		}
		
		static constexpr unsigned long long basis = 14695981039346656037ull;
		static constexpr unsigned long long prime = 1099511628211ull;
		
		// FNV-1a 64 bit hash
		static constexpr unsigned long long impl(const char* _str, std::size_t _size) noexcept{
			unsigned long long hash = basis;
			for(std::size_t i = 0; i < _size; i++)
				hash = (hash ^ _str[i]) * prime;
			return hash;
		}
		
		const char* str;
		std::size_t length;
	};
	
	namespace detail
	{
		
		template<typename _Lambda>
		constexpr view lambda_view(_Lambda _lambda) noexcept{
			return fake::symbol::view::symbol_extractor<_Lambda>();
		}
		
		struct foo{static constexpr view name = fake::symbol::detail::lambda_view([]{});};
		struct bar{static constexpr view name = fake::symbol::detail::lambda_view([]{});};
		
	}
	
	template<typename _Type>
	constexpr view make_view() noexcept{
		constexpr view void_v = view::symbol_extractor<void>();
		constexpr view null_v = view::symbol_extractor<char>();
		constexpr std::size_t pre = view::prefix(void_v.str, null_v.str);
		constexpr std::size_t suf = view::suffix(void_v.str, void_v.length, null_v.str, null_v.length);
		constexpr view type_v = view::symbol_extractor<_Type>();
		return fake::symbol::view(type_v.str + pre, type_v.length - pre - suf);
	}
	
	template<typename _Type>
	constexpr std::string_view string_view() noexcept{
		return fake::symbol::make_view<_Type>().to_string_view();
	}
	
	namespace detail
	{
		
		constexpr std::array<std::size_t, 2> fix = []{
			std::string_view x = fake::symbol::string_view<fake::mezz_t<'x'>>();
			std::string_view y = fake::symbol::string_view<fake::mezz_t<'y'>>();
			std::size_t prefix, suffix;
			for(prefix = 0; x[prefix] == y[prefix]; prefix++);
			for(suffix = x.length() - 1; x[suffix] == y[suffix]; suffix--);
			return std::array{prefix - 1, suffix - prefix + 1};
		}();
		
	}
	
	template<auto _value>
	constexpr view make_view() noexcept{
		constexpr std::size_t prefix = detail::fix[0], suffix = detail::fix[1];
		constexpr std::string_view raw = fake::symbol::string_view<fake::mezz_t<_value>>();
		return fake::symbol::view(raw.data() + prefix, raw.size() - prefix - suffix);
	}
	
	template<auto _value>
	constexpr std::string_view string_view() noexcept{
		return fake::symbol::make_view<_value>().to_string_view();
	}
	
	template<typename _Lambda>
	constexpr view make_self() noexcept{
		constexpr view void_v = view::symbol_extractor<void>();
		constexpr view null_v = view::symbol_extractor<char>();
		using foo = detail::foo;
		using bar = detail::bar;
		constexpr std::size_t pre = view::prefix(void_v.str, null_v.str);
		constexpr std::size_t suf = view::suffix(foo::name.str, foo::name.length, bar::name.str, bar::name.length);
		constexpr view lambda_v = view::symbol_extractor<_Lambda>();
		
		return fake::symbol::view(lambda_v.str + pre, lambda_v.length - pre - suf);
	}
	
	template<typename _Lambda>
	constexpr std::string_view self(_Lambda&&) noexcept{
		return fake::symbol::make_self<std::remove_cvref_t<_Lambda>>().to_string_view();
	}
	
}

#endif /*__FAKE_SYMBOL_H__*/ 
