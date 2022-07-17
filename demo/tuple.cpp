/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include <iostream>

#include "symbol.h"
#include "tuple.h"

namespace tool
{
	
	template<std::size_t _Number> struct leg{};
	template<std::size_t _Number> struct wing{};
	template<typename _Animal> struct egg{constexpr _Animal hatch() const noexcept{return {};}};
	template<typename... _Animal> struct zoo{};
	
}

int main(int _argc, char* _argv[])
{
	struct lion{
		using legs = tool::leg<4>;
		constexpr lion clone() const noexcept{return {};}
	};
	struct crocodile{
		using legs = tool::leg<4>;
		constexpr tool::egg<crocodile> clone() const noexcept{return {};}
	};
	struct kiwi{
		using legs = tool::leg<2>;
		constexpr tool::egg<kiwi> clone() const noexcept{return {};}
	};
	struct owl{
		using legs = tool::leg<2>;
		using wings = tool::wing<2>;
		constexpr tool::egg<owl> clone() const noexcept{return {};}
	};
	
	using animals = std::tuple<lion, crocodile, kiwi, owl>;
	
	constexpr auto get_first_ovipara_index = [](fake::pack_c auto _pack){
		using type = fake::take_t<_pack>;
		return requires(type _e){{_e.clone()} -> std::same_as<tool::egg<type>>;};
	};
	
	std::cout << fake::symbol::string_view<fake::tuple::match_index_t<animals, get_first_ovipara_index>>() << std::endl;
	
	constexpr auto find_fly_bird = [](fake::pack_c auto _pack){
		using type = fake::take_t<_pack>;
		return requires(type _e){{typename type::wings{}} -> std::same_as<tool::wing<2>>;};
	};
	
	using fly_bird = fake::tuple::find_if_t<animals, find_fly_bird>;
	std::cout << fake::symbol::string_view<fly_bird>() << std::endl;
	
	constexpr auto has_bird = [](fake::pack_c auto _pack){
		using type = fake::take_t<_pack>;
		return std::is_same_v<typename type::legs, tool::leg<2>>;
	};
	
	std::cout << fake::symbol::string_view<fake::mezz_t<fake::tuple::contains_v<animals, has_bird>>>() << std::endl;
	
	constexpr auto try_lay = [](fake::pack_c auto _pack){
		using type = fake::take_t<_pack>;
		if constexpr(requires(type _e){{_e.clone()} -> std::same_as<tool::egg<type>>;})
			return fake::pack_v<std::tuple<fake::mezz_t<type{}.clone()>>>;
		else
			return fake::pack_v<std::tuple<>>;
	};
	
	std::cout << fake::symbol::string_view<fake::tuple::scale_t<animals, try_lay>>() << std::endl;
	
	constexpr auto multiply = [](fake::pack_c auto _pack){
		using type = fake::take_t<_pack>;
		return fake::pack_v<fake::mezz_t<type{}.clone()>>;
	};
	
	std::cout << fake::symbol::string_view<fake::tuple::transform_t<animals, multiply>>() << std::endl;
	
	constexpr auto select_quadruped = [](fake::pack_c auto _pack){
		using type = fake::take_t<_pack>;
		return std::is_same_v<typename type::legs, tool::leg<4>>;
	};
	
	std::cout << fake::symbol::string_view<fake::tuple::select_if_t<animals, select_quadruped>>() << std::endl;
	
	constexpr auto erase_mammal = [](fake::pack_c auto _pack){
		using type = fake::take_t<_pack>;
		return requires(type _e){{_e.clone()} -> std::same_as<tool::egg<type>>;} == false;
	};
	
	using non_mammal = fake::tuple::erase_if_t<animals, erase_mammal>;
	std::cout << fake::symbol::string_view<non_mammal>() << std::endl;
	
	using wing_bird_and_non_mammal = fake::tuple::concat_t<std::tuple<fly_bird>, non_mammal>;
	std::cout << fake::symbol::string_view<wing_bird_and_non_mammal>() << std::endl;
	
	std::cout << fake::symbol::string_view<fake::tuple::shrink_t<wing_bird_and_non_mammal>>() << std::endl;
	
	using terre_non_mammal = fake::tuple::erase_t<non_mammal, 2>;
	std::cout << fake::symbol::string_view<terre_non_mammal>() << std::endl;
	
	using terre_animal = fake::tuple::emplace_t<terre_non_mammal, 0, lion>;
	std::cout << fake::symbol::string_view<terre_animal>() << std::endl;
	
	using zoo = tool::zoo<owl, lion, crocodile, kiwi>;
	std::cout << fake::symbol::string_view<fake::tuple::make_t<zoo>>() << std::endl;
	
	return 0;
}
