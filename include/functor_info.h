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

#include <functional>

#include "is_valid.h"

namespace fake
{
	
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
		using args = typename functor_info<decltype(&_Lambda::operator())>::args<_Index>;
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
		using rec_args = typename functor_info<_Functor>::args<_Index>;
		using rec_tuple = typename functor_info<_Functor>::tuple;
		static constexpr std::size_t rec_size = functor_info<_Functor>::size;
		
		using retn = rec_retn;
		using func = retn(_Args...);
		template<std::size_t _Index>
		using args = fake::pack_index_t<_Index, _Args...>;
		using tuple = std::tuple<_Args...>;
		static constexpr std::size_t size = sizeof... (_Args);
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

#endif /*__FUNCTOR_INFO_H__*/ 
