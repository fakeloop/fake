#ifndef __DEMO_FOREACH_H__
#define __DEMO_FOREACH_H__

#include "view.h"
#include "meta.h"

namespace demo
{
	
	namespace tool
	{
		
		template<template<typename...> typename>
		struct tempack{};
		
		template<typename>
		struct tempextr{};
		
		template<template<typename...> typename _Template, typename... _Paramaters>
		struct tempextr<_Template<_Paramaters...>>{using type = tempack<_Template>;};
		
		template<typename _Type>
		using tempextr_t = typename tempextr<_Type>::type;
		
		template<typename>
		struct adaptor{};
		
		template<template<typename...> typename _Template>
		struct adaptor<tempack<_Template>>{};
		
		template<typename _Type>
		concept adaptor_c = fake::trait_v<adaptor, _Type>;
		
		template<typename, auto...>
		struct steal;
		
		template<typename>
		struct is_steal : std::false_type{};
		
		template<typename _Adaptor, auto... _members>
		struct is_steal<steal<_Adaptor, _members...>> : std::true_type{};
		
		template<typename _Type>
		inline constexpr bool is_steal_v = is_steal<std::remove_cvref_t<_Type>>::value;
		
		template<typename _Type>
		concept steal_c = is_steal_v<_Type>;
		
		template<auto, auto>
		struct name;
		
		template<typename _Object, typename _Type, _Type _Object::*_member>
		struct name<_member, _member>{
		private:
			template<fake::mezz_c _Mezz, std::size_t _size>
			static consteval std::string_view shrink() noexcept{
				std::string_view mezz = fake::symbol::string_view<_Mezz>();
				constexpr std::string_view temp = fake::symbol::string_view<fake::mezz_t<0>>();
				return mezz.remove_prefix(temp.length() + _size + 1), mezz.remove_suffix(1), mezz;
			}
			
		public:
			static constexpr std::string_view type = fake::symbol::string_view<_Object>();
			static constexpr std::string_view member = shrink<fake::mezz_t<_member>, type.size()>();
		};
		
	}
	
	namespace custom
	{
		
		namespace detail::for_each{template<typename, fake::meta::array_c auto>struct broker;}
		
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
			
			template<tool::adaptor_c _Adaptor, template<typename, auto...> typename _Steal, auto... _members>
			struct extract<_Steal<_Adaptor, _members...>>{
				using adaptor = _Adaptor;
				using members = std::tuple<fake::mezz_t<_members>...>;
			};
			
		public:
			struct config{
				template<typename _ConfigToken>
				static consteval auto footprint(auto) noexcept{
					configure.emplace<[]{}>(fake::pack_v<guard<_ConfigToken>>, fake::pack_v<forbid>);
					using local_t = fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
					
					if constexpr(std::same_as<local_t, fake::null_t>)
						return fake::meta::array<>{};
					else
						return local_t{}.data([]{});
				}
				
				// * should be 'consteval'. 
				// * using 'constexpr' to make 'gcc (11.x.x)' happy, or there would be an ICE. 
				template<auto, typename _ConfigToken, tool::adaptor_c _Adaptor, typename _Lambda>
				requires (!std::same_as<fake::take_t<configure.at<[]{}>(fake::pack_v<guard<_ConfigToken>>)>, forbid>)
				static constexpr auto emplace_visitor(_Lambda &&_lambda) noexcept{
					using adaptor_t = std::remove_cvref_t<_Adaptor>;
					using lambda_t = std::remove_cvref_t<_Lambda>;
					using local_t = fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
					
					if constexpr(std::same_as<local_t, fake::null_t>){
						constexpr fake::meta::unordered_map local = []{};
						using local_t = std::remove_cvref_t<decltype(local)>;
						configure.emplace<[]{}>(fake::pack_v<key<_ConfigToken>>, fake::pack_v<local_t>);
						
						local_t{}.template emplace<[]{}>(fake::pack_v<adaptor_t>, fake::pack_v<lambda_t>);
					}
					else{
						local_t{}.template emplace<[]{}>(fake::pack_v<adaptor_t>, fake::pack_v<lambda_t>);
					}
					
					return true;
				}
				
				template<auto, typename _ConfigToken, tool::steal_c _Steal>
				requires (!std::same_as<fake::take_t<configure.at<[]{}>(fake::pack_v<guard<_ConfigToken>>)>, forbid>)
				static consteval auto emplace_visitor() noexcept{
					using adaptor_t = typename extract<_Steal>::adaptor;
					using members_t = typename extract<_Steal>::members;
					
					emplace_visitor<[]{}, _ConfigToken, adaptor_t>(
						[](auto &&_e, const auto &_f){
							[&]<std::size_t... _index>(std::index_sequence<_index...>){
								(
									_f(
										_e.*std::tuple_element_t<_index, members_t>::value,
										tool::name<
											std::tuple_element_t<_index, members_t>::value,
											std::tuple_element_t<_index, members_t>::value
										>::member
									),
									...
								);
							}(std::make_index_sequence<std::tuple_size_v<members_t>>());
						}
					);
					
					return true;
				}
			};
			
		private:
			template<typename _Aggr, std::size_t _index>
			struct aggr_t final{};
			
			template<std::size_t _index>
			struct index_t final{};
			
			static constexpr fake::meta::unordered_map aggr = []{};
			
			template<typename _Aggr, std::size_t _index>
			struct any{
				template<typename _Type>
				requires requires{aggr.emplace<[]{}>(fake::pack_v<aggr_t<_Aggr, _index>>, fake::pack_v<_Type>);}
				consteval operator _Type(){return *(_Type*)nullptr;}
			};
			
			template<typename _Aggr, std::size_t... _index>
			static consteval auto iterate(std::index_sequence<_index...>) noexcept{
				if constexpr(requires{_Aggr{any<_Aggr, _index>{}...};})
					return iterate<_Aggr>(std::make_index_sequence<sizeof...(_index) + 1>());
			}
			
			template<typename _Aggr>
			requires requires{{iterate<std::remove_cvref_t<_Aggr>>(std::index_sequence<>{})} -> std::same_as<void>;}
			static consteval auto aggregate_visitor() noexcept{
				using type = std::remove_cvref_t<_Aggr>;
				return [](auto &&_e, const auto &_f){
					std::size_t offset = 0;
					auto impl = [&]<std::size_t _index>(auto &_self, index_t<_index>) mutable requires(
						!std::same_as<fake::take_t<aggr.at<[]{}>(fake::pack_v<aggr_t<type, _index>>)>, fake::null_t>
					){
						using type = fake::take_t<aggr.at<[]{}>(fake::pack_v<aggr_t<type, _index>>)>;
						constexpr std::size_t element = alignof(type);
						constexpr std::size_t size = sizeof(type);
						constexpr std::size_t bigger = (size > element ? size : element);
						offset = (offset + element - 1 & ~(element - 1)) + bigger;
						
						_f(*reinterpret_cast<type*>(reinterpret_cast<char*>(&_e) + offset - bigger));
						if constexpr(requires{_self(_self, index_t<_index + 1>{});})
							_self(_self, index_t<_index + 1>{});
					};
					
					if constexpr(requires{impl(impl, index_t<0>{});})
						impl(impl, index_t<0>{});
				};
			}
			
		private:
			template<typename _ConfigToken, fake::meta::array_c auto _footprint>
			friend struct detail::for_each::broker;
			
			template<typename _ConfigToken, fake::meta::array_c auto _footprint, typename _Type, typename _Func>
			requires requires(
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
						fake::type_package<tool::adaptor<tool::tempextr_t<decltype(_t)>>> _key,
						fake::take_t<decltype(local){}.template at<[]{}>(decltype(_key){})> functor
					){functor(std::forward<_Type>(_e), _f);} ||
					std::is_aggregate_v<decltype(_t)>;
			}
			inline static constexpr auto method(_Type &&_e, const _Func &_f){
				using type_t = std::remove_cvref_t<_Type>;
				
				constexpr auto query = [](fake::pack_c auto _pack){
					using local_t = fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
					return local_t{}.template at<[]{}>(_pack);
				};
				
				using adaptor_t = tool::adaptor<type_t>;
				using functor_t = fake::take_t<query(fake::pack_v<adaptor_t>)>;
				if constexpr(!std::same_as<functor_t, fake::null_t>){
					functor_t{}(std::forward<_Type>(_e), _f);
				}
				else if constexpr(requires{typename tool::tempextr_t<type_t>;}){
					using adaptor_t = tool::adaptor<tool::tempextr_t<type_t>>;
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
			
			template<typename _ConfigToken, fake::meta::array_c auto _footprint, typename _Type, typename _Func>
			concept invocable_c = requires(_Type &&_e, const _Func &_f){
				custom::for_each::method<_ConfigToken, _footprint>(std::forward<_Type>(_e), _f);
			};
			
			// currying broker. 
			template<typename _ConfigToken, fake::meta::array_c auto _footprint>
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
	
	template<typename _ConfigToken, auto _footprint = custom::for_each::config::footprint<_ConfigToken>([]{})>
	requires fake::meta::array_c<decltype(_footprint)>
	inline constexpr custom::detail::for_each::broker<_ConfigToken, _footprint> for_each;
	
}

#endif /*__DEMO_FOREACH_H__*/ 
