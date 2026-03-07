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
#include <ranges>
#include <charconv>
#include <bit>
#include <tuple>
#include <atomic>
#include <chrono>
#include <stdexcept>

#include "patterns.h"
#include "view.h"

namespace fake
{
	
	namespace detail::utility
	{
		
		struct undefine final{};
		
		template<typename _Type>
		concept cast_c = std::is_integral_v<_Type> || std::is_floating_point_v<_Type>;
		
		template<typename _Type, auto _config>
		concept config_c = std::is_floating_point_v<_Type> && fake::like<decltype(_config), std::chars_format> ||
			std::is_integral_v<_Type> &&
				fake::like<decltype(_config), int> &&
				requires{requires (2 <= _config && _config <= 36);} ||
			fake::like<decltype(_config), undefine>;
		
		template<typename _Type, auto _config, auto _precision>
		concept precision_c = config_c<_Type, _config> && fake::like<decltype(_precision), undefine> || (
				fake::like<decltype(_config), undefine> == false &&
				std::is_floating_point_v<_Type> &&
				fake::like<decltype(_precision), int> &&
				requires{requires (0x0 <= _precision && _precision <= 0x8000);}
			);
		
		template<std::size_t _size>
		inline std::array<char, _size>& format_buffer() noexcept{
			static thread_local std::array<char, _size> buffer{};
			return buffer;
		}
		
		inline consteval bool format_fix(undefine _config) noexcept{
			return false;
		}
		
		inline consteval bool format_fix(std::chars_format _config) noexcept{
			return (_config & std::chars_format::fixed) != std::chars_format{} &&
				(_config & std::chars_format::scientific) == std::chars_format{};
		}
		
		inline constexpr std::string_view trim(std::string_view _e) noexcept{
			constexpr auto is_space = [](char _lhs){
				return ("\t\n\v\f\r " | std::views::drop_while([_lhs](char _rhs){return _lhs != _rhs;})).size();
			};
			
			while(_e.size() && is_space(_e.front()))
				_e.remove_prefix(0x1);
			
			while(_e.size() && is_space(_e.back()))
				_e.remove_suffix(0x1);
			
			return _e;
		}
		
	}
	
	template<detail::utility::cast_c _Type, auto _config = detail::utility::undefine{}>
	inline constexpr std::optional<_Type> cast(std::string_view _e) noexcept
	requires detail::utility::config_c<_Type, _config> {
		constexpr auto config = []{
			if constexpr(fake::like<decltype(_config), detail::utility::undefine>)
				if constexpr(std::is_floating_point_v<_Type>)
					return std::chars_format::general;
				else
					return 10;
			else
				return _config;
		}();
		
		_e = detail::utility::trim(_e);
		
		_Type buffer;
		const std::from_chars_result result = std::from_chars(_e.begin(), _e.end(), buffer, config);
		
		if(result.ec != std::errc{}) [[unlikely]]
			return {};
		
		if(result.ptr != _e.end()) [[unlikely]]
			return {};
		
		return buffer;
	}
	
	template<auto _config = detail::utility::undefine{}, auto _precision = detail::utility::undefine{}>
	inline constexpr std::string cast(detail::utility::cast_c auto _e)
	requires detail::utility::precision_c<decltype(_e), _config, _precision> {
		constexpr std::size_t size = []{
			if constexpr(fake::like<decltype(_precision), detail::utility::undefine> == false)
				return std::max<std::size_t>(0x4000, std::bit_ceil<std::size_t>(_precision << 0x1));
			else if constexpr(detail::utility::format_fix(_config))
				return 0x4000;
			else
				return 0x40;
		}();
		
		auto &buffer = detail::utility::format_buffer<size>();
		
		std::to_chars_result result;
		
		if constexpr(fake::like<decltype(_config), detail::utility::undefine>)
			result = std::to_chars(buffer.begin(), buffer.end(), _e);
		else if constexpr(fake::like<decltype(_precision), detail::utility::undefine>)
			result = std::to_chars(buffer.begin(), buffer.end(), _e, _config);
		else
			result = std::to_chars(buffer.begin(), buffer.end(), _e, _config, _precision);
		
		if(result.ec != std::errc{}) [[unlikely]]
			return {};
		
		return {buffer.begin(), result.ptr};
	}
	
	template<>
	inline constexpr std::optional<char> cast<char>(std::string_view _e) noexcept{
		_e = detail::utility::trim(_e);
		
		if(_e.size() > 0x2) [[unlikely]]
			return {};
		
		if(_e.size() == 0x1 && _e.front() == '\\') [[unlikely]]
			return {};
		
		if(_e.size() == 0x2 && _e.front() != '\\') [[unlikely]]
			return {};
		
		if(const char e = _e.back(); _e.size() == 0x1) [[likely]]
			return e;
		else [[unlikely]]
			switch(e){
				case '0': return '\0';
				case 'b': return '\b';
				case 'e': return '\e';
				case 'f': return '\f';
				case 'r': return '\r';
				case 't': return '\t';
				case 'n': return '\n';
				case 'v': return '\v';
				default: return e;
			}
	}
	
	inline constexpr std::string cast(char _e){
		switch(_e){
			case '\0': return "\\0";
			case '\b': return "\\b";
			case '\e': return "\\e";
			case '\f': return "\\f";
			case '\r': return "\\r";
			case '\t': return "\\t";
			case '\n': return "\\n";
			case '\v': return "\\v";
			case '\\': return "\\\\";
			case '\'': return "\\\'";
			case '\"': return "\\\"";
			default: return {_e};
		}
	}
	
	template<>
	inline constexpr std::optional<char8_t> cast<char8_t>(std::string_view _e) noexcept{
		if(const auto opt = fake::cast<char>(_e)) [[likely]]
			return char8_t(opt.value());
		return {};
	}
	
	inline constexpr std::string cast(char8_t _e){
		return fake::cast(char(_e));
	}
	
	template<>
	inline constexpr std::optional<bool> cast<bool>(std::string_view _e) noexcept{
		_e = detail::utility::trim(_e);
		if(_e == "true")
			return true;
		if(_e == "false")
			return false;
		return {};
	}
	
	inline constexpr std::string cast(bool _e){
		return _e ? "true" : "false";
	}
	
	template<fake::trait_c<std::chrono::duration> _Duration>
	requires std::same_as<_Duration, std::remove_cvref_t<_Duration>>
	inline constexpr std::optional<_Duration> cast(std::string_view _e) noexcept{
		if(const auto rep = fake::cast<typename _Duration::rep>(_e))
			return _Duration{rep.value()};
		return {};
	}
	
	template<fake::trait_c<std::chrono::duration> _Duration>
	requires std::same_as<_Duration, std::remove_cvref_t<_Duration>>
	inline constexpr std::string cast(_Duration _e){
		return fake::cast(_e.count());
	}
	
	template<fake::trait_c<std::chrono::time_point> _TimePoint>
	requires std::same_as<_TimePoint, std::remove_cvref_t<_TimePoint>>
	inline constexpr std::optional<_TimePoint> cast(std::string_view _e) noexcept{
		if(const auto rep = fake::cast<typename _TimePoint::duration::rep>(_e))
			return _TimePoint{typename _TimePoint::duration{rep.value()}};
		return {};
	}
	
	template<fake::trait_c<std::chrono::time_point> _TimePoint>
	requires std::same_as<_TimePoint, std::remove_cvref_t<_TimePoint>>
	inline constexpr std::string cast(_TimePoint _e){
		return fake::cast(_e.time_since_epoch().count());
	}
	
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
				const std::streampos pos = _is.rdbuf()->pubseekoff(0x0, std::ios_base::cur, std::ios_base::in);
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
				
				const auto pos = _is.rdbuf()->pubseekoff(0x0, std::ios_base::cur, std::ios_base::in);
				
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
			
			if constexpr(_front == colors::undefine && _back == colors::undefine)
				return ""_v;
			else
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
			static consteval auto clear() noexcept{
				if constexpr(_front == colors::undefine && _back == colors::undefine)
					return fake::view_v<"">;
				else
					return fake::view_v<"\e[0m">;
			}
			
		private:
			template<std::same_as<std::basic_ostream<char>> _Stream>
			friend _Stream& operator<<(_Stream &_os, const stream &_view){
				using bare = std::remove_cvref_t<type>;
				if constexpr(std::is_integral_v<bare> && std::is_same_v<bare, bool> == false){
					using decay = std::make_unsigned_t<bare>;
					if constexpr(std::same_as<decay, uint8_t> || std::same_as<decay, std::byte>)
						return _os << color<_front, _back>() << int32_t(_view.data) << clear();
					else
						return _os << color<_front, _back>() << _view.data << clear();
				}
				else{
					return _os << color<_front, _back>() << _view.data << clear();
				}
			}
			
			template<std::same_as<std::basic_istream<char>> _Stream>
			friend _Stream& operator>>(_Stream &_is, const stream &_view){
				using bare = std::remove_cvref_t<type>;
				if constexpr(std::is_integral_v<bare> && std::is_same_v<bare, bool> == false){
					using decay = std::make_unsigned_t<bare>;
					if constexpr(std::same_as<decay, uint8_t> || std::same_as<decay, std::byte>){
						int32_t buffer = 0x0;
						_is >> fake::ensure(color<_front, _back>()) >> buffer >> fake::ensure(clear());
						_view.data = buffer;
						return _is;
					}
					else{
						return _is >> fake::ensure(color<_front, _back>()) >> _view.data >> fake::ensure(clear());
					}
				}
				else{
					return _is >> fake::ensure(color<_front, _back>()) >> _view.data >> fake::ensure(clear());
				}
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
		return [_number]<std::size_t... _Index>(std::index_sequence<_Index...>){
			return std::array{
				[_number]{
					constexpr std::size_t index = sizeof...(_Index) - _Index - 1;
					const char bits = (_number >> (index << 2)) & 0xF;
					const char mask = (-!(bits & 8)) | (-((bits & 8) && !(bits & 6)));
					
					return char((mask & (bits | '0')) | (~mask & ((bits - 9) | '@'))); //'@''A''B''C'... 
				}()...
			};
		}(std::make_index_sequence<(sizeof(_Number) << 1)>());
	}
	
	template<fake::std_array_c _Array>
	requires std::same_as<char, typename _Array::value_type>
	constexpr auto from_hex(const _Array &_array) -> fake::unsigned_bytes<(std::tuple_size_v<_Array> >> 0x1)>{
		using result_t = fake::unsigned_bytes<(std::tuple_size_v<_Array> >> 0x1)>;
		return [&_array]<std::size_t... _Index>(std::index_sequence<_Index...>){
			return (
				[](const char _e){
					const result_t mask = result_t(bool(_e & '@')) - 0x1;
					
					return result_t(mask & _e - '0' | ~mask & _e + 9 - '@') << (_Index << 0x2);
				}(_array[sizeof...(_Index) - _Index - 1]) | ...
			);
		}(std::make_index_sequence<std::tuple_size_v<_Array>>());
	}
	
	template<auto _vest>
	inline void once(auto _lambda) noexcept(noexcept(_lambda())){
		static std::atomic<bool> flag = true;
		if(flag.exchange(false, std::memory_order::relaxed))
			_lambda();
	}
	
	namespace detail::signet
	{
		
		inline constexpr std::tuple salt{
			fake::view_v<"MGR">,
			fake::view_v<"YUH">,
			fake::view_v<"DIYUSI">,
			fake::view_v<"NEL">
		};
		
	}
	
	struct signet_t final : std::array<std::size_t, std::tuple_size_v<decltype(detail::signet::salt)>>
	{
		using array_type = std::array<std::size_t, std::tuple_size_v<decltype(detail::signet::salt)>>;
		static constexpr std::size_t size_value = std::tuple_size_v<decltype(detail::signet::salt)>;
	};
	
	consteval signet_t signet(fake::view_c auto _view) noexcept{
		return [&_view]<std::size_t... _index>(std::index_sequence<_index...>){
			using namespace fake::literals;
			return signet_t{(std::get<_index>(detail::signet::salt) + "@"_v + _view).hash()...};
		}(std::make_index_sequence<signet_t::size_value>());
	}
	
	template<std::size_t _size>
	constexpr signet_t signet(const char (&_view)[_size]) noexcept{
		return [&_view]<std::size_t... _index>(std::index_sequence<_index...>){
			using namespace fake::literals;
			return signet_t{tool::view::hash(_view, (std::get<_index>(detail::signet::salt) + "@"_v).hash())...};
		}(std::make_index_sequence<signet_t::size_value>());
	}
	
	constexpr signet_t signet(std::string_view _view) noexcept{
		return [&_view]<std::size_t... _index>(std::index_sequence<_index...>){
			using namespace fake::literals;
			return signet_t{tool::view::hash(_view, (std::get<_index>(detail::signet::salt) + "@"_v).hash())...};
		}(std::make_index_sequence<signet_t::size_value>());
	}
	
	template<fake::view_c auto _view>
	inline constexpr signet_t signet_v = signet(_view);
	
	template<std::invocable<> _Lambda>
	struct scope_guard final{
		[[nodiscard]] scope_guard(_Lambda &&_lambda): lambda(std::forward<_Lambda>(_lambda)){}
		[[nodiscard]] scope_guard(scope_guard&&) = default;
		~scope_guard(){if(intact) lambda();}
		
	private:
		scope_guard(const scope_guard&) = delete;
		scope_guard& operator=(const scope_guard&) = delete;
		scope_guard& operator=(scope_guard&&) = delete;
		
	private:
		fake::patterns::intact<fake::patterns::intact_e::moveonly> intact;
		std::remove_cvref_t<_Lambda> lambda;
	};
	
	namespace detail::global
	{
		
		template<typename _Derive, auto _lambda>
		using base = fake::patterns::registry<_Derive, _lambda>;
		
		template<auto _lambda>
		struct invoke final : base<invoke<_lambda>, _lambda>{};
		
	}
	
	template<auto _lambda>
	inline constexpr fake::detail::global::invoke<_lambda> global;
	
}

template<>
struct std::hash<fake::signet_t>{
	std::size_t operator()(const fake::signet_t &_e) const noexcept{
		return [&_e]<std::size_t... _index>(std::index_sequence<_index...>){
			return (0x0 ^ ... ^ std::rotl(_e[_index], _index));
		}(std::make_index_sequence<fake::signet_t::size_value>());
	}
};

#include "pop.h"

#endif /*__FAKE_UTILITY_H__*/ 
