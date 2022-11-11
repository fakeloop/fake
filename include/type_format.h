#ifndef __FAKE_CPO_TYPEPRINT_H__
#define __FAKE_CPO_TYPEPRINT_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "utility.h"
#include "type_stream.h"

namespace fake::custom
{
	
	struct format
	{
	private:
		// delegate storage to the meta-container in 'type_stream'. 
		// therefore a wrapper is necessary. 
		template<typename _ConfigToken>
		struct token_t final{using type = _ConfigToken;};
		
		struct key final{
			struct tab final{};
			struct endl final{};
			struct color final{};
			struct map final{};
			struct decorate final{};
			struct member final{};
			struct json final{};
			struct container final{};
			struct bracket final{};
			struct name final{};
		};
		
	private:
		template<typename _ConfigToken>
		using access = fake::custom::detail::type_stream::accessor<token_t<_ConfigToken>>;
		
	public:
		struct color final{
			fake::colors indent = fake::colors::black;
			fake::colors type = fake::colors::cyan;
			fake::colors member = fake::colors::yellow;
			fake::colors value = fake::colors::red;
			fake::colors colon = fake::colors::white;
			fake::colors quote = fake::colors::red;
			fake::colors comma = fake::colors::white;
			fake::colors bracket = fake::colors::blue;
		};
		
		struct config final{
			struct style final{
				template<auto _vest, typename _ConfigToken, auto _tab>
				requires
					fake::view_c<decltype(_tab)> &&
					requires{access<_ConfigToken>::template emplace<_vest, key::tab, fake::mezz_t<_tab>>();}
				static consteval auto tab() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::tab, fake::mezz_t<_tab>>();
				}
				
				template<auto _vest, typename _ConfigToken, auto _endl>
				requires
					fake::view_c<decltype(_endl)> &&
					requires{access<_ConfigToken>::template emplace<_vest, key::endl, fake::mezz_t<_endl>>();}
				static consteval auto endl() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::endl, fake::mezz_t<_endl>>();
				}
			};
			
			struct color final{
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::color, fake::mezz_t<true>>();}
				static consteval auto enable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::color, fake::mezz_t<true>>();
				}
				
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::color, fake::mezz_t<false>>();}
				static consteval auto disable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::color, fake::mezz_t<false>>();
				}
				
				template<auto _vest, typename _ConfigToken, format::color _color>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::map, fake::mezz_t<_color>>();}
				static consteval auto map() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::map, fake::mezz_t<_color>>();
				}
				
				template<auto _vest, typename _ConfigToken, typename _Decorator>
				requires requires{
					access<_ConfigToken>::template emplace<_vest, key::decorate, _Decorator>();
					_Decorator::template color<fake::colors::black>(fake::view_v<"">);
				}
				static consteval auto decorator() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::decorate, _Decorator>();
				}
			};
			
			struct member final{
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::member, fake::mezz_t<true>>();}
				static consteval auto enable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::member, fake::mezz_t<true>>();
				}
				
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::member, fake::mezz_t<false>>();}
				static consteval auto disable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::member, fake::mezz_t<false>>();
				}
			};
			
			struct json final{
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::json, fake::mezz_t<true>>();}
				static consteval auto enable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::json, fake::mezz_t<true>>();
				}
				
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::json, fake::mezz_t<false>>();}
				static consteval auto disable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::json, fake::mezz_t<false>>();
				}
				
				template<auto _vest, typename _ConfigToken, fake::mezz_c _Filter>
				requires (access<_ConfigToken>::template contains<_vest, key::container>() == false)
				static consteval auto register_container() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::container, fake::meta::array<_Filter>>();
				}
				
				template<auto _vest, typename _ConfigToken, fake::mezz_c _Filter>
				requires (access<_ConfigToken>::template contains<_vest, key::container>())
				static consteval auto register_container() noexcept{
					using array = fake::take_t<access<_ConfigToken>::template peep<_vest, key::container>()>;
					using value = fake::meta::array_cat_t<array, fake::meta::array<_Filter>>;
					
					return access<_ConfigToken>::template emplace<_vest, key::container, value>();
				}
			};
			
			struct bracket_pair_colorization final{
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::bracket, fake::mezz_t<true>>();}
				static consteval auto enable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::bracket, fake::mezz_t<true>>();
				}
				
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::bracket, fake::mezz_t<false>>();}
				static consteval auto disable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::bracket, fake::mezz_t<false>>();
				}
			};
			
			struct member_name_colorization final{
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::name, fake::mezz_t<true>>();}
				static consteval auto enable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::name, fake::mezz_t<true>>();
				}
				
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::name, fake::mezz_t<false>>();}
				static consteval auto disable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::name, fake::mezz_t<false>>();
				}
			};
		};
		
	private:
		struct colorization final{
			template<fake::colors _color>
			static constexpr auto color(const auto &_data) noexcept(noexcept(fake::color<_color>(_data)))
			requires requires{fake::color<_color>(_data);}{
				return fake::color<_color>(_data);
			}
		};
		
	private:
		struct query final{
			template<typename _ConfigToken>
			static consteval auto tab() noexcept{
				using tab_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::tab>()>;
				if constexpr(std::same_as<tab_t, fake::null_t>)
					return fake::view_v<"\t">;
				else
					return tab_t::value;
			}
			
			template<typename _ConfigToken>
			static consteval auto endl() noexcept{
				using endl_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::endl>()>;
				if constexpr(std::same_as<endl_t, fake::null_t>)
					return fake::view_v<"\n">;
				else
					return endl_t::value;
			}
			
			template<typename _ConfigToken>
			static consteval auto color() noexcept{
				using color_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::color>()>;
				if constexpr(std::same_as<color_t, fake::null_t>)
					return false;
				else
					return color_t::value;
			}
			
			template<typename _ConfigToken>
			static consteval auto map() noexcept{
				using map_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::map>()>;
				if constexpr(std::same_as<map_t, fake::null_t>)
					return format::color{};
				else
					return map_t::value;
			}
			
			template<typename _ConfigToken>
			static consteval auto decorator() noexcept{
				using decorator_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::decorate>()>;
				if constexpr(std::same_as<decorator_t, fake::null_t>)
					return fake::pack_v<colorization>;
				else
					return fake::pack_v<decorator_t>;
			}
			
			template<typename _ConfigToken>
			static consteval auto member() noexcept{
				using member_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::member>()>;
				if constexpr(std::same_as<member_t, fake::null_t>)
					return true;
				else
					return member_t::value;
			}
			
			template<typename _ConfigToken>
			static consteval auto json() noexcept{
				using json_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::json>()>;
				if constexpr(std::same_as<json_t, fake::null_t>)
					return false;
				else
					return json_t::value;
			}
			
			template<typename _ConfigToken>
			static consteval auto container() noexcept{
				using selector_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::container>()>;
				if constexpr(std::same_as<selector_t, fake::null_t>)
					return fake::pack_v<fake::meta::array<>>;
				else
					return fake::pack_v<selector_t>;
			}
			
			template<typename _ConfigToken>
			static consteval auto bracket() noexcept{
				using bracket_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::bracket>()>;
				if constexpr(std::same_as<bracket_t, fake::null_t>)
					return false;
				else
					return bracket_t::value;
			}
			
			template<typename _ConfigToken>
			static consteval auto name() noexcept{
				using bracket_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::name>()>;
				if constexpr(std::same_as<bracket_t, fake::null_t>)
					return false;
				else
					return bracket_t::value;
			}
		};
		
	private:
		using rotate = fake::meta::array<
			fake::mezz_t<fake::colors::white>,
			fake::mezz_t<fake::colors::blue>,
			fake::mezz_t<fake::colors::yellow>,
			fake::mezz_t<fake::colors::cyan>,
			fake::mezz_t<fake::colors::red>,
			fake::mezz_t<fake::colors::green>,
			fake::mezz_t<fake::colors::magenta>
		>;
		
		using shift = fake::meta::array<
			fake::mezz_t<fake::colors::white>,
			fake::mezz_t<fake::colors::yellow>,
			fake::mezz_t<fake::colors::green>,
			fake::mezz_t<fake::colors::magenta>
		>;
		
		template<fake::trait_c<token_t> _ConfigToken, auto _footprint, typename _Type>
		requires fake::meta::array_c<decltype(_footprint)>
		struct stream final{
		private:
			static constexpr auto type_name = fake::type_name<typename _ConfigToken::type>;
			static constexpr fake::view_c auto tab = query::tab<typename _ConfigToken::type>();
			static constexpr fake::view_c auto endl = query::endl<typename _ConfigToken::type>();
			static constexpr bool color = query::color<typename _ConfigToken::type>();
			static constexpr format::color map = query::map<typename _ConfigToken::type>();
			using decorator = fake::take_t<query::decorator<typename _ConfigToken::type>()>;
			static constexpr bool member = query::member<typename _ConfigToken::type>();
			static constexpr bool json = query::json<typename _ConfigToken::type>();
			using filters = fake::take_t<query::container<typename _ConfigToken::type>()>;
			static constexpr bool bracket = query::bracket<typename _ConfigToken::type>();
			static constexpr bool name = query::name<typename _ConfigToken::type>();
			
			enum struct context : std::size_t {undefine, bracket, index};
			
		public:
			constexpr stream(const _Type& _data) noexcept: data(_data){}
			stream(const stream&) = delete;
			stream& operator=(const stream&) = delete;
			
		private:
			template<fake::colors _color, context _context = context::undefine, std::size_t _layer = 0>
			static consteval auto decorate(fake::view_c auto _view) noexcept
			requires (decltype(_view)::template find_first_not_of<"\t\n\v\f\r ">() == decltype(_view)::npos){
				return _view;
			}
			
			template<fake::colors _color, context _context = context::undefine, std::size_t _layer = 0>
			static consteval auto decorate(fake::view_c auto _view) noexcept
			requires (decltype(_view)::template find_first_not_of<"\t\n\v\f\r ">() != decltype(_view)::npos){
				if constexpr(color){
					if constexpr(bracket && _context == context::bracket){
						using mezz = fake::meta::array_element_t<_layer % fake::meta::array_size_v<rotate>, rotate>;
						
						return decorator::template color<mezz::value>(_view);
					}
					else if constexpr(name && _context == context::index){
						using mezz = fake::meta::array_element_t<_layer % fake::meta::array_size_v<shift>, shift>;
						
						return decorator::template color<mezz::value>(_view);
					}
					else{
						return decorator::template color<_color>(_view);
					}
				}
				else{
					return _view;
				}
			}
			
			template<fake::colors _color, context _context = context::undefine, std::size_t _layer = 0>
			static constexpr auto decorate(const auto &_data) noexcept
			requires (fake::view_c<decltype(_data)> == false){
				if constexpr(color){
					if constexpr(bracket && _context == context::bracket){
						using mezz = fake::meta::array_element_t<_layer % fake::meta::array_size_v<rotate>, rotate>;
						
						return decorator::template color<mezz::value>(_data);
					}
					else if constexpr(name && _context == context::index){
						using mezz = fake::meta::array_element_t<_layer % fake::meta::array_size_v<shift>, shift>;
						
						return decorator::template color<mezz::value>(_data);
					}
					else{
						return decorator::template color<_color>(_data);
					}
				}
				else{
					return _data;
				}
			}
			
			template<auto _view, std::size_t _layer>
			static consteval auto indent() noexcept{
				return []<std::size_t... _index>(std::index_sequence<_index...>){
					return ((_index, _view) + ... + fake::view_v<"">);
				}(std::make_index_sequence<_layer>());
			}
			
			static consteval auto escape(fake::view_c auto _key) noexcept{
				if constexpr(json)
					return _key.template replace<"\\", "\\\\">().template replace<"\"", "\\\"">();
				else
					return _key;
			}
			
			static consteval auto container(fake::pack_c auto _pack) noexcept{
				constexpr auto filter = []<std::size_t... _index>(std::index_sequence<_index...>) consteval{
					return fake::filter<fake::meta::array_element_t<_index, filters>{}...>;
				}(std::make_index_sequence<fake::meta::array_size_v<filters>>());
				
				return filter(_pack);
			}
			
		private:
			friend std::basic_ostream<char>& operator<<(std::basic_ostream<char> &_os, const stream &_stream){
				// recurring-lambda currying. 
				auto impl = [&_os]<fake::view_c _View, std::size_t _layer, fake::meta::array_c _Status>(
					auto &_self,
					std::size_t &_i,
					auto &&_e
				){
					using namespace fake::literals;
					using type = std::remove_cvref_t<decltype(_e)>;
					using status = _Status;
					using parent = fake::meta::array_element_t<0, status>;
					using current = fake::meta::array_cat_t<fake::meta::array<type>, status>;
					constexpr fake::view_c auto space = std::conditional_t<_View::empty(), _View, fake::view<' '>>{};
					constexpr fake::view_c auto name = std::conditional_t<member, _View, fake::view<>>{};
					constexpr bool contian = json && container(fake::pack_v<type>);
					constexpr bool element = container(fake::pack_v<parent>);
					
					std::size_t i = 0;
					auto scope = []<bool _comma, std::size_t _depth>(fake::mezz_t<_comma>, fake::mezz_t<_depth>){
						if constexpr(_comma)
							return decorate<map.comma>(","_v) + endl + decorate<map.indent>(indent<tab, _depth>());
						else
							return endl + decorate<map.indent>(indent<tab, _depth>());
					};
					
					if(_i++)
						_os << scope(fake::mezz_v<true>, fake::mezz_v<_layer>);
					
					constexpr fake::view_c auto quote = decorate<map.quote>("\""_v);
					constexpr fake::view_c auto json_quote = std::conditional_t<json, decltype(quote), fake::view<>>{};
					
					constexpr fake::view title = [](auto _name, auto _space, auto _json_quote, auto _element){
						if constexpr(json && _element.value)
							return ""_v;
						else
							return _json_quote + escape(
								decorate<map.type>(type_name(fake::pack_v<type>)) + _space +
									decorate<map.member, context::index, _layer>(_name)
							) + _json_quote + decorate<map.colon>(" : "_v);
					}(name, space, json_quote, fake::mezz_v<element>);
					
					constexpr fake::view_c auto l = std::conditional_t<contian, fake::view_t<"[">, fake::view_t<"{">>{};
					constexpr fake::view_c auto r = std::conditional_t<contian, fake::view_t<"]">, fake::view_t<"}">>{};
					
					if constexpr(requires{_os << _e;}){
						if constexpr(requires{std::quoted(_e);})
							_os << title + quote << _e << quote;
						else
							_os << title << decorate<map.value>(_e);
					}
					else{
						_os << title + decorate<map.bracket, context::bracket, _layer>(l) +
							scope(fake::mezz_v<false>, fake::mezz_v<_layer + 1>);
						
						fake::for_each<_ConfigToken>(
							_e,
							[&_self, &i]<fake::view_c _V>(auto &&_e){
								_self.template operator()<_V, _layer + 1, current>(_self, i, _e);
							}
						);
						
						_os << scope(fake::mezz_v<false>, fake::mezz_v<_layer>) +
							decorate<map.bracket, context::bracket, _layer>(r);
					}
				};
				
				if constexpr(json)
					_os << decorate<map.bracket, context::bracket, 0x0>(fake::view_v<"{">) + endl
						+ decorate<map.indent>(indent<tab, 0x1>());
				
				std::size_t index = 0;
				using status = fake::meta::array<fake::null_t>;
				impl.template operator()<fake::view<>, std::size_t{json}, status>(impl, index, _stream.data);
				
				if constexpr(json)
					_os << endl + decorate<map.bracket, context::bracket, 0x0>(fake::view_v<"}">);
				
				return _os;
			}
			
		private:
			const _Type &data;
		};
		
	private:
		template<typename... _Type>
		using adaptor = fake::tool::adaptor<_Type...>;
		
		// register meta-implementations for token-based-cpo 'fake::type_stream' and 'fake::for_each' at compile-time. 
		template<typename _ConfigToken>
		static consteval auto inject() noexcept{
			using type_stream = fake::custom::type_stream;
			using for_each = fake::custom::for_each;
			
			constexpr auto vest = []{};
			
			type_stream::config::emplace_stream<vest, token_t<_ConfigToken>, type_stream::adaptor<stream>>();
			
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<fake::generic<std::pair>>>(
				[](auto &&_e, const auto &_f){
					_f.template operator()<fake::view_t<"first">>(_e.first);
					_f.template operator()<fake::view_t<"second">>(_e.second);
				}
			);
			
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<fake::generic<std::tuple>>>(
				[](auto &&_e, const auto &_f){
					[&]<std::size_t... _index>(std::index_sequence<_index...>){
						(_f.template operator()<decltype(fake::to_view_v<_index>)>(std::get<_index>(_e)), ...);
					}(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<decltype(_e)>>>());
				}
			);
			
			constexpr auto visitor = [](auto &&_e, const auto &_f){
				for(auto &&e : _e)
					_f.template operator()<fake::view<>>(e);
			};
			
			using vector_g = fake::pattern_t<fake::vector<int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<vector_g>>(visitor);
			using vector_m = fake::mezz_t<[]<fake::vector_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, vector_m>();
			
			using deque_g = fake::pattern_t<fake::deque<int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<deque_g>>(visitor);
			using deque_m = fake::mezz_t<[]<fake::deque_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, deque_m>();
			
			using forward_list_g = fake::pattern_t<fake::forward_list<int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<forward_list_g>>(visitor);
			using forward_list_m = fake::mezz_t<[]<fake::forward_list_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, forward_list_m>();
			
			using list_g = fake::pattern_t<fake::list<int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<list_g>>(visitor);
			using list_m = fake::mezz_t<[]<fake::list_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, list_m>();
			
			using set_g = fake::pattern_t<fake::set<int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<set_g>>(visitor);
			using set_m = fake::mezz_t<[]<fake::set_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, set_m>();
			
			using multiset_g = fake::pattern_t<fake::multiset<int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<multiset_g>>(visitor);
			using multiset_m = fake::mezz_t<[]<fake::multiset_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, multiset_m>();
			
			using unordered_set_g = fake::pattern_t<fake::unordered_set<int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<unordered_set_g>>(visitor);
			using unordered_set_m = fake::mezz_t<[]<fake::unordered_set_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, unordered_set_m>();
			
			using unordered_multiset_g = fake::pattern_t<fake::unordered_multiset<int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<unordered_multiset_g>>(visitor);
			using unordered_multiset_m = fake::mezz_t<[]<fake::unordered_multiset_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, unordered_multiset_m>();
			
			using map_g = fake::pattern_t<fake::map<int, int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<map_g>>(visitor);
			using map_m = fake::mezz_t<[]<fake::map_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, map_m>();
			
			using multimap_g = fake::pattern_t<fake::multimap<int, int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<multimap_g>>(visitor);
			using multimap_m = fake::mezz_t<[]<fake::multimap_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, multimap_m>();
			
			using unordered_map_g = fake::pattern_t<fake::unordered_map<int, int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<unordered_map_g>>(visitor);
			using unordered_map_m = fake::mezz_t<[]<fake::unordered_map_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, unordered_map_m>();
			
			using unordered_multimap_g = fake::pattern_t<fake::unordered_multimap<int, int>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<unordered_multimap_g>>(visitor);
			using unordered_multimap_m = fake::mezz_t<[]<fake::unordered_multimap_c _T>(fake::type_package<_T>){}>;
			config::json::register_container<[]{}, _ConfigToken, unordered_multimap_m>();
			
			return fake::pack_v<token_t<_ConfigToken>>;
		}
		
	public:
		template<typename _ConfigToken>
		using token = fake::take_t<inject<_ConfigToken>()>;
	};
	
}

namespace fake::cpo
{
	
	template<typename _ConfigToken>
	using format = fake::custom::format::token<_ConfigToken>;
	
}

namespace fake
{
	
	// 'fake::type_stream<config_token>' delegate. 
	template<
		typename _ConfigToken,
		auto _footprint = custom::type_stream::config::footprint<fake::cpo::format<_ConfigToken>>([]{})
	>
	requires fake::meta::array_c<decltype(_footprint)>
	inline constexpr auto format = fake::type_stream<fake::cpo::format<_ConfigToken>, _footprint>;
	
}

namespace fake::io
{
	
	// preset configurations. 
	namespace token
	{
		
		struct plain final{};
		struct pretty final{};
		struct json final{};
		struct html final{};
		
	}
	
	namespace detail::inject
	{
		
		namespace tool::html
		{
			
			namespace detail
			{
				
				template<fake::colors _front, fake::colors _back = fake::colors::black>
				consteval auto color() noexcept{
					using namespace fake::literals;
					
					using table = fake::meta::array<
						fake::view_t<"#878787">,
						fake::view_t<"#DB6A6A">,
						fake::view_t<"#0DBC79">,
						fake::view_t<"#E5E510">,
						fake::view_t<"#4E8ED3">,
						fake::view_t<"#C965C9">,
						fake::view_t<"#11A8CD">,
						fake::view_t<"#CCCCCC">
					>;
					
					constexpr std::size_t index = static_cast<std::underlying_type_t<fake::colors>>(_front);
					
					return fake::view_v<"<font color=\""> + fake::meta::array_element_t<index, table>{} +
						fake::view_v<"\">">;
				}
				
				consteval auto clear() noexcept{return fake::view_v<"</font>">;}
				
				template<typename _Type, fake::colors _front, fake::colors _back>
				struct stream final
				{
					using type = std::remove_cvref_t<_Type>;
					
					constexpr stream(const type &_data): data(_data){}
					stream(const stream&) = delete;
					stream& operator=(const stream&) = delete;
					
					friend std::basic_ostream<char>& operator<<(std::basic_ostream<char> &_os, stream _view){
						return _os << color<_front, _back>() << _view.data << clear();
					}
					
				private:
					const type &data;
				};
				
			}
			
			template<fake::colors _front, fake::colors _back = fake::colors::black>
			constexpr auto color(const fake::view_c auto &_view) noexcept{
				return detail::color<_front, _back>() + _view + detail::clear();
			}
			
			template<fake::colors _front, fake::colors _back = fake::colors::black>
			constexpr auto color(std::string_view _sv){return detail::stream<std::string_view, _front, _back>{_sv};}
			
			template<fake::colors _front, fake::colors _back = fake::colors::black>
			constexpr auto color(const auto &_data){return detail::stream<decltype(_data), _front, _back>{_data};}
			
		}
		
		// register meta-implementations for token-based-cpo 'fake::type_name' at compile-time. 
		struct preset_formatter{
		private:
			struct colorization final{
				template<fake::colors _color>
				static constexpr auto color(const auto &_data) noexcept(noexcept(tool::html::color<_color>(_data)))
				requires requires{tool::html::color<_color>(_data);}{
					return tool::html::color<_color>(_data);
				}
			};
			
		private:
			static consteval void inject_plain() noexcept{
				fake::custom::format::config::style::tab<[]{}, token::plain, fake::view_v<"    ">>();
			}
			
			static consteval void inject_pretty() noexcept{
				fake::custom::format::config::color::enable<[]{}, token::pretty>();
				fake::custom::format::config::bracket_pair_colorization::enable<[]{}, token::pretty>();
				fake::custom::format::config::member_name_colorization::enable<[]{}, token::pretty>();
				fake::custom::format::config::style::tab<[]{}, token::pretty, fake::view_v<"|   ">>();
			}
			
			static consteval void inject_json() noexcept{
				fake::custom::format::config::json::enable<[]{}, token::json>();
				fake::custom::format::config::style::tab<[]{}, token::json, fake::view_v<"    ">>();
			}
			
			static consteval void inject_html() noexcept{
				fake::custom::format::config::json::enable<[]{}, token::html>();
				fake::custom::format::config::color::enable<[]{}, token::html>();
				fake::custom::format::config::style::endl<[]{}, token::html, fake::view_v<"</br>">>();
				fake::custom::format::config::color::decorator<[]{}, token::html, colorization>();
				fake::custom::format::config::bracket_pair_colorization::enable<[]{}, token::html>();
				fake::custom::format::config::member_name_colorization::enable<[]{}, token::html>();
				fake::custom::format::config::style::tab<[]{}, token::html, fake::view_v<"&nbsp; &nbsp; ">>();
				using bracket = std::pair<fake::mezz_t<fake::view_v<"&lt;">>, fake::mezz_t<fake::view_v<"&gt;">>>;
				fake::custom::type_name::config::set_templace_bracket<[]{}, token::html, bracket>();
			}
		};
		
	}
	
	// preset CPOs. 
	template<auto _footprint = custom::type_stream::config::footprint<fake::cpo::format<token::plain>>([]{})>
	requires fake::meta::array_c<decltype(_footprint)>
	inline constexpr auto plain = fake::format<token::plain, _footprint>;
	
	template<auto _footprint = custom::type_stream::config::footprint<fake::cpo::format<token::pretty>>([]{})>
	requires fake::meta::array_c<decltype(_footprint)>
	inline constexpr auto pretty = fake::format<token::pretty, _footprint>;
	
	template<auto _footprint = custom::type_stream::config::footprint<fake::cpo::format<token::json>>([]{})>
	requires fake::meta::array_c<decltype(_footprint)>
	inline constexpr auto json = fake::format<token::json, _footprint>;
	
	template<auto _footprint = custom::type_stream::config::footprint<fake::cpo::format<token::html>>([]{})>
	requires fake::meta::array_c<decltype(_footprint)>
	inline constexpr auto html = fake::format<token::html, _footprint>;
	
}

#endif /*__FAKE_CPO_TYPEPRINT_H__*/ 
