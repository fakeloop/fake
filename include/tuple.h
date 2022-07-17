#ifndef __FAKE_TUPLE_H__
#define __FAKE_TUPLE_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "traits.h"

namespace fake::tuple
{
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct match_index final{
	private:
		static consteval auto impl(){
			using result_t = typename decltype(
				[]<std::size_t... _Index>(std::index_sequence<_Index...>){
					return fake::pack_v<
						decltype(
							std::tuple_cat(
								std::declval<
									std::conditional_t<
										_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>),
										std::tuple<std::integral_constant<std::size_t, _Index>>,
										std::tuple<>
									>
								>()...
							)
						)
					>;
				}(std::make_index_sequence<std::tuple_size_v<_Tuple>>())
			)::type;
			
			if constexpr(std::tuple_size_v<result_t>)
				return fake::pack_v<std::tuple_element_t<0, result_t>>;
			else
				return fake::pack_v<fake::null_t>;
		}
		
	public:
		using type = typename decltype(impl())::type;
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	using match_index_t = typename match_index<_Tuple, _functor>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct find_if final{
	private:
		static consteval auto impl(){
			using result_t = typename decltype(
				[]<std::size_t... _Index>(std::index_sequence<_Index...>){
					return fake::pack_v<
						decltype(
							std::tuple_cat(
								std::declval<
									std::conditional_t<
										_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>),
										std::tuple<std::tuple_element_t<_Index, _Tuple>>,
										std::tuple<>
									>
								>()...
							)
						)
					>;
				}(std::make_index_sequence<std::tuple_size_v<_Tuple>>())
			)::type;
			
			if constexpr(std::tuple_size_v<result_t>)
				return fake::pack_v<std::tuple_element_t<0, result_t>>;
			else
				return fake::pack_v<fake::null_t>;
		}
		
	public:
		using type = typename decltype(impl())::type;
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	using find_if_t = typename find_if<_Tuple, _functor>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	constexpr auto find_if_v = fake::pack_v<find_if_t<_Tuple, _functor>>;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct contains final{
	private:
		static consteval auto impl(){
			return []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return (_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>) || ...);
			}(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
	public:
		static constexpr bool value = impl();
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	constexpr auto contains_v = contains<_Tuple, _functor>::value;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct scale final{
	private:
		static consteval auto impl(){
			return []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return fake::pack_v<
					decltype(
						std::tuple_cat(
							std::declval<
								typename decltype(_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>))::type
							>()...
						)
					)
				>;
			}(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
	public:
		using type = typename decltype(impl())::type;
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	using scale_t = typename scale<_Tuple, _functor>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	constexpr auto scale_v = fake::pack_v<scale_t<_Tuple, _functor>>;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct transform final{
	private:
		static consteval auto impl(){
			return []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return fake::pack_v<
					decltype(
						std::tuple_cat(
							std::declval<
								std::tuple<
									typename decltype(
										_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>)
									)::type
								>
							>()...
						)
					)
				>;
			}(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
	public:
		using type = typename decltype(impl())::type;
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	using transform_t = typename transform<_Tuple, _functor>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	constexpr auto transform_v = fake::pack_v<transform_t<_Tuple, _functor>>;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct select_if final{
	private:
		static consteval auto impl(){
			return []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return fake::pack_v<
					decltype(
						std::tuple_cat(
							std::declval<
								std::conditional_t<
									_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>),
									std::tuple<std::tuple_element_t<_Index, _Tuple>>,
									std::tuple<>
								>
							>()...
						)
					)
				>;
			}(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
	public:
		using type = typename decltype(impl())::type;
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	using select_if_t = typename select_if<_Tuple, _functor>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	constexpr auto select_if_v = fake::pack_v<select_if_t<_Tuple, _functor>>;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct erase_if final{
		private:
		static consteval auto impl(){
			return []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return fake::pack_v<
					decltype(
						std::tuple_cat(
							std::declval<
								std::conditional_t<
									_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>),
									std::tuple<>,
									std::tuple<std::tuple_element_t<_Index, _Tuple>>
								>
							>()...
						)
					)
				>;
			}(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
	public:
		using type = typename decltype(impl())::type;
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	using erase_if_t = typename erase_if<_Tuple, _functor>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	constexpr auto erase_if_v = fake::pack_v<erase_if_t<_Tuple, _functor>>;
	
	template<fake::tuple_c... _Tuples>
	struct concat final{
	private:
		static consteval auto impl(){
			return fake::pack_v<decltype(std::tuple_cat(std::declval<_Tuples>()...))>;
		}
		
	public:
		using type = typename decltype(impl())::type;
	};
	
	template<fake::tuple_c... _Tuples>
	using concat_t = typename concat<_Tuples...>::type;
	
	template<fake::tuple_c... _Tuples>
	constexpr auto concat_v = fake::pack_v<concat_t<_Tuples...>>;
	
	template<fake::tuple_c _Tuple>
	struct shrink final{
		private:
		static consteval auto impl(){
			return []<typename... _Type, std::size_t... _Indices>(
				fake::type_package<std::tuple<_Type...>>,
				std::index_sequence<_Indices...>
			){
				using tuple_t = std::tuple<std::pair<_Type, std::integral_constant<std::size_t, _Indices>>...>;
				constexpr auto inner = []<typename _Key, std::size_t _Index>(
					fake::type_package<std::pair<_Key, std::integral_constant<std::size_t, _Index>>>
				){
					constexpr auto find = []<typename _K, std::size_t _I>(
						fake::type_package<std::pair<_K, std::integral_constant<std::size_t, _I>>>
					){
						return std::is_same_v<_Key, _K> && _I < _Index;
					};
					constexpr bool found = fake::tuple::contains_v<tuple_t, find>;
					return fake::pack_v<std::conditional_t<found, std::tuple<>, std::tuple<_Key>>>;
				};
				
				return fake::tuple::scale_v<tuple_t, inner>;
			}(fake::pack_v<_Tuple>, std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
	public:
		using type = typename decltype(impl())::type;
	};
	
	template<fake::tuple_c _Tuple>
	using shrink_t = typename shrink<_Tuple>::type;
	
	template<fake::tuple_c _Tuple>
	constexpr auto shrink_v = fake::pack_v<shrink_t<_Tuple>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	struct emplace final{
	private:
		static consteval auto impl(){
			static_assert(
				_Index <= std::tuple_size_v<_Tuple>,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::tuple::emplace>: '_Index' out of range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			
			if constexpr(_Index <= std::tuple_size_v<_Tuple>)
				return []<std::size_t... _Front, std::size_t... _Back>(
					std::index_sequence<_Front...>,
					std::index_sequence<_Back...>
				){
					return fake::pack_v<
						decltype(
							std::tuple_cat(
								std::declval<std::tuple<std::tuple_element_t<_Front, _Tuple>>>()...,
								std::declval<std::tuple<_Element>>(),
								std::declval<std::tuple<std::tuple_element_t<_Index + _Back, _Tuple>>>()...
							)
						)
					>;
				}(
					std::make_index_sequence<_Index>(),
					std::make_index_sequence<std::tuple_size_v<_Tuple> - _Index>()
				);
			else
				return fake::pack_v<std::tuple<>>;
		}
		
	public:
		using type = typename decltype(impl())::type;
	};
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	using emplace_t = typename emplace<_Tuple, _Index, _Element>::type;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	constexpr auto emplace_v = fake::pack_v<emplace_t<_Tuple, _Index, _Element>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	struct push final{using type = emplace_t<_Tuple, _Index, _Element>;};
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	using push_t = emplace_t<_Tuple, _Index, _Element>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	constexpr auto push_v = fake::pack_v<push_t<_Tuple, _Index, _Element>>;
	
	template<fake::tuple_c _Tuple, typename _Element>
	struct emplace_front final{using type = emplace_t<_Tuple, 0, _Element>;};
	
	template<fake::tuple_c _Tuple, typename _Element>
	using emplace_front_t = typename emplace_front<_Tuple, _Element>::type;
	
	template<fake::tuple_c _Tuple, typename _Element>
	constexpr auto emplace_front_v = fake::pack_v<emplace_front_t<_Tuple, _Element>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	struct push_front final{using type = emplace_front_t<_Tuple, _Element>;};
	
	template<fake::tuple_c _Tuple, typename _Element>
	using push_front_t = emplace_front_t<_Tuple, _Element>;
	
	template<fake::tuple_c _Tuple, typename _Element>
	constexpr auto push_front_v = fake::pack_v<push_front_t<_Tuple, _Element>>;
	
	template<fake::tuple_c _Tuple, typename _Element>
	struct emplace_back final{using type = emplace_t<_Tuple, std::tuple_size_v<_Tuple>, _Element>;};
	
	template<fake::tuple_c _Tuple, typename _Element>
	using emplace_back_t = typename emplace_back<_Tuple, _Element>::type;
	
	template<fake::tuple_c _Tuple, typename _Element>
	constexpr auto emplace_back_v = fake::pack_v<emplace_back_t<_Tuple, _Element>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	struct push_back final{using type = emplace_back_t<_Tuple, _Element>;};
	
	template<fake::tuple_c _Tuple, typename _Element>
	using push_back_t = emplace_back_t<_Tuple, _Element>;
	
	template<fake::tuple_c _Tuple, typename _Element>
	constexpr auto push_back_v = fake::pack_v<push_back_t<_Tuple, _Element>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	struct erase final{
	private:
		static consteval auto impl(){
			static_assert(
				_Index < std::tuple_size_v<_Tuple>,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::tuple::erase>: '_Index' out of range.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			
			if constexpr(_Index < std::tuple_size_v<_Tuple>)
				return []<std::size_t... _Front, std::size_t... _Back>(
					std::index_sequence<_Front...>,
					std::index_sequence<_Back...>
				){
					return fake::pack_v<
						decltype(
							std::tuple_cat(
								std::declval<std::tuple<std::tuple_element_t<_Front, _Tuple>>>()...,
								std::declval<std::tuple<std::tuple_element_t<_Index + _Back + 1, _Tuple>>>()...
							)
						)
					>;
				}(
					std::make_index_sequence<_Index>(),
					std::make_index_sequence<std::tuple_size_v<_Tuple> - _Index - 1>()
				);
			else
				return fake::pack_v<std::tuple<>>;
		}
		
	public:
		using type = typename decltype(impl())::type;
	};
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	using erase_t = typename erase<_Tuple, _Index>::type;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	constexpr auto erase_v = fake::pack_v<erase_t<_Tuple, _Index>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	struct pop final{using type = erase_t<_Tuple, _Index>;};
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	using pop_t = erase_t<_Tuple, _Index>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	constexpr auto pop_v = fake::pack_v<pop_t<_Tuple, _Index>>;
	
	template<fake::tuple_c _Tuple>
	struct erase_front final{using type = erase_t<_Tuple, 0>;};
	
	template<fake::tuple_c _Tuple>
	using erase_front_t = typename erase_front<_Tuple>::type;
	
	template<fake::tuple_c _Tuple>
	constexpr auto erase_front_v = fake::pack_v<erase_front_t<_Tuple>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	struct pop_front final{using type = erase_front_t<_Tuple>;};
	
	template<fake::tuple_c _Tuple>
	using pop_front_t = erase_front_t<_Tuple>;
	
	template<fake::tuple_c _Tuple>
	constexpr auto pop_front_v = fake::pack_v<pop_front_t<_Tuple>>;
	
	template<fake::tuple_c _Tuple>
	struct erase_back final{using type = erase_t<_Tuple, std::tuple_size_v<_Tuple>>;};
	
	template<fake::tuple_c _Tuple>
	using erase_back_t = typename erase_back<_Tuple>::type;
	
	template<fake::tuple_c _Tuple>
	constexpr auto erase_back_v = fake::pack_v<erase_back_t<_Tuple>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	struct pop_back final{using type = erase_back_t<_Tuple>;};
	
	template<fake::tuple_c _Tuple>
	using pop_back_t = erase_back_t<_Tuple>;
	
	template<fake::tuple_c _Tuple>
	constexpr auto pop_back_v = fake::pack_v<pop_back_t<_Tuple>>;
	
	template<typename = void>
	struct make{};
	
	template<template<typename...> typename _Template, typename... _Args>
	struct make<_Template<_Args...>>{using type = std::tuple<_Args...>;};
	
	template<typename _Type>
	using make_t = typename make<std::remove_cvref_t<_Type>>::type;
	
	template<typename... _Mix>
	using make_tuple_t = std::tuple<
		std::conditional_t<
			fake::is_ref_v<_Mix>,
			std::add_lvalue_reference_t<fake::remove_ref_t<_Mix>>,
			std::remove_reference_t<_Mix>
		>
		...
	>;
	
}

#endif /*__FAKE_TUPLE_H__*/ 
