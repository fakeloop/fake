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

#include "push.h"

#include <utility>
#include <stdexcept>

#include "view.h"

namespace fake
{
	
	namespace exception
	{
		
		struct mismatch : std::exception
		{
			mismatch(const char* _msg, std::size_t _line, std::size_t _row) noexcept:
				msg(_msg),
				line(_line),
				row(_row)
			{}
			
			inline const char* what() const noexcept override{return msg;};
			
		public:
			template<std::same_as<std::basic_istream<char>> _Stream>
			inline static mismatch make(fake::view_c auto _message, _Stream &_is){
				const std::streampos pos = _is.tellg();
				_is.seekg(0);
				std::size_t line = 1, row = 1;
				for(std::streampos i = 0; i < pos; i += 1)
					if(_is.get() == '\n')
						line++, row = 1;
					else
						row++;
				_is.seekg(pos);
				
				return {_message.data(), line, row};
			}
			
		public:
			const std::size_t line;
			const std::size_t row;
			
		private:
			const char* const msg;
		};
		
	}
	
	namespace detail::utility
	{
		
		template<fake::view_c _View, typename _Skip = std::nullptr_t>
		struct view_matcher{
			consteval view_matcher(_View, _Skip) noexcept{}
			consteval view_matcher(_View) noexcept{}
			
		public:
			template<std::same_as<std::basic_istream<char>> _Stream>
			static bool try_match(_Stream &_is){
				constexpr std::string_view sv{_View::data()};
				
				if(_is.good() == false)
					return false;
				
				const auto pos = _is.tellg();
				
				constexpr std::string_view space{"\t\n\v\f\r "};
				while(_is.good() && space.find_first_of(_is.peek()) != std::string_view::npos)
					_is.get();
				
				if constexpr(requires{_Skip::try_match(_is);}){
					if(_Skip::try_match(_is) == false)
						return _is.clear(), _is.seekg(pos), false;
					_is >> _Skip{};
				}
				
				for(const char e : sv)
					if(_is.good() == false || _is.get() != e)
						return _is.clear(), _is.seekg(pos), false;
				return _is.clear(), _is.seekg(pos), true;
			}
			
		private:
			template<std::same_as<std::basic_istream<char>> _Stream>
			friend _Stream& operator>>(_Stream &_is, const view_matcher &_matcher){
				using namespace fake::literals;
				constexpr std::string_view sv{_View::data()};
				
				constexpr std::string_view space{"\t\n\v\f\r "};
				while(_is.good() && space.find_first_of(_is.peek()) != std::string_view::npos)
					_is.get();
				
				if constexpr(requires{_is >> _Skip{};})
					_is >> _Skip{};
				
				constexpr fake::view_c auto error_message = "match failed: ["_v + _View{} + "]"_v;
				
				for(const char e : sv){
					if(_is.good() == false || _is.get() != e){
						const auto exception = fake::exception::mismatch::make(error_message, _is);
						_is.unget();
						
						throw exception;
					}
				}
				
				while(_is.good() && space.find_first_of(_is.peek()) != std::string_view::npos)
					_is.get();
				
				return _is;
			}
		};
		
		template<>
		struct view_matcher<fake::view<>>{
			consteval view_matcher() noexcept{}
			
		public:
			template<std::same_as<std::basic_istream<char>> _Stream>
			static bool try_match(_Stream &_is){return true;}
			
		private:
			template<std::same_as<std::basic_istream<char>> _Stream>
			friend _Stream& operator>>(_Stream &_is, view_matcher _matcher) noexcept{
				constexpr std::string_view space{"\t\n\v\f\r "};
				while(_is.good() && space.find_first_of(_is.peek()) != std::string_view::npos)
					_is.get();
				
				return _is;
			}
		};
		
	}
	
	template<fake::view_c _View>
	requires (_View::template find_first_not_of<"\t\n\v\f\r ">() != std::string_view::npos)
	consteval auto ensure(_View) noexcept{
		constexpr fake::view_c auto view = _View{};
		constexpr std::size_t offset = view.template find_first_not_of<"\t\n\v\f\r ">();
		constexpr std::size_t length = view.template find_last_not_of<"\t\n\v\f\r ">() + 0x1 - offset;
		return detail::utility::view_matcher{view.template substr<offset, length>()};
	}
	
	template<fake::view_c _View>
	requires (_View::template find_first_not_of<"\t\n\v\f\r ">() != std::string_view::npos)
	consteval auto ensure(_View, auto _skip) noexcept{
		constexpr fake::view_c auto view = _View{};
		constexpr std::size_t offset = view.template find_first_not_of<"\t\n\v\f\r ">();
		constexpr std::size_t length = view.template find_last_not_of<"\t\n\v\f\r ">() + 0x1 - offset;
		return detail::utility::view_matcher{view.template substr<offset, length>(), _skip};
	}
	
	template<fake::view_c _View>
	requires (_View::template find_first_not_of<"\t\n\v\f\r ">() == std::string_view::npos)
	consteval auto ensure(_View) noexcept{
		return detail::utility::view_matcher<fake::view<>>{};
	}
	
	enum struct colors : std::size_t{black, red, green, yellow, blue, magenta, cyan, white, undefine};
	
	namespace detail::utility
	{
		
		template<colors _front, colors _back = colors::undefine>
		consteval auto color() noexcept{
			using namespace fake::literals;
			
			constexpr bool f = _front != colors::undefine;
			constexpr bool b = _back != colors::undefine;
			constexpr char fc = '0' + static_cast<std::underlying_type_t<colors>>(_front);
			constexpr char bc = '0' + static_cast<std::underlying_type_t<colors>>(_back);
			constexpr fake::view_c auto fv = "3"_v + fake::view<fc>{};
			constexpr fake::view_c auto bv = "4"_v + fake::view<bc>{};
			
			constexpr fake::view_c auto front = std::conditional_t<f, decltype(fv), fake::view_t<"">>{};
			constexpr fake::view_c auto semi = std::conditional_t<f && b, fake::view_t<";">, fake::view_t<"">>{};
			constexpr fake::view_c auto back = std::conditional_t<b, decltype(bv), fake::view_t<"">>{};
			constexpr fake::view_c auto reset = std::conditional_t<f || b, fake::view_t<"">, fake::view_t<"0">>{};
			
			return "\e["_v + front + semi + back + reset + "m"_v;
		}
		
		consteval auto clear() noexcept{return fake::view_v<"\e[0m">;}
		
		template<typename _Type, colors _front, colors _back>
		struct stream final
		{
			using type = std::conditional_t<std::is_rvalue_reference_v<_Type>, std::remove_reference_t<_Type>, _Type>;
			
			constexpr stream(_Type &&_data): data(std::forward<_Type>(_data)){}
			stream(const stream&) = delete;
			stream& operator=(const stream&) = delete;
			
		private:
			template<std::same_as<std::basic_ostream<char>> _Stream>
			friend _Stream& operator<<(_Stream &_os, const stream &_view){
				return _os << color<_front, _back>() << _view.data << clear();
			}
			
			template<std::same_as<std::basic_istream<char>> _Stream>
			friend _Stream& operator>>(_Stream &_is, const stream &_view){
				return _is >> fake::ensure(color<_front, _back>()) >> _view.data >> fake::ensure(clear());
			}
			
		private:
			type data;
		};
		
	}
	
	template<colors _front, colors _back = colors::undefine>
	constexpr auto color(const fake::view_c auto &_view) noexcept{
		return detail::utility::color<_front, _back>() + _view + detail::utility::clear();
	}
	
	template<colors _front, colors _back = colors::undefine>
	constexpr auto color(auto &&_data) requires (fake::view_c<decltype(_data)> == false){
		return detail::utility::stream<decltype(_data), _front, _back>{std::forward<decltype(_data)>(_data)};
	}
	
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

#include "pop.h"

#endif /*__FAKE_UTILITY_H__*/ 
