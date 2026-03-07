#ifndef __FAKE_ENUM_INFO_H__
#define __FAKE_ENUM_INFO_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "push.h"

#include <optional>

#include "view.h"

namespace fake
{
	
	template<auto... _enums>
	requires std::is_enum_v<fake::merge_t<decltype(_enums)...>>
	struct enum_info final{
	private:
		static constexpr std::size_t zero{};
		
		template<std::size_t _size, fake::tuple_c _Enums, fake::tuple_c _Names>
		struct info_t final{
			static constexpr std::size_t size = _size;
			using enums = _Enums;
			using names = _Names;
		};
		
		static consteval auto filter() noexcept{
			static constexpr std::size_t size = sizeof...(_enums);
			using type = fake::merge_t<decltype(_enums)...>;
			using enums = std::tuple<fake::mezz_t<_enums>...>;
			using names = std::tuple<decltype(fake::value_view(fake::mezz_v<_enums>))...>;
			
			static constexpr auto valid = [](fake::mezz_c auto _e){
				static constexpr auto e = _e.value;
				constexpr bool contains = []<std::size_t... _index>(std::index_sequence<_index...>){
					return ((std::tuple_element_t<_index, enums>::value == e) || ...);
				}(std::make_index_sequence<size>());
				
				constexpr std::size_t index = []<std::size_t... _index>(std::index_sequence<_index...>){
					return (((std::size_t(std::tuple_element_t<_index, enums>::value != e) - 1) & _index) | ... | zero);
				}(std::make_index_sequence<size>());
				
				if constexpr(contains == false || size == 0)
					return fake::pack_v<std::tuple<>>;
				else if constexpr(std::tuple_element_t<index, names>::data()[0] != '(')
					return fake::pack_v<std::tuple<fake::mezz_t<e>>>;
				else
					return fake::pack_v<std::tuple<>>;
			};
			
			static constexpr fake::pack_c auto enums_v = []<std::size_t... _index>(std::index_sequence<_index...>){
				return fake::pack_v<decltype(std::tuple_cat(fake::take_t<valid(fake::mezz_v<_enums>)>{}...))>;
			}(std::make_index_sequence<size>());
			
			using enums_t = std::remove_cvref_t<fake::take_t<enums_v>>;
			
			constexpr std::size_t valid_size = std::tuple_size_v<enums_t>;
			
			static constexpr fake::pack_c auto names_v = []<std::size_t... _index>(std::index_sequence<_index...>){
				static constexpr fake::tuple_c auto tuple = enums_t{};
				return fake::pack_v<std::tuple<decltype(fake::value_view(std::get<_index>(tuple)))...>>;
			}(std::make_index_sequence<valid_size>());
			
			using names_t = std::remove_cvref_t<fake::take_t<names_v>>;
			
			return fake::pack_v<info_t<valid_size, enums_t, names_t>>;
		}
		
		using info = fake::take_t<enum_info::filter()>;
		
	public:
		using type = fake::merge_t<decltype(_enums)...>;
		static constexpr std::size_t size = info::size;
		using enums = typename info::enums;
		using names = typename info::names;
		
	private:
		template<auto _projection>
		constexpr std::string_view dynamic(type _enum) const noexcept{
			const bool contains = [_enum]<std::size_t... _index>(std::index_sequence<_index...>){
				return ((std::tuple_element_t<_index, enums>::value == _enum) || ...);
			}(std::make_index_sequence<size>());
			
			const std::size_t index = [_enum]<std::size_t... _index>(std::index_sequence<_index...>){
				return (((std::size_t(std::tuple_element_t<_index, enums>::value != _enum) - 1) & _index) | ... | zero);
			}(std::make_index_sequence<size>());
			
			const auto impl = [index](auto _self, fake::mezz_c auto _index) -> std::string_view {
				if constexpr(_index.value == size)
					return fake::view_v<"">;
				else if(_index.value == index)
					return _projection(std::tuple_element_t<_index.value, names>{}).data();
				else
					return _self(_self, fake::mezz_v<_index.value + 1>);
			};
			
			if(contains && size)
				return impl(impl, fake::mezz_v<std::size_t{}>);
			else
				return fake::view_v<"">;
		}
		
	public:
		template<type _enum>
		consteval auto operator[](fake::value_mezzanine<_enum>) const noexcept{
			constexpr bool contains = []<std::size_t... _index>(std::index_sequence<_index...>){
				return ((std::tuple_element_t<_index, enums>::value == _enum) || ...);
			}(std::make_index_sequence<size>());
			
			constexpr std::size_t index = []<std::size_t... _index>(std::index_sequence<_index...>){
				return (((std::size_t(std::tuple_element_t<_index, enums>::value != _enum) - 1) & _index) | ... | zero);
			}(std::make_index_sequence<size>());
			
			if constexpr(contains && size)
				return std::tuple_element_t<index, names>{};
			else
				return fake::view_v<"">;
		}
		
		constexpr std::string_view operator[](type _enum) const noexcept{
			static constexpr auto projection = [](fake::view_c auto _e){return _e;};
			return enum_info::dynamic<projection>(_enum);
		}
		
		constexpr std::string_view core(type _enum) const noexcept{
			static constexpr auto projection = [](fake::view_c auto _e){
				return _e.template substr<_e.template find_last_of<":">() + 0x1>();
			};
			return enum_info::dynamic<projection>(_enum);
		}
		
		constexpr std::optional<type> core(std::string_view _name) const noexcept{
			return enum_info::core(std::string(_name));
		}
		
		constexpr std::optional<type> core(const std::string &_name) const noexcept{
			static constexpr fake::view_c auto prefix = fake::type_view(fake::pack_v<type>) + fake::view_v<"::">;
			
			return enum_info::operator[](prefix.data() + _name);
		}
		
		template<std::size_t _size>
		constexpr std::optional<type> core(const char (&_name)[_size]) const noexcept{
			static constexpr fake::view_c auto prefix = fake::type_view(fake::pack_v<type>);
			
			return enum_info::core(std::string{_name});
		}
		
		consteval std::optional<type> operator[](fake::view_c auto _name) const noexcept{
			static constexpr fake::view_c auto name = decltype(_name){};
			
			constexpr bool contains = []<std::size_t... _index>(std::index_sequence<_index...>){
				return ((std::tuple_element_t<_index, names>{} == name) || ...);
			}(std::make_index_sequence<size>());
			
			constexpr std::size_t index = []<std::size_t... _index>(std::index_sequence<_index...>){
				return (((std::size_t(std::tuple_element_t<_index, names>{} != name) - 1) & _index) | ... | zero);
			}(std::make_index_sequence<size>());
			
			if constexpr(contains && size)
				return std::tuple_element_t<index, enums>::value;
			else
				return std::nullopt;
		}
		
		constexpr std::optional<type> operator[](std::string_view _name) const noexcept{
			using underlying_t = std::underlying_type_t<type>;
			
			return [_name]<std::size_t... _index>(std::index_sequence<_index...>){
				if(size == 0 || ((std::tuple_element_t<_index, names>::data() == _name) || ...) == false)
					return std::optional<type>{std::nullopt};
				
				return std::optional<type>{
					static_cast<type>(
						(
							(
								std::tuple_element_t<_index, names>::data() == _name ?
									static_cast<underlying_t>(std::tuple_element_t<_index, enums>::value) :
									underlying_t{}
							) | ... | zero
						)
					)
				};
			}(std::make_index_sequence<size>());
		}
	};
	
	template<auto... _enums>
	requires std::is_enum_v<fake::merge_t<decltype(_enums)...>>
	inline constexpr enum_info<_enums...> enum_info_v;
	
	namespace detail::enum_info
	{
		
		template<typename _EnumType, std::underlying_type_t<_EnumType>... _index>
		consteval auto make(std::integer_sequence<std::underlying_type_t<_EnumType>, _index...>) noexcept{
			return fake::enum_info<static_cast<_EnumType>(_index)...>{};
		}
		
	}
	
	template<auto _enum>
	requires std::is_enum_v<decltype(_enum)>
	constexpr fake::enum_info enum_info_for_v = fake::detail::enum_info::make<decltype(_enum)>(
		std::make_integer_sequence<
			std::underlying_type_t<decltype(_enum)>,
			static_cast<std::underlying_type_t<decltype(_enum)>>(_enum) + 1
		>()
	);
	
	template<auto _enum>
	requires std::is_enum_v<decltype(_enum)>
	using enum_info_for = std::remove_cvref_t<decltype(enum_info_for_v<_enum>)>;
	
}

#include "pop.h"

#endif /*__FAKE_ENUM_INFO_H__*/ 
