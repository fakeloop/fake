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

#include <iomanip>
#include <ostream>

#include "type_name.h"
#include "type_info.h"

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
		template<template<typename, auto, typename> typename _Template>
		struct adaptor final{
			template<typename _ConfigToken, auto _footprint, typename _Type>
			requires fake::meta::array_c<decltype(_footprint)>
			using type = _Template<_ConfigToken, _footprint, _Type>;
		};
		
		template<typename>
		struct is_adaptor final : std::false_type{};
		
		template<template<typename, auto, typename> typename _Template>
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
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		requires
			fake::meta::array_c<decltype(_footprint)> &&
			requires(fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)> _local){
				requires _local.template contains<[]{}>(fake::pack_v<stream>);
			}
		inline static constexpr auto method(const _Type &_e){
			constexpr fake::take_t<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)> local;
			using stream_t = fake::take_t<local.template at<[]{}>(fake::pack_v<stream>)>;
			
			return typename stream_t::template type<_ConfigToken, _footprint, _Type>{_e};
		}
	};
	
	namespace detail::type_stream
	{
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		concept invocable_c = requires(const _Type &_e){
			requires fake::meta::array_c<decltype(_footprint)>;
			custom::type_stream::method<_ConfigToken, _footprint>(_e);
		};
		
		// currying broker. 
		template<typename _ConfigToken, auto _footprint>
		requires fake::meta::array_c<decltype(_footprint)>
		struct broker
		{
		public:
			template<typename _Type>
			requires invocable_c<_ConfigToken, _footprint, _Type>
			inline constexpr auto operator()(const _Type &_e) const{
				return custom::type_stream::method<_ConfigToken, _footprint>(_e);
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

#endif /*__FAKE_CPO_TYPESTREAM_H__*/ 
