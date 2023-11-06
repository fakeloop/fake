#ifndef __FAKE_FLAT_H__
#define __FAKE_FLAT_H__

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

#include "traits.h"

namespace fake
{
	
	template<typename _First, typename _Second>
	struct mate{
		using first_type = _First;
		using second_type = _Second;
		
		first_type first;
		second_type second;
		
	private:
		friend constexpr auto operator<=>(const mate&, const mate&) = default;
	};
	
	template<fake::mezz_c _First, typename _Second>
	struct mate<_First, _Second>{
		using first_type = _First;
		using second_type = _Second;
		
		static constexpr first_type first{};
		second_type second;
		
	private:
		friend constexpr auto operator<=>(const mate&, const mate&) = default;
	};
	
	template<typename _Second>
	struct mate<void, _Second>{
		using first_type = void;
		using second_type = _Second;
		
		second_type second;
		
	private:
		friend constexpr auto operator<=>(const mate&, const mate&) = default;
	};
	
	template<typename _First>
	struct mate<_First, void>{
		using first_type = _First;
		using second_type = void;
		
		first_type first;
		
	private:
		friend constexpr auto operator<=>(const mate&, const mate&) = default;
	};
	
	template<fake::mezz_c _First>
	struct mate<_First, void>{
		using first_type = _First;
		using second_type = void;
		
		static constexpr first_type first{};
		
	private:
		friend constexpr auto operator<=>(const mate&, const mate&) = default;
	};
	
	template<>
	struct mate<void, void>{
		using first_type = void;
		using second_type = void;
		
	private:
		friend constexpr auto operator<=>(const mate&, const mate&) = default;
	};
	
	template<typename _Type>
	concept mate_c = fake::trait_v<mate, std::remove_cvref_t<_Type>>;
	
	namespace detail::flat
	{
		
		template<typename, typename...>
		struct broker;
		
		template<std::size_t... _index, typename... _Args>
		struct broker<std::index_sequence<_index...>, _Args...> : public fake::mate<fake::mezz_t<_index>, _Args>...{
		private:
			friend constexpr auto operator<=>(const broker&, const broker&) = default;
		};
		
		template<typename... _Types>
		requires ((requires{sizeof(_Types);} || std::same_as<_Types, void>) && ...)
		struct has_definitions;
		
	}
	
	// use a constraint of definition existence to avoid the possible ODR violations arising from 'fake::flat_based_c'. 
	template<typename... _Args>
	requires requires{typename detail::flat::has_definitions<_Args...>;}
	struct flat : public detail::flat::broker<std::index_sequence_for<_Args...>, _Args...>{
		static constexpr std::size_t size = sizeof... (_Args);
		
	private:
		template<std::size_t _index, typename _Type>
		static consteval fake::pack_c auto extract(fake::mate<fake::mezz_t<_index>, _Type>&&) noexcept{
			return fake::pack_v<_Type>;
		}
		
		template<std::size_t _index, typename _Type>
		requires (_index < size)
		static constexpr decltype(auto) ref(fake::mate<fake::mezz_t<_index>, _Type> &_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
		template<std::size_t _index, typename _Type>
		requires (_index < size)
		static constexpr decltype(auto) ref(const fake::mate<fake::mezz_t<_index>, _Type> &_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
		template<std::size_t _index, typename _Type>
		requires (_index < size)
		static constexpr decltype(auto) ref(fake::mate<fake::mezz_t<_index>, _Type> &&_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
		template<std::size_t _index, typename _Type>
		requires (_index < size)
		static constexpr decltype(auto) ref(const fake::mate<fake::mezz_t<_index>, _Type> &&_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
		template<typename _Type, std::size_t _index>
		requires (_index < size)
		static constexpr decltype(auto) ref(fake::mate<fake::mezz_t<_index>, _Type> &_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
		template<typename _Type, std::size_t _index>
		requires (_index < size)
		static constexpr decltype(auto) ref(const fake::mate<fake::mezz_t<_index>, _Type> &_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
		template<typename _Type, std::size_t _index>
		requires (_index < size)
		static constexpr decltype(auto) ref(fake::mate<fake::mezz_t<_index>, _Type> &&_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
		template<typename _Type, std::size_t _index>
		requires (_index < size)
		static constexpr decltype(auto) ref(const fake::mate<fake::mezz_t<_index>, _Type> &&_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
	public:
		template<std::size_t _index>
		requires (_index < size)
		static consteval fake::pack_c auto type(std::integral_constant<std::size_t, _index>) noexcept{
			return decltype(extract<_index>(std::declval<flat>())){};
		}
		
		template<std::size_t _index>
		constexpr auto& value() & noexcept
		requires requires{ref<_index>(*this).second;}{
			return ref<_index>(*this).second;
		}
		
		template<std::size_t _index>
		constexpr auto& value() const & noexcept
		requires requires{ref<_index>(*this).second;}{
			return ref<_index>(*this).second;
		}
		
		template<std::size_t _index>
		constexpr auto&& value() && noexcept
		requires requires{ref<_index>(*this).second;}{
			return ref<_index>(std::move(*this)).second;
		}
		
		template<std::size_t _index>
		constexpr auto&& value() const && noexcept
		requires requires{ref<_index>(*this).second;}{
			return ref<_index>(std::move(*this)).second;
		}
		
		template<typename _Type>
		constexpr auto& value() & noexcept
		requires requires{ref<_Type>(*this).second;}{
			return ref<_Type>(*this).second;
		}
		
		template<typename _Type>
		constexpr auto& value() const & noexcept
		requires requires{ref<_Type>(*this).second;}{
			return ref<_Type>(*this).second;
		}
		
		template<typename _Type>
		constexpr auto&& value() && noexcept
		requires requires{ref<_Type>(*this).second;}{
			return ref<_Type>(std::move(*this)).second;
		}
		
		template<typename _Type>
		constexpr auto&& value() const && noexcept
		requires requires{ref<_Type>(*this).second;}{
			return ref<_Type>(std::move(*this)).second;
		}
		
		template<std::size_t _index>
		requires (_index < size && requires(flat _flat){ref<_index>(_flat).second;} == false)
		constexpr void value() const noexcept{}
		
		template<typename _Type>
		requires (std::same_as<_Type, void> && requires(flat _flat){ref<_Type>(_flat);})
		constexpr void value() const noexcept{}
		
	private:
		friend constexpr auto operator<=>(const flat&, const flat&) = default;
	};
	
	template<typename _Type>
	concept flat_c = fake::trait_v<fake::flat, std::remove_cvref_t<_Type>>;
	
	namespace detail::flat
	{
		
		template<typename _Arg>
		consteval fake::pack_c auto type_adaptor() noexcept{
			if constexpr(fake::trait_v<std::reference_wrapper, std::remove_cvref_t<_Arg>>)
				return fake::pack_v<std::add_lvalue_reference_t<typename std::remove_reference_t<_Arg>::type>>;
			else
				return fake::pack_v<std::remove_reference_t<_Arg>>;
		}
		
		constexpr decltype(auto) value_adaptor(auto &&_arg) noexcept{
			if constexpr(fake::trait_v<std::reference_wrapper, std::remove_cvref_t<decltype(_arg)>>)
				return std::forward<decltype(_arg.get())>(_arg.get());
			else
				return std::forward<decltype(_arg)>(_arg);
		}
		
	}
	
	template<typename _Type>
	concept flat_based_c = (
		requires{typename fake::mimic_t<std::remove_cvref_t<_Type>, detail::flat::has_definitions>;} &&
		std::derived_from<std::remove_cvref_t<_Type>, fake::mimic_t<std::remove_cvref_t<_Type>, fake::flat>>
	);
	
	template<flat_based_c...>
	struct flat_cat_result;
	
	template<>
	struct flat_cat_result<> final{
		// can NOT detect derived template. 
	};
	
	template<template<typename...> typename _Template, typename... _Args>
	struct flat_cat_result<_Template<_Args...>> final{
		using type = _Template<_Args...>;
	};
	
	template<template<typename...> typename _Template, typename... _Lhs, typename... _Rhs>
	struct flat_cat_result<_Template<_Lhs...>, _Template<_Rhs...>> final{
		using type = _Template<_Lhs..., _Rhs...>;
	};
	
	template<template<typename...> typename _Template, typename... _Lhs, typename... _Rhs, typename... _Tail>
	requires requires{typename flat_cat_result<_Template<_Lhs..., _Rhs...>, _Tail...>::type;}
	struct flat_cat_result<_Template<_Lhs...>, _Template<_Rhs...>, _Tail...> final{
		using type = typename flat_cat_result<_Template<_Lhs..., _Rhs...>, _Tail...>::type;
	};
	
	template<fake::flat_based_c... _Segments>
	using flat_cat_result_t = typename flat_cat_result<_Segments...>::type;
	
	template<fake::flat_based_c>
	struct flat_size;
	
	template<template<typename...> typename _Template, typename... _Args>
	struct flat_size<_Template<_Args...>> final{
		static constexpr std::size_t value = sizeof...(_Args);
	};
	
	template<fake::flat_based_c _Flat>
	constexpr std::size_t flat_size_v = flat_size<std::remove_cvref_t<_Flat>>::value;
	
	inline constexpr auto make_flat(auto &&..._args) noexcept ->
	fake::flat<fake::take_t<detail::flat::type_adaptor<decltype(_args)>()>...>{
		return {detail::flat::value_adaptor(std::forward<decltype(_args)>(_args))...};
	}
	
	template<template<typename...> typename _Template>
	inline constexpr auto make_flat(auto &&..._args) noexcept ->
	_Template<fake::take_t<detail::flat::type_adaptor<decltype(_args)>()>...>
	requires fake::flat_based_c<_Template<decltype(_args)...>>{
		return {detail::flat::value_adaptor(std::forward<decltype(_args)>(_args))...};
	}
	
	template<std::size_t _index>
	inline constexpr decltype(auto) get(fake::flat_based_c auto &&_flat) noexcept
	requires (
		requires{std::forward<decltype(_flat)>(_flat).template value<_index>();} &&
		requires{{std::forward<decltype(_flat)>(_flat).template value<_index>()} -> std::same_as<void>;} == false
	){
		return std::forward<decltype(_flat)>(_flat).template value<_index>();
	}
	
	template<std::size_t _index>
	inline constexpr void get(fake::flat_based_c auto &&_flat) noexcept
	requires requires{{_flat.template value<_index>()} -> std::same_as<void>;}{}
	
	template<typename _Type>
	inline constexpr decltype(auto) get(fake::flat_based_c auto &&_flat) noexcept
	requires (
		requires{std::forward<decltype(_flat)>(_flat).template value<_Type>();} &&
		requires{{std::forward<decltype(_flat)>(_flat).template value<_Type>()} -> std::same_as<void>;} == false
	){
		return std::forward<decltype(_flat)>(_flat).template value<_Type>();
	}
	
	template<typename _Type>
	inline constexpr void get(fake::flat_based_c auto &&_flat) noexcept
	requires requires{{_flat.template value<_Type>()} -> std::same_as<void>;}{}
	
	namespace detail::flat
	{
		
		template<typename>
		struct mimic_impl;
		
		template<template<typename...> typename _Template, typename... _Types>
		struct mimic_impl<_Template<_Types...>> final{
			template<fake::pack_c... _Args>
			inline static constexpr auto pack(_Args ..._args){
				return fake::pack_v<_Template<fake::take_t<_Args{}>...>>;
			}
		};
		
		template<typename _Type, typename... _Args>
		using mimic_t = fake::take_t<mimic_impl<std::remove_cvref_t<_Type>>::pack(fake::pack_v<_Args>...)>;
		
	}
	
	template<std::size_t _index, std::size_t _size = std::size_t(-1)>
	inline constexpr auto subflat(fake::flat_based_c auto &&_f)
	requires requires{requires (_index <= flat_size_v<decltype(_f)>);}{
		using type = std::remove_cvref_t<decltype(_f)>;
		return []<std::size_t... _elems>(auto &&_e, std::index_sequence<_elems...>){
			// use 'decltype(...)' to make 'IntelliSense' happy. 
			using result_t = detail::flat::mimic_t<
				type,
				fake::take_t<decltype(type::type(fake::index_v<_index + _elems>)){}>...
			>;
			return result_t{fake::get<_index + _elems>(std::forward<decltype(_e)>(_e))...};
		}(std::forward<decltype(_f)>(_f), std::make_index_sequence<std::min(_size, flat_size_v<type> - _index)>());
	}
	
	template<fake::flat_based_c _Type, std::size_t _index, std::size_t _size = std::size_t(-1)>
	using subflat_t = decltype(subflat<_index, _size>(std::declval<std::remove_cvref_t<_Type>>()));
	
	inline constexpr auto tie(auto &..._args) noexcept ->
	fake::flat<decltype(_args)...>{
		return {_args...};
	}
	
	template<template<typename...> typename _Template>
	inline constexpr auto tie(auto &..._args) noexcept ->
	_Template<decltype(_args)...>
	requires fake::flat_based_c<_Template<decltype(_args)...>>{
		return {_args...};
	}
	
	namespace detail::flat
	{
		
		template<fake::flat_based_c...>
		struct flat_concat;
		
		template<fake::flat_based_c _Result>
		struct flat_concat<_Result> final{
			template<typename... _Args>
			inline static constexpr _Result impl(_Args &&..._args){
				return {std::forward<_Args>(_args)...};
			}
		};
		
		template<fake::flat_based_c _Result, fake::flat_based_c _Head, fake::flat_based_c... _Tails>
		struct flat_concat<_Result, _Head, _Tails...> final{
			template<typename... _Args>
			inline static constexpr _Result impl(_Head &&_head, _Tails &&..._tails, _Args &&..._args){
				return []<std::size_t... _index>(std::index_sequence<_index...>, auto &&_h, auto &&..._tails_args){
					return flat_concat<_Result, _Tails...>::impl(
						std::forward<decltype(_tails_args)>(_tails_args)...,
						fake::get<_index>(std::forward<decltype(_h)>(_h))...
					);
				}(
					std::make_index_sequence<flat_size_v<_Head>>(),
					std::forward<_Head>(_head),
					std::forward<_Tails>(_tails)...,
					std::forward<_Args>(_args)...
				);
			}
		};
		
	}
	
	inline constexpr auto flat_cat(fake::flat_based_c auto &&..._flats) ->
	flat_cat_result_t<std::remove_cvref_t<decltype(_flats)>...>{
		using result = flat_cat_result_t<std::remove_cvref_t<decltype(_flats)>...>;
		return detail::flat::flat_concat<result, decltype(_flats)...>::impl(std::forward<decltype(_flats)>(_flats)...);
	}
	
	template<fake::mate_c... _Mates>
	struct query : public _Mates...{
		static constexpr std::size_t size = sizeof... (_Mates);
		
		using first_types = fake::flat<typename _Mates::first_type...>;
		using second_types = fake::flat<typename _Mates::second_type...>;
		
	private:
		template<typename _First, typename _Type>
		static consteval fake::pack_c auto extract(fake::mate<_First, _Type>) noexcept{
			return fake::pack_v<fake::mate<_First, _Type>>;
		}
		
		template<typename _First, typename _Type>
		static constexpr auto& ref(fake::mate<_First, _Type> &_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
		template<typename _First, typename _Type>
		static constexpr const auto& ref(const fake::mate<_First, _Type> &_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
		template<typename _First, typename _Type>
		static constexpr auto&& ref(fake::mate<_First, _Type> &&_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
		template<typename _First, typename _Type>
		static constexpr const auto&& ref(const fake::mate<_First, _Type> &&_self) noexcept{
			return std::forward<decltype(_self)>(_self);
		}
		
	public:
		template<typename _First>
		requires requires(query _query){extract<_First>(_query);}
		static consteval fake::pack_c auto type() noexcept{
			using mate_t = fake::take_t<decltype(extract<_First>(std::declval<query>())){}>;
			return fake::pack_v<typename mate_t::second_type>;
		}
		
		template<typename _First>
		requires requires(query _query){extract<_First>(_query);}
		static consteval fake::pack_c auto mate() noexcept{
			using mate_t = fake::take_t<decltype(extract<_First>(std::declval<query>())){}>;
			return fake::pack_v<mate_t>;
		}
		
		template<typename _First>
		constexpr auto& value() & noexcept
		requires requires{ref<_First>(*this);}{
			return ref<_First>(*this);
		}
		
		template<typename _First>
		constexpr const auto& value() const & noexcept
		requires requires{ref<_First>(*this);}{
			return ref<_First>(*this);
		}
		
		template<typename _First>
		constexpr auto&& value() && noexcept
		requires requires{ref<_First>(*this);}{
			return ref<_First>(std::move(*this));
		}
		
		template<typename _First>
		constexpr const auto&& value() const && noexcept
		requires requires{ref<_First>(*this);}{
			return ref<_First>(std::move(*this));
		}
		
		template<typename _First>
		constexpr auto& operator[](_First) & noexcept
		requires requires{ref<_First>(*this);}{
			return ref<_First>(*this).second;
		}
		
		template<typename _First>
		constexpr auto& operator[](_First) const & noexcept
		requires requires{ref<_First>(*this);}{
			return ref<_First>(*this).second;
		}
		
		template<typename _First>
		constexpr auto&& operator[](_First) && noexcept
		requires requires{ref<_First>(*this);}{
			return ref<_First>(std::move(*this)).second;
		}
		
		template<typename _First>
		constexpr auto&& operator[](_First) const && noexcept
		requires requires{ref<_First>(*this);}{
			return ref<_First>(std::move(*this)).second;
		}
		
	private:
		friend constexpr auto operator<=>(const query&, const query&) = default;
	};
	
	template<typename _Type>
	concept query_c = fake::trait_v<query, std::remove_cvref_t<_Type>>;
	
	template<query_c...>
	struct query_cat_result;
	
	template<>
	struct query_cat_result<> final{
		using type = fake::query<>;
	};
	
	template<fake::mate_c... _Args>
	struct query_cat_result<fake::query<_Args...>> final{
		using type = fake::query<_Args...>;
	};
	
	template<fake::mate_c... _Lhs, fake::mate_c... _Rhs>
	struct query_cat_result<fake::query<_Lhs...>, fake::query<_Rhs...>> final{
		using type = fake::query<_Lhs..., _Rhs...>;
	};
	
	template<fake::mate_c... _Lhs, fake::mate_c... _Rhs, fake::mate_c... _Tail>
	requires requires{typename query_cat_result<fake::query<_Lhs..., _Rhs...>, _Tail...>::type;}
	struct query_cat_result<fake::query<_Lhs...>, fake::query<_Rhs...>, _Tail...> final{
		using type = typename query_cat_result<fake::query<_Lhs..., _Rhs...>, _Tail...>::type;
	};
	
	template<query_c... _Segments>
	using query_cat_result_t = typename query_cat_result<_Segments...>::type;
	
	template<query_c>
	struct query_size;
	
	template<fake::mate_c... _Args>
	struct query_size<fake::query<_Args...>> final{
		static constexpr std::size_t value = sizeof...(_Args);
	};
	
	template<query_c _Query>
	constexpr std::size_t query_size_v = query_size<std::remove_cvref_t<_Query>>::value;
	
	template<auto _key>
	inline constexpr decltype(auto) get(fake::query_c auto &&_query) noexcept
	requires requires{std::forward<decltype(_query)>(_query).template value<std::remove_cvref_t<decltype(_key)>>();}{
		return std::forward<decltype(_query)>(_query).template value<std::remove_cvref_t<decltype(_key)>>();
	}
	
	template<std::size_t _index>
	inline constexpr decltype(auto) get(fake::query_c auto &&_query) noexcept
	requires requires{
		std::forward<decltype(_query)>(_query).template value<
			fake::take_t<std::remove_cvref_t<decltype(_query)>::first_types::type(fake::index_v<_index>)>
		>();
	}
	{
		using key = fake::take_t<std::remove_cvref_t<decltype(_query)>::first_types::type(fake::index_v<_index>)>;
		return std::forward<decltype(_query)>(_query).template value<key>();
	}
	
	template<typename _Type>
	inline constexpr decltype(auto) get(fake::query_c auto &&_query) noexcept
	requires requires{std::forward<decltype(_query)>(_query).template value<_Type>();}{
		return std::forward<decltype(_query)>(_query).template value<_Type>();
	}
	
	template<query_c _Query, auto _key, typename _Value>
	struct query_rebind final{
	private:
		static consteval auto impl() noexcept{
			using query_t = std::remove_cvref_t<_Query>;
			using replace_t = std::remove_cvref_t<decltype(_key)>;
			return []<std::size_t... _index>(std::index_sequence<_index...>){
				constexpr auto exist = [](fake::mezz_c auto _i){
					using first_type = std::tuple_element_t<_i.value, typename query_t::first_types>;
					using second_type = std::tuple_element_t<_i.value, typename query_t::second_types>;
					
					return std::same_as<replace_t, std::remove_cvref_t<first_type>>;
				};
				
				static_assert((exist(fake::mezz_v<_index>) || ...), "fake::query_rebind_t: key does NOT exist.");
				
				constexpr auto each = [](fake::mezz_c auto _i){
					using first_type = std::tuple_element_t<_i.value, typename query_t::first_types>;
					using second_type = std::tuple_element_t<_i.value, typename query_t::second_types>;
					
					if constexpr(std::same_as<replace_t, std::remove_cvref_t<first_type>>)
						return fake::pack_v<fake::mate<first_type, _Value>>;
					else
						return fake::pack_v<fake::mate<first_type, second_type>>;
				};
				using first_types = typename query_t::first_types;
				using second_types = typename query_t::second_types;
				
				return fake::pack_v<fake::query<fake::take_t<each(fake::mezz_v<_index>)>...>>;
			}(std::make_index_sequence<query_t::size>());
		}
		
	public:
		using type = fake::take_t<impl()>;
	};
	
	template<query_c _Query, auto _key, typename _Value>
	using query_rebind_t = typename query_rebind<_Query, _key, _Value>::type;
	
}

template<template<typename...> typename _Template, typename... _Args>
requires fake::flat_based_c<_Template<_Args...>>
struct std::tuple_size<_Template<_Args...>> : public std::integral_constant<std::size_t, sizeof...(_Args)>{};

template<std::size_t _index, template<typename...> typename _Template, typename... _Args>
requires (_index < sizeof...(_Args) && fake::flat_based_c<_Template<_Args...>>)
struct std::tuple_element<_index, _Template<_Args...>> :
	public fake::type_package<fake::take_t<_Template<_Args...>::type(fake::index_v<_index>)>>{};

template<template<typename...> typename _Template, typename... _Args>
requires fake::query_c<_Template<_Args...>>
struct std::tuple_size<_Template<_Args...>> : public std::integral_constant<std::size_t, sizeof...(_Args)>{};

template<std::size_t _index, template<typename...> typename _Template, typename... _Args>
requires (_index < sizeof...(_Args) && fake::query_c<_Template<_Args...>>)
struct std::tuple_element<_index, _Template<_Args...>> :
	public fake::type_package<
		fake::take_t<
			_Template<_Args...>::template mate<
				fake::take_t<_Template<_Args...>::first_types::type(fake::index_v<_index>)>
			>()
		>
	>{};

#include "pop.h"

#endif /* __FAKE_FLAT_H__ */ 
