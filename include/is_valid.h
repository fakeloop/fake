#ifndef __FAKE_IS_VALID_H__
#define __FAKE_IS_VALID_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include <utility>

namespace fake
{
	
	struct null_t{};
	
	template<auto _value>
	concept null_c = std::same_as<std::remove_cvref_t<decltype(_value)>, null_t>;
	
	template<template<typename...> typename, typename>
	struct trait : std::false_type{};
	
	template<template<typename...> typename _Tamplate, template<typename...> typename _Type, typename... _Args>
	requires requires{typename _Tamplate<_Args...>; typename _Type<_Args...>;}
	struct trait<_Tamplate, _Type<_Args...>> : std::is_same<_Tamplate<_Args...>, _Type<_Args...>>{};
	
	template<template<typename...> typename _Tamplate, typename _Type>
	constexpr bool trait_v = trait<_Tamplate, _Type>::value;
	
	template<typename _Type, template<typename...> typename _Tamplate>
	concept trait_c = trait_v<_Tamplate, _Type>;
	
	template<template<auto...> typename, typename>
	struct trait_auto : std::false_type{};
	
	template<template<auto...> typename _Tamplate, template<auto...> typename _Type, auto... _Args>
	requires requires{typename _Tamplate<_Args...>; typename _Type<_Args...>;}
	struct trait_auto<_Tamplate, _Type<_Args...>> : std::is_same<_Tamplate<_Args...>, _Type<_Args...>>{};
	
	template<template<auto...> typename _Tamplate, typename _Type>
	constexpr bool trait_auto_v = trait_auto<_Tamplate, _Type>::value;
	
	template<typename _Type, template<auto...> typename _Tamplate>
	concept trait_auto_c = trait_auto_v<_Tamplate, _Type>;
	
	template<bool _Condition>
	concept to_trait_c = _Condition;
	
	template<typename _Type>
	struct type_package
	{
		using type = _Type;
		static constexpr type_package echo{};
	};
	
	template<typename _Type>
	constexpr type_package<_Type> pack_v{};
	
	template<typename _Type>
	concept pack_c = trait_v<type_package, std::remove_cvref_t<_Type>>;
	
	template<auto _value>
	constexpr type_package<decltype(_value)> wrap_v{};
	
	template<pack_c auto _pack>
	using take_t = typename std::remove_cvref_t<decltype(_pack)>::type;
	
	template<pack_c auto _pack>
	constexpr take_t<_pack> take_v{};
	
	template<auto _value>
	requires std::is_integral_v<std::remove_cvref_t<decltype(_value)>>
	constexpr std::integral_constant<std::remove_cvref_t<decltype(_value)>, _value> number{};
	
	template<auto _Value>
	struct value_mezzanine
	{
		using type = decltype(_Value);
		static constexpr auto value = _Value;
	};
	
	template<auto _Value>
	using mezz_t = value_mezzanine<_Value>;
	
	template<auto _Value>
	constexpr value_mezzanine<_Value> mezz_v{};
	
	template<typename _Type>
	concept mezz_c = trait_auto_v<value_mezzanine, std::remove_cvref_t<_Type>>;
	
	template<template<typename...> typename _Template>
	struct generic
	{
		template<typename... _Parameters>
		using type = _Template<_Parameters...>;
	};
	
	template<template<typename, std::size_t> typename _Template>
	struct array_like{
		template<typename _T, std::size_t _s>
		using type = _Template<_T, _s>;
	};
	
	template<template<char...> typename _Template>
	struct view_like{
		template<char... _c>
		using type = _Template<_c...>;
	};
	
	template<template<typename...> typename _Template>
	constexpr generic<_Template> gene_v{};
	
	template<typename>
	struct is_generic : std::false_type{};
	
	template<template<typename...> typename _Template>
	struct is_generic<generic<_Template>> : std::true_type{};
	
	template<template<typename, std::size_t> typename _Template>
	struct is_generic<array_like<_Template>> : std::true_type{};
	
	template<template<char...> typename _Template>
	struct is_generic<view_like<_Template>> : std::true_type{};
	
	template<typename _Type>
	constexpr bool is_generic_v = is_generic<std::remove_cvref_t<_Type>>::value;
	
	template<typename _Type>
	concept gene_c = is_generic_v<_Type>;
	
	template<typename _Type>
	struct pattern
	{
		using type = _Type;
	};
	
	template<template<typename...> typename _Template, typename... _Parameters>
	struct pattern<_Template<_Parameters...>>
	{
		using type = generic<_Template>;
	};
	
	template<template<typename, std::size_t> typename _Template, typename _Type, std::size_t _size>
	struct pattern<_Template<_Type, _size>>
	{
		using type = array_like<_Template>;
	};
	
	template<template<char...> typename _Template, char... _c>
	struct pattern<_Template<_c...>>
	{
		using type = view_like<_Template>;
	};
	
	template<typename _Type>
	using pattern_t = typename pattern<std::remove_cvref_t<_Type>>::type;
	
	template<typename _T>
	constexpr auto type_extract(const _T &_t) noexcept
		->std::add_lvalue_reference_t<typename _T::type>;
	
	template<typename _T>
	constexpr auto type_lvalue(const _T &_t) noexcept
		->std::add_lvalue_reference_t<std::remove_cvref_t<typename _T::type>>;
	
	template<typename _T>
	constexpr auto type_rvalue(const _T &_t) noexcept
		->std::add_rvalue_reference_t<std::remove_cvref_t<typename _T::type>>;
	
	template<typename _T>
	constexpr auto type_clvalue(const _T &_t) noexcept
		->std::add_lvalue_reference_t<std::add_const_t<std::remove_cvref_t<typename _T::type>>>;
	
	template<typename _T>
	constexpr auto type_crvalue(const _T &_t) noexcept
		->std::add_rvalue_reference_t<std::add_const_t<std::remove_cvref_t<typename _T::type>>>;
	
	template<typename _T>
	struct true_type
	{
		static constexpr bool value = true;
	};
	
	template<typename _T>
	struct false_type
	{
		static constexpr bool value = false;
	};
	
	template<typename _T>
	constexpr bool true_value = true_type<_T>::value;
	
	template<typename _T>
	constexpr bool false_value = false_type<_T>::value;
	
	template<std::size_t _value>
	constexpr auto index_v = std::integral_constant<std::size_t, _value>{};
	
	template<typename _Ignore, typename _Type = void>
	using type_t = _Type;
	
	template<std::size_t _Priority>
	struct sfinae : sfinae<_Priority - 1>
	{
		static constexpr std::size_t value = _Priority;
	};
	
	template<>
	struct sfinae<std::size_t{}>
	{
		static constexpr std::size_t value = std::size_t{};
	};
	
	template<std::size_t _Index>
	struct index
	{
		index<_Index - 1> prev;
		static constexpr std::size_t value = _Index;
	};
	
	template<>
	struct index<std::size_t{}>
	{
		static constexpr std::size_t value = std::size_t{};
	};
	
	template<std::size_t _Index, typename... _Types>
	struct arg_pack final
	{
	private:
		template<std::size_t _Current>
		static constexpr auto impl(){
			return fake::pack_v<void>;
		}
		
		template<std::size_t _Current, typename _Head, typename... _Tails>
		static constexpr auto impl(){
			if constexpr(_Current)
				return impl<_Current - 1, _Tails...>();
			else
				return fake::pack_v<_Head>;
		}
		
	public:
		using type = typename decltype(arg_pack::impl<_Index, _Types...>())::type;
	};
	
	template<std::size_t _Index, typename... _Types>
	requires requires{typename arg_pack<_Index, _Types...>::type;}
	using pack_index_t = typename arg_pack<_Index, _Types...>::type;
	
	template<typename... _Types>
	constexpr std::size_t pack_size_v = sizeof...(_Types);
	
	template<std::size_t = 0, typename = void>
	struct element{};
	
	template<std::size_t _Index, typename... _Types, template<typename...> typename _Type>
	struct element<_Index, _Type<_Types...>>
	{
		using type = pack_index_t<_Index, _Types...>;
		static constexpr std::size_t size = sizeof...(_Types);
	};
	
	template<std::size_t _Index, typename _Type>
	requires requires{typename element<_Index, _Type>::type;}
	using element_index_t = typename element<_Index, _Type>::type;
	
	template<typename _Type>
	requires requires{element<0, _Type>::size;}
	constexpr std::size_t element_size_v = element<0, std::remove_cvref_t<_Type>>::size;
	
	namespace detail::is_valid
	{
		
		template<typename _Checker>
		struct wrapper
		{
		private:
			using checker_type = _Checker;
			
			using default_t = fake::sfinae<0>;
			using existed_t = fake::sfinae<1>;
			
			template<typename... _Args>
			requires requires(checker_type _checker, _Args... _args){_checker(_args...);}
			static constexpr auto is_valid(existed_t){return true;}
			
			template<typename... _Args>
			static constexpr auto is_valid(default_t){return false;}
			
		public:
			template<typename... _Args>
			constexpr auto operator()(const _Args&...) const{
				return wrapper::is_valid<_Args...>(existed_t{});
			}
		};
		
	}
	
	template<typename _Checker>
	constexpr auto is_valid(const _Checker&){
		return detail::is_valid::wrapper<_Checker>{};
	}
	
	template<auto _constraint, typename... _Args>
	concept constraint_c = requires(_Args ..._args){requires _constraint.value(_args...);};
	
}

#endif /*__FAKE_IS_VALID_H__*/ 
