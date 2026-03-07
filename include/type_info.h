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

#include "push.h"

#include "view.h"
#include "tuple.h"
#include "detect.h"
#include "fields.h"
#include "meta.h"

namespace fake::tool
{
	
	template<typename>
	struct adaptor{};
	
	template<template<typename...> typename _Template>
	struct adaptor<fake::generic<_Template>>{};
	
	template<template<typename, std::size_t> typename _Template>
	struct adaptor<fake::array_like<_Template>>{};
	
	template<template<char...> typename _Template>
	struct adaptor<fake::view_like<_Template>>{};
	
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
	
	template<typename _Type>
	concept custom_facet_c = requires(_Type &_e){
		{
			fake::mezz_v<
				facet_invoke(fake::view_v<"fake::facet::for_each::names">, fake::pack_v<std::remove_cvref_t<_Type>>)
			>.value
		} -> fake::flat_c;
		
		{facet_invoke(fake::view_v<"fake::facet::for_each::refs">, _e)} -> fake::flat_c;
		
		requires fake::tuple::contains_v<
			fake::mimic_t<
				decltype(
					facet_invoke(fake::view_v<"fake::facet::for_each::names">, fake::pack_v<std::remove_cvref_t<_Type>>)
				),
				std::tuple
			>,
			!fake::just_d<[]<fake::view_c>(){}>
		> == false;
		
		requires fake::tuple::contains_v<
			fake::mimic_t<decltype(facet_invoke(fake::view_v<"fake::facet::for_each::refs">, _e)), std::tuple>,
			!fake::lvalue_reference_d
		> == false;
		
		requires decltype(facet_invoke(fake::view_v<"fake::facet::for_each::refs">, _e))::size ==
			facet_invoke(fake::view_v<"fake::facet::for_each::names">, fake::pack_v<std::remove_cvref_t<_Type>>).size;
	};
	
}

namespace fake::custom
{
	
	namespace detail::for_each
	{
		
		namespace tool
		{
			
			template<typename _Type>
			concept unnamed_union_c = requires(decltype(fake::type_view(fake::pack_v<_Type>)) _name){
				requires (
					_name.template ends_with<"::<unnamed union>">() ||
					_name.template contains<"::(anonymous union">()
				);
			};
			
			template<typename _Type>
			concept unnamed_struct_c = requires(decltype(fake::type_view(fake::pack_v<_Type>)) _name){
				requires (
					_name.template ends_with<"::<unnamed struct>">() ||
					_name.template contains<"::(anonymous struct">()
				);
			};
			
			template<typename _Type>
			concept non_tricky_c = tool::unnamed_union_c<_Type> == false && tool::unnamed_struct_c<_Type> == false;
			
		}
		
		template<typename, auto _footprint> requires fake::meta::array_c<decltype(_footprint)> struct broker;
		
		inline constexpr auto placeholder_functor = []<fake::view_c, auto...>(auto&&){};
		
	}
	
	struct for_each
	{
	private:
		static constexpr fake::meta::unordered_map configure = []{};
		
		template<typename _ConfigToken> struct key final{};
		template<typename _ConfigToken> struct guard final{};
		struct forbid final{};
		struct placeholder final{};
		struct fields final{};
		template<typename _ConfigToken> struct scale final{};
		
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
			
			template<auto _vest, typename _ConfigToken, tool::adaptor_c _Adaptor, std::size_t _size, typename _Lambda>
			requires (instantiate_after_configure<_vest, _ConfigToken>())
			static consteval auto emplace_visitor(_Lambda &&_lambda) noexcept{
				using adaptor_t = std::remove_cvref_t<_Adaptor>;
				using lambda_t = std::remove_cvref_t<_Lambda>;
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				
				local.template emplace<[]{}>(fake::pack_v<adaptor_t>, fake::pack_v<lambda_t>);
				local.template emplace<[]{}>(fake::pack_v<scale<adaptor_t>>, fake::pack_v<fake::mezz_t<_size>>);
				
				return true;
			}
			
			template<auto _vest, typename _ConfigToken, tool::adaptor_c _Adaptor, typename _Lambda, typename _Size>
			requires (instantiate_after_configure<_vest, _ConfigToken>())
			static consteval auto emplace_visitor(_Lambda &&_lambda, _Size &&_size) noexcept{
				using adaptor_t = std::remove_cvref_t<_Adaptor>;
				using lambda_t = std::remove_cvref_t<_Lambda>;
				using scale_t = std::remove_cvref_t<_Size>;
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				
				local.template emplace<[]{}>(fake::pack_v<adaptor_t>, fake::pack_v<lambda_t>);
				local.template emplace<[]{}>(fake::pack_v<scale<adaptor_t>>, fake::pack_v<fake::mezz_t<scale_t{}>>);
				
				return true;
			}
			
			template<auto _vest, typename _ConfigToken, tool::steal_c _Steal>
			requires (instantiate_after_configure<_vest, _ConfigToken>() && fake::meta::array_c<_ConfigToken> == false)
			static consteval auto emplace_visitor() noexcept{
				using adaptor_t = typename extract<_Steal>::adaptor;
				using members_t = typename extract<_Steal>::members;
				
				emplace_visitor<[]{}, _ConfigToken, adaptor_t, std::tuple_size_v<members_t>>(
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
			
		private:
			template<typename _Tokens>
			struct element_extractor final{
				template<std::size_t _index>
				using type = fake::meta::array_element_t<_index, _Tokens>;
			};
			
		public:
			template<auto _vest, fake::meta::array_c _ConfigTokens, tool::steal_c _Steal>
			requires (instantiate_after_configure<_vest, _ConfigTokens>())
			static consteval auto emplace_visitor() noexcept{
				using adaptor_t = typename extract<_Steal>::adaptor;
				using members_t = typename extract<_Steal>::members;
				
				using element = element_extractor<_ConfigTokens>;
				
				[]<std::size_t... _index>(std::index_sequence<_index...>){
					(
						emplace_visitor<[]{}, typename element::type<_index>, adaptor_t, std::tuple_size_v<members_t>>(
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
			
			struct aggregate final{
				struct fields final{
				private:
					using tag = for_each::fields;
					
				public:
					template<auto _vest, typename _ConfigToken>
					requires (instantiate_after_configure<_vest, _ConfigToken>())
					static consteval auto enable() noexcept{
						constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
						
						local.template emplace<[]{}>(fake::pack_v<tag>, fake::pack_v<fake::mezz_t<true>>);
						
						return true;
					}
					
					template<auto _vest, typename _ConfigToken>
					requires (instantiate_after_configure<_vest, _ConfigToken>())
					static consteval auto disable() noexcept{
						constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
						
						local.template emplace<[]{}>(fake::pack_v<tag>, fake::pack_v<fake::mezz_t<false>>);
						
						return true;
					}
				};
				
				struct placeholder final{
				private:
					using tag = for_each::placeholder;
					
				public:
					template<auto _vest, typename _ConfigToken>
					requires (instantiate_after_configure<_vest, _ConfigToken>())
					static consteval auto enable() noexcept{
						constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
						
						local.template emplace<[]{}>(fake::pack_v<tag>, fake::pack_v<fake::mezz_t<true>>);
						
						return true;
					}
					
					template<auto _vest, typename _ConfigToken>
					requires (instantiate_after_configure<_vest, _ConfigToken>())
					static consteval auto disable() noexcept{
						constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
						
						local.template emplace<[]{}>(fake::pack_v<tag>, fake::pack_v<fake::mezz_t<false>>);
						
						return true;
					}
				};
			};
		};
		
	private:
		template<typename _Aggr, std::size_t _index>
		struct aggr_t final{};
		
		template<typename _Aggr>
		struct derive_t final{};
		
		template<typename _Aggr>
		struct empty_t final{};
		
		template<std::size_t _index>
		struct index_t final{};
		
		template<std::uintptr_t _offset>
		struct member_t final{};
		
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
			template<detail::for_each::tool::non_tricky_c _Type>
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
					
					if constexpr(std::derived_from<_Aggr, std::remove_cvref_t<_Type>>){
						if constexpr(aggr.contains<[]{}>(fake::pack_v<derive_t<_Aggr>>) == false)
							aggr.emplace<[]{}>(fake::pack_v<derive_t<_Aggr>>, fake::pack_v<std::true_type>);
						
						if constexpr(std::is_empty_v<std::remove_cvref_t<_Type>>)
							if constexpr(aggr.contains<[]{}>(fake::pack_v<empty_t<_Aggr>>) == false)
								aggr.emplace<[]{}>(fake::pack_v<empty_t<_Aggr>>, fake::pack_v<std::true_type>);
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
		requires std::is_aggregate_v<_Aggr>
		static consteval auto iterate(std::index_sequence<_index...>) noexcept{
			if constexpr(requires{_Aggr{any<_Aggr, _index>{}...};})
				return iterate<_Aggr>(std::make_index_sequence<sizeof...(_index) + 1>());
			else
				aggr_clear([]{});
		}
		
		template<typename _ConfigToken>
		static consteval auto enable_fields() noexcept{
			constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
			
			using enable = fake::take_t<local.template at<[]{}>(fake::pack_v<for_each::fields>)>;
			
			if constexpr(std::same_as<enable, fake::null_t>)
				return false;
			else
				return enable::value;
		}
		
		template<typename _ConfigToken>
		static consteval auto enable_placeholder() noexcept{
			constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
			
			using enable = fake::take_t<local.template at<[]{}>(fake::pack_v<for_each::placeholder>)>;
			
			if constexpr(std::same_as<enable, fake::null_t>)
				return true;
			else
				return enable::value;
		}
		
		template<typename _Type, bool _placeholder>
		static constexpr auto facet_size() noexcept{
			using namespace fake::literals;
			
			using type = std::remove_cvref_t<_Type>;
			
			constexpr fake::flat names = facet_invoke("fake::facet::for_each::names"_v, fake::pack_v<type>);
			
			return names.size;
		}
		
		template<typename _Type, bool _placeholder>
		static consteval auto facet_fields() noexcept{
			using type = std::remove_cvref_t<_Type>;
			
			return [](auto &&_e, const auto &_f){
				using namespace fake::literals;
				
				static constexpr fake::flat names = facet_invoke("fake::facet::for_each::names"_v, fake::pack_v<type>);
				const fake::flat_c auto refs = facet_invoke("fake::facet::for_each::refs"_v, _e);
				constexpr std::size_t size = names.size;
				
				[&refs, &_f]<std::size_t... _index>(std::index_sequence<_index...>){
					const auto each = [&refs, &_f](fake::mezz_c auto _i){
						auto &ref = refs.template value<_i.value>();
						if constexpr(requires{_f(ref);})
							_f(ref);
						else if constexpr(_placeholder)
							_f.template operator()<fake::take_t<names.type(fake::index_v<_i.value>)>>(ref);
						else
							_f.template operator()<fake::view_t<"">>(ref);
					};
					
					(each(fake::mezz_v<_index>), ...);
				}(std::make_index_sequence<refs.size>());
			};
		}
		
		template<typename _Aggr>
		requires requires{{iterate<std::remove_cvref_t<_Aggr>>(std::index_sequence<>{})} -> std::same_as<void>;}
		static consteval auto aggregate_size() noexcept{
			using type = std::remove_cvref_t<_Aggr>;
			
			// using a passed '_vest' from an evaluated context to make clang happy. 
			constexpr auto impl = []<std::size_t _index, auto _vest>(
				auto &_self,
				fake::mezz_t<_vest>,
				index_t<_index>
			) requires(
				!std::same_as<fake::take_t<aggr.at<_vest>(fake::pack_v<aggr_t<type, _index>>)>, fake::null_t>
			){
				if constexpr(requires{_self(_self, fake::mezz_v<[]{}>, index_t<_index + 1>{});})
					return _self(_self, fake::mezz_v<[]{}>, index_t<_index + 1>{});
				else
					return _index + 1;
			};
			
			if constexpr(requires{impl(impl, fake::mezz_v<[]{}>, index_t<0x0>{});})
				return impl(impl, fake::mezz_v<[]{}>, index_t<0x0>{});
			return std::size_t{};
		}
		
		template<typename _Aggr, bool _placeholder>
		requires // invalid if '_Aggr' derived from some empty-bases. 
			requires{{iterate<std::remove_cvref_t<_Aggr>>(std::index_sequence<>{})} -> std::same_as<void>;} &&
			std::same_as<fake::take_t<aggr.at<[]{}>(fake::pack_v<empty_t<std::remove_cvref_t<_Aggr>>>)>, fake::null_t>
		static consteval auto aggregate_visitor() noexcept{
			using origin = std::remove_cvref_t<_Aggr>;
			
			return [](auto &&_e, const auto &_f){
				const std::uintptr_t address = std::bit_cast<std::uintptr_t>(&_e);
				
				// using a passed '_vest' from an evaluated context to make clang happy. 
				auto impl = [address, &_f]<std::size_t _index, std::uintptr_t _offset, auto _vest>(
					auto &_self,
					fake::mezz_t<_vest>,
					index_t<_index>,
					member_t<_offset>
				) requires(
					!std::same_as<fake::take_t<aggr.at<_vest>(fake::pack_v<aggr_t<origin, _index>>)>, fake::null_t>
				){
					using type = fake::take_t<aggr.at<[]{}>(fake::pack_v<aggr_t<origin, _index>>)>;
					
					constexpr std::uintptr_t element = alignof(type);
					constexpr std::uintptr_t size = sizeof(type);
					constexpr std::uintptr_t align = std::max(size, element);
					constexpr std::uintptr_t offset = _offset + element - 1 & ~(element - 1);
					constexpr std::uintptr_t member = offset;
					constexpr std::uintptr_t next = offset + align;
					type &reference = *std::launder(std::bit_cast<type*>(address + member));
					
					if constexpr(requires{_f(reference);})
						_f(reference);
					else if constexpr(_placeholder)
						_f.template operator()<decltype(fake::to_view_v<_index>)>(reference);
					else
						_f.template operator()<fake::view_t<"">>(reference);
					
					if constexpr(requires{_self(_self, fake::mezz_v<[]{}>, index_t<_index + 1>{}, member_t<next>{});})
						_self(_self, fake::mezz_v<[]{}>, index_t<_index + 1>{}, member_t<next>{});
				};
				
				if constexpr(requires{impl(impl, fake::mezz_v<[]{}>, index_t<0>{}, member_t<0x0>{});})
					impl(impl, fake::mezz_v<[]{}>, index_t<0>{}, member_t<0x0>{});
			};
		}
		
		template<typename _Aggr, bool _placeholder>
		requires requires{{iterate<std::remove_cvref_t<_Aggr>>(std::index_sequence<>{})} -> std::same_as<void>;}
		static consteval auto aggregate_fields() noexcept{
			using type = std::remove_cvref_t<_Aggr>;
			
			return [](auto &&_e, const auto &_f){
				if constexpr(aggr.contains<[]{}>(fake::pack_v<derive_t<type>>)){
					aggregate_visitor<_Aggr, _placeholder>()(std::forward<_Aggr>(_e), _f);
				}
				else{
					constexpr std::size_t size = aggregate_size<_Aggr>();
					static constexpr fake::flat names = fake::aggregate::fields<type, size>();
					const fake::flat_c auto refs = fake::aggregate::references<size>(_e);
					
					[&refs, &_f]<std::size_t... _index>(std::index_sequence<_index...>){
						const auto each = [&refs, &_f](fake::mezz_c auto _i){
							auto &ref = refs.template value<_i.value>();
							if constexpr(requires{_f(ref);})
								_f(ref);
							else if constexpr(_placeholder)
								_f.template operator()<fake::take_t<names.type(fake::index_v<_i.value>)>>(ref);
							else
								_f.template operator()<fake::view_t<"">>(ref);
						};
						
						(each(fake::mezz_v<_index>), ...);
					}(std::make_index_sequence<refs.size>());
				}
			};
		}
		
	private:
		template<auto...>
		struct query_result;
		
		template<fake::pack_c auto _functor>
		struct query_result<_functor> final{
			static constexpr auto functor = _functor;
		};
		
		template<fake::pack_c auto _functor, fake::mezz_c auto _size>
		requires requires{{_size.value} -> std::convertible_to<std::size_t>;}
		struct query_result<_functor, _size> final{
			static constexpr auto functor = _functor;
			static constexpr std::size_t size = _size.value;
		};
		
		template<fake::pack_c auto _functor, fake::mezz_c auto _size>
		requires (requires{{_size.value} -> std::convertible_to<std::size_t>;} == false)
		struct query_result<_functor, _size> final{
			static constexpr auto functor = _functor;
			static constexpr auto size = _size.value;
		};
		
	private:
		template<typename _ConfigToken, auto _footprint> struct cpo_type;
		
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
					requires(
						fake::type_package<tool::adaptor<decltype(_t)>> _key,
						fake::take_t<decltype(local){}.template at<[]{}>(decltype(_key){})> functor,
						cpo_type<_ConfigToken, _footprint> _cpo
					){functor(std::forward<_Type>(_e), _f, _cpo);} ||
					requires(
						fake::type_package<tool::adaptor<fake::pattern_t<decltype(_t)>>> _key,
						fake::take_t<decltype(local){}.template at<[]{}>(decltype(_key){})> functor,
						cpo_type<_ConfigToken, _footprint> _cpo
					){functor(std::forward<_Type>(_e), _f, _cpo);} ||
					tool::custom_facet_c<_Type> ||
					std::is_aggregate_v<decltype(_t)>;
			}
		inline static constexpr auto method(_Type &&_e, const _Func &_f){
			using type_t = std::remove_cvref_t<_Type>;
			
			constexpr auto query = [](fake::pack_c auto _pack){
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				
				constexpr fake::pack_c auto size = local.template at<[]{}>(fake::pack_v<scale<fake::take_t<_pack>>>);
				
				if constexpr(std::same_as<fake::take_t<size>, fake::null_t>)
					return query_result<local.template at<[]{}>(_pack)>{};
				else
					return query_result<local.template at<[]{}>(_pack), fake::take_v<size>>{};
			};
			
			using adaptor_t = tool::adaptor<type_t>;
			using functor_t = fake::take_t<query(fake::pack_v<adaptor_t>).functor>;
			constexpr bool fields = enable_fields<_ConfigToken>();
			constexpr bool placeholder = enable_placeholder<_ConfigToken>();
			constexpr auto cpo = cpo_type<_ConfigToken, _footprint>{};
			
			if constexpr(requires{functor_t{}(std::forward<_Type>(_e), _f, cpo);}){
				functor_t{}(std::forward<_Type>(_e), _f, cpo);
			}
			else if constexpr(!std::same_as<functor_t, fake::null_t>){
				functor_t{}(std::forward<_Type>(_e), _f);
			}
			else if constexpr(requires{typename fake::pattern_t<type_t>;}){
				using adaptor_t = tool::adaptor<fake::pattern_t<type_t>>;
				using functor_t = fake::take_t<query(fake::pack_v<adaptor_t>).functor>;
				
				if constexpr(requires{functor_t{}(std::forward<_Type>(_e), _f, cpo);})
					functor_t{}(std::forward<_Type>(_e), _f, cpo);
				else if constexpr(!std::same_as<functor_t, fake::null_t>)
					functor_t{}(std::forward<_Type>(_e), _f);
				else if constexpr(tool::custom_facet_c<_Type>)
					facet_fields<_Type, placeholder>()(std::forward<_Type>(_e), _f);
				else if constexpr(fields)
					aggregate_fields<_Type, placeholder>()(std::forward<_Type>(_e), _f);
				else
					aggregate_visitor<_Type, placeholder>()(std::forward<_Type>(_e), _f);
			}
			else if constexpr(tool::custom_facet_c<_Type>){
				facet_fields<_Type, placeholder>()(std::forward<_Type>(_e), _f);
			}
			else if constexpr(fields){
				aggregate_fields<_Type, placeholder>()(std::forward<_Type>(_e), _f);
			}
			else{
				aggregate_visitor<_Type, placeholder>()(std::forward<_Type>(_e), _f);
			}
		}
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		requires
			fake::meta::array_c<decltype(_footprint)> &&
			requires(
				fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)> local,
				std::remove_cvref_t<_Type> _t
			){
				requires !std::same_as<decltype(local), fake::null_t>;
				requires std::is_aggregate_v<decltype(_t)> && enable_fields<_ConfigToken>() ||
					tool::custom_facet_c<_Type>;
			}
		inline static consteval auto field(){
			using namespace fake::literals;
			
			using type = std::remove_cvref_t<_Type>;
			
			if constexpr(tool::custom_facet_c<_Type>)
				return facet_invoke("fake::facet::for_each::names"_v, fake::pack_v<type>);
			else
				return fake::aggregate::fields<type, aggregate_size<type>()>();
		}
		
		template<typename _ConfigToken, fake::meta::array_c auto _footprint, typename _Type>
		inline static consteval auto object() noexcept{
			using type = std::remove_cvref_t<_Type>;
			return requires(_Type &&_e){
				for_each::method<_ConfigToken, _footprint>(_e, detail::for_each::placeholder_functor);
			};
		}
		
		template<typename _ConfigToken, fake::meta::array_c auto _footprint, typename _Type>
		inline static consteval auto array() noexcept{
			using type = std::remove_cvref_t<_Type>;
			using array_type = std::conditional_t<fake::array_c<type>, type, std::nullptr_t[]>;
			return fake::array_c<type> && !std::same_as<fake::array_value_t<array_type>, char>;
		}
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		requires
			fake::meta::array_c<decltype(_footprint)> &&
			requires(
				fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)> local,
				std::remove_cvref_t<_Type> _t
			){
				requires !std::same_as<decltype(local), fake::null_t>;
				requires
					requires(fake::type_package<tool::adaptor<decltype(_t)>> _key){
						decltype(local){}.template at<[]{}>(decltype(_key){});
					} ||
					requires(fake::type_package<tool::adaptor<fake::pattern_t<decltype(_t)>>> _key){
						decltype(local){}.template at<[]{}>(decltype(_key){});
					} ||
					tool::custom_facet_c<_Type> ||
					std::is_aggregate_v<decltype(_t)>;
			}
		inline static constexpr auto size(_Type &&_e){
			using type_t = std::remove_cvref_t<_Type>;
			
			using array_type = std::conditional_t<fake::array_c<type_t>, type_t, std::nullptr_t[]>;
			constexpr bool object = for_each::object<_ConfigToken, _footprint, _Type>();
			constexpr bool array = for_each::array<_ConfigToken, _footprint, _Type>();
			constexpr auto cpo = cpo_type<_ConfigToken, _footprint>{};
			
			constexpr auto query = [](fake::pack_c auto _pack){
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				
				constexpr fake::pack_c auto size = local.template at<[]{}>(fake::pack_v<scale<fake::take_t<_pack>>>);
				
				if constexpr(std::same_as<fake::take_t<size>, fake::null_t>)
					return query_result<local.template at<[]{}>(_pack)>{};
				else
					return query_result<local.template at<[]{}>(_pack), fake::take_v<size>>{};
			};
			
			using adaptor_t = tool::adaptor<type_t>;
			constexpr auto qurey_result_v = query(fake::pack_v<adaptor_t>);
			using functor_t = fake::take_t<qurey_result_v.functor>;
			if constexpr(object == false && array == false){
				return fake::null_t{};
			}
			else if constexpr(!std::same_as<functor_t, fake::null_t>){
				if constexpr(requires{{qurey_result_v.size} -> std::convertible_to<std::size_t>;})
					return qurey_result_v.size;
				else if constexpr(requires{{qurey_result_v.size(_e)} -> std::convertible_to<std::size_t>;})
					return qurey_result_v.size(_e);
				else if constexpr(requires{{qurey_result_v.size(_e, cpo)} -> std::convertible_to<std::size_t>;})
					return qurey_result_v.size(_e, cpo);
				else if constexpr(requires{{std::tuple_size<type_t>::value} -> std::convertible_to<std::size_t>;})
					return std::tuple_size_v<type_t>;
				else if constexpr(requires{{std::ranges::size(_e)} -> std::convertible_to<std::size_t>;})
					return std::ranges::size(_e);
				else if constexpr(requires{{_e.size} -> std::convertible_to<std::size_t>;})
					return _e.size;
				else
					return fake::null_t{};
			}
			else if constexpr(requires{typename fake::pattern_t<type_t>;}){
				using adaptor_t = tool::adaptor<fake::pattern_t<type_t>>;
				constexpr auto qurey_result_v = query(fake::pack_v<adaptor_t>);
				using functor_t = fake::take_t<qurey_result_v.functor>;
				
				if constexpr(!std::same_as<functor_t, fake::null_t>){
					if constexpr(requires{{qurey_result_v.size} -> std::convertible_to<std::size_t>;})
						return qurey_result_v.size;
					else if constexpr(requires{{qurey_result_v.size(_e)} -> std::convertible_to<std::size_t>;})
						return qurey_result_v.size(_e);
					else if constexpr(requires{{qurey_result_v.size(_e, cpo)} -> std::convertible_to<std::size_t>;})
						return qurey_result_v.size(_e, cpo);
					else if constexpr(requires{{std::tuple_size<type_t>::value} -> std::convertible_to<std::size_t>;})
						return std::tuple_size_v<type_t>;
					else if constexpr(requires{{std::ranges::size(_e)} -> std::convertible_to<std::size_t>;})
						return std::ranges::size(_e);
					else if constexpr(requires{{_e.size} -> std::convertible_to<std::size_t>;})
						return _e.size;
					else
						return fake::null_t{};
				}
				else if constexpr(tool::custom_facet_c<_Type>){
					return facet_size<_Type, placeholder>();
				}
				else{
					return aggregate_size<_Type>();
				}
			}
			else if constexpr(tool::custom_facet_c<_Type>){
				return facet_size<_Type, placeholder>();
			}
			else{
				return aggregate_size<_Type>();
			}
		}
		
	private:
		template<typename _ConfigToken, auto _footprint>
		struct cpo_type final{
			template<typename _Type, typename _Func>
			inline static constexpr auto method(_Type &&_e, const _Func &_f)
			requires requires{for_each::method<_ConfigToken, _footprint>(std::forward<_Type>(_e), _f);}{
				return for_each::method<_ConfigToken, _footprint>(std::forward<_Type>(_e), _f);
			}
			
			template<typename _Type>
			inline static constexpr auto size(_Type &&_e)
			requires requires{for_each::size<_ConfigToken, _footprint>(std::forward<_Type>(_e));}{
				return for_each::size<_ConfigToken, _footprint>(std::forward<_Type>(_e));
			}
			
			template<typename _Type>
			inline static consteval auto field()
			requires requires{for_each::field<_ConfigToken, _footprint, _Type>();}{
				return for_each::field<_ConfigToken, _footprint, _Type>();
			}
		};
	};
	
	namespace detail::for_each
	{
		
		template<typename _ConfigToken, auto _footprint, typename _Type, typename _Func>
		concept invocable_c = requires(_Type &&_e, const _Func &_f){
			requires fake::meta::array_c<decltype(_footprint)>;
			custom::for_each::method<_ConfigToken, _footprint>(std::forward<_Type>(_e), _f);
		};
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		concept size_c = requires(_Type &&_e){
			requires fake::meta::array_c<decltype(_footprint)>;
			custom::for_each::method<_ConfigToken, _footprint>(std::forward<_Type>(_e), placeholder_functor);
			{custom::for_each::size<_ConfigToken, _footprint>(std::forward<_Type>(_e))} -> std::same_as<std::size_t>;
		};
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		concept object_c = requires{
			requires fake::meta::array_c<decltype(_footprint)>;
			{custom::for_each::object<_ConfigToken, _footprint, _Type>()} -> std::same_as<bool>;
		};
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		concept array_c = requires{
			requires fake::meta::array_c<decltype(_footprint)>;
			{custom::for_each::array<_ConfigToken, _footprint, _Type>()} -> std::same_as<bool>;
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
			
			template<typename _Type>
			requires size_c<_ConfigToken, _footprint, _Type>
			inline constexpr auto size(_Type &&_e) const{
				return custom::for_each::size<_ConfigToken, _footprint>(std::forward<_Type>(_e));
			}
			
			template<typename _Type>
			requires object_c<_ConfigToken, _footprint, _Type>
			inline constexpr auto object() const{
				return custom::for_each::object<_ConfigToken, _footprint, _Type>();
			}
			
			template<typename _Type>
			requires array_c<_ConfigToken, _footprint, _Type>
			inline constexpr auto array() const{
				return custom::for_each::array<_ConfigToken, _footprint, _Type>();
			}
		};
		
	}
	
}

namespace fake
{
	
	template<typename _ConfigToken, auto _footprint = custom::for_each::config::footprint<_ConfigToken>([]{})>
	requires fake::meta::array_c<decltype(_footprint)>
	inline constexpr custom::detail::for_each::broker<_ConfigToken, _footprint> for_each;
	
	template<auto _vest, typename _ConfigToken, auto _fp = custom::for_each::config::footprint<_ConfigToken>(_vest)>
	requires fake::meta::array_c<decltype(_fp)>
	inline constexpr auto for_each_w = fake::for_each<_ConfigToken, _fp>;
	
}

#include "pop.h"

#endif /*__FAKE_CPO_TYPEINFO_H__*/ 
