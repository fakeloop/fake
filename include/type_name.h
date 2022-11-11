#ifndef __FAKE_CPO_TYPENAME_H__
#define __FAKE_CPO_TYPENAME_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include <string>

#include "view.h"
#include "meta.h"
#include "container.h"

namespace fake::custom
{
	
	namespace detail::type_name
	{
		
		template<typename, auto _footprint> requires fake::meta::array_c<decltype(_footprint)> struct broker;
		
	}
	
	struct type_name
	{
	private:
		static constexpr fake::meta::unordered_map configure = []{};
		
	private:
		template<typename _ConfigToken> struct key final{};
		template<typename _ConfigToken> struct guard final{};
		template<fake::gene_c _Gene> struct trunc final{};
		struct bracket final{};
		struct forbid final{};
		
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
				inject<_vest, _ConfigToken>();
				
				return !std::same_as<fake::take_t<configure.at<_vest>(fake::pack_v<guard<_ConfigToken>>)>, forbid>;
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
			template<auto _vest, typename _ConfigToken, typename _Type>
			requires (
				instantiate_after_configure<_vest, _ConfigToken>() &&
				!fake::gene_c<_Type>
			)
			static consteval auto emplace_transform(auto _lambda) noexcept{
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				local.template emplace<[]{}>(fake::pack_v<_Type>, fake::pack_v<decltype(_lambda)>);
			}
			
			template<auto _vest, typename _ConfigToken, fake::gene_c _Gene>
			requires (instantiate_after_configure<_vest, _ConfigToken>())
			static consteval auto emplace_transform(auto _lambda) noexcept{
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				local.template emplace<[]{}>(fake::pack_v<_Gene>, fake::pack_v<decltype(_lambda)>);
			}
			
			template<auto _vest, typename _ConfigToken, fake::gene_c _Gene, std::size_t _TemplateParamaterNumber>
			requires (
				instantiate_after_configure<_vest, _ConfigToken>() &&
				!std::same_as<_Gene, fake::generic<fake::container::null_t>>
			)
			static consteval auto set_templace_trunc() noexcept{
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				local.template emplace<[]{}>(
					fake::pack_v<trunc<_Gene>>,
					fake::pack_v<fake::mezz_t<_TemplateParamaterNumber>>
				);
			}
			
			template<auto _vest, typename _ConfigToken, fake::gene_c _Gene, std::size_t _TemplateParamaterNumber>
			requires (
				instantiate_after_configure<_vest, _ConfigToken>() &&
				std::same_as<_Gene, fake::generic<fake::container::null_t>>
			)
			static consteval auto set_templace_trunc() noexcept{}
			
			template<auto _vest, typename _ConfigToken, fake::pair_c _Bracket>
			requires (
				instantiate_after_configure<_vest, _ConfigToken>() &&
				fake::mezz_c<typename _Bracket::first_type> &&
				fake::mezz_c<typename _Bracket::second_type>
			)
			static consteval auto set_templace_bracket() noexcept{
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				local.template emplace<[]{}>(fake::pack_v<bracket>, fake::pack_v<_Bracket>);
			}
		};
		
	private:
		template<typename _ConfigToken, auto _footprint, typename _Type>
		requires
			fake::meta::array_c<decltype(_footprint)> &&
			fake::is_template_v<fake::bare_t<_Type>>
		struct template_name final{
		private:
			static constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
			using bracket_t = fake::take_t<local.template at<[]{}>(fake::pack_v<bracket>)>;
			static constexpr fake::view_c auto bracket_left = bracket_t::first_type::value;
			static constexpr fake::view_c auto bracket_right = bracket_t::second_type::value;
			
		public:
			static consteval auto view() noexcept{
				using bare = fake::bare_t<_Type>;
				using tuple = typename fake::template_info<bare>::args;
				constexpr std::string_view symbol = fake::symbol::string_view<_Type>();
				constexpr std::size_t length = symbol.size();
				constexpr std::string_view prefix = [](std::string_view _symbol){
					std::size_t i = _symbol.find_last_of('>');
					for(std::size_t stack = 1; stack && i; i--)
						switch(_symbol[i - 1]){case '>': stack++; break; case '<': stack--; break;};
					return _symbol.substr(0, i);
				}(symbol);
				constexpr std::string_view suffix = [](std::string_view _symbol){
					for(std::size_t i = length; i; i--)
						if(_symbol[i - 1] == '>')
							return _symbol.substr(i);
				}(symbol);
				
				using mezz = fake::take_t<local.template at<[]{}>(fake::pack_v<trunc<fake::pattern_t<bare>>>)>;
				
				using max_t = fake::mezz_t<~std::size_t{}>;
				constexpr std::size_t size = std::conditional_t<std::same_as<mezz, fake::null_t>, max_t, mezz>::value;
				
				return fake::view_v<fake::detail::view::string<prefix.size() + 1>{prefix.data()}> +
					[]<std::size_t... _index>(std::index_sequence<_index...>){
						return (
							bracket_left + (
								(
									std::conditional_t<_index, fake::view_t<", ">, fake::view_t<"">>{} +
									method<_ConfigToken, _footprint, std::tuple_element_t<_index, tuple>>()
								) + ... + bracket_right
							)
						);
					}(std::make_index_sequence<std::min(size, std::tuple_size_v<tuple>)>()) +
					fake::view_v<fake::detail::view::string<suffix.size() + 1>{suffix.data()}>;
			}
		};
		
	private:
		// register meta-implementations for token-based-cpo 'fake::type_name' at compile-time. 
		template<auto, typename _ConfigToken>
		static consteval auto inject() noexcept{
			config::emplace_transform<[]{}, _ConfigToken, fake::generic<std::basic_string>>(
				[](fake::pack_c auto _pack){
					using namespace fake::literals;
					using type = fake::take_t<decltype(_pack){}>;
					using bare = fake::bare_t<type>;
					using value = typename bare::value_type;
					constexpr fake::view type_v = fake::type_view(fake::pack_v<type>);
					constexpr fake::view bare_v = fake::type_view(fake::pack_v<bare>);
					
					if constexpr(std::same_as<value, char>){
						return type_v.template replace<bare_v, "std::string"_v>();
					}
					else{
						constexpr fake::view value_v = fake::type_view(fake::pack_v<value>);
						return type_v.template replace<bare_v, "std::string<"_v + value_v + ">"_v>();
					}
				}
			);
			
			config::emplace_transform<[]{}, _ConfigToken, fake::generic<std::basic_string_view>>(
				[](fake::pack_c auto _pack){
					using namespace fake::literals;
					using type = fake::take_t<decltype(_pack){}>;
					using bare = fake::bare_t<type>;
					using value = typename bare::value_type;
					constexpr fake::view type_v = fake::type_view(fake::pack_v<type>);
					constexpr fake::view bare_v = fake::type_view(fake::pack_v<bare>);
					
					if constexpr(std::same_as<value, char>){
						return type_v.template replace<bare_v, "std::string_view"_v>();
					}
					else{
						constexpr fake::view value_v = fake::type_view(fake::pack_v<value>);
						return type_v.template replace<bare_v, "std::string_view<"_v + value_v + ">"_v>();
					}
				}
			);
			
			using vector_g = fake::pattern_t<fake::vector<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, vector_g, 0x1>();
			
			using deque_g = fake::pattern_t<fake::deque<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, deque_g, 0x1>();
			
			using forward_list_g = fake::pattern_t<fake::forward_list<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, forward_list_g, 0x1>();
			
			using list_g = fake::pattern_t<fake::list<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, list_g, 0x1>();
			
			using set_g = fake::pattern_t<fake::set<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, set_g, 0x1>();
			
			using multiset_g = fake::pattern_t<fake::multiset<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, multiset_g, 0x1>();
			
			using unordered_set_g = fake::pattern_t<fake::unordered_set<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, unordered_set_g, 0x1>();
			
			using unordered_multiset_g = fake::pattern_t<fake::unordered_multiset<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, unordered_multiset_g, 0x1>();
			
			using map_g = fake::pattern_t<fake::map<int, int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, map_g, 0x2>();
			
			using multimap_g = fake::pattern_t<fake::multimap<int, int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, multimap_g, 0x2>();
			
			using unordered_map_g = fake::pattern_t<fake::unordered_map<int, int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, unordered_map_g, 0x2>();
			
			using unordered_multimap_g = fake::pattern_t<fake::unordered_multimap<int, int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, unordered_multimap_g, 0x2>();
			
			using bracket = std::pair<fake::mezz_t<fake::view_v<"<">>, fake::mezz_t<fake::view_v<">">>>;
			config::set_templace_bracket<[]{}, _ConfigToken, bracket>();
		}
		
	private:
		template<typename _ConfigToken, auto _footprint>
		requires fake::meta::array_c<decltype(_footprint)>
		friend struct detail::type_name::broker;
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		requires (fake::meta::array_c<decltype(_footprint)>)
		inline static consteval auto method() noexcept{
			using type_t = fake::bare_t<_Type>;
			constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
			if constexpr(local.template contains<[]{}>(fake::pack_v<type_t>))
				return fake::take_v<local.template at<[]{}>(fake::pack_v<type_t>)>(fake::pack_v<_Type>);
			else if constexpr(fake::is_template_v<type_t>){
				using generic_t = fake::pattern_t<type_t>;
				if constexpr(local.template contains<[]{}>(fake::pack_v<generic_t>))
					return fake::take_v<local.template at<[]{}>(fake::pack_v<generic_t>)>(fake::pack_v<_Type>);
				else
					return template_name<_ConfigToken, _footprint, _Type>::view();
			}
			else
				return fake::type_view(fake::pack_v<_Type>);
		}
	};
	
	namespace detail::type_name
	{
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		concept invocable_c = requires{
			requires fake::meta::array_c<decltype(_footprint)>;
			custom::type_name::method<_ConfigToken, _footprint, _Type>();
		};
		
		// currying broker. 
		template<typename _ConfigToken, auto _footprint>
		requires fake::meta::array_c<decltype(_footprint)>
		struct broker
		{
		public:
			template<typename _Type>
			requires invocable_c<_ConfigToken, _footprint, _Type>
			inline consteval auto operator()(fake::type_package<_Type>) const{
				return custom::type_name::method<_ConfigToken, _footprint, _Type>();
			}
		};
		
	}
	
}

namespace fake
{
	
	template<typename _ConfigToken, auto _footprint = custom::type_name::config::footprint<_ConfigToken>([]{})>
	requires fake::meta::array_c<decltype(_footprint)>
	inline constexpr custom::detail::type_name::broker<_ConfigToken, _footprint> type_name;
	
}

#endif /*__FAKE_CPO_TYPENAME_H__*/ 