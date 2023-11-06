#ifndef __FAKE_TRAITS_H__
#define __FAKE_TRAITS_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "push.h"

#include <cstdint>
#include <type_traits>
#include <array>
#include <tuple>
#include <optional>
#include <variant>
#include <concepts>

#include "is_valid.h"

namespace fake
{
	
	template<typename _T>
	constexpr auto echo_v = fake::take_v<fake::pack_v<_T>>;
	
	struct debug final{};
	
	template<typename _Type>
	concept debug_c = std::same_as<fake::debug, std::remove_cvref_t<_Type>>;
	
	template<typename = void, typename = void>
	struct aligned_int{};
	
	template<typename _T>
	struct aligned_int<_T, std::enable_if_t<sizeof(void*) == sizeof(int), _T>>
	{
		using signed_type = int;
		using unsigned_type = unsigned int;
	};
	
	template<typename _T>
	struct aligned_int<_T, std::enable_if_t<sizeof(void*) == sizeof(long long), _T>>
	{
		using signed_type = long long;
		using unsigned_type = unsigned long long;
	};
	
	template<typename = void>
	using aligned_signed = typename aligned_int<void>::signed_type;
	
	template<typename = void>
	using aligned_unsigned = typename aligned_int<void>::unsigned_type;
	
	template<typename _T = void*>
	constexpr std::size_t system_bit = sizeof(_T) << 3;
	
	template<typename _Type>
	using remove_lvalue_reference_t = std::conditional_t<
		std::is_lvalue_reference_v<_Type>,
		std::remove_reference_t<_Type>,
		_Type
	>;
	
	template<typename _Type>
	using remove_rvalue_reference_t = std::conditional_t<
		std::is_rvalue_reference_v<_Type>,
		std::remove_reference_t<_Type>,
		_Type
	>;
	
	template<std::size_t _Size>
	struct char_array{
		static_assert((_Size & _Size - 1) == 0);
		static constexpr std::size_t size = _Size;
		char data[_Size];
	};
	
	template<typename = void>
	struct is_char_array
	{
		static constexpr bool value = false;
	};
	
	template<std::size_t _Size>
	struct is_char_array<char_array<_Size>>
	{
		static constexpr bool value = true;
	};
	
	template<typename _T>
	constexpr bool is_char_array_v = is_char_array<std::remove_cvref_t<_T>>::value;
	
	template<typename _Type>
	concept char_array_c = is_char_array_v<_Type>;
	
	template<typename _Type>
	requires (std::is_integral_v<_Type> || is_char_array_v<_Type>)
	constexpr auto byteswap(_Type _e){
		using type_t = std::remove_cvref_t<decltype(_e)>;
		if constexpr(sizeof(type_t) == 1){
			return _e;
		}
		else if constexpr(std::is_integral_v<type_t>){
			constexpr std::size_t half_v = sizeof(_e) >> 1;
			constexpr type_t mask = 0xFF;
			return [&]<std::size_t... _index>(std::index_sequence<_index...>) -> type_t
			{
				return (((_e & mask << (_index << 3)) << ((half_v - _index << 1) - 1 << 3)) | ...) |
					(((_e & mask << (half_v + _index << 3)) >> ((_index << 1) + 1 << 3)) | ...);
			}(std::make_index_sequence<half_v>());
		}
		else{
			constexpr std::size_t size_v = type_t::size;
			return [&]<std::size_t... _index>(std::index_sequence<_index...>) -> type_t
			{
				return {_e.data[size_v - _index - 1]...};
			}(std::make_index_sequence<size_v>());
		}
	};
	
	template<typename = void>
	struct is_constant
	{
		static constexpr bool value = false;
	};
	
	template<typename _Type, _Type _value>
	struct is_constant<std::integral_constant<_Type, _value>>
	{
		static constexpr bool value = true;
	};
	
	template<typename _T>
	constexpr bool is_constant_v = is_constant<std::remove_cvref_t<_T>>::value;
	
	template<typename _Type>
	concept constant_c = is_constant_v<_Type>;
	
	template<auto _Constant>
	requires (
		is_constant_v<decltype(_Constant)> &&
		std::is_signed_v<typename std::remove_cvref_t<decltype(_Constant)>::value_type>
	)
	constexpr auto nega_v = std::integral_constant<
		typename std::remove_cvref_t<decltype(_Constant)>::value_type,
		-std::remove_cvref_t<decltype(_Constant)>::value
	>{};
	
	namespace detail::traits{template<bool...> struct bool_package;}
	
	template<bool... _args>
	constexpr bool conjunction_v = std::is_same_v<
		detail::traits::bool_package<true, _args...>,
		detail::traits::bool_package<_args..., true>
	>;
	
	template<typename... _Types>
	struct merge;
	
	template<>
	struct merge<> final{};
	
	template<typename _Head, std::same_as<_Head>... _Types>
	struct merge<_Head, _Types...> final{
		using type = _Head;
	};
	
	template<typename... _Types>
	using merge_t = typename merge<_Types...>::type;
	
	template<typename... _Types>
	struct common_base;
	
	template<typename _Type>
	struct common_base<_Type> final{
		using type = _Type;
	};
	
	template<typename _Lhs, std::derived_from<_Lhs> _Rhs, typename... _Types>
	requires requires{requires std::same_as<_Lhs, _Rhs> == false; typename common_base<_Rhs, _Types...>::type;}
	struct common_base<_Lhs, _Rhs, _Types...> final{
		using type = typename common_base<_Lhs, _Types...>::type;
	};
	
	template<typename _Lhs, typename _Rhs, typename... _Types>
	requires std::derived_from<_Lhs, _Rhs> && requires{typename common_base<_Lhs, _Types...>::type;}
	struct common_base<_Lhs, _Rhs, _Types...> final{
		using type = typename common_base<_Rhs, _Types...>::type;
	};
	
	template<typename... _Types>
	using common_base_t = typename common_base<_Types...>::type;
	
	template<typename... _Types>
	struct common_derive;
	
	template<typename _Type>
	struct common_derive<_Type> final{
		using type = _Type;
	};
	
	template<typename _Lhs, std::derived_from<_Lhs> _Rhs, typename... _Types>
	requires requires{requires std::same_as<_Lhs, _Rhs> == false; typename common_derive<_Rhs, _Types...>::type;}
	struct common_derive<_Lhs, _Rhs, _Types...> final{
		using type = typename common_derive<_Rhs, _Types...>::type;
	};
	
	template<typename _Lhs, typename _Rhs, typename... _Types>
	requires std::derived_from<_Lhs, _Rhs> && requires{typename common_derive<_Lhs, _Types...>::type;}
	struct common_derive<_Lhs, _Rhs, _Types...> final{
		using type = typename common_derive<_Lhs, _Types...>::type;
	};
	
	template<typename... _Types>
	using common_derive_t = typename common_derive<_Types...>::type;
	
	template<typename... _Lambda>
	struct branch : public _Lambda...
	{
		using _Lambda::operator()...;
	};
	
	template<typename... _Lambda>
	branch(_Lambda...) -> branch<_Lambda...>;
	
	template<typename _Instance, typename... _Lambda>
	constexpr auto visit(_Instance &&_instance, _Lambda&& ..._lambda){
		return std::visit(branch{std::forward<_Lambda>(_lambda)...}, std::forward<_Instance>(_instance));
	}
	
	template<auto... _mezz>
	requires (fake::mezz_c<decltype(_mezz)> && ...)
	inline constexpr auto filter = [](auto &&..._args) consteval{
		struct invalid final{};
		constexpr auto selector = fake::branch{[](...) consteval{return invalid{};}, _mezz.value...};
		return requires{{selector(std::forward<decltype(_args)>(_args)...)} -> std::same_as<invalid>;} == false;
	};
	
	template<typename = void>
	struct is_pair
	{
		static constexpr bool value = false;
	};
	
	template<typename _First, typename _Second>
	struct is_pair<std::pair<_First, _Second>>
	{
		static constexpr bool value = true;
	};
	
	template<typename _T>
	constexpr bool is_pair_v = is_pair<std::remove_cvref_t<_T>>::value;
	
	template<typename _Type>
	concept pair_c = is_pair_v<_Type>;
	
	template<typename = void>
	struct is_tuple
	{
		static constexpr bool value = false;
	};
	
	template<typename... _Args>
	struct is_tuple<std::tuple<_Args...>>
	{
		static constexpr bool value = true;
	};
	
	template<typename _T>
	constexpr bool is_tuple_v = is_tuple<std::remove_cvref_t<_T>>::value;
	
	template<typename _Type>
	concept tuple_c = is_tuple_v<_Type>;
	
	template<typename _T>
	constexpr bool is_combine_v = is_pair_v<_T> || is_tuple_v<_T>;
	
	template<typename _Type>
	concept combine_c = is_combine_v<_Type>;
	
	template<typename _T>
	constexpr bool is_aggregate_v = std::is_aggregate_v<std::remove_cvref_t<_T>>;
	
	template<typename _Type>
	concept aggregate_c = is_aggregate_v<_Type>;
	
	template<typename _Array>
	struct empty_array final
	{
		template<typename _Type> static constexpr auto match(const _Type(&)[0]){return std::true_type{};}
		static constexpr auto match(...){return std::false_type{};}
		static constexpr auto impl(){
			if constexpr(std::is_same_v<_Array, void>)
				return false;
			else
				return decltype(match(std::declval<_Array>()))::value;
		}
		
		static constexpr bool value = impl();
	};
	
	template<typename _T>
	constexpr bool is_array_v = std::is_array_v<std::remove_cvref_t<_T>> || empty_array<std::remove_cvref_t<_T>>::value;
	
	template<typename _Type>
	concept array_c = is_array_v<_Type>;
	
	template<array_c _Array>
	struct array_value final{
	private:
		using ref_t = std::add_lvalue_reference_t<std::remove_cvref_t<_Array>>;
		
		template<typename _Type, std::size_t _Size>
		static const auto impl(_Type (&)[_Size]){return fake::pack_v<_Type>;}
		
		template<typename _Type>
		static const auto impl(_Type (&)[0]){return fake::pack_v<_Type>;}
		
		template<typename _Type>
		static const auto impl(_Type (&)[]){return fake::pack_v<_Type>;}
		
	public:
		using type = fake::take_t<decltype(impl(std::declval<ref_t>())){}>;
	};
	
	template<array_c _Array>
	using array_value_t = typename array_value<_Array>::type;
	
	template<array_c _Array>
	struct array_size final{
	private:
		using ref_t = std::add_lvalue_reference_t<std::remove_cvref_t<_Array>>;
		
		template<typename _Type, std::size_t _Size>
		static const auto deduce(_Type (&)[_Size]){return fake::index_v<_Size>;}
		
		template<typename _Type>
		static consteval std::size_t impl(){
			if constexpr(empty_array<std::remove_cvref_t<_Type>>::value)
				return 0;
			else
				return decltype(deduce(std::declval<ref_t>()))::value;
		}
		
	public:
		static constexpr std::size_t value = impl<_Array>();
	};
	
	template<array_c _Array>
	constexpr std::size_t array_size_v = array_size<_Array>::value;
	
	template<typename>
	struct is_std_array final : std::false_type{};
	
	template<template<typename, std::size_t> typename _Template, typename _Type, std::size_t _size>
	requires std::same_as<_Template<_Type, _size>, std::array<_Type, _size>>
	struct is_std_array<_Template<_Type, _size>> final : std::true_type{};
	
	template<typename _Type>
	constexpr bool is_std_array_v = is_std_array<std::remove_cvref_t<_Type>>::value;
	
	template<typename _Type>
	concept std_array_c = is_std_array_v<_Type>;
	
	template<typename _T>
	constexpr bool is_container()
	{
		return requires(_T _e){
			{_e.begin()} -> std::same_as<typename _T::iterator>;
			{_e.end()} -> std::same_as<typename _T::iterator>;
			{_e.size()} -> std::same_as<std::size_t>;
			typename _T::value_type;
		};
	}
	
	template<typename _T>
	constexpr bool is_container_v = is_container<std::remove_cvref_t<_T>>();
	
	template<typename _Type>
	concept container_c = is_container_v<_Type>;
	
	template<typename = void>
	struct is_optional
	{
		static constexpr bool value = false;
	};
	
	template<typename _T>
	struct is_optional<std::optional<_T>>
	{
		static constexpr bool value = true;
	};
	
	template<typename _T>
	constexpr bool is_optional_v = is_optional<std::remove_cvref_t<_T>>::value;
	
	template<typename _Type>
	concept optional_c = is_optional_v<_Type>;
	
	template<typename = void>
	struct is_variant
	{
		static constexpr bool value = false;
	};
	
	template<typename... _Types>
	struct is_variant<std::variant<_Types...>>
	{
		static constexpr bool value = true;
	};
	
	template<typename _T>
	constexpr bool is_variant_v = is_variant<std::remove_cvref_t<_T>>::value;
	
	template<typename _Type>
	concept variant_c = is_variant_v<_Type>;
	
	template<typename = void>
	struct variant_info;
	
	template<template<typename...> typename _Variant, typename... _Types>
	requires is_variant_v<_Variant<_Types...>>
	struct variant_info<_Variant<_Types...>>{
		using types = std::tuple<_Types...>;
		static constexpr std::size_t size = std::variant_size_v<_Variant<_Types...>>;
		static_assert(size == sizeof...(_Types));
	};
	
	template<typename = void>
	struct template_info{
		static constexpr bool value = false;
	};
	
	template<template<typename...> typename _Template, typename... _Types>
	struct template_info<_Template<_Types...>>{
		static constexpr bool value = true;
		static constexpr bool is_array_like = false;
		static constexpr bool is_view_like = false;
		using args = std::tuple<_Types...>;
		using remove_cvref_args = std::tuple<std::remove_cvref_t<_Types>...>;
		static constexpr std::size_t size = sizeof...(_Types);
		template<typename... _Args>
		using type = _Template<_Args...>;
	};
	
	template<template<typename, std::size_t> typename _Template, typename _Type, std::size_t _size>
	struct template_info<_Template<_Type, _size>>{
		static constexpr bool value = true;
		static constexpr bool is_array_like = true;
		static constexpr bool is_view_like = false;
		using arg = _Type;
		using remove_cvref_arg = std::remove_cvref_t<_Type>;
		static constexpr std::size_t array_size = _size;
		template<typename _Arg, std::size_t _s>
		using type = _Template<_Arg, _s>;
	};
	
	template<template<char...> typename _Template, char... _char>
	struct template_info<_Template<_char...>>{
		static constexpr bool value = true;
		static constexpr bool is_array_like = false;
		static constexpr bool is_view_like = true;
		static constexpr _Template<_char...> self{};
		static constexpr std::size_t size = sizeof...(_char);
		template<char... _c>
		using type = _Template<_c...>;
	};
	
	template<typename _T>
	constexpr bool is_template_v = template_info<std::remove_cvref_t<_T>>::value;
	
	template<typename _Type>
	struct bare final{using type = _Type;};
	
	template<typename _Type>
	requires (!std::same_as<_Type, std::remove_pointer_t<_Type>> || !std::same_as<_Type, std::remove_cvref_t<_Type>>)
	struct bare<_Type> final{
		using type = typename bare<std::remove_cvref_t<std::remove_pointer_t<_Type>>>::type;
	};
	
	template<typename _Type>
	using bare_t = typename bare<_Type>::type;
	
	template<typename = void>
	struct is_ref
	{
		static constexpr bool value = false;
	};
	
	template<typename _T>
	struct is_ref<std::reference_wrapper<_T>>
	{
		static constexpr bool value = true;
	};
	
	template<typename _T>
	constexpr bool is_ref_v = is_ref<std::remove_cvref_t<_T>>::value;
	
	template<typename _Type>
	concept ref_c = is_ref_v<_Type>;
	
	template<typename _T>
	struct remove_ref
	{
		using type = std::remove_reference_t<_T>;
	};
	
	template<typename _T>
	struct remove_ref<std::reference_wrapper<_T>>
	{
		using type = std::remove_reference_t<_T>;
	};
	
	template<typename _T>
	using remove_ref_t = typename remove_ref<std::remove_cvref_t<_T>>::type;
	
	template<typename _T>
	auto remove_ref_v(_T &_value) -> std::add_lvalue_reference_t<remove_ref_t<_T>>
	{
		if constexpr(is_ref_v<_T>)
			return _value.get();
		else
			return _value;
	}
	
	template<typename _T>
	auto remove_ref_v(const _T &_value) -> std::add_lvalue_reference_t<remove_ref_t<_T>>
	{
		if constexpr(is_ref_v<_T>)
			return _value.get();
		else
			return _value;
	}
	
	template<typename _T>
	auto remove_ref_v(_T &&_value) -> std::add_lvalue_reference_t<remove_ref_t<_T>>
	{
		static_assert(
			is_ref_v<_T>,
			"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			"\nerror<fake::remove_ref_v>:\n"
			"\ta rvalue passed as arg for being transformed into reference,\n"
			"\twhich must be a 'reference_wrapper'.\n"
			"\ta regular rvalue transforming into lvalue reference may cause a reference hanging.\n"
			"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
		);
		
		if constexpr(is_ref_v<_T>)
			return _value.get();
		else
			return _value;
	}
	
	template<template<typename...> typename _Template, typename... _Type>
	concept deducible_c = requires{typename _Template<_Type...>;};
	
	template<std::size_t _size, auto _concept>
	requires fake::mezz_c<decltype(_concept)>
	struct concept_sequence{
		static constexpr bool value = []<std::size_t... _index>(std::index_sequence<_index...>){
			return (_concept.value(fake::mezz_v<_index>) && ...);
		}(std::make_index_sequence<_size>());
	};
	
	template<std::size_t _size, auto _concept>
	requires fake::mezz_c<decltype(_concept)>
	constexpr bool concept_sequence_v = concept_sequence<_size, _concept>::value;
	
	template<std::size_t _size, auto _concept>
	concept concept_sequence_c = concept_sequence_v<_size, _concept>;
	
	template<typename _Functor, typename _Tuple>
	concept applicable_c = fake::tuple_c<_Tuple> && requires(_Functor _functor, _Tuple _tuple){
		requires []<std::size_t... _index>(std::index_sequence<_index...>){
			return requires{_functor(std::get<_index>(_tuple)...);};
		}(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<_Tuple>>>());
	};
	
	template<typename _Functor, typename _Tuple>
	using apply_result_t = decltype(std::apply(std::declval<_Functor>(), std::declval<_Tuple>()));
	
	template<bool _Value, auto _Message>
	struct clamour final{
		using message_t = decltype(std::declval<decltype(_Message)>()(std::integral_constant<bool, _Value>{}));
		static constexpr bool value = _Value;
	};
	
	template<bool _Value, auto _Message>
	concept clamour_c = clamour<_Value, _Message>::value;
	
	template<bool _Conditionm, auto _True, auto _False>
	struct conditional;
	
	template<auto _True, auto _False>
	struct conditional<true, _True, _False> final{
		static constexpr auto value = _True;
	};
	
	template<auto _True, auto _False>
	struct conditional<false, _True, _False> final{
		static constexpr auto value = _False;
	};
	
	template<std::convertible_to<bool> auto _Condition, auto _True, auto _False>
	constexpr auto conditional_v = conditional<bool(_Condition), _True, _False>::value;
	
	template<typename, template<typename...> typename>
	struct mimic{};
	
	template<template<typename...> typename _From, template<typename...> typename _To, typename... _Args>
	requires requires{typename _To<_Args...>;}
	struct mimic<_From<_Args...>, _To>{using type = _To<_Args...>;};
	
	template<typename _From, template<typename...> typename _To>
	using mimic_t = typename mimic<std::remove_cvref_t<_From>, _To>::type;
	
	template<typename, typename, template<typename...> typename>
	struct gemma{};
	
	template<std::size_t... _index, typename _Arg, template<typename...> typename _Template>
	requires requires{typename _Template<fake::type_t<fake::mezz_t<_index>, _Arg>...>;}
	struct gemma<std::index_sequence<_index...>, _Arg, _Template>{
		using type = _Template<fake::type_t<fake::mezz_t<_index>, _Arg>...>;
	};
	
	template<std::size_t _size, typename _Arg, template<typename...> typename _Template>
	using gemma_t = typename gemma<std::make_index_sequence<_size>, std::remove_cvref_t<_Arg>, _Template>::type;
	
}

#include "pop.h"

#endif /*__FAKE_TRAITS_H__*/ 
