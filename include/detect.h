#ifndef __FAKE_DETECT_H__
#define __FAKE_DETECT_H__

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
#include <compare>
#include <ranges>

#include "view.h"
#include "flat.h"

namespace fake
{
	
	namespace detect::op{struct creed{}; struct shift{};}
	
	template<typename _Type>
	concept creed_c = std::derived_from<std::remove_cvref_t<_Type>, fake::detect::op::creed>;
	
	template<typename _Type>
	concept shift_c = std::derived_from<std::remove_cvref_t<_Type>, fake::detect::op::shift>;
	
	template<typename>
	struct is_creeds : std::false_type{};
	
	template<fake::creed_c... _Type>
	struct is_creeds<fake::flat<_Type...>> : std::true_type{};
	
	template<typename _Type>
	constexpr bool is_creeds_v = is_creeds<std::remove_cvref_t<_Type>>::value;
	
	template<typename _Type>
	concept creeds_c = is_creeds_v<_Type>;
	
}

namespace fake::detect
{
	
	namespace tool
	{
		
		template<typename _Type>
		concept pass_c = fake::pack_c<_Type> || fake::pazz_c<_Type>;
		
		template<tool::pass_c _Pass, typename _Type>
		using repass_t = std::conditional_t<fake::pack_c<_Pass>, fake::pack_t<_Type>, fake::pazz_t<_Type>>;
		
		template<tool::pass_c _Pass, typename _Type>
		inline constexpr repass_t<_Pass, _Type> repass_v;
		
		template<template<typename...> typename _Template, typename... _Args>
		inline constexpr auto constraint = []<typename _Type> requires _Template<_Type, _Args...>::value (){};
		
		template<template<typename...> typename _Template, typename... _Args>
		inline constexpr auto transform = [](tool::pass_c auto _e, auto &&...)
		requires requires{typename _Template<fake::take_t<decltype(_e){}>, _Args...>;}{
			return tool::repass_v<decltype(_e), _Template<fake::take_t<decltype(_e){}>, _Args...>>;
		};
		
		template<template<typename, auto...> typename _Template, auto... _args>
		inline constexpr auto transform_constant = [](tool::pass_c auto _e, auto &&...)
		requires requires{typename _Template<fake::take_t<decltype(_e){}>, _args...>;}{
			return tool::repass_v<decltype(_e), _Template<fake::take_t<decltype(_e){}>, _args...>>;
		};
		
		template<template<auto...> typename _Template, auto... _args>
		inline constexpr auto transform_auto = [](tool::pass_c auto _e, auto &&...)
		requires requires{typename _Template<fake::take_t<decltype(_e){}>{}, _args...>;}{
			return tool::repass_v<decltype(_e), _Template<fake::take_t<decltype(_e){}>{}, _args...>>;
		};
		
		template<
			template<typename, template<typename...> typename> typename _Template,
			template<typename...> typename _Arg
		>
		inline constexpr auto transform_template = [](tool::pass_c auto _e, auto &&...)
		requires requires{typename _Template<fake::take_t<decltype(_e){}>, _Arg>;}{
			return tool::repass_v<decltype(_e), _Template<fake::take_t<decltype(_e){}>, _Arg>>;
		};
		
		template<fake::creed_c auto _condition, fake::shift_c auto _true, fake::shift_c auto _false>
		inline constexpr auto transform_condition = [](tool::pass_c auto _e, auto &&..._args){
			if constexpr(_condition(_e, std::forward<decltype(_args)>(_args)...))
				return _true(_e, std::forward<decltype(_args)>(_args)...);
			else
				return _false(_e, std::forward<decltype(_args)>(_args)...);
		};
		
		template<auto _lambda, typename _Arg>
		concept transform_c = tool::pass_c<_Arg> && requires{{_lambda(_Arg{})} -> tool::pass_c;};
		
		template<auto _lambda, typename _Arg, typename... _Args>
		concept detector_c = tool::pass_c<_Arg> && requires(_Args ..._args){
			{_lambda(_Arg{}, std::forward<_Args>(_args)...)} -> std::convertible_to<bool>;
		};
		
		template<auto _lambda, typename _Arg, typename... _Args>
		concept legal_c = tool::pass_c<_Arg> && (
			detector_c<_lambda, _Arg, _Args...> || !requires(_Args ..._args){
				_lambda(_Arg{}, std::forward<_Args>(_args)...);
			}
		);
		
		template<auto _lhs, auto _rhs, typename _Arg, typename... _Args>
		concept detector_binary_c = tool::detector_c<_lhs, _Arg, _Args...> && tool::detector_c<_rhs, _Arg, _Args...>;
		
	}
	
	namespace op
	{
		
		// if the '[]<some_c>{}' was passed to '_lambda', that means to ignore the '_args'. 
		template<auto _lambda>
		struct just final : fake::detect::op::creed{
			constexpr bool operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires tool::legal_c<_lambda, decltype(_e), decltype(_args)...>{
				constexpr tool::pass_c auto e = decltype(_e){};
				if constexpr(tool::detector_c<_lambda, decltype(e), decltype(_args)...>)
					return _lambda(e, std::forward<decltype(_args)>(_args)...);
				else
					return requires{_lambda.template operator()<fake::take_t<e>>();};
			}
		};
		
		// if the '[]<some_c>{}' was passed to '_lambda', that means to ignore the '_args'. 
		template<auto _lambda>
		struct must final : fake::detect::op::creed{
			constexpr bool operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires tool::detector_c<_lambda, decltype(_e), decltype(_args)...>{
				constexpr tool::pass_c auto e = decltype(_e){};
				return _lambda(e, std::forward<decltype(_args)>(_args)...);
			}
		};
		
		template<fake::creed_c _Op>
		struct negation final : fake::detect::op::creed{
			constexpr bool operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires tool::detector_c<_Op{}, decltype(_e), decltype(_args)...>{
				return _Op{}(_e, std::forward<decltype(_args)>(_args)...) == false;
			}
		};
		
		template<fake::creed_c _Lhs, fake::creed_c _Rhs>
		struct conjunction final : fake::detect::op::creed{
			constexpr bool operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires tool::detector_binary_c<_Lhs{}, _Rhs{}, decltype(_e), decltype(_args)...>{
				return _Lhs{}(_e, std::forward<decltype(_args)>(_args)...) &&
					_Rhs{}(_e, std::forward<decltype(_args)>(_args)...);
			}
		};
		
		template<fake::creed_c _Lhs, fake::creed_c _Rhs>
		struct disjunction final : fake::detect::op::creed{
			constexpr bool operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires tool::detector_binary_c<_Lhs{}, _Rhs{}, decltype(_e), decltype(_args)...>{
				return _Lhs{}(_e, std::forward<decltype(_args)>(_args)...) ||
					_Rhs{}(_e, std::forward<decltype(_args)>(_args)...);
			}
		};
		
		template<fake::creed_c _Lhs, fake::creed_c _Rhs>
		struct equivalence final : fake::detect::op::creed{
			constexpr bool operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires tool::detector_binary_c<_Lhs{}, _Rhs{}, decltype(_e), decltype(_args)...>{
				return bool(_Lhs{}(_e, std::forward<decltype(_args)>(_args)...)) ==
					bool(_Rhs{}(_e, std::forward<decltype(_args)>(_args)...));
			}
		};
		
		template<fake::creed_c _Lhs, fake::creed_c _Rhs>
		struct nonequivalence final : fake::detect::op::creed{
			constexpr bool operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires tool::detector_binary_c<_Lhs{}, _Rhs{}, decltype(_e), decltype(_args)...>{
				return bool(_Lhs{}(_e, std::forward<decltype(_args)>(_args)...)) !=
					bool(_Rhs{}(_e, std::forward<decltype(_args)>(_args)...));
			}
		};
		
		template<fake::creed_c _Lhs, fake::creed_c _Rhs>
		inline constexpr auto operator&&(_Lhs, _Rhs) noexcept{return fake::detect::op::conjunction<_Lhs, _Rhs>{};}
		
		template<fake::creed_c _Lhs, fake::creed_c _Rhs>
		inline constexpr auto operator||(_Lhs, _Rhs) noexcept{return fake::detect::op::disjunction<_Lhs, _Rhs>{};}
		
		template<fake::creed_c _Lhs, fake::creed_c _Rhs>
		inline constexpr auto operator==(_Lhs, _Rhs) noexcept{return fake::detect::op::equivalence<_Lhs, _Rhs>{};}
		
		template<fake::creed_c _Lhs, fake::creed_c _Rhs>
		inline constexpr auto operator!=(_Lhs, _Rhs) noexcept{return fake::detect::op::nonequivalence<_Lhs, _Rhs>{};}
		
		template<fake::creed_c _Lhs, fake::creed_c _Rhs>
		inline constexpr auto operator^(_Lhs, _Rhs) noexcept{return fake::detect::op::nonequivalence<_Lhs, _Rhs>{};}
		
		template<fake::creed_c _Op>
		inline constexpr auto operator!(_Op) noexcept{return fake::detect::op::negation<_Op>{};}
		
		template<fake::creed_c _Op>
		inline constexpr auto operator!(fake::detect::op::negation<_Op>) noexcept{return _Op{};}
		
	}
	
	namespace op
	{
		
		template<auto _lambda>
		struct rein final : fake::detect::op::shift{
			constexpr auto operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires tool::transform_c<_lambda, decltype(_e)>{
				return _lambda(_e, std::forward<decltype(_args)>(_args)...);
			}
		};
		
		template<fake::shift_c _Shift, fake::creed_c _Creed>
		struct combine final : fake::detect::op::creed{
			constexpr bool operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires tool::detector_c<_Creed{}, decltype(_Shift{}(_e)), decltype(_args)...>{
				return _Creed{}(
					_Shift{}(_e, std::forward<decltype(_args)>(_args)...),
					std::forward<decltype(_args)>(_args)...
				);
			}
			constexpr bool operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires (requires{_Shift{}(_e);} == false){
				return false;
			}
		};
		
		template<fake::shift_c _Lhs, fake::shift_c _Rhs>
		struct pipe final : fake::detect::op::shift{
			constexpr auto operator()(tool::pass_c auto _e, auto &&..._args) const noexcept
			requires requires{{_Rhs{}(_Lhs{}(_e, std::forward<decltype(_args)>(_args)...))} -> tool::pass_c;}{
				return _Rhs{}(
					_Lhs{}(_e, std::forward<decltype(_args)>(_args)...),
					std::forward<decltype(_args)>(_args)...
				);
			}
		};
		
		template<fake::shift_c _Shift, fake::creed_c _Creed>
		inline constexpr auto operator<<=(_Shift, _Creed) noexcept{return fake::detect::op::combine<_Shift, _Creed>{};}
		
		template<fake::shift_c _Lhs, fake::shift_c _Rhs>
		inline constexpr auto operator<<=(_Lhs, _Rhs) noexcept{return fake::detect::op::pipe<_Lhs, _Rhs>{};}
		
	}
	
	namespace adaptor
	{
		
		// primary type categories. 
		
		inline constexpr auto void_v
			= tool::constraint<std::is_void>;
		inline constexpr auto null_pointer_v
			= tool::constraint<std::is_null_pointer>;
		inline constexpr auto integral_v
			= tool::constraint<std::is_integral>;
		inline constexpr auto floating_point_v
			= tool::constraint<std::is_floating_point>;
		inline constexpr auto array_v
			= []<fake::array_c>(){};
		inline constexpr auto enum_v
			= tool::constraint<std::is_enum>;
		inline constexpr auto union_v
			= tool::constraint<std::is_union>;
		inline constexpr auto class_v
			= tool::constraint<std::is_class>;
		inline constexpr auto function_v
			= tool::constraint<std::is_function>;
		inline constexpr auto pointer_v
			= tool::constraint<std::is_pointer>;
		inline constexpr auto lvalue_reference_v
			= tool::constraint<std::is_lvalue_reference>;
		inline constexpr auto rvalue_reference_v
			= tool::constraint<std::is_rvalue_reference>;
		inline constexpr auto member_object_pointer_v
			= tool::constraint<std::is_member_object_pointer>;
		inline constexpr auto member_function_pointer_v
			= tool::constraint<std::is_member_function_pointer>;
		
		// composite type categories. 
		
		inline constexpr auto fundamental_v
			= tool::constraint<std::is_fundamental>;
		inline constexpr auto arithmetic_v
			= tool::constraint<std::is_arithmetic>;
		inline constexpr auto scalar_v
			= tool::constraint<std::is_scalar>;
		inline constexpr auto object_v
			= tool::constraint<std::is_object>;
		inline constexpr auto compound_v
			= tool::constraint<std::is_compound>;
		inline constexpr auto reference_v
			= tool::constraint<std::is_reference>;
		inline constexpr auto member_pointer_v
			= tool::constraint<std::is_member_pointer>;
		
		// type properties. 
		
		inline constexpr auto const_v
			= tool::constraint<std::is_const>;
		inline constexpr auto volatile_v
			= tool::constraint<std::is_volatile>;
		inline constexpr auto trivial_v
			= tool::constraint<std::is_trivial>;
		inline constexpr auto trivially_copyable_v
			= tool::constraint<std::is_trivially_copyable>;
		inline constexpr auto standard_layout_v
			= tool::constraint<std::is_standard_layout>;
		inline constexpr auto has_unique_object_representations_v
			= tool::constraint<std::has_unique_object_representations>;
		inline constexpr auto empty_v
			= tool::constraint<std::is_empty>;
		inline constexpr auto polymorphic_v
			= tool::constraint<std::is_polymorphic>;
		inline constexpr auto abstract_v
			= tool::constraint<std::is_abstract>;
		inline constexpr auto final_v
			= tool::constraint<std::is_final>;
		inline constexpr auto aggregate_v
			= tool::constraint<std::is_aggregate>;
		inline constexpr auto signed_v
			= tool::constraint<std::is_signed>;
		inline constexpr auto unsigned_v
			= tool::constraint<std::is_unsigned>;
		inline constexpr auto bounded_array_v
			= tool::constraint<std::is_bounded_array>;
		inline constexpr auto unbounded_array_v
			= tool::constraint<std::is_unbounded_array>;
		
		// supported operations. 
		
		template<typename... _Args> inline constexpr auto is_constructible_v
			= tool::constraint<std::is_constructible, _Args...>;
		template<typename... _Args> inline constexpr auto is_trivially_constructible_v
			= tool::constraint<std::is_trivially_constructible, _Args...>;
		template<typename... _Args> inline constexpr auto is_nothrow_constructible_v
			= tool::constraint<std::is_nothrow_constructible, _Args...>;
		inline constexpr auto is_default_constructible_v
			= tool::constraint<std::is_default_constructible>;
		inline constexpr auto is_trivially_default_constructible_v
			= tool::constraint<std::is_trivially_default_constructible>;
		inline constexpr auto is_nothrow_default_constructible_v
			= tool::constraint<std::is_nothrow_default_constructible>;
		inline constexpr auto is_copy_constructible_v
			= tool::constraint<std::is_copy_constructible>;
		inline constexpr auto is_trivially_copy_constructible_v
			= tool::constraint<std::is_trivially_copy_constructible>;
		inline constexpr auto is_nothrow_copy_constructible_v
			= tool::constraint<std::is_nothrow_copy_constructible>;
		inline constexpr auto is_move_constructible_v
			= tool::constraint<std::is_move_constructible>;
		inline constexpr auto is_trivially_move_constructible_v
			= tool::constraint<std::is_trivially_move_constructible>;
		inline constexpr auto is_nothrow_move_constructible_v
			= tool::constraint<std::is_nothrow_move_constructible>;
		template<typename _Arg> inline constexpr auto is_assignable_v
			= tool::constraint<std::is_assignable, _Arg>;
		template<typename _Arg> inline constexpr auto is_trivially_assignable_v
			= tool::constraint<std::is_trivially_assignable, _Arg>;
		template<typename _Arg> inline constexpr auto is_nothrow_assignable_v
			= tool::constraint<std::is_nothrow_assignable, _Arg>;
		inline constexpr auto is_copy_assignable_v
			= tool::constraint<std::is_copy_assignable>;
		inline constexpr auto is_trivially_copy_assignable_v
			= tool::constraint<std::is_trivially_copy_assignable>;
		inline constexpr auto is_nothrow_copy_assignable_v
			= tool::constraint<std::is_nothrow_copy_assignable>;
		inline constexpr auto is_move_assignable_v
			= tool::constraint<std::is_move_assignable>;
		inline constexpr auto is_trivially_move_assignable_v
			= tool::constraint<std::is_trivially_move_assignable>;
		inline constexpr auto is_nothrow_move_assignable_v
			= tool::constraint<std::is_nothrow_move_assignable>;
		inline constexpr auto is_destructible_v
			= tool::constraint<std::is_destructible>;
		inline constexpr auto is_trivially_destructible_v
			= tool::constraint<std::is_trivially_destructible>;
		inline constexpr auto is_nothrow_destructible_v
			= tool::constraint<std::is_nothrow_destructible>;
		inline constexpr auto has_virtual_destructor_v
			= tool::constraint<std::has_virtual_destructor>;
		template<typename _Arg> inline constexpr auto is_swappable_with_v
			= tool::constraint<std::is_swappable_with, _Arg>;
		inline constexpr auto is_swappable_v
			= tool::constraint<std::is_swappable>;
		template<typename _Arg> inline constexpr auto is_nothrow_swappable_with_v
			= tool::constraint<std::is_nothrow_swappable_with, _Arg>;
		inline constexpr auto is_nothrow_swappable_v
			= tool::constraint<std::is_nothrow_swappable>;
		
		// type relationships. 
		
		template<typename _Arg> inline constexpr auto is_base_of_v
			= tool::constraint<std::is_base_of, _Arg>;
		template<typename _Arg> inline constexpr auto is_convertible_v
			= tool::constraint<std::is_convertible, _Arg>;
		template<typename _Arg> inline constexpr auto is_nothrow_convertible_v
			= tool::constraint<std::is_nothrow_convertible, _Arg>;
		template<typename _Arg> inline constexpr auto is_layout_compatible_v
			= tool::constraint<std::is_layout_compatible, _Arg>;
		template<typename _Arg> inline constexpr auto is_pointer_interconvertible_base_of_v
			= tool::constraint<std::is_pointer_interconvertible_base_of, _Arg>;
		template<typename... _Args> inline constexpr auto is_invocable_v
			= tool::constraint<std::is_invocable, _Args...>;
		template<typename _R, typename... _Args> inline constexpr auto is_invocable_r_v
			= []<typename _Functor> requires std::is_invocable_r<_R, _Functor, _Args...>::value (){};
		template<typename... _Args> inline constexpr auto is_nothrow_invocable_v
			= tool::constraint<std::is_nothrow_invocable, _Args...>;
		template<typename _R, typename... _Args> inline constexpr auto is_nothrow_invocable_r_v
			= []<typename _Functor> requires std::is_nothrow_invocable_r<_R, _Functor, _Args...>::value (){};
		
		// core language concepts. 
		
		template<typename _Type> inline constexpr auto same_v
			= []<std::same_as<_Type>>(){};
		template<typename _Type> inline constexpr auto derived_from_v
			= []<std::derived_from<_Type>>(){};
		inline constexpr auto signed_integral_v
			= []<std::signed_integral>(){};
		inline constexpr auto unsigned_integral_v
			= []<std::unsigned_integral>(){};
		template<typename _Type> inline constexpr auto convertible_to_v
			= []<std::convertible_to<_Type>>(){};
		
		// comparison concepts. 
		
		inline constexpr auto equality_comparable_v
			= []<std::equality_comparable>(){};
		template<typename _Type> inline constexpr auto equality_comparable_with_v
			= []<std::equality_comparable_with<_Type>>(){};
		inline constexpr auto totally_ordered_v
			= []<std::totally_ordered>(){};
		template<typename _Type> inline constexpr auto totally_ordered_with_v
			= []<std::totally_ordered_with<_Type>>(){};
		inline constexpr auto three_way_comparable_v
			= []<std::three_way_comparable>(){};
		template<typename _Type> inline constexpr auto three_way_comparable_with_v
			= []<std::three_way_comparable_with<_Type>>(){};
		
		// object concepts. 
		
		inline constexpr auto movable_v
			= []<std::movable>(){};
		inline constexpr auto copyable_v
			= []<std::copyable>(){};
		inline constexpr auto semiregular_v
			= []<std::semiregular>(){};
		inline constexpr auto regular_v
			= []<std::regular>(){};
		
		// range concepts. 
		
		inline constexpr auto range_v
			= []<std::ranges::range>(){};
		inline constexpr auto borrowed_range_v
			= []<std::ranges::borrowed_range>(){};
		inline constexpr auto sized_range_v
			= []<std::ranges::sized_range>(){};
		inline constexpr auto views_v
			= []<std::ranges::view>(){};
		inline constexpr auto input_range_v
			= []<std::ranges::input_range>(){};
		template<typename _Type> inline constexpr auto output_range_v
			= []<std::ranges::output_range<_Type>>(){};
		inline constexpr auto forward_range_v
			= []<std::ranges::forward_range>(){};
		inline constexpr auto bidirectional_range_v
			= []<std::ranges::bidirectional_range>(){};
		inline constexpr auto random_access_range_v
			= []<std::ranges::random_access_range>(){};
		inline constexpr auto contiguous_range_v
			= []<std::ranges::contiguous_range>(){};
		inline constexpr auto common_range_v
			= []<std::ranges::common_range>(){};
		inline constexpr auto viewable_range_v
			= []<std::ranges::viewable_range>(){};
		
		// fake concepts. 
		
		template<typename _Type> inline constexpr auto hold_v
			= []<fake::hold<_Type>>(){};
		template<typename _Type> inline constexpr auto like_v
			= []<fake::like<_Type>>(){};
		template<typename... _Type> inline constexpr auto meets_v
			= []<fake::meets<_Type...>>(){};
		template<typename... _Type> inline constexpr auto holds_v
			= []<fake::holds<_Type...>>(){};
		template<typename... _Type> inline constexpr auto likes_v
			= []<fake::likes<_Type...>>(){};
		template<template<typename...> typename _Tamplate> inline constexpr auto trait_v
			= []<fake::trait_c<_Tamplate>>(){};
		
	}
	
	namespace adaptor
	{
		
		namespace transformer
		{
			
			template<typename _Type>
			using key = typename std::remove_cvref_t<_Type>::key_type;
			
			template<typename _Type>
			using mapped = typename std::remove_cvref_t<_Type>::mapped_type;
			
			template<typename _Type>
			using first = typename std::remove_cvref_t<_Type>::first_type;
			
			template<typename _Type>
			requires requires{typename std::remove_cvref_t<_Type>::second_type;}
			using second = typename std::remove_cvref_t<_Type>::second_type;
			
			template<typename _Type>
			using element = typename std::remove_cvref_t<_Type>::element_type;
			
			template<typename _Type>
			using type = typename std::remove_cvref_t<_Type>::type;
			
			template<typename _Type, std::size_t _index>
			using index = std::tuple_element_t<_index, std::remove_cvref_t<_Type>>;
			
			namespace detail
			{
				
				template<typename _Type>
				struct try_take final{using type = _Type;};
				
				template<fake::pack_c _Type>
				struct try_take<_Type> final{using type = fake::take_t<_Type{}>;};
				
				template<fake::pazz_c _Type>
				struct try_take<_Type> final{using type = fake::take_t<_Type{}>;};
				
				template<typename _Type>
				struct take_all final{using type = _Type;};
				
				template<typename _Type>
				requires fake::pack_c<fake::take_t<_Type{}>> || fake::pazz_c<fake::take_t<_Type{}>>
				struct take_all<_Type> final{using type = typename take_all<fake::take_t<_Type{}>>::type;};
				
				template<fake::pack_c _Type>
				requires (fake::pack_c<fake::take_t<_Type{}>> == false && fake::pazz_c<fake::take_t<_Type{}>> == false)
				struct take_all<_Type> final{using type = fake::take_t<_Type{}>;};
				
				template<fake::pazz_c _Type>
				requires (fake::pack_c<fake::take_t<_Type{}>> == false && fake::pazz_c<fake::take_t<_Type{}>> == false)
				struct take_all<_Type> final{using type = fake::take_t<_Type{}>;};
				
			}
			
			template<typename _Type>
			using try_take_t = typename detail::try_take<_Type>::type;
			
			template<typename _Type>
			using take_all_t = typename detail::take_all<_Type>::type;
			
		}
		
		// type transformations. 
		
		inline constexpr auto key_v
			= tool::transform<transformer::key>;
		inline constexpr auto mapped_v
			= tool::transform<transformer::mapped>;
		inline constexpr auto first_v
			= tool::transform<transformer::first>;
		inline constexpr auto second_v
			= tool::transform<transformer::second>;
		inline constexpr auto element_v
			= tool::transform<transformer::element>;
		inline constexpr auto type_v
			= tool::transform<transformer::type>;
		template<std::size_t _index> inline constexpr auto index_v
			= tool::transform_constant<transformer::index, _index>;
		template<template<typename...> typename _Target> inline constexpr auto mimic_v
			= tool::transform_template<fake::mimic_t, _Target>;
		inline constexpr auto remove_cv_v
			= tool::transform<std::remove_cv_t>;
		inline constexpr auto remove_const_v
			= tool::transform<std::remove_const_t>;
		inline constexpr auto remove_volatile_v
			= tool::transform<std::remove_volatile_t>;
		inline constexpr auto add_cv_v
			= tool::transform<std::add_cv_t>;
		inline constexpr auto add_const_v
			= tool::transform<std::add_const_t>;
		inline constexpr auto add_volatile_v
			= tool::transform<std::add_volatile_t>;
		inline constexpr auto remove_reference_v
			= tool::transform<std::remove_reference_t>;
		inline constexpr auto remove_lvalue_reference_v
			= tool::transform<fake::remove_lvalue_reference_t>;
		inline constexpr auto remove_rvalue_reference_v
			= tool::transform<fake::remove_rvalue_reference_t>;
		inline constexpr auto add_lvalue_reference_v
			= tool::transform<std::add_lvalue_reference_t>;
		inline constexpr auto add_rvalue_reference_v
			= tool::transform<std::add_rvalue_reference_t>;
		inline constexpr auto make_signed_v
			= tool::transform<std::make_signed_t>;
		inline constexpr auto make_unsigned_v
			= tool::transform<std::make_unsigned_t>;
		inline constexpr auto remove_extent_v
			= tool::transform<std::remove_extent_t>;
		inline constexpr auto remove_all_extents_v
			= tool::transform<std::remove_all_extents_t>;
		inline constexpr auto remove_pointer_v
			= tool::transform<std::remove_pointer_t>;
		inline constexpr auto add_pointer_v
			= tool::transform<std::add_pointer_t>;
		inline constexpr auto decay_v
			= tool::transform<std::decay_t>;
		inline constexpr auto bare_v
			= tool::transform<fake::bare_t>;
		inline constexpr auto remove_cvref_v
			= tool::transform<std::remove_cvref_t>;
		template<typename... _Args> inline constexpr auto common_type_v
			= tool::transform<std::common_type_t, _Args...>;
		template<typename... _Args> inline constexpr auto common_reference_v
			= tool::transform<std::common_reference_t, _Args...>;
		inline constexpr auto underlying_type_v
			= tool::transform<std::underlying_type_t>;
		template<typename... _Args> inline constexpr auto invoke_result_v
			= tool::transform<std::invoke_result_t, _Args...>;
		inline constexpr auto pattern_v
			= tool::transform<fake::pattern_t>;
		inline constexpr auto pack_v
			= tool::transform<fake::pack_t>;
		inline constexpr auto pazz_v
			= tool::transform<fake::pazz_t>;
		inline constexpr auto take_v
			= tool::transform_auto<fake::take_t>;
		inline constexpr auto try_take_v
			= tool::transform<transformer::try_take_t>;
		inline constexpr auto take_all_v
			= tool::transform<transformer::take_all_t>;
		template<template<typename...> typename _Target, typename... _Appends> inline constexpr auto apply_v
			= tool::transform<_Target, _Appends...>;
		inline constexpr auto keep_v
			= tool::transform<std::type_identity_t>;
		template<typename _Type> inline constexpr auto replace_v
			= tool::transform<fake::type_t, _Type>;
		template<fake::creed_c auto _condition, fake::shift_c auto _true, fake::shift_c auto _false>
		inline constexpr auto conditional_v
			= tool::transform_condition<_condition, _true, _false>;
		
	}
	
	namespace traits
	{
		
		// primary type categories. 
		
		using void_d
			= op::just<adaptor::void_v>;
		using null_pointer_d
			= op::just<adaptor::null_pointer_v>;
		using integral_d
			= op::just<adaptor::integral_v>;
		using floating_point_d
			= op::just<adaptor::floating_point_v>;
		using array_d
			= op::just<adaptor::array_v>;
		using enum_d
			= op::just<adaptor::enum_v>;
		using union_d
			= op::just<adaptor::union_v>;
		using class_d
			= op::just<adaptor::class_v>;
		using function_d
			= op::just<adaptor::function_v>;
		using pointer_d
			= op::just<adaptor::pointer_v>;
		using lvalue_reference_d
			= op::just<adaptor::lvalue_reference_v>;
		using rvalue_reference_d
			= op::just<adaptor::rvalue_reference_v>;
		using member_object_pointer_d
			= op::just<adaptor::member_object_pointer_v>;
		using member_function_pointer_d
			= op::just<adaptor::member_function_pointer_v>;
		
		// composite type categories. 
		
		using fundamental_d
			= op::just<adaptor::fundamental_v>;
		using arithmetic_d
			= op::just<adaptor::arithmetic_v>;
		using scalar_d
			= op::just<adaptor::scalar_v>;
		using object_d
			= op::just<adaptor::object_v>;
		using compound_d
			= op::just<adaptor::compound_v>;
		using reference_d
			= op::just<adaptor::reference_v>;
		using member_pointer_d
			= op::just<adaptor::member_pointer_v>;
		
		// type properties. 
		
		using const_d
			= op::just<adaptor::const_v>;
		using volatile_d
			= op::just<adaptor::volatile_v>;
		using trivial_d
			= op::just<adaptor::trivial_v>;
		using trivially_copyable_d
			= op::just<adaptor::trivially_copyable_v>;
		using standard_layout_d
			= op::just<adaptor::standard_layout_v>;
		using unique_object_representations_d
			= op::just<adaptor::has_unique_object_representations_v>;
		using empty_d
			= op::just<adaptor::empty_v>;
		using polymorphic_d
			= op::just<adaptor::polymorphic_v>;
		using abstract_d
			= op::just<adaptor::abstract_v>;
		using final_d
			= op::just<adaptor::final_v>;
		using aggregate_d
			= op::just<adaptor::aggregate_v>;
		using signed_d
			= op::just<adaptor::signed_v>;
		using unsigned_d
			= op::just<adaptor::unsigned_v>;
		using bounded_array_d
			= op::just<adaptor::bounded_array_v>;
		using unbounded_array_d
			= op::just<adaptor::unbounded_array_v>;
		
		// supported operations. 
		
		template<typename... _Args> using constructible_d
			= op::just<adaptor::is_constructible_v<_Args...>>;
		template<typename... _Args> using trivially_constructible_d
			= op::just<adaptor::is_trivially_constructible_v<_Args...>>;
		template<typename... _Args> using nothrow_constructible_d
			= op::just<adaptor::is_nothrow_constructible_v<_Args...>>;
		using default_constructible_d
			= op::just<adaptor::is_default_constructible_v>;
		using trivially_default_constructible_d
			= op::just<adaptor::is_trivially_default_constructible_v>;
		using nothrow_default_constructible_d
			= op::just<adaptor::is_nothrow_default_constructible_v>;
		using copy_constructible_d
			= op::just<adaptor::is_copy_constructible_v>;
		using trivially_copy_constructible_d
			= op::just<adaptor::is_trivially_copy_constructible_v>;
		using nothrow_copy_constructible_d
			= op::just<adaptor::is_nothrow_copy_constructible_v>;
		using move_constructible_d
			= op::just<adaptor::is_move_constructible_v>;
		using trivially_move_constructible_d
			= op::just<adaptor::is_trivially_move_constructible_v>;
		using nothrow_move_constructible_d
			= op::just<adaptor::is_nothrow_move_constructible_v>;
		template<typename _Arg> using assignable_d
			= op::just<adaptor::is_assignable_v<_Arg>>;
		template<typename _Arg> using trivially_assignable_d
			= op::just<adaptor::is_trivially_assignable_v<_Arg>>;
		template<typename _Arg> using nothrow_assignable_d
			= op::just<adaptor::is_nothrow_assignable_v<_Arg>>;
		using copy_assignable_d
			= op::just<adaptor::is_copy_assignable_v>;
		using trivially_copy_assignable_d
			= op::just<adaptor::is_trivially_copy_assignable_v>;
		using nothrow_copy_assignable_d
			= op::just<adaptor::is_nothrow_copy_assignable_v>;
		using move_assignable_d
			= op::just<adaptor::is_move_assignable_v>;
		using trivially_move_assignable_d
			= op::just<adaptor::is_trivially_move_assignable_v>;
		using nothrow_move_assignable_d
			= op::just<adaptor::is_nothrow_move_assignable_v>;
		using destructible_d
			= op::just<adaptor::is_destructible_v>;
		using trivially_destructible_d
			= op::just<adaptor::is_trivially_destructible_v>;
		using nothrow_destructible_d
			= op::just<adaptor::is_nothrow_destructible_v>;
		using virtual_destructor_d
			= op::just<adaptor::has_virtual_destructor_v>;
		template<typename _Arg> using swappable_with_d
			= op::just<adaptor::is_swappable_with_v<_Arg>>;
		using swappable_d
			= op::just<adaptor::is_swappable_v>;
		template<typename _Arg> using nothrow_swappable_with_d
			= op::just<adaptor::is_nothrow_swappable_with_v<_Arg>>;
		using nothrow_swappable_d
			= op::just<adaptor::is_nothrow_swappable_v>;
		
		// type relationships. 
		
		template<typename _Arg> using base_of_d
			= op::just<adaptor::is_base_of_v<_Arg>>;
		template<typename _Arg> using convertible_d
			= op::just<adaptor::is_convertible_v<_Arg>>;
		template<typename _Arg> using nothrow_convertible_d
			= op::just<adaptor::is_nothrow_convertible_v<_Arg>>;
		template<typename _Arg> using layout_compatible_d
			= op::just<adaptor::is_layout_compatible_v<_Arg>>;
		template<typename _Arg> using pointer_interconvertible_base_of_d
			= op::just<adaptor::is_pointer_interconvertible_base_of_v<_Arg>>;
		template<typename... _Args> using invocable_d
			= op::just<adaptor::is_invocable_v<_Args...>>;
		template<typename _R, typename... _Args> using invocable_r_d
			= op::just<adaptor::is_invocable_r_v<_R, _Args...>>;
		template<typename... _Args> using nothrow_invocable_d
			= op::just<adaptor::is_nothrow_invocable_v<_Args...>>;
		template<typename _R, typename... _Args> using nothrow_invocable_r_d
			= op::just<adaptor::is_nothrow_invocable_r_v<_R, _Args...>>;
		
		// core language concepts. 
		
		template<typename _Type> using same_d
			= op::just<adaptor::same_v<_Type>>;
		template<typename _Type> using derived_from_d
			= op::just<adaptor::derived_from_v<_Type>>;
		using signed_integral_d
			= op::just<adaptor::signed_integral_v>;
		using unsigned_integral_d
			= op::just<adaptor::unsigned_integral_v>;
		template<typename _Type> using convertible_to_d
			= op::just<adaptor::convertible_to_v<_Type>>;
		
		template<typename _Type>
		struct convertible_from_d final : fake::detect::op::creed{
			constexpr bool operator()(tool::pass_c auto _e, auto &&..._args) const noexcept{
				return std::convertible_to<_Type, fake::take_t<decltype(_e){}>>;
			}
		};
		
		// comparison concepts. 
		
		using equality_comparable_d
			= op::just<adaptor::equality_comparable_v>;
		template<typename _Type> using equality_comparable_with_d
			= op::just<adaptor::equality_comparable_with_v<_Type>>;
		using totally_ordered_d
			= op::just<adaptor::totally_ordered_v>;
		template<typename _Type> using totally_ordered_with_d
			= op::just<adaptor::totally_ordered_with_v<_Type>>;
		using three_way_comparable_d
			= op::just<adaptor::three_way_comparable_v>;
		template<typename _Type> using three_way_comparable_with_d
			= op::just<adaptor::three_way_comparable_with_v<_Type>>;
		
		// object concepts. 
		
		using movable_d
			= op::just<adaptor::movable_v>;
		using copyable_d
			= op::just<adaptor::copyable_v>;
		using semiregular_d
			= op::just<adaptor::semiregular_v>;
		using regular_d
			= op::just<adaptor::regular_v>;
		
		// range concepts. 
		
		using range_d
			= op::just<adaptor::range_v>;
		using borrowed_range_d
			= op::just<adaptor::borrowed_range_v>;
		using sized_range_d
			= op::just<adaptor::sized_range_v>;
		using views_d
			= op::just<adaptor::views_v>;
		using input_range_d
			= op::just<adaptor::input_range_v>;
		template<typename _Type> using output_range_d
			= op::just<adaptor::output_range_v<_Type>>;
		using forward_range_d
			= op::just<adaptor::forward_range_v>;
		using bidirectional_range_d
			= op::just<adaptor::bidirectional_range_v>;
		using random_access_range_d
			= op::just<adaptor::random_access_range_v>;
		using contiguous_range_d
			= op::just<adaptor::contiguous_range_v>;
		using common_range_d
			= op::just<adaptor::common_range_v>;
		using viewable_range_d
			= op::just<adaptor::viewable_range_v>;
		
		// fake concepts. 
		
		template<typename _Type> using hold_d
			= op::just<adaptor::hold_v<_Type>>;
		template<typename _Type> using like_d
			= op::just<adaptor::like_v<_Type>>;
		template<typename... _Type> using meets_d
			= op::just<adaptor::meets_v<_Type...>>;
		template<typename... _Type> using holds_d
			= op::just<adaptor::holds_v<_Type...>>;
		template<typename... _Type> using likes_d
			= op::just<adaptor::likes_v<_Type...>>;
		template<template<typename...> typename _Tamplate> using trait_d
			= op::just<adaptor::trait_v<_Tamplate>>;
		
	}
	
	template<typename _Type, auto _op>
	concept as = requires{requires _op(fake::pazz_v<_Type>);} || requires{requires _op(fake::pack_v<_Type>);};
	
	namespace traits
	{
		
		namespace tool
		{
			
			template<auto _lambda>
			inline constexpr auto repass = [](fake::detect::tool::pass_c auto _pass, auto &&..._args){
				if constexpr(requires{_lambda(_pass, std::forward<decltype(_args)>(_args)...);}){
					constexpr fake::detect::tool::pass_c auto result = _lambda(
						_pass,
						std::forward<decltype(_args)>(_args)...
					);
					
					return fake::detect::tool::repass_v<decltype(_pass), fake::take_t<result>>;
				}
				else{
					constexpr fake::detect::tool::pass_c auto result = _lambda(_pass);
					
					return fake::detect::tool::repass_v<decltype(_pass), fake::take_t<result>>;
				}
			};
			
		}
		
		// type transformations. 
		template<auto _lambda> using just_p
			= fake::detect::op::rein<fake::detect::traits::tool::repass<_lambda>>;
		using key_p
			= fake::detect::op::rein<adaptor::key_v>;
		using mapped_p
			= fake::detect::op::rein<adaptor::mapped_v>;
		using first_p
			= fake::detect::op::rein<adaptor::first_v>;
		using second_p
			= fake::detect::op::rein<adaptor::second_v>;
		using element_p
			= fake::detect::op::rein<adaptor::element_v>;
		using type_p
			= fake::detect::op::rein<adaptor::type_v>;
		template<std::size_t _index> using index_p
			= fake::detect::op::rein<adaptor::index_v<_index>>;
		template<template<typename...> typename _Target> using mimic_p
			= fake::detect::op::rein<adaptor::mimic_v<_Target>>;
		using remove_cv_p
			= fake::detect::op::rein<adaptor::remove_cv_v>;
		using remove_const_p
			= fake::detect::op::rein<adaptor::remove_const_v>;
		using remove_volatile_p
			= fake::detect::op::rein<adaptor::remove_volatile_v>;
		using add_cv_p
			= fake::detect::op::rein<adaptor::add_cv_v>;
		using add_const_p
			= fake::detect::op::rein<adaptor::add_const_v>;
		using add_volatile_p
			= fake::detect::op::rein<adaptor::add_volatile_v>;
		using remove_reference_p
			= fake::detect::op::rein<adaptor::remove_reference_v>;
		using remove_lvalue_reference_p
			= fake::detect::op::rein<adaptor::remove_lvalue_reference_v>;
		using remove_rvalue_reference_p
			= fake::detect::op::rein<adaptor::remove_rvalue_reference_v>;
		using add_lvalue_reference_p
			= fake::detect::op::rein<adaptor::add_lvalue_reference_v>;
		using add_rvalue_reference_p
			= fake::detect::op::rein<adaptor::add_rvalue_reference_v>;
		using make_signed_p
			= fake::detect::op::rein<adaptor::make_signed_v>;
		using make_unsigned_p
			= fake::detect::op::rein<adaptor::make_unsigned_v>;
		using remove_extent_p
			= fake::detect::op::rein<adaptor::remove_extent_v>;
		using remove_all_extents_p
			= fake::detect::op::rein<adaptor::remove_all_extents_v>;
		using remove_pointer_p
			= fake::detect::op::rein<adaptor::remove_pointer_v>;
		using add_pointer_p
			= fake::detect::op::rein<adaptor::add_pointer_v>;
		using decay_p
			= fake::detect::op::rein<adaptor::decay_v>;
		using bare_p
			= fake::detect::op::rein<adaptor::bare_v>;
		using remove_cvref_p
			= fake::detect::op::rein<adaptor::remove_cvref_v>;
		template<typename... _Args> using common_type_p
			= fake::detect::op::rein<adaptor::common_type_v<_Args...>>;
		template<typename... _Args> using common_reference_p
			= fake::detect::op::rein<adaptor::common_reference_v<_Args...>>;
		using underlying_type_p
			= fake::detect::op::rein<adaptor::underlying_type_v>;
		template<typename... _Args> using invoke_result_p
			= fake::detect::op::rein<adaptor::invoke_result_v<_Args...>>;
		using pattern_p
			= fake::detect::op::rein<adaptor::pattern_v>;
		using pack_p
			= fake::detect::op::rein<adaptor::pack_v>;
		using pazz_p
			= fake::detect::op::rein<adaptor::pazz_v>;
		using take_p
			= fake::detect::op::rein<adaptor::take_v>;
		using try_take_p
			= fake::detect::op::rein<adaptor::try_take_v>;
		using take_all_p
			= fake::detect::op::rein<adaptor::take_all_v>;
		template<template<typename...> typename _Target, typename... _Appends> using apply_p
			= fake::detect::op::rein<adaptor::apply_v<_Target, _Appends...>>;
		using keep_p
			= fake::detect::op::rein<adaptor::keep_v>;
		template<typename _Type> using replace_p
			= fake::detect::op::rein<adaptor::replace_v<_Type>>;
		template<fake::creed_c auto _condition, fake::shift_c auto _true, fake::shift_c auto _false> using conditional_p
			= fake::detect::op::rein<adaptor::conditional_v<_condition, _true, _false>>;
		
	}
	
}

namespace fake
{
	
	template<auto _lambda> inline constexpr fake::detect::op::just<_lambda>
		just_d;
	template<auto _lambda> inline constexpr fake::detect::op::must<_lambda>
		must_d;
	
	// primary type categories. 
	
	inline constexpr fake::detect::traits::void_d
		void_d;
	inline constexpr fake::detect::traits::null_pointer_d
		null_pointer_d;
	inline constexpr fake::detect::traits::integral_d
		integral_d;
	inline constexpr fake::detect::traits::floating_point_d
		floating_point_d;
	inline constexpr fake::detect::traits::array_d
		array_d;
	inline constexpr fake::detect::traits::enum_d
		enum_d;
	inline constexpr fake::detect::traits::union_d
		union_d;
	inline constexpr fake::detect::traits::class_d
		class_d;
	inline constexpr fake::detect::traits::function_d
		function_d;
	inline constexpr fake::detect::traits::pointer_d
		pointer_d;
	inline constexpr fake::detect::traits::lvalue_reference_d
		lvalue_reference_d;
	inline constexpr fake::detect::traits::rvalue_reference_d
		rvalue_reference_d;
	inline constexpr fake::detect::traits::member_object_pointer_d
		member_object_pointer_d;
	inline constexpr fake::detect::traits::member_function_pointer_d
		member_function_pointer_d;
	
	// fake type categories. 
	
	inline constexpr fake::creed_c auto null_d = fake::just_d<[]<fake::like<fake::null_t>>(){}>;
	inline constexpr fake::creed_c auto view_d = fake::just_d<[]<fake::view_c>(){}>;
	inline constexpr fake::creed_c auto flat_d = fake::just_d<[]<fake::flat_c>(){}>;
	inline constexpr fake::creed_c auto query_d = fake::just_d<[]<fake::query_c>(){}>;
	inline constexpr fake::creed_c auto pack_d = fake::just_d<[]<fake::pack_c>(){}>;
	inline constexpr fake::creed_c auto pazz_d = fake::just_d<[]<fake::pazz_c>(){}>;
	inline constexpr fake::creed_c auto mezz_d = fake::just_d<[]<fake::mezz_c>(){}>;
	inline constexpr fake::creed_c auto gene_d = fake::just_d<[]<fake::gene_c>(){}>;
	inline constexpr fake::creed_c auto debug_d = fake::just_d<[]<fake::debug_c>(){}>;
	inline constexpr fake::creed_c auto char_array_d = fake::just_d<[]<fake::char_array_c>(){}>;
	inline constexpr fake::creed_c auto constant_d = fake::just_d<[]<fake::constant_c>(){}>;
	inline constexpr fake::creed_c auto pair_d = fake::just_d<[]<fake::pair_c>(){}>;
	inline constexpr fake::creed_c auto tuple_d = fake::just_d<[]<fake::tuple_c>(){}>;
	inline constexpr fake::creed_c auto combine_d = fake::just_d<[]<fake::combine_c>(){}>;
	inline constexpr fake::creed_c auto std_array_d = fake::just_d<[]<fake::std_array_c>(){}>;
	inline constexpr fake::creed_c auto container_d = fake::just_d<[]<fake::container_c>(){}>;
	inline constexpr fake::creed_c auto optional_d = fake::just_d<[]<fake::optional_c>(){}>;
	inline constexpr fake::creed_c auto variant_d = fake::just_d<[]<fake::variant_c>(){}>;
	
	// composite type categories. 
	
	inline constexpr fake::detect::traits::fundamental_d
		fundamental_d;
	inline constexpr fake::detect::traits::arithmetic_d
		arithmetic_d;
	inline constexpr fake::detect::traits::scalar_d
		scalar_d;
	inline constexpr fake::detect::traits::object_d
		object_d;
	inline constexpr fake::detect::traits::compound_d
		compound_d;
	inline constexpr fake::detect::traits::reference_d
		reference_d;
	inline constexpr fake::detect::traits::member_pointer_d
		member_pointer_d;
	
	// type properties. 
	
	inline constexpr fake::detect::traits::const_d
		const_d;
	inline constexpr fake::detect::traits::volatile_d
		volatile_d;
	inline constexpr fake::detect::traits::trivial_d
		trivial_d;
	inline constexpr fake::detect::traits::trivially_copyable_d
		trivially_copyable_d;
	inline constexpr fake::detect::traits::standard_layout_d
		standard_layout_d;
	inline constexpr fake::detect::traits::unique_object_representations_d
		unique_object_representations_d;
	inline constexpr fake::detect::traits::empty_d
		empty_d;
	inline constexpr fake::detect::traits::polymorphic_d
		polymorphic_d;
	inline constexpr fake::detect::traits::abstract_d
		abstract_d;
	inline constexpr fake::detect::traits::final_d
		final_d;
	inline constexpr fake::detect::traits::aggregate_d
		aggregate_d;
	inline constexpr fake::detect::traits::signed_d
		signed_d;
	inline constexpr fake::detect::traits::unsigned_d
		unsigned_d;
	inline constexpr fake::detect::traits::bounded_array_d
		bounded_array_d;
	inline constexpr fake::detect::traits::unbounded_array_d
		unbounded_array_d;
	
	// supported operations. 
	
	template<typename... _Args> inline constexpr fake::detect::traits::constructible_d<_Args...>
		constructible_d;
	template<typename... _Args> inline constexpr fake::detect::traits::trivially_constructible_d<_Args...>
		trivially_constructible_d;
	template<typename... _Args> inline constexpr fake::detect::traits::nothrow_constructible_d<_Args...>
		nothrow_constructible_d;
	inline constexpr fake::detect::traits::default_constructible_d
		default_constructible_d;
	inline constexpr fake::detect::traits::trivially_default_constructible_d
		trivially_default_constructible_d;
	inline constexpr fake::detect::traits::nothrow_default_constructible_d
		nothrow_default_constructible_d;
	inline constexpr fake::detect::traits::copy_constructible_d
		copy_constructible_d;
	inline constexpr fake::detect::traits::trivially_copy_constructible_d
		trivially_copy_constructible_d;
	inline constexpr fake::detect::traits::nothrow_copy_constructible_d
		nothrow_copy_constructible_d;
	inline constexpr fake::detect::traits::move_constructible_d
		move_constructible_d;
	inline constexpr fake::detect::traits::trivially_move_constructible_d
		trivially_move_constructible_d;
	inline constexpr fake::detect::traits::nothrow_move_constructible_d
		nothrow_move_constructible_d;
	template<typename _Arg> inline constexpr fake::detect::traits::assignable_d<_Arg>
		assignable_d;
	template<typename _Arg> inline constexpr fake::detect::traits::trivially_assignable_d<_Arg>
		trivially_assignable_d;
	template<typename _Arg> inline constexpr fake::detect::traits::nothrow_assignable_d<_Arg>
		nothrow_assignable_d;
	inline constexpr fake::detect::traits::copy_assignable_d
		copy_assignable_d;
	inline constexpr fake::detect::traits::trivially_copy_assignable_d
		trivially_copy_assignable_d;
	inline constexpr fake::detect::traits::nothrow_copy_assignable_d
		nothrow_copy_assignable_d;
	inline constexpr fake::detect::traits::move_assignable_d
		move_assignable_d;
	inline constexpr fake::detect::traits::trivially_move_assignable_d
		trivially_move_assignable_d;
	inline constexpr fake::detect::traits::nothrow_move_assignable_d
		nothrow_move_assignable_d;
	inline constexpr fake::detect::traits::destructible_d
		destructible_d;
	inline constexpr fake::detect::traits::trivially_destructible_d
		trivially_destructible_d;
	inline constexpr fake::detect::traits::nothrow_destructible_d
		nothrow_destructible_d;
	inline constexpr fake::detect::traits::virtual_destructor_d
		virtual_destructor_d;
	template<typename _Arg> inline constexpr fake::detect::traits::swappable_with_d<_Arg>
		swappable_with_d;
	inline constexpr fake::detect::traits::swappable_d
		swappable_d;
	template<typename _Arg> inline constexpr fake::detect::traits::nothrow_swappable_with_d<_Arg>
		nothrow_swappable_with_d;
	inline constexpr fake::detect::traits::nothrow_swappable_d
		nothrow_swappable_d;
	
	// type relationships. 
	
	template<typename _Arg> inline constexpr fake::detect::traits::base_of_d<_Arg>
		base_of_d;
	template<typename _Arg> inline constexpr fake::detect::traits::convertible_d<_Arg>
		convertible_d;
	template<typename _Arg> inline constexpr fake::detect::traits::nothrow_convertible_d<_Arg>
		nothrow_convertible_d;
	template<typename _Arg> inline constexpr fake::detect::traits::layout_compatible_d<_Arg>
		layout_compatible_d;
	template<typename _Arg> inline constexpr fake::detect::traits::pointer_interconvertible_base_of_d<_Arg>
		pointer_interconvertible_base_of_d;
	template<typename... _Args> inline constexpr fake::detect::traits::invocable_d<_Args...>
		invocable_d;
	template<typename _R, typename... _Args> inline constexpr fake::detect::traits::invocable_r_d<_R, _Args...>
		invocable_r_d;
	template<typename... _Args> inline constexpr fake::detect::traits::nothrow_invocable_d<_Args...>
		nothrow_invocable_d;
	template<typename _R, typename... _Args> inline constexpr fake::detect::traits::nothrow_invocable_r_d<_R, _Args...>
		nothrow_invocable_r_d;
	
	// core language concepts. 
	
	template<typename _Type> inline constexpr fake::detect::traits::same_d<_Type>
		same_d;
	template<typename _Type> inline constexpr fake::detect::traits::derived_from_d<_Type>
		derived_from_d;
	inline constexpr fake::detect::traits::signed_integral_d
		signed_integral_d;
	inline constexpr fake::detect::traits::unsigned_integral_d
		unsigned_integral_d;
	template<typename _Type> inline constexpr fake::detect::traits::convertible_to_d<_Type>
		convertible_to_d;
	template<typename _Type> inline constexpr fake::detect::traits::convertible_from_d<_Type>
		convertible_from_d;
	
	// comparison concepts. 
	
	inline constexpr fake::detect::traits::equality_comparable_d
		equality_comparable_d;
	template<typename _Type> inline constexpr fake::detect::traits::equality_comparable_with_d<_Type>
		equality_comparable_with_d;
	inline constexpr fake::detect::traits::totally_ordered_d
		totally_ordered_d;
	template<typename _Type> inline constexpr fake::detect::traits::totally_ordered_with_d<_Type>
		totally_ordered_with_d;
	inline constexpr fake::detect::traits::three_way_comparable_d
		three_way_comparable_d;
	template<typename _Type> inline constexpr fake::detect::traits::three_way_comparable_with_d<_Type>
		three_way_comparable_with_d;
	
	// object concepts. 
	
	inline constexpr fake::detect::traits::movable_d
		movable_d;
	inline constexpr fake::detect::traits::copyable_d
		copyable_d;
	inline constexpr fake::detect::traits::semiregular_d
		semiregular_d;
	inline constexpr fake::detect::traits::regular_d
		regular_d;
	
	// range concepts. 
	
	inline constexpr fake::detect::traits::range_d
		range_d;
	inline constexpr fake::detect::traits::borrowed_range_d
		borrowed_range_d;
	inline constexpr fake::detect::traits::sized_range_d
		sized_range_d;
	inline constexpr fake::detect::traits::views_d
		views_d;
	inline constexpr fake::detect::traits::input_range_d
		input_range_d;
	template<typename _Type> inline constexpr fake::detect::traits::output_range_d<_Type>
		output_range_d;
	inline constexpr fake::detect::traits::forward_range_d
		forward_range_d;
	inline constexpr fake::detect::traits::bidirectional_range_d
		bidirectional_range_d;
	inline constexpr fake::detect::traits::random_access_range_d
		random_access_range_d;
	inline constexpr fake::detect::traits::contiguous_range_d
		contiguous_range_d;
	inline constexpr fake::detect::traits::common_range_d
		common_range_d;
	inline constexpr fake::detect::traits::viewable_range_d
		viewable_range_d;
	
	// fake concepts. 
	
	template<typename _Type> inline constexpr fake::detect::traits::hold_d<_Type>
		hold_d;
	template<typename _Type> inline constexpr fake::detect::traits::like_d<_Type>
		like_d;
	template<typename... _Type> inline constexpr fake::detect::traits::meets_d<_Type...>
		meets_d;
	template<typename... _Type> inline constexpr fake::detect::traits::holds_d<_Type...>
		holds_d;
	template<typename... _Type> inline constexpr fake::detect::traits::likes_d<_Type...>
		likes_d;
	template<template<typename...> typename _Tamplate> inline constexpr fake::detect::traits::trait_d<_Tamplate>
		trait_d;
	
}

namespace fake
{
	
	// type transformations. 
	
	template<auto _lambda> inline constexpr fake::detect::traits::just_p<_lambda> just_p;
	
	inline constexpr fake::detect::traits::key_p key_p;
	
	inline constexpr fake::detect::traits::mapped_p mapped_p;
	
	inline constexpr fake::detect::traits::first_p first_p;
	
	inline constexpr fake::detect::traits::second_p second_p;
	
	inline constexpr fake::detect::traits::element_p element_p;
	
	inline constexpr fake::detect::traits::type_p type_p;
	
	template<std::size_t _index> inline constexpr fake::detect::traits::index_p<_index> index_p;
	
	template<template<typename...> typename _Target> inline constexpr fake::detect::traits::mimic_p<_Target> mimic_p;
	
	inline constexpr fake::detect::traits::remove_cv_p remove_cv_p;
	
	inline constexpr fake::detect::traits::remove_const_p remove_const_p;
	
	inline constexpr fake::detect::traits::remove_volatile_p remove_volatile_p;
	
	inline constexpr fake::detect::traits::add_cv_p add_cv_p;
	
	inline constexpr fake::detect::traits::add_const_p add_const_p;
	
	inline constexpr fake::detect::traits::add_volatile_p add_volatile_p;
	
	inline constexpr fake::detect::traits::remove_reference_p remove_reference_p;
	
	inline constexpr fake::detect::traits::remove_lvalue_reference_p remove_lvalue_reference_p;
	
	inline constexpr fake::detect::traits::remove_rvalue_reference_p remove_rvalue_reference_p;
	
	inline constexpr fake::detect::traits::add_lvalue_reference_p add_lvalue_reference_p;
	
	inline constexpr fake::detect::traits::add_rvalue_reference_p add_rvalue_reference_p;
	
	inline constexpr fake::detect::traits::make_signed_p make_signed_p;
	
	inline constexpr fake::detect::traits::make_unsigned_p make_unsigned_p;
	
	inline constexpr fake::detect::traits::remove_extent_p remove_extent_p;
	
	inline constexpr fake::detect::traits::remove_all_extents_p remove_all_extents_p;
	
	inline constexpr fake::detect::traits::remove_pointer_p remove_pointer_p;
	
	inline constexpr fake::detect::traits::add_pointer_p add_pointer_p;
	
	inline constexpr fake::detect::traits::decay_p decay_p;
	
	inline constexpr fake::detect::traits::bare_p bare_p;
	
	inline constexpr fake::detect::traits::remove_cvref_p remove_cvref_p;
	
	template<typename... _Args> inline constexpr fake::detect::traits::common_type_p<_Args...> common_type_p;
	
	template<typename... _Args> inline constexpr fake::detect::traits::common_reference_p<_Args...> common_reference_p;
	
	inline constexpr fake::detect::traits::underlying_type_p underlying_type_p;
	
	template<typename... _Args> inline constexpr fake::detect::traits::invoke_result_p<_Args...> invoke_result_p;
	
	inline constexpr fake::detect::traits::pattern_p pattern_p;
	
	inline constexpr fake::detect::traits::pack_p pack_p;
	
	inline constexpr fake::detect::traits::pazz_p pazz_p;
	
	inline constexpr fake::detect::traits::take_p take_p;
	
	inline constexpr fake::detect::traits::try_take_p try_take_p;
	
	inline constexpr fake::detect::traits::take_all_p take_all_p;
	
	template<template<typename...> typename _Target, typename... _Appends>
	inline constexpr fake::detect::traits::apply_p<_Target, _Appends...> apply_p;
	
	inline constexpr fake::detect::traits::keep_p keep_p;
	
	template<typename _Type> inline constexpr fake::detect::traits::replace_p<_Type> replace_p;
	
	template<fake::creed_c auto _condition, fake::shift_c auto _true, fake::shift_c auto _false>
	inline constexpr fake::detect::traits::conditional_p<_condition, _true, _false> conditional_p;
	
}

#include "pop.h"

#endif /*__FAKE_DETECT_H__*/ 
