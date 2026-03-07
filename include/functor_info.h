#ifndef __FUNCTOR_INFO_H__
#define __FUNCTOR_INFO_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "push.h"

#include <concepts>
#include <functional>
#include <tuple>

#include "is_valid.h"

namespace fake
{
	
	namespace detail::functor_info
	{
		
		template<typename>
		struct place_holder_extractor final{};
		
		template<template<auto> typename _Ph, auto _index>
		struct place_holder_extractor<_Ph<_index>> final{static constexpr auto value = _index;};
		
		template<typename _Type>
		constexpr auto place_holder_extract_v = place_holder_extractor<std::remove_cvref_t<_Type>>::value;
		
		template<typename>
		struct is_place_holder final : std::false_type{};
		
		template<template<auto> typename _Ph, auto _index>
		requires std::same_as<
			_Ph<place_holder_extract_v<decltype(std::placeholders::_1)>>,
			std::remove_cvref_t<decltype(std::placeholders::_1)>
		>
		struct is_place_holder<_Ph<_index>> final : std::true_type{};
		
		template<typename _Type>
		concept is_place_holder_c = is_place_holder<std::remove_cvref_t<_Type>>::value;
		
		template<typename _Retn, typename _Rec, typename... _Binds>
		struct args_mapping final{
		private:
			static consteval auto extract() noexcept{
				return []<std::size_t... _index>(std::index_sequence<_index...>){
					constexpr auto each = [](fake::pazz_c auto _pazz, fake::mezz_c auto _i){
						constexpr std::size_t origin = place_holder_extract_v<decltype(std::placeholders::_1)>;
						
						if constexpr(is_place_holder_c<fake::take_t<_pazz.value>>)
							return fake::pazz_v<
								std::tuple<
									std::tuple_element_t<
										place_holder_extract_v<fake::take_t<_pazz.value>> - origin,
										_Rec
									>
								>
							>;
						else
							return fake::pazz_v<std::tuple<>>;
					};
					
					return fake::pazz_v<
						decltype(
							std::tuple_cat(
								std::declval<fake::take_t<each(fake::pazz_v<_Binds>, fake::mezz_v<_index>)>>()...
							)
						)
					>;
				}(std::make_index_sequence<sizeof...(_Binds)>());
			}
			
			static consteval auto functor() noexcept{
				using tuple = fake::take_t<extract()>;
				
				return []<std::size_t... _index>(std::index_sequence<_index...>){
					return fake::pazz_v<_Retn(std::tuple_element_t<_index, tuple>...)>;
				}(std::make_index_sequence<std::tuple_size_v<tuple>>());
			}
			
		public:
			using tuple = fake::take_t<args_mapping::extract()>;
			using func = fake::take_t<args_mapping::functor()>;
			template<std::size_t _Index>
			using args = std::tuple_element_t<_Index, tuple>;
			static constexpr std::size_t size = std::tuple_size_v<tuple>;
		};
		
	}
	
	// declaration only, for the sfinae instantiation failure in the argument deduction of 'fake::is_functor'. 
	template<typename, typename = void>
	struct functor_info;
	
	template<class _Lambda>
	struct functor_info<_Lambda, std::void_t<decltype(&_Lambda::operator())>>
	{
		static constexpr const char* name = "functor_object";
		using retn = typename functor_info<decltype(&_Lambda::operator())>::retn;
		using func = typename functor_info<decltype(&_Lambda::operator())>::func;
		template<std::size_t _Index>
		using args = typename functor_info<decltype(&_Lambda::operator())>::template args<_Index>;
		using tuple = typename functor_info<decltype(&_Lambda::operator())>::tuple;
		static constexpr std::size_t size = functor_info<decltype(&_Lambda::operator())>::size;
	};
	
	template<typename _Functor, typename... _Args, template<typename, typename...> class _Bind>
	struct functor_info<
		_Bind<_Functor(_Args...)>,
		std::enable_if_t<std::is_bind_expression_v<_Bind<_Functor(_Args...)>>>
	>
	{
		static constexpr const char* name = "bind_expression";
		using rec_retn = typename functor_info<_Functor>::retn;
		using rec_func = typename functor_info<_Functor>::func;
		template<std::size_t _Index>
		using rec_args = typename functor_info<_Functor>::template args<_Index>;
		using rec_tuple = typename functor_info<_Functor>::tuple;
		static constexpr std::size_t rec_size = functor_info<_Functor>::size;
		
		using bind_retn = rec_retn;
		using bind_func = rec_retn(_Args...);
		template<std::size_t _Index>
		using bind_args = fake::pack_index_t<_Index, _Args...>;
		using bind_tuple = std::tuple<_Args...>;
		static constexpr std::size_t bind_size = sizeof... (_Args);
		
	private:
		using mapping = detail::functor_info::args_mapping<rec_retn, rec_tuple, _Args...>;
		
	public:
		using retn = rec_retn;
		using func = typename mapping::func;
		template<std::size_t _Index>
		using args = typename mapping::template args<_Index>;
		using tuple = typename mapping::tuple;
		static constexpr std::size_t size = mapping::size;
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...)>
	{
		static constexpr const char* name = "member_function_pointer";
		static constexpr bool constant = false;
		static constexpr bool volatility = false;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const>
	{
		static constexpr const char* name = "member_function_pointer_const";
		static constexpr bool constant = true;
		static constexpr bool volatility = false;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) volatile>
	{
		static constexpr const char* name = "member_function_pointer_volatile";
		static constexpr bool constant = false;
		static constexpr bool volatility = true;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const volatile>
	{
		static constexpr const char* name = "member_function_pointer_const_volatile";
		static constexpr bool constant = true;
		static constexpr bool volatility = true;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) &>
	{
		static constexpr const char* name = "member_function_pointer_lvalue";
		static constexpr bool constant = false;
		static constexpr bool volatility = false;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = true;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const &>
	{
		static constexpr const char* name = "member_function_pointer_const_lvalue";
		static constexpr bool constant = true;
		static constexpr bool volatility = false;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = true;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) volatile &>
	{
		static constexpr const char* name = "member_function_pointer_volatile_lvalue";
		static constexpr bool constant = false;
		static constexpr bool volatility = true;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = true;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const volatile &>
	{
		static constexpr const char* name = "member_function_pointer_const_volatile_lvalue";
		static constexpr bool constant = true;
		static constexpr bool volatility = true;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = true;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) &&>
	{
		static constexpr const char* name = "member_function_pointer_rvalue";
		static constexpr bool constant = false;
		static constexpr bool volatility = false;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = true;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const &&>
	{
		static constexpr const char* name = "member_function_pointer_const_rvalue";
		static constexpr bool constant = true;
		static constexpr bool volatility = false;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = true;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) volatile &&>
	{
		static constexpr const char* name = "member_function_pointer_volatile_rvalue";
		static constexpr bool constant = false;
		static constexpr bool volatility = true;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = true;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const volatile &&>
	{
		static constexpr const char* name = "member_function_pointer_const_volatile_rvalue";
		static constexpr bool constant = true;
		static constexpr bool volatility = true;
		static constexpr bool exception = true;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = true;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) noexcept>
	{
		static constexpr const char* name = "member_function_pointer_noexcept";
		static constexpr bool constant = false;
		static constexpr bool volatility = false;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const noexcept>
	{
		static constexpr const char* name = "member_function_pointer_const_noexcept";
		static constexpr bool constant = true;
		static constexpr bool volatility = false;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) volatile noexcept>
	{
		static constexpr const char* name = "member_function_pointer_volatile_noexcept";
		static constexpr bool constant = false;
		static constexpr bool volatility = true;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const volatile noexcept>
	{
		static constexpr const char* name = "member_function_pointer_const_volatile_noexcept";
		static constexpr bool constant = true;
		static constexpr bool volatility = true;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) & noexcept>
	{
		static constexpr const char* name = "member_function_pointer_lvalue_noexcept";
		static constexpr bool constant = false;
		static constexpr bool volatility = false;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = true;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const & noexcept>
	{
		static constexpr const char* name = "member_function_pointer_const_lvalue_noexcept";
		static constexpr bool constant = true;
		static constexpr bool volatility = false;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = true;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) volatile & noexcept>
	{
		static constexpr const char* name = "member_function_pointer_volatile_lvalue_noexcept";
		static constexpr bool constant = false;
		static constexpr bool volatility = true;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = true;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const volatile & noexcept>
	{
		static constexpr const char* name = "member_function_pointer_const_volatile_lvalue_noexcept";
		static constexpr bool constant = true;
		static constexpr bool volatility = true;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = true;
		static constexpr bool rvalue_reference = false;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) && noexcept>
	{
		static constexpr const char* name = "member_function_pointer_rvalue_noexcept";
		static constexpr bool constant = false;
		static constexpr bool volatility = false;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = true;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const && noexcept>
	{
		static constexpr const char* name = "member_function_pointer_const_rvalue_noexcept";
		static constexpr bool constant = true;
		static constexpr bool volatility = false;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = true;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) volatile && noexcept>
	{
		static constexpr const char* name = "member_function_pointer_volatile_rvalue_noexcept";
		static constexpr bool constant = false;
		static constexpr bool volatility = true;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = true;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename _Class, typename... _Args>
	struct functor_info<_Retn(_Class::*)(_Args...) const volatile && noexcept>
	{
		static constexpr const char* name = "member_function_pointer_const_volatile_rvalue_noexcept";
		static constexpr bool constant = true;
		static constexpr bool volatility = true;
		static constexpr bool exception = false;
		static constexpr bool lvalue_reference = false;
		static constexpr bool rvalue_reference = true;
		using object = _Class;
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename... _Args>
	struct functor_info<_Retn(_Args...)>
	{
		static constexpr const char* name = "global_function";
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename... _Args>
	struct functor_info<_Retn(*)(_Args...)>
	{
		static constexpr const char* name = "global_function_pointer";
		using retn = _Retn;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename _Retn, typename... _Args>
	struct functor_info<_Retn(&)(_Args...)>
	{
		static constexpr const char* name = "global_function_reference";
		using retn = _Retn ;
		using func = _Retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
	};
	
	template<typename = void, typename = void>
	struct is_functor
	{
		static constexpr bool value = false;
	};
	
	template<typename _Functor>
	struct is_functor<_Functor, std::void_t<decltype(functor_info<_Functor>{})>>
	{
		static constexpr bool value = true;
	};
	
	template<typename _Type>
	constexpr bool is_functor_v = is_functor<std::remove_cvref_t<_Type>>::value;
	
	template<typename _Type>
	concept functor_c = is_functor_v<_Type>;
	
}

#include "pop.h"

#endif /*__FUNCTOR_INFO_H__*/ 
