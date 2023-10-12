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

#include <array>
#include <algorithm>

#include "traits.h"
#include "symbol.h"
#include "type_array.h"

namespace fake::tuple
{
	
	namespace tool
	{
		
		template<typename _Type>
		struct type_key final{static constexpr unsigned long long value = fake::symbol::make_view<_Type>().hash();};
		
		template<typename _Type, std::size_t _index>
		struct order final{
			static constexpr std::pair<unsigned long long, std::size_t> value{type_key<_Type>::value, _index};
		};
		
		template<template<typename, std::size_t> typename, typename>
		struct type_sort;
		
		template<
			template<typename, std::size_t> typename _Order,
			template<typename...> typename _Template,
			typename... _Types
		>
		struct type_sort<_Order, _Template<_Types...>> final{
		private:
			static constexpr std::size_t size = sizeof...(_Types);
			
			template<typename>
			struct impl;
			
			template<std::size_t... _index>
			requires (sizeof...(_index) == 0)
			struct impl<std::index_sequence<_index...>> final{
				static constexpr std::array<std::size_t, 0> mapping{};
				
				using type = _Template<>;
			};
			
			template<std::size_t... _index>
			requires (sizeof...(_index) != 0)
			struct impl<std::index_sequence<_index...>> final{
				static constexpr std::array<std::size_t, size> mapping = []{
					using order_t = std::common_type_t<decltype(_Order<_Types, _index>::value)...>;
					std::pair<order_t, std::size_t> match[] = {{_Order<_Types, _index>::value, _index}...};
					std::ranges::sort(match);
					
					return std::array<std::size_t, size>{match[_index].second...};
				}();
				
				using type = _Template<fake::meta::array_element_t<mapping[_index], fake::meta::array<_Types...>>...>;
			};
			
			using impl_t = impl<std::make_index_sequence<size>>;
			
		public:
			using type = typename impl_t::type;
			static constexpr std::array<std::size_t, size> mapping = impl_t::mapping;
		};
		
		struct triple final{
			unsigned long long key;
			std::size_t index;
			std::size_t reverse;
		};
		
	}
	
	template<typename = void>
	struct make{};
	
	template<template<typename...> typename _Template, typename... _Args>
	struct make<_Template<_Args...>>{using type = std::tuple<_Args...>;};
	
	template<typename _Type>
	using make_t = typename make<std::remove_cvref_t<_Type>>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct match_index final{
	private:
		static consteval auto impl(){
			constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			
			constexpr std::size_t result = []<std::size_t... _Index>(std::index_sequence<_Index...>){
				constexpr std::array<bool, size> pick = {
					_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>)...
				};
				for(std::size_t i = 0; i < size; i++)
					if(pick[i])
						return i;
				return size;
			}(std::make_index_sequence<size>());
			
			if constexpr(result < size)
				return fake::pack_v<std::integral_constant<std::size_t, result>>;
			else
				return fake::pack_v<fake::null_t>;
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	using match_index_t = typename match_index<_Tuple, _functor>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct find_if final{
	private:
		static consteval auto impl(){
			constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			
			constexpr std::size_t result = []<std::size_t... _Index>(std::index_sequence<_Index...>){
				constexpr std::array<bool, size> pick = {
					_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>)...
				};
				for(std::size_t i = 0; i < size; i++)
					if(pick[i])
						return i;
				return size;
			}(std::make_index_sequence<size>());
			
			if constexpr(result < size)
				return fake::pack_v<std::tuple_element_t<result, _Tuple>>;
			else
				return fake::pack_v<fake::null_t>;
		}
		
	public:
		using type = fake::take_t<impl()>;
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
	
	template<fake::tuple_c _Tuple, typename _Type>
	struct contains_type final{
	private:
		static consteval auto impl(){
			return []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return (std::is_same_v<std::tuple_element_t<_Index, _Tuple>, _Type> || ...);
			}(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
	public:
		static constexpr bool value = impl();
	};
	
	template<fake::tuple_c _Tuple, typename _Type>
	constexpr auto contains_type_v = contains_type<_Tuple, _Type>::value;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct scale final{
	private:
		static consteval auto impl(){
			return []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return fake::pack_v<
					tuple::make_t<
						meta::array_cat_t<
							meta::make_t<
								fake::take_t<_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>)>
							>...
						>
					>
				>;
			}(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
	public:
		using type = fake::take_t<impl()>;
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
					std::tuple<fake::take_t<_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>)>...>
				>;
			}(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	using transform_t = typename transform<_Tuple, _functor>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	constexpr auto transform_v = fake::pack_v<transform_t<_Tuple, _functor>>;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct select_if final{
	private:
		static consteval auto impl(){
			constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			
			constexpr auto result = []<std::size_t... _Index>(std::index_sequence<_Index...>){
				constexpr std::array<bool, size> pick = {
					_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>)...
				};
				constexpr std::size_t match = [](const std::array<bool, size> &_pick){
					std::size_t count = 0;
					for(std::size_t i = 0; i < sizeof...(_Index); i++)
						count += std::size_t(_pick[i]);
					return count;
				}(pick);
				std::array<std::size_t, match> query;
				for(std::size_t i = 0, j = 0; j < match; i++)
					if(pick[i])
						query[j++] = i;
				
				return query;
			}(std::make_index_sequence<size>());
			
			return []<std::size_t... _Index, auto _result>(std::index_sequence<_Index...>, fake::mezz_t<_result>){
				return fake::pack_v<std::tuple<std::tuple_element_t<_result[_Index], _Tuple>...>>;
			}(std::make_index_sequence<result.size()>(), fake::mezz_v<result>);
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	using select_if_t = typename select_if<_Tuple, _functor>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	constexpr auto select_if_v = fake::pack_v<select_if_t<_Tuple, _functor>>;
	
	template<fake::tuple_c _Tuple, auto _functor>
	struct erase_if final{
	private:
		static consteval auto impl(){
			constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			
			constexpr auto result = []<std::size_t... _Index>(std::index_sequence<_Index...>){
				constexpr std::array<bool, size> pick = {
					(_functor(fake::pack_v<std::tuple_element_t<_Index, _Tuple>>) == false)...
				};
				constexpr std::size_t match = [](const std::array<bool, size> &_pick){
					std::size_t count = 0;
					for(std::size_t i = 0; i < sizeof...(_Index); i++)
						count += std::size_t(_pick[i]);
					return count;
				}(pick);
				std::array<std::size_t, match> query;
				for(std::size_t i = 0, j = 0; j < match; i++)
					if(pick[i])
						query[j++] = i;
				
				return query;
			}(std::make_index_sequence<size>());
			
			return []<std::size_t... _Index, auto _result>(std::index_sequence<_Index...>, fake::mezz_t<_result>){
				return fake::pack_v<std::tuple<std::tuple_element_t<_result[_Index], _Tuple>...>>;
			}(std::make_index_sequence<result.size()>(), fake::mezz_v<result>);
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple, auto _functor>
	using erase_if_t = typename erase_if<_Tuple, _functor>::type;
	
	template<fake::tuple_c _Tuple, auto _functor>
	constexpr auto erase_if_v = fake::pack_v<erase_if_t<_Tuple, _functor>>;
	
	template<fake::tuple_c... _Tuples>
	struct concat final{
	private:
		static consteval auto impl(){
			return fake::pack_v<make_t<meta::array_cat_t<meta::make_t<_Tuples>...>>>;
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c... _Tuples>
	using concat_t = typename concat<_Tuples...>::type;
	
	template<fake::tuple_c... _Tuples>
	constexpr auto concat_v = fake::pack_v<concat_t<_Tuples...>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	requires (_Index <= std::tuple_size_v<_Tuple>)
	struct emplace final{
	private:
		static consteval auto impl(){
			return []<std::size_t... _Front, std::size_t... _Back>(
				std::index_sequence<_Front...>,
				std::index_sequence<_Back...>
			){
				return fake::pack_v<
					std::tuple<
						std::tuple_element_t<_Front, _Tuple>...,
						_Element,
						std::tuple_element_t<_Index + _Back, _Tuple>...
					>
				>;
			}(
				std::make_index_sequence<_Index>(),
				std::make_index_sequence<std::tuple_size_v<_Tuple> - _Index>()
			);
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	requires requires{typename emplace<_Tuple, _Index, _Element>::type;}
	using emplace_t = typename emplace<_Tuple, _Index, _Element>::type;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	requires requires{typename emplace_t<_Tuple, _Index, _Element>;}
	constexpr auto emplace_v = fake::pack_v<emplace_t<_Tuple, _Index, _Element>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	requires requires{typename emplace_t<_Tuple, _Index, _Element>;}
	struct push final{using type = emplace_t<_Tuple, _Index, _Element>;};
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	requires requires{typename emplace_t<_Tuple, _Index, _Element>;}
	using push_t = emplace_t<_Tuple, _Index, _Element>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	requires requires{typename push_t<_Tuple, _Index, _Element>;}
	constexpr auto push_v = fake::pack_v<push_t<_Tuple, _Index, _Element>>;
	
	template<fake::tuple_c _Tuple, typename _Element>
	requires requires{typename emplace_t<_Tuple, 0, _Element>;}
	struct emplace_front final{using type = emplace_t<_Tuple, 0, _Element>;};
	
	template<fake::tuple_c _Tuple, typename _Element>
	requires requires{typename emplace_front<_Tuple, _Element>::type;}
	using emplace_front_t = typename emplace_front<_Tuple, _Element>::type;
	
	template<fake::tuple_c _Tuple, typename _Element>
	requires requires{typename emplace_front_t<_Tuple, _Element>;}
	constexpr auto emplace_front_v = fake::pack_v<emplace_front_t<_Tuple, _Element>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	requires requires{typename emplace_front_t<_Tuple, _Element>;}
	struct push_front final{using type = emplace_front_t<_Tuple, _Element>;};
	
	template<fake::tuple_c _Tuple, typename _Element>
	requires requires{typename emplace_front_t<_Tuple, _Element>;}
	using push_front_t = emplace_front_t<_Tuple, _Element>;
	
	template<fake::tuple_c _Tuple, typename _Element>
	requires requires{typename push_front_t<_Tuple, _Element>;}
	constexpr auto push_front_v = fake::pack_v<push_front_t<_Tuple, _Element>>;
	
	template<fake::tuple_c _Tuple, typename _Element>
	requires requires{typename emplace_t<_Tuple, std::tuple_size_v<_Tuple>, _Element>;}
	struct emplace_back final{using type = emplace_t<_Tuple, std::tuple_size_v<_Tuple>, _Element>;};
	
	template<fake::tuple_c _Tuple, typename _Element>
	requires requires{typename emplace_back<_Tuple, _Element>::type;}
	using emplace_back_t = typename emplace_back<_Tuple, _Element>::type;
	
	template<fake::tuple_c _Tuple, typename _Element>
	requires requires{typename emplace_back_t<_Tuple, _Element>;}
	constexpr auto emplace_back_v = fake::pack_v<emplace_back_t<_Tuple, _Element>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, typename _Element>
	requires requires{typename emplace_back_t<_Tuple, _Element>;}
	struct push_back final{using type = emplace_back_t<_Tuple, _Element>;};
	
	template<fake::tuple_c _Tuple, typename _Element>
	requires requires{typename emplace_back_t<_Tuple, _Element>;}
	using push_back_t = emplace_back_t<_Tuple, _Element>;
	
	template<fake::tuple_c _Tuple, typename _Element>
	requires requires{typename push_back_t<_Tuple, _Element>;}
	constexpr auto push_back_v = fake::pack_v<push_back_t<_Tuple, _Element>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	requires (_Index < std::tuple_size_v<_Tuple>)
	struct erase final{
	private:
		static consteval auto impl(){
			return []<std::size_t... _Front, std::size_t... _Back>(
				std::index_sequence<_Front...>,
				std::index_sequence<_Back...>
			){
				return fake::pack_v<
					std::tuple<
						std::tuple_element_t<_Front, _Tuple>...,
						std::tuple_element_t<_Index + _Back + 1, _Tuple>...
					>
				>;
			}(
				std::make_index_sequence<_Index>(),
				std::make_index_sequence<std::tuple_size_v<_Tuple> - _Index - 1>()
			);
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	requires requires{typename erase<_Tuple, _Index>::type;}
	using erase_t = typename erase<_Tuple, _Index>::type;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	requires requires{typename erase_t<_Tuple, _Index>;}
	constexpr auto erase_v = fake::pack_v<erase_t<_Tuple, _Index>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	requires requires{typename erase_t<_Tuple, _Index>;}
	struct pop final{using type = erase_t<_Tuple, _Index>;};
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	requires requires{typename erase_t<_Tuple, _Index>;}
	using pop_t = erase_t<_Tuple, _Index>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	requires requires{typename pop_t<_Tuple, _Index>;}
	constexpr auto pop_v = fake::pack_v<pop_t<_Tuple, _Index>>;
	
	template<fake::tuple_c _Tuple>
	requires requires{typename erase_t<_Tuple, 0>;}
	struct erase_front final{using type = erase_t<_Tuple, 0>;};
	
	template<fake::tuple_c _Tuple>
	requires requires{typename erase_front<_Tuple>::type;}
	using erase_front_t = typename erase_front<_Tuple>::type;
	
	template<fake::tuple_c _Tuple>
	requires requires{typename erase_front_t<_Tuple>;}
	constexpr auto erase_front_v = fake::pack_v<erase_front_t<_Tuple>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	requires requires{typename erase_front_t<_Tuple>;}
	struct pop_front final{using type = erase_front_t<_Tuple>;};
	
	template<fake::tuple_c _Tuple>
	requires requires{typename erase_front_t<_Tuple>;}
	using pop_front_t = erase_front_t<_Tuple>;
	
	template<fake::tuple_c _Tuple>
	requires requires{typename pop_front_t<_Tuple>;}
	constexpr auto pop_front_v = fake::pack_v<pop_front_t<_Tuple>>;
	
	template<fake::tuple_c _Tuple>
	requires requires{typename erase_t<_Tuple, std::tuple_size_v<_Tuple>>;}
	struct erase_back final{using type = erase_t<_Tuple, std::tuple_size_v<_Tuple>>;};
	
	template<fake::tuple_c _Tuple>
	requires requires{typename erase_back<_Tuple>::type;}
	using erase_back_t = typename erase_back<_Tuple>::type;
	
	template<fake::tuple_c _Tuple>
	requires requires{typename erase_back_t<_Tuple>;}
	constexpr auto erase_back_v = fake::pack_v<erase_back_t<_Tuple>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index>
	requires requires{typename erase_back_t<_Tuple>;}
	struct pop_back final{using type = erase_back_t<_Tuple>;};
	
	template<fake::tuple_c _Tuple>
	requires requires{typename erase_back_t<_Tuple>;}
	using pop_back_t = erase_back_t<_Tuple>;
	
	template<fake::tuple_c _Tuple>
	requires requires{typename pop_back_t<_Tuple>;}
	constexpr auto pop_back_v = fake::pack_v<pop_back_t<_Tuple>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, std::size_t _Length>
	requires (_Index + _Length <= std::tuple_size_v<_Tuple>)
	struct subtuple final{
	private:
		static consteval auto impl(){
			return []<std::size_t... _SubIndex>(std::index_sequence<_SubIndex...>){
				return fake::pack_v<std::tuple<std::tuple_element_t<_Index + _SubIndex, _Tuple>...>>;
			}(std::make_index_sequence<_Length>());
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple, std::size_t _Index, std::size_t _Length>
	requires requires{typename subtuple<_Tuple, _Index, _Length>::type;}
	using subtuple_t = typename subtuple<_Tuple, _Index, _Length>::type;
	
	template<fake::tuple_c _Tuple, std::size_t _Index, std::size_t _Length>
	requires requires{typename subtuple_t<_Tuple, _Index, _Length>;}
	constexpr auto subtuple_v = fake::pack_v<subtuple_t<_Tuple, _Index, _Length>>;
	
	template<fake::tuple_c _Tuple, std::size_t _Lhs, std::size_t _Rhs>
	requires (_Lhs < std::tuple_size_v<_Tuple> && _Rhs < std::tuple_size_v<_Tuple>)
	struct swap final{
	private:
		static consteval auto impl(){
			return []<std::size_t... _Index>(std::index_sequence<_Index...>){
				return fake::pack_v<
					std::tuple<
						std::tuple_element_t<(_Index ^ _Lhs && _Index ^ _Rhs) - 1 & (_Lhs ^ _Rhs) ^ _Index, _Tuple>...
					>
				>;
			}(std::make_index_sequence<std::tuple_size_v<_Tuple>>());
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple, std::size_t _Lhs, std::size_t _Rhs>
	requires requires{typename swap<_Tuple, _Lhs, _Rhs>::type;}
	using swap_t = typename swap<_Tuple, _Lhs, _Rhs>::type;
	
	template<fake::tuple_c _Tuple, std::size_t _Lhs, std::size_t _Rhs>
	requires requires{typename swap_t<_Tuple, _Lhs, _Rhs>;}
	constexpr auto swap_v = fake::pack_v<swap_t<_Tuple, _Lhs, _Rhs>>;
	
	template<fake::tuple_c _Tuple>
	struct shrink final{
	private:
		static constexpr fake::pack_c auto init = fake::pack_v<std::tuple<>>;
		
		static consteval auto impl(){
			constexpr std::size_t size = std::tuple_size_v<_Tuple>;
			
			using sort_info = tool::type_sort<tool::order, _Tuple>;
			return []<std::size_t... _index>(std::index_sequence<_index...>) consteval{
				using sort_t = typename sort_info::type;
				constexpr std::array result = []{
					constexpr std::array<tool::triple, size> source = {
						tool::triple{
							tool::type_key<std::tuple_element_t<_index, sort_t>>::value,
							sort_info::mapping[_index],
							_index
						}...
					};
					
					constexpr auto unique = []<
						std::size_t _begin,
						std::size_t _end,
						auto _source,
						fake::tuple_c _Unique,
						fake::tuple_c _Result
					>(
						auto _self,
						fake::mezz_t<_begin> _begin_m,
						fake::mezz_t<_end> _end_m,
						fake::mezz_t<_source> _source_m,
						fake::type_package<_Unique> _unique_p,
						fake::type_package<_Result> _result_p
					){
						if constexpr(_begin == _end){
							return _result_p;
						}
						else{
							constexpr tool::triple current = _source[_begin];
							using current_t = std::tuple_element_t<current.index, _Tuple>;
							
							if constexpr(contains_type_v<_Unique, current_t>)
								return _self(_self, fake::mezz_v<_begin + 1>, _end_m, _source_m, _unique_p, _result_p);
							else
								return _self(
									_self,
									fake::mezz_v<_begin + 1>,
									_end_m,
									_source_m,
									push_back_v<_Unique, current_t>,
									push_back_v<_Result, fake::mezz_t<current.reverse>>
								);
						}
					};
					
					constexpr auto rec = []<
						std::size_t _begin,
						std::size_t _end,
						auto _source,
						auto _unique,
						fake::tuple_c _Result
					>(
						auto &_self,
						fake::mezz_t<_begin> _begin_m,
						fake::mezz_t<_end> _end_m,
						fake::mezz_t<_source> _source_m,
						fake::mezz_t<_unique> _unique_m,
						fake::type_package<_Result> _result_p
					){
						if constexpr(_end == size){
							using local_t = fake::take_t<_unique(_unique, _begin_m, _end_m, _source_m, init, init)>;
							
							return concat_v<_Result, local_t>;
						}
						else if constexpr(_source[_begin].key == _source[_end].key){
							return _self(_self, _begin_m, fake::mezz_v<_end + 1>, _source_m, _unique_m, _result_p);
						}
						else if constexpr(_begin + 1 == _end){
							constexpr auto concat = push_back_v<_Result, fake::mezz_t<_source[_begin].reverse>>;
							
							return _self(_self, _end_m, fake::mezz_v<_end + 1>, _source_m, _unique_m, concat);
						}
						else{
							using local_t = fake::take_t<_unique(_unique, _begin_m, _end_m, _source_m, init, init)>;
							constexpr auto concat = concat_v<_Result, local_t>;
							
							return _self(_self, _end_m, fake::mezz_v<_end + 1>, _source_m, _unique_m, concat);
						}
					};
					
					constexpr auto zero = fake::mezz_v<std::size_t{}>;
					using target = fake::take_t<rec(rec, zero, zero, fake::mezz_v<source>, fake::mezz_v<unique>, init)>;
					
					constexpr std::size_t remain = std::tuple_size_v<target>;
					std::array<tool::triple, remain> buffer;
					
					[&buffer]<std::size_t... _i, auto _source>(std::index_sequence<_i...>, fake::mezz_t<_source>){
						((buffer[_i] = _source[std::tuple_element_t<_i, target>::value]), ...);
					}(std::make_index_sequence<remain>(), fake::mezz_v<source>);
					
					std::ranges::sort(
						buffer,
						[](std::size_t _x, std::size_t _y){return _x < _y;},
						[](tool::triple _t){return _t.index;}
					);
					
					return buffer;
				}();
				
				return []<std::size_t... _i, auto _array>(std::index_sequence<_i...>, fake::mezz_t<_array>){
					return fake::pack_v<std::tuple<std::tuple_element_t<_array[_i].reverse, sort_t>...>>;
				}(std::make_index_sequence<result.size()>(), fake::mezz_v<result>);
			}(std::make_index_sequence<size>());
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple>
	using shrink_t = typename shrink<_Tuple>::type;
	
	template<fake::tuple_c _Tuple>
	constexpr auto shrink_v = fake::pack_v<shrink_t<_Tuple>>;
	
	template<fake::tuple_c _Tuple>
	struct unique final{
	private:
		static constexpr std::size_t size = std::tuple_size_v<_Tuple>;
		
		static consteval auto impl(){
			constexpr auto recur = []<typename _Self, std::size_t _current, fake::tuple_c _Result>(){
				if constexpr(_current == size){
					return fake::pack_v<_Result>;
				}
				else{
					using current_t = std::tuple_element_t<_current, _Tuple>;
					return _Self{}.template operator()<
						_Self,
						_current + 1,
						std::conditional_t<
							contains_type_v<_Result, current_t>,
							_Result,
							push_back_t<_Result, current_t>
						>
					>();
				}
			};
			
			return recur.template operator()<decltype(recur), 0, std::tuple<>>();
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple>
	using unique_t = typename unique<_Tuple>::type;
	
	template<fake::tuple_c _Tuple>
	constexpr auto unique_v = fake::pack_v<unique_t<_Tuple>>;
	
	template<fake::tuple_c _Tuple, typename _Init, auto _functor>
	struct for_each final{
	private:
		static constexpr std::size_t size = std::tuple_size_v<_Tuple>;
		
		static consteval auto impl(){
			constexpr auto recur = [](fake::mezz_c auto _self, fake::pack_c auto _state, fake::mezz_c auto _index){
				if constexpr(_index.value == size){
					return _state;
				}
				else{
					using type = std::tuple_element_t<_index.value, _Tuple>;
					constexpr auto result = _functor(_state, _index, fake::pack_v<type>);
					
					if constexpr(std::same_as<std::remove_cvref_t<decltype(result)>, fake::null_t>)
						return _state;
					else
						return _self.value(_self, result, fake::mezz_v<_index.value + 1>);
				}
			};
			
			return recur(fake::mezz_v<recur>, fake::pack_v<_Init>, fake::mezz_v<std::size_t{}>);
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c _Tuple, typename _Init, auto _functor>
	using for_each_t = typename for_each<_Tuple, _Init, _functor>::type;
	
	template<fake::tuple_c _Tuple, typename _Init, auto _functor>
	constexpr auto for_each_v = fake::pack_v<for_each_t<_Tuple, _Init, _functor>>;
	
	template<fake::tuple_c... _Tuples>
	struct zip final{
	private:
		static constexpr std::array sizes{std::tuple_size_v<_Tuples>...};
		static constexpr std::size_t size = std::ranges::min(sizes);
		
		static consteval auto impl(){
			return []<std::size_t... _index>(std::index_sequence<_index...>){
				constexpr auto index = [](fake::mezz_c auto _i){
					if constexpr(sizeof...(_Tuples) == 0x2)
						return fake::pack_v<std::pair<std::tuple_element_t<_i.value, _Tuples>...>>;
					else
						return fake::pack_v<std::tuple<std::tuple_element_t<_i.value, _Tuples>...>>;
				};
				
				return fake::pack_v<std::tuple<fake::take_t<index(fake::mezz_v<_index>)>...>>;
			}(std::make_index_sequence<size>());
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<fake::tuple_c... _Tuples>
	using zip_t = typename zip<_Tuples...>::type;
	
	template<fake::tuple_c... _Tuples>
	constexpr auto zip_v = fake::pack_v<zip_t<_Tuples...>>;
	
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
