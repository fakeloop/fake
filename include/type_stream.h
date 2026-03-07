#ifndef __FAKE_CPO_TYPESTREAM_H__
#define __FAKE_CPO_TYPESTREAM_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "push.h"

#include <iomanip>
#include <ostream>

#include "flat.h"
#include "type_name.h"
#include "type_info.h"

namespace fake::io
{
	
	template<fake::pack_c _Init = fake::pack_t<fake::null_t>, fake::mezz_c _Transform = fake::mezz_t<true>>
	using info = fake::query<
		fake::mate<fake::view_t<"init">, _Init>,
		fake::mate<fake::view_t<"transform">, _Transform>
	>;
	
	template<typename _Type>
	concept info_c = fake::query_c<_Type> &&
		std::same_as<typename _Type::first_types, fake::flat<fake::view_t<"init">, fake::view_t<"transform">>>;
	
}

namespace fake::custom
{
	
	namespace detail::type_stream
	{
		
		template<typename, auto _footprint> requires fake::meta::array_c<decltype(_footprint)> struct broker;
		template<typename> struct accessor;
		
	}
	
	struct type_stream
	{
	private:
		static constexpr fake::meta::unordered_map configure = []{};
		
		template<typename _ConfigToken> struct key final{};
		template<typename _ConfigToken> struct guard final{};
		struct stream final{};
		struct forbid final{};
		
	public:
		template<template<typename, auto, typename, typename> typename _Template>
		struct adaptor final{
			template<typename _ConfigToken, auto _footprint, typename _Type, fake::io::info_c _Info = fake::io::info<>>
			requires fake::like<decltype(_footprint), fake::signet_t>
			using type = _Template<_ConfigToken, _footprint, _Type, _Info>;
		};
		
		template<typename>
		struct is_adaptor final : std::false_type{};
		
		template<template<typename, auto, typename, typename> typename _Template>
		struct is_adaptor<adaptor<_Template>> final : std::true_type{};
		
		template<typename _Type>
		static constexpr bool is_adaptor_v = is_adaptor<std::remove_cvref_t<_Type>>::value;
		
	public:
		struct config final
		{
		private:
			template<auto _vest, typename _ConfigToken>
			requires (configure.contains<_vest>(fake::pack_v<key<_ConfigToken>>))
			static consteval auto instantiate_after_configure() noexcept{
				return !std::same_as<fake::take_t<configure.at<_vest>(fake::pack_v<guard<_ConfigToken>>)>, forbid>;
			}
			
			template<auto _vest, typename _ConfigToken>
			requires (configure.contains<_vest>(fake::pack_v<key<_ConfigToken>>) == false)
			static consteval auto instantiate_after_configure() noexcept{
				constexpr fake::meta::unordered_map local = []{};
				using local_t = std::remove_cvref_t<decltype(local)>;
				configure.emplace<[]{}>(fake::pack_v<key<_ConfigToken>>, fake::pack_v<local_t>);
				
				return !std::same_as<fake::take_t<configure.at<_vest>(fake::pack_v<guard<_ConfigToken>>)>, forbid>;
			}
			
		private:
			template<typename _ConfigToken>
			static consteval auto self_footprint(auto) noexcept{
				instantiate_after_configure<[]{}, _ConfigToken>();
				configure.emplace<[]{}>(fake::pack_v<guard<_ConfigToken>>, fake::pack_v<forbid>);
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				
				return local.data([]{});
			}
			
		public:
			template<typename _ConfigToken>
			static consteval auto footprint(auto _vest) noexcept{
				configure.emplace<[]{}>(fake::pack_v<guard<_ConfigToken>>, fake::pack_v<forbid>);
				
				using type_stream_f = decltype(self_footprint<_ConfigToken>(_vest));
				using type_name_f = decltype(fake::custom::type_name::config::footprint<_ConfigToken>(_vest));
				using for_each_f = decltype(fake::custom::for_each::config::footprint<_ConfigToken>(_vest));
				
				return fake::meta::array<type_stream_f, type_name_f, for_each_f>{};
			}
			
		public:
			template<auto _vest, typename _ConfigToken, typename _Stream>
			requires (
				instantiate_after_configure<_vest, _ConfigToken>() &&
				is_adaptor_v<_Stream>
			)
			static consteval auto emplace_stream() noexcept{
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				local.template emplace<[]{}>(fake::pack_v<stream>, fake::pack_v<_Stream>);
			}
			
		private:
			template<typename>
			friend struct detail::type_stream::accessor;
			
			template<auto _vest, typename _ConfigToken, typename _Key, typename _Value>
			requires (instantiate_after_configure<_vest, _ConfigToken>())
			static consteval auto emplace_config() noexcept{
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				local.template emplace<[]{}>(fake::pack_v<_Key>, fake::pack_v<_Value>);
			}
			
			template<auto _vest, typename _ConfigToken, typename _Key>
			requires (instantiate_after_configure<_vest, _ConfigToken>())
			static consteval auto contains_config() noexcept{
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				return local.template contains<[]{}>(fake::pack_v<_Key>);
			}
			
			template<auto _vest, typename _ConfigToken, typename _Key>
			requires (instantiate_after_configure<_vest, _ConfigToken>())
			static consteval auto peep_config() noexcept{
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				
				return local.template at<[]{}>(fake::pack_v<_Key>);
			}
			
			template<auto _vest, typename _ConfigToken, typename _Key>
			requires (configure.contains<_vest>(fake::pack_v<key<_ConfigToken>>))
			static consteval auto query_config() noexcept{
				if constexpr(
					std::same_as<fake::take_t<configure.at<_vest>(fake::pack_v<guard<_ConfigToken>>)>, forbid> == false
				)
					configure.emplace<[]{}>(fake::pack_v<guard<_ConfigToken>>, fake::pack_v<forbid>);
				
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				
				return local.template at<[]{}>(fake::pack_v<_Key>);
			}
		};
		
	private:
		template<typename, auto _footprint>
		requires fake::meta::array_c<decltype(_footprint)>
		friend struct detail::type_stream::broker;
		
		template<fake::mezz_c _Transform = fake::mezz_t<true>>
		using info = fake::query<
			fake::mate<fake::view_t<"transform">, _Transform>
		>;
		
		template<typename _ConfigToken, auto _footprint, fake::query_c _Info, typename _Type>
		requires
			fake::meta::array_c<decltype(_footprint)> &&
			requires(fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)> _local){
				requires decltype(_local){}.template contains<[]{}>(fake::pack_v<stream>);
			}
		inline static constexpr auto method(_Type &&_e){
			constexpr fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)> local;
			using stream_t = fake::take_t<local.template at<[]{}>(fake::pack_v<stream>)>;
			constexpr fake::signet_t hash = fake::signet_v<fake::type_view(_footprint)>;
			
			using info_i = fake::query_roll_t<fake::io::info<>, _Info>;
			
			return typename stream_t::template type<_ConfigToken, hash, _Type, info_i>{std::forward<_Type>(_e)};
		}
		
		template<typename _ConfigToken, auto _footprint, fake::query_c _Info, typename _Type, typename _Init>
		requires
			fake::meta::array_c<decltype(_footprint)> &&
			requires(fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)> _local){
				requires decltype(_local){}.template contains<[]{}>(fake::pack_v<stream>);
			}
		inline static constexpr auto method(_Type &&_e, _Init &&_i){
			constexpr fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)> local;
			using stream_t = fake::take_t<local.template at<[]{}>(fake::pack_v<stream>)>;
			constexpr fake::signet_t hash = fake::signet_v<fake::type_view(_footprint)>;
			
			using init_i = fake::query_rebind_t<fake::io::info<>, fake::view_v<"init">, fake::pack_t<_Init>>;
			using info_i = fake::query_roll_t<init_i, _Info>;
			
			return typename stream_t::template type<_ConfigToken, hash, _Type, info_i>{
				std::forward<_Type>(_e),
				std::forward<_Init>(_i)
			};
		}
	};
	
	namespace detail::type_stream
	{
		
		template<typename _ConfigToken, auto _footprint, typename _Info, typename... _Type>
		concept invocable_c = requires(_Type ..._e){
			requires fake::meta::array_c<decltype(_footprint)>;
			custom::type_stream::method<_ConfigToken, _footprint, _Info>(_e...);
		};
		
		// currying broker. 
		template<typename _ConfigToken, auto _footprint>
		requires fake::meta::array_c<decltype(_footprint)>
		struct broker
		{
		private:
			using default_i = fake::custom::type_stream::info<fake::mezz_t<true>>;
			using origin_i = fake::custom::type_stream::info<fake::mezz_t<false>>;
			
		public:
			template<typename _Type>
			requires invocable_c<_ConfigToken, _footprint, default_i, _Type>
			inline constexpr auto operator()(_Type &&_e) const{
				return custom::type_stream::method<_ConfigToken, _footprint, default_i>(std::forward<_Type>(_e));
			}
			
			template<typename _Type, typename _Init>
			requires invocable_c<_ConfigToken, _footprint, default_i, _Type, _Init>
			inline constexpr auto operator()(_Type &&_e, _Init &&_i) const{
				return custom::type_stream::method<_ConfigToken, _footprint, default_i>(
					std::forward<_Type>(_e),
					std::forward<_Init>(_i)
				);
			}
			
			template<typename _Type>
			requires invocable_c<_ConfigToken, _footprint, origin_i, _Type>
			inline constexpr auto origin(_Type &&_e) const{
				return custom::type_stream::method<_ConfigToken, _footprint, origin_i>(std::forward<_Type>(_e));
			}
			
			template<typename _Type, typename _Init>
			requires invocable_c<_ConfigToken, _footprint, origin_i, _Type, _Init>
			inline constexpr auto origin(_Type &&_e, _Init &&_i) const{
				return custom::type_stream::method<_ConfigToken, _footprint, origin_i>(
					std::forward<_Type>(_e),
					std::forward<_Init>(_i)
				);
			}
		};
		
		template<typename _ConfigToken>
		struct accessor final
		{
			template<auto _vest, typename _Key, typename _Value>
			requires requires{custom::type_stream::config::emplace_config<_vest, _ConfigToken, _Key, _Value>();}
			static consteval auto emplace() noexcept{
				return custom::type_stream::config::emplace_config<_vest, _ConfigToken, _Key, _Value>();
			}
			
			template<auto _vest, typename _Key>
			requires requires{custom::type_stream::config::contains_config<_vest, _ConfigToken, _Key>();}
			static consteval auto contains() noexcept{
				return custom::type_stream::config::contains_config<_vest, _ConfigToken, _Key>();
			}
			
			template<auto _vest, typename _Key>
			requires requires{custom::type_stream::config::peep_config<_vest, _ConfigToken, _Key>();}
			static consteval auto peep() noexcept{
				return custom::type_stream::config::peep_config<_vest, _ConfigToken, _Key>();
			}
			
			template<auto _vest, typename _Key>
			requires requires{custom::type_stream::config::query_config<_vest, _ConfigToken, _Key>();}
			static consteval auto query() noexcept{
				return custom::type_stream::config::query_config<_vest, _ConfigToken, _Key>();
			}
		};
		
	}
	
}

namespace fake
{
	
	template<typename _ConfigToken, auto _footprint = custom::type_stream::config::footprint<_ConfigToken>([]{})>
	requires fake::meta::array_c<decltype(_footprint)>
	inline constexpr custom::detail::type_stream::broker<_ConfigToken, _footprint> type_stream;
	
}

#include "pop.h"

#endif /*__FAKE_CPO_TYPESTREAM_H__*/ 
