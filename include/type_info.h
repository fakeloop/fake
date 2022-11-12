#ifndef __FAKE_CPO_TYPEINFO_H__
#define __FAKE_CPO_TYPEINFO_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "view.h"
#include "meta.h"
#include "container.h"

namespace fake::tool
{
	
	template<typename>
	struct adaptor{};
	
	template<template<typename...> typename _Template>
	struct adaptor<fake::generic<_Template>>{};
	
	template<template<typename, std::size_t> typename _Template>
	struct adaptor<fake::array_like<_Template>>{};
	
	template<typename _Type>
	concept adaptor_c = fake::trait_v<adaptor, _Type>;
	
	template<auto, typename, auto...>
	struct steal;
	
	template<typename>
	struct is_steal : std::false_type{};
	
	template<auto _vest, typename _Adaptor, auto... _members>
	struct is_steal<steal<_vest, _Adaptor, _members...>> : std::true_type{};
	
	template<typename _Type>
	inline constexpr bool is_steal_v = is_steal<std::remove_cvref_t<_Type>>::value;
	
	template<typename _Type>
	concept steal_c = is_steal_v<_Type>;
	
	template<auto>
	struct name;
	
	template<typename _Object, typename _Type, _Type _Object::*_member>
	struct name<_member>{
	private:
		template<fake::mezz_c _Mezz, std::size_t _size>
		static consteval std::string_view shrink() noexcept{
			std::string_view mezz = fake::symbol::string_view<_Mezz>();
			constexpr std::string_view temp = fake::symbol::string_view<fake::mezz_t<0>>();
			return mezz.remove_prefix(temp.length() + _size + 1), mezz.remove_suffix(1), mezz;
		}
		
		static constexpr std::string_view type_sv = fake::symbol::string_view<_Object>();
		static constexpr std::string_view member_sv = shrink<fake::mezz_t<_member>, type_sv.size()>();
		
	public:
		using member = fake::view_t<fake::detail::view::string<member_sv.size() + 1>{member_sv.data()}>;
	};
	
}

namespace fake::custom
{
	
	namespace detail::for_each
	{
		
		template<typename, auto _footprint> requires fake::meta::array_c<decltype(_footprint)> struct broker;
		
	}
	
	struct for_each
	{
	private:
		static constexpr fake::meta::unordered_map configure = []{};
		
		template<typename _ConfigToken> struct key final{};
		template<typename _ConfigToken> struct guard final{};
		struct forbid final{};
		
	private:
		template<typename>
		struct extract;
		
		template<tool::adaptor_c _Adaptor, template<auto, typename, auto...> typename _Steal, auto _v, auto... _members>
		struct extract<_Steal<_v, _Adaptor, _members...>>{
			using adaptor = _Adaptor;
			using members = std::tuple<fake::mezz_t<_members>...>;
		};
		
	public:
		struct config final
		{
		private:
			template<auto _vest, typename _ConfigToken>
			requires (
				configure.contains<_vest>(fake::pack_v<key<_ConfigToken>>) &&
				fake::meta::array_c<_ConfigToken> == false
			)
			static consteval auto instantiate_after_configure() noexcept{
				return !std::same_as<fake::take_t<configure.at<_vest>(fake::pack_v<guard<_ConfigToken>>)>, forbid>;
			}
			
			template<auto _vest, typename _ConfigToken>
			requires (
				configure.contains<_vest>(fake::pack_v<key<_ConfigToken>>) == false &&
				fake::meta::array_c<_ConfigToken> == false
			)
			static consteval auto instantiate_after_configure() noexcept{
				constexpr fake::meta::unordered_map local = []{};
				using local_t = std::remove_cvref_t<decltype(local)>;
				configure.emplace<[]{}>(fake::pack_v<key<_ConfigToken>>, fake::pack_v<local_t>);
				
				return !std::same_as<fake::take_t<configure.at<_vest>(fake::pack_v<guard<_ConfigToken>>)>, forbid>;
			}
			
			template<auto _vest, fake::meta::array_c _ConfigTokens>
			static consteval auto instantiate_after_configure() noexcept{
				return []<std::size_t... _i>(std::index_sequence<_i...>){
					return (instantiate_after_configure<[]{}, fake::meta::array_element_t<_i, _ConfigTokens>>() && ...);
				}(std::make_index_sequence<fake::meta::array_size_v<_ConfigTokens>>());
			}
			
		public:
			template<typename _ConfigToken>
			static consteval auto footprint(auto) noexcept{
				instantiate_after_configure<[]{}, _ConfigToken>();
				configure.emplace<[]{}>(fake::pack_v<guard<_ConfigToken>>, fake::pack_v<forbid>);
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				
				return local.data([]{});
			}
			
		public:
			template<auto _vest, typename _ConfigToken, tool::adaptor_c _Adaptor, typename _Lambda>
			requires (instantiate_after_configure<_vest, _ConfigToken>())
			static consteval auto emplace_visitor(_Lambda &&_lambda) noexcept{
				using adaptor_t = std::remove_cvref_t<_Adaptor>;
				using lambda_t = std::remove_cvref_t<_Lambda>;
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				
				local.template emplace<[]{}>(fake::pack_v<adaptor_t>, fake::pack_v<lambda_t>);
				
				return true;
			}
			
			template<auto _vest, typename _ConfigToken, tool::steal_c _Steal>
			requires (instantiate_after_configure<_vest, _ConfigToken>() && fake::meta::array_c<_ConfigToken> == false)
			static consteval auto emplace_visitor() noexcept{
				using adaptor_t = typename extract<_Steal>::adaptor;
				using members_t = typename extract<_Steal>::members;
				
				emplace_visitor<[]{}, _ConfigToken, adaptor_t>(
					[](auto &&_e, const auto &_f){
						[&]<std::size_t... _index>(std::index_sequence<_index...>){
							if constexpr(requires{(_f(_e.*std::tuple_element_t<_index, members_t>::value), ...);})
								(_f(_e.*std::tuple_element_t<_index, members_t>::value), ...);
							else
								(
									_f.template operator()<
										typename tool::name<std::tuple_element_t<_index, members_t>::value>::member
									>(_e.*std::tuple_element_t<_index, members_t>::value),
									...
								);
						}(std::make_index_sequence<std::tuple_size_v<members_t>>());
					}
				);
				
				return true;
			}
			
			template<auto _vest, fake::meta::array_c _ConfigTokens, tool::steal_c _Steal>
			requires (instantiate_after_configure<_vest, _ConfigTokens>())
			static consteval auto emplace_visitor() noexcept{
				using adaptor_t = typename extract<_Steal>::adaptor;
				using members_t = typename extract<_Steal>::members;
				
				[]<std::size_t... _index>(std::index_sequence<_index...>){
					(
						emplace_visitor<[]{}, fake::meta::array_element_t<_index, _ConfigTokens>, adaptor_t>(
							[](auto &&_e, const auto &_f){
								[&]<std::size_t... _i>(std::index_sequence<_i...>){
									if constexpr(requires{(_f(_e.*std::tuple_element_t<_i, members_t>::value), ...);})
										(_f(_e.*std::tuple_element_t<_i, members_t>::value), ...);
									else
										(
											_f.template operator()<
												typename tool::name<std::tuple_element_t<_i, members_t>::value>::member
											>(_e.*std::tuple_element_t<_i, members_t>::value),
											...
										);
								}(std::make_index_sequence<std::tuple_size_v<members_t>>());
							}
						),
						...
					);
				}(std::make_index_sequence<fake::meta::array_size_v<_ConfigTokens>>());
				
				return true;
			}
		};
		
	private:
		template<typename _Type>
		static auto byte_address(_Type &_type) noexcept{
			return &const_cast<char&>(reinterpret_cast<const volatile char&>(_type));
		}
		
	private:
		template<typename _Aggr, std::size_t _index>
		struct aggr_t final{};
		
		template<std::size_t _index>
		struct index_t final{};
		
		static constexpr fake::meta::unordered_map aggr = []{};
		static constexpr fake::meta::variant aggr_resume = []{};
		static constexpr fake::meta::variant aggr_ignore = []{};
		
		static consteval auto aggr_clear(auto) noexcept{
			aggr_resume.assign<[]{}>(fake::mezz_v<0>);
			aggr_ignore.assign<[]{}>(fake::mezz_v<0>);
		}
		
		static consteval void aggr_init() noexcept{
			aggr_clear([]{});
		}
		
		// using a passed '_vest' from an evaluated context to make clang happy. 
		template<typename _Aggr, std::size_t _index>
		struct any final{
		private:
			template<typename _Type>
			static consteval auto record() noexcept{
				constexpr std::size_t array_resume = fake::take_t<aggr_resume([]{})>{}.value;
				constexpr std::size_t array_ignore = fake::take_t<aggr_ignore([]{})>{}.value;
				
				if constexpr(_index >= array_resume && empty_array<std::remove_cvref_t<_Type>>::value == false){
					constexpr std::size_t index = _index - array_ignore;
					aggr.emplace<[]{}>(fake::pack_v<aggr_t<_Aggr, index>>, fake::pack_v<_Type>);
					
					if constexpr(fake::is_array_v<_Type>){
						constexpr std::size_t array_size = fake::array_size_v<_Type>;
						aggr_resume.assign<[]{}>(fake::mezz_v<_index + array_size>);
						aggr_ignore.assign<[]{}>(fake::mezz_v<array_ignore + array_size - 1>);
					}
				}
				
				return true;
			}
			
			template<typename _Type>
			static constexpr decltype(auto) evil_cast() noexcept{
				union eval_union{std::nullptr_t null; _Type* type;};
				
				return *eval_union{nullptr}.type;
			};
			
		public:
			template<typename _Type>
			requires fake::to_trait_c<record<_Type>()>
			constexpr operator _Type() const noexcept{return std::move(evil_cast<_Type>());}
		};
		
		template<typename _Aggr, std::size_t... _index>
		static consteval auto iterate(std::index_sequence<_index...>) noexcept{
			if constexpr(requires{_Aggr{any<_Aggr, _index>{}...};})
				return iterate<_Aggr>(std::make_index_sequence<sizeof...(_index) + 1>());
			else
				aggr_clear([]{});
		}
		
		template<typename _Aggr>
		requires requires{{iterate<std::remove_cvref_t<_Aggr>>(std::index_sequence<>{})} -> std::same_as<void>;}
		static consteval auto aggregate_visitor() noexcept{
			using type = std::remove_cvref_t<_Aggr>;
			
			return [](auto &&_e, const auto &_f){
				std::size_t offset = 0;
				// using a passed '_vest' from an evaluated context to make clang happy. 
				auto impl = [&]<std::size_t _index, auto _vest>(
					auto &_self,
					fake::mezz_t<_vest>,
					index_t<_index>
				) requires(
					!std::same_as<fake::take_t<aggr.at<_vest>(fake::pack_v<aggr_t<type, _index>>)>, fake::null_t>
				){
					using type = fake::take_t<aggr.at<[]{}>(fake::pack_v<aggr_t<type, _index>>)>;
					constexpr std::size_t element = alignof(type);
					constexpr std::size_t size = sizeof(type);
					constexpr std::size_t bigger = (size > element ? size : element);
					offset = (offset + element - 1 & ~(element - 1)) + bigger;
					type &reference = *reinterpret_cast<type*>(for_each::byte_address(_e) + offset - bigger);
					
					if constexpr(requires{_f(reference);})
						_f(reference);
					else
						_f.template operator()<decltype(fake::to_view_v<_index>)>(reference);
					
					if constexpr(requires{_self(_self, fake::mezz_v<[]{}>, index_t<_index + 1>{});})
						_self(_self, fake::mezz_v<[]{}>, index_t<_index + 1>{});
				};
				
				if constexpr(requires{impl(impl, fake::mezz_v<[]{}>, index_t<0>{});})
					impl(impl, fake::mezz_v<[]{}>, index_t<0>{});
			};
		}
		
	private:
		template<typename, auto _footprint>
		requires fake::meta::array_c<decltype(_footprint)>
		friend struct detail::for_each::broker;
		
		template<typename _ConfigToken, auto _footprint, typename _Type, typename _Func>
		requires
			fake::meta::array_c<decltype(_footprint)> &&
			requires(
				fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)> local,
				std::remove_cvref_t<_Type> _t,
				_Type &&_e,
				const _Func &_f
			){
				requires !std::same_as<decltype(local), fake::null_t>;
				requires
					requires(
						fake::type_package<tool::adaptor<decltype(_t)>> _key,
						fake::take_t<decltype(local){}.template at<[]{}>(decltype(_key){})> functor
					){functor(std::forward<_Type>(_e), _f);} ||
					requires(
						fake::type_package<tool::adaptor<fake::pattern_t<decltype(_t)>>> _key,
						fake::take_t<decltype(local){}.template at<[]{}>(decltype(_key){})> functor
					){functor(std::forward<_Type>(_e), _f);} ||
					std::is_aggregate_v<decltype(_t)>;
			}
		inline static constexpr auto method(_Type &&_e, const _Func &_f){
			using type_t = std::remove_cvref_t<_Type>;
			
			constexpr auto query = [](fake::pack_c auto _pack){
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				return local.template at<[]{}>(_pack);
			};
			
			using adaptor_t = tool::adaptor<type_t>;
			using functor_t = fake::take_t<query(fake::pack_v<adaptor_t>)>;
			if constexpr(!std::same_as<functor_t, fake::null_t>){
				functor_t{}(std::forward<_Type>(_e), _f);
			}
			else if constexpr(requires{typename fake::pattern_t<type_t>;}){
				using adaptor_t = tool::adaptor<fake::pattern_t<type_t>>;
				using functor_t = fake::take_t<query(fake::pack_v<adaptor_t>)>;
				
				if constexpr(!std::same_as<functor_t, fake::null_t>)
					functor_t{}(std::forward<_Type>(_e), _f);
				else
					aggregate_visitor<_Type>()(std::forward<_Type>(_e), _f);
			}
			else{
				aggregate_visitor<_Type>()(std::forward<_Type>(_e), _f);
			}
		}
	};
	
	namespace detail::for_each
	{
		
		template<typename _ConfigToken, auto _footprint, typename _Type, typename _Func>
		concept invocable_c = requires(_Type &&_e, const _Func &_f){
			requires fake::meta::array_c<decltype(_footprint)>;
			custom::for_each::method<_ConfigToken, _footprint>(std::forward<_Type>(_e), _f);
		};
		
		// currying broker. 
		template<typename _ConfigToken, auto _footprint>
		requires fake::meta::array_c<decltype(_footprint)>
		struct broker
		{
		public:
			template<typename _Type, typename _Func>
			requires invocable_c<_ConfigToken, _footprint, _Type, _Func>
			inline constexpr auto operator()(_Type &&_e, const _Func &_f) const{
				return custom::for_each::method<_ConfigToken, _footprint>(std::forward<_Type>(_e), _f);
			}
		};
		
	}
	
}

namespace fake
{
	
	template<typename _ConfigToken, auto _footprint = custom::for_each::config::footprint<_ConfigToken>([]{})>
	requires fake::meta::array_c<decltype(_footprint)>
	inline constexpr custom::detail::for_each::broker<_ConfigToken, _footprint> for_each;
	
}

#endif /*__FAKE_CPO_TYPEINFO_H__*/ 
