#include <iostream>
#include <vector>
#include <variant>

#include "symbol.h"
#include "tuple.h"

template<typename>
struct is_template : std::false_type{};

template<template<typename...> typename _Temp, typename... _Paras>
struct is_template<_Temp<_Paras...>> : std::true_type{};

template<typename _Type>
concept is_template_v = is_template<_Type>::value;

template<typename>
struct rebind{static_assert(!!sizeof(rebind*) == 0, "fake::rebind: 'type' must be a 'template<typename...>' type.");};

template<template<typename...> typename _Temp, typename... _Paras>
struct rebind<_Temp<_Paras...>>
{
	template<typename... _Args>
	using type = _Temp<_Args...>;
};

template<typename _Type, typename... _Args>
using rebind_t = typename rebind<_Type>::template type<_Args...>;

template<typename, template<typename...> typename>
struct mimic{static_assert(!!sizeof(mimic*) == 0, "fake::mimic: 'origin' must be a 'template<typename...>' type.");};

template<template<typename...> typename _Temp, typename... _Paras, template<typename...> typename _Target>
struct mimic<_Temp<_Paras...>, _Target>
{
	using type = _Target<_Paras...>;
};

template<typename _Origin, template<typename...> typename _Target>
using mimic_t = typename mimic<_Origin, _Target>::type;

template<typename _Type, template<typename...> typename _Something>
concept is_something_c = std::same_as<mimic_t<_Type, _Something>, _Type>;

template<typename _Type>
concept is_vector_c = is_template_v<_Type> && is_something_c<_Type, std::vector>;

int main(int _argc, char* _argv[])
{
	using mimiced = mimic_t<std::tuple<std::monostate, int, char>, std::variant>;
	std::cout << fake::symbol::string_view<mimiced>() << std::endl;
	
	using bound = rebind_t<std::variant<std::monostate, int, char>, float, double>;
	std::cout << fake::symbol::string_view<bound>() << std::endl;
	
	using args = mimic_t<std::variant<std::monostate, int, char>, std::tuple>;
	std::cout << fake::symbol::string_view<args>() << std::endl;
	
	using concat = fake::tuple::emplace_back_t<args, float>;
	std::cout << fake::symbol::string_view<concat>() << std::endl;
	
	using variant = mimic_t<concat, std::variant>;
	std::cout << fake::symbol::string_view<variant>() << std::endl;
	
	constexpr fake::pack_c auto pack = []<typename... _Args>(fake::type_package<std::variant<_Args...>>){
		return fake::pack_v<std::variant<_Args..., double>>;
	}(fake::pack_v<std::variant<std::monostate, int, char>>);
	std::cout << fake::symbol::string_view<fake::take_t<pack>>() << std::endl;
	
	static_assert(is_vector_c<std::vector<short>>);
	static_assert(!is_vector_c<int>);
	static_assert(!is_vector_c<std::tuple<int, float>>);
	
	return 0;
}
