#ifndef __FAKE_CPO_TYPEFORMAT_H__
#define __FAKE_CPO_TYPEFORMAT_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "flat.h"
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
			struct type final{};
			struct member final{};
			struct json final{};
			struct container final{};
			struct binary final{}; // TODO: binary flag for serialization, no impl yet. 
			struct bracket final{};
			struct name final{};
		};
		
	private:
		template<typename _ConfigToken>
		using access = fake::custom::detail::type_stream::accessor<token_t<_ConfigToken>>;
		
	private:
		template<typename... _Type>
		using adaptor = fake::tool::adaptor<_Type...>;
		
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
			struct visitor final{
				template<auto _vest, typename _ConfigToken, typename _Matcher>
				static consteval auto emplace(auto _visitor) noexcept{
					for_each::config::emplace_visitor<[]{}, token_t<_ConfigToken>, adaptor<_Matcher>>(_visitor);
				}
			};
			
			struct style final{
			private:
				template<typename _View>
				static consteval bool is_valid_tab(_View) noexcept{
					if constexpr(fake::view_c<_View>){
						constexpr std::string_view tab_v{_View::data()};
						constexpr std::string_view invalid = "\"'(),:;<>[\\]_{}";
						constexpr auto isalnum = [](char _e){
							return '0' <= _e && _e <= '9' && 'a' <= _e && _e <= 'z' && 'A' <= _e && _e <= 'Z';
						};
						bool term_escape = false;
						bool html_escape = false;
						for(const char e : tab_v){
							if(term_escape && e == 'm')
								term_escape = false;
							else if(html_escape && e == ';')
								html_escape = false;
							else if(e == '\e')
								term_escape = true;
							else if(e == '&')
								html_escape = true;
							else if(isalnum(e) || invalid.find(e) != std::string_view::npos)
								return false;
						}
						return term_escape == false && html_escape == false;
					}
					return false;
				}
				
			public:
				template<auto _vest, typename _ConfigToken, auto _tab>
				requires
					fake::view_c<decltype(_tab)> &&
					fake::to_trait_c<is_valid_tab(_tab)> &&
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
			
			struct type final{
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::type, fake::mezz_t<true>>();}
				static consteval auto enable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::type, fake::mezz_t<true>>();
				}
				
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::type, fake::mezz_t<false>>();}
				static consteval auto disable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::type, fake::mezz_t<false>>();
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
			
			struct binary final{
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::binary, fake::mezz_t<true>>();}
				static consteval auto enable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::binary, fake::mezz_t<true>>();
				}
				
				template<auto _vest, typename _ConfigToken>
				requires requires{access<_ConfigToken>::template emplace<_vest, key::binary, fake::mezz_t<false>>();}
				static consteval auto disable() noexcept{
					return access<_ConfigToken>::template emplace<_vest, key::binary, fake::mezz_t<false>>();
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
			
			template<fake::colors _color>
			static constexpr auto color(auto &_data) noexcept(noexcept(fake::color<_color>(_data)))
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
			static consteval auto type() noexcept{
				using member_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::type>()>;
				if constexpr(std::same_as<member_t, fake::null_t>)
					return true;
				else
					return member_t::value;
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
			static consteval auto binary() noexcept{
				using binary_t = fake::take_t<access<_ConfigToken>::template query<[]{}, key::binary>()>;
				if constexpr(std::same_as<binary_t, fake::null_t>)
					return false;
				else
					return binary_t::value;
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
		
		enum struct context : std::size_t {undefine, bracket, index};
		
		template<typename _Type, typename _Term>
		struct escape_stream{
			escape_stream(_Type &_e, _Term): ref(_e){}
			
		private:
			using type = std::remove_cvref_t<_Type>;
			
			friend std::basic_ostream<char>& operator<<(std::basic_ostream<char> &_os, escape_stream _s){
				constexpr auto escape = [](std::basic_ostream<char> &_o, const char _e){
					switch(_e){
						case '\0': _o << "\\0"; break;
						case '\b': _o << "\\b"; break;
						case '\e': _o << "\\e"; break;
						case '\f': _o << "\\f"; break;
						case '\r': _o << "\\r"; break;
						case '\t': _o << "\\t"; break;
						case '\n': _o << "\\n"; break;
						case '\v': _o << "\\v"; break;
						case '\\': _o << "\\\\"; break;
						case '\'': _o << "\\\'"; break;
						case '\"': _o << "\\\""; break;
						default: _o << _e;
					}
				};
				
				if constexpr(std::same_as<type, char>){
					escape(_os, _s.ref);
				}
				else if constexpr(requires{std::quoted(_s.ref);} && fake::array_c<type>){
					for(std::size_t i = 0; i < fake::array_size_v<type>; i++)
						if(i + 1 < fake::array_size_v<type> || _s.ref[i])
							escape(_os, _s.ref[i]);
				}
				else if constexpr(requires{std::quoted(_s.ref);}){
					for(const char e : _s.ref)
						escape(_os, e);
				}
				
				return _os;
			}
			
			friend std::basic_istream<char>& operator>>(std::basic_istream<char> &_is, escape_stream _s){
				constexpr auto escape = [](const char _e){
					switch(_e){
						case '0': return '\0';
						case 'b': return '\b';
						case 'e': return '\e';
						case 'f': return '\f';
						case 'r': return '\r';
						case 't': return '\t';
						case 'n': return '\n';
						case 'v': return '\v';
						default: return _e;
					}
				};
				
				if(_is.good() == false)
					return _is;
				
				const auto pos = _is.tellg();
				
				if constexpr(std::same_as<type, char>){
					_s.ref = _is.get();
					
					if(_s.ref == '\\'){
						if(_is.good() == false)
							return _is.clear(), _is.seekg(pos), _is;
						_s.ref = escape(_is.get());
					}
				}
				else if constexpr(requires{std::quoted(_s.ref);} && fake::array_c<type>){
					bool esc = false;
					std::size_t index = 0;
					while(_is.good() && (esc || _Term::try_match(_is) == false) && index < fake::array_size_v<type>){
						if(const char e = _is.get(); esc == false && e == '\\')
							esc = true;
						else if(esc)
							_s.ref[index++] = escape(e), esc = false;
						else
							_s.ref[index++] = e;
					}
					while(index < fake::array_size_v<type>)
						_s.ref[index++] = '\0';
				}
				else if constexpr(requires{std::quoted(_s.ref);}){
					_s.ref.clear();
					bool esc = false;
					while(_is.good() && (esc || _Term::try_match(_is) == false)){
						if(const char e = _is.get(); esc == false && e == '\\')
							esc = true;
						else if(esc)
							_s.ref += escape(e), esc = false;
						else
							_s.ref += e;
					}
				}
				
				return _is;
			}
			
		private:
			_Type &ref;
		};
		
		static auto escape(const char &_e){return escape_stream{_e, nullptr};}
		static auto escape(char &_e){return escape_stream{_e, nullptr};}
		
		template<auto _term>
		static auto escape(const auto &_e) requires requires{std::quoted(_e);}{return escape_stream{_e, _term};}
		template<auto _term>
		static auto escape(auto &_e) requires requires{std::quoted(_e);}{return escape_stream{_e, _term};}
		
		static consteval auto escape(fake::view_c auto _key) noexcept{
			return _key.template replace<"\\", "\\\\">().template replace<"\"", "\\\"">();
		}
		
		inline static constexpr fake::null_t null{};
		
		template<fake::trait_c<token_t> _ConfigToken, auto _footprint, typename _Type, typename _Init = decltype(null)>
		requires fake::meta::array_c<decltype(_footprint)> || std::same_as<decltype(_footprint), unsigned long long>
		struct stream final{
			using init_t = _Init;
			
		private:
			static constexpr bool enable_type = query::type<typename _ConfigToken::type>();
			static constexpr auto type_name = []{
				if constexpr(enable_type)
					return fake::type_name<typename _ConfigToken::type>;
				else
					return [](const fake::pack_c auto){return fake::view_v<"">;};
			}();
			static constexpr fake::view_c auto tab = query::tab<typename _ConfigToken::type>();
			static constexpr fake::view_c auto endl = query::endl<typename _ConfigToken::type>();
			static constexpr bool color = query::color<typename _ConfigToken::type>();
			static constexpr format::color map = query::map<typename _ConfigToken::type>();
			using decorator = fake::take_t<query::decorator<typename _ConfigToken::type>()>;
			static constexpr bool member = query::member<typename _ConfigToken::type>();
			static constexpr bool json = query::json<typename _ConfigToken::type>();
			using filters = fake::take_t<query::container<typename _ConfigToken::type>()>;
			static constexpr bool binary = query::binary<typename _ConfigToken::type>();
			static constexpr bool bracket = query::bracket<typename _ConfigToken::type>();
			static constexpr bool name = query::name<typename _ConfigToken::type>();
			
		public:
			constexpr stream(_Type &_data) noexcept: data(_data), init(null){}
			constexpr stream(_Type &_data, _Init &_init) noexcept: data(_data), init(_init){}
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
				constexpr std::size_t offset = _view.template find_first_not_of<"\t\n\v\f\r ">();
				constexpr std::size_t endpos = _view.template find_last_not_of<"\t\n\v\f\r ">() + 0x1;
				constexpr fake::view_c auto prefix = _view.template substr<0, offset>();
				constexpr fake::view_c auto infix = _view.template substr<offset, endpos - offset>();
				constexpr fake::view_c auto suffix = _view.template substr<endpos>();
				
				if constexpr(color){
					if constexpr(bracket && _context == context::bracket){
						using mezz = fake::meta::array_element_t<_layer % fake::meta::array_size_v<rotate>, rotate>;
						
						return prefix + decorator::template color<mezz::value>(infix) + suffix;
					}
					else if constexpr(name && _context == context::index){
						using mezz = fake::meta::array_element_t<_layer % fake::meta::array_size_v<shift>, shift>;
						
						return prefix + decorator::template color<mezz::value>(infix) + suffix;
					}
					else{
						return prefix + decorator::template color<_color>(infix) + suffix;
					}
				}
				else{
					return _view;
				}
			}
			
			template<fake::colors _color, context _context = context::undefine, std::size_t _layer = 0>
			static constexpr decltype(auto) decorate(const auto &_data) noexcept
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
			
			template<fake::colors _color, context _context = context::undefine, std::size_t _layer = 0>
			static constexpr decltype(auto) decorate(auto &_data) noexcept
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
			
			template<fake::view_c auto _view, std::size_t _layer>
			static consteval auto indent() noexcept{
				return []<std::size_t... _index>(std::index_sequence<_index...>){
					return (decorate<map.indent>((_index, _view)) + ... + fake::view_v<"">);
				}(std::make_index_sequence<_layer>());
			}
			
			template<fake::view_c auto _view, std::size_t _layer>
			struct indenter final{
			private:
				static constexpr auto decorate_broker(std::size_t) noexcept{
					return fake::ensure(decorate<map.indent>(_view));
				}
				
			public:
				static bool try_match(std::basic_istream<char> &_is){
					if(_is.good() == false)
						return false;
					
					if constexpr(_view.template find_first_not_of<"\t\n\v\f\r ">() == std::string_view::npos){
						return true;
					}
					else{
						const auto pos = _is.tellg();
						
						const bool match = [&_is]<std::size_t... _index>(std::index_sequence<_index...>){
							return ((decorate_broker(_index).try_match(_is) && _is >> decorate_broker(_index)) && ...);
						}(std::make_index_sequence<_layer>());
						
						if(_is.good() == false || match == false)
							return _is.clear(), _is.seekg(pos), false;
						return _is.clear(), _is.seekg(pos), true;
					}
				}
				
			private:
				friend std::basic_istream<char>& operator>>(std::basic_istream<char> &_is, indenter){
					return [&_is]<std::size_t... _index>(std::index_sequence<_index...>) -> std::basic_istream<char>& {
						return (_is >> ... >> decorate_broker(_index));
					}(std::make_index_sequence<_layer>());
				}
			};
			
			template<auto... _streamers>
			struct scoper final{
				static bool try_match(std::basic_istream<char> &_is){
					if(_is.good() == false)
						return false;
					
					const auto pos = _is.tellg();
					
					const bool match = ((_streamers.try_match(_is) && _is >> _streamers) && ...);
					
					if(_is.good() == false || match == false)
						return _is.clear(), _is.seekg(pos), false;
					return _is.clear(), _is.seekg(pos), true;
				}
				
			private:
				friend std::basic_istream<char>& operator>>(std::basic_istream<char> &_is, scoper){
					return (_is >> ... >> _streamers);
				}
			};
			
			static consteval auto container(fake::pack_c auto _pack) noexcept{
				constexpr auto filter = []<std::size_t... _index>(std::index_sequence<_index...>) consteval{
					return fake::filter<fake::meta::array_element_t<_index, filters>{}...>;
				}(std::make_index_sequence<fake::meta::array_size_v<filters>>());
				
				using type = std::remove_cvref_t<fake::take_t<decltype(_pack){}>>;
				using array_adaptor = std::conditional_t<fake::array_c<type>, type, std::nullptr_t[]>;
				constexpr bool array_v = fake::array_c<type> && !std::same_as<fake::array_value_t<array_adaptor>, char>;
				
				return filter(_pack) || array_v;
			}
			
		private:
			static std::basic_ostream<char>& ostream(std::basic_ostream<char> &_os, const stream &_stream){
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
					constexpr bool sole = _View::empty() || enable_type == false;
					constexpr bool bare = _View::empty() && enable_type == false;
					constexpr fake::view_c auto space = std::conditional_t<sole, fake::view<>, fake::view<' '>>{};
					constexpr fake::view_c auto name = std::conditional_t<member, _View, fake::view<>>{};
					constexpr bool contain = json && container(fake::pack_v<type>);
					constexpr bool element = container(fake::pack_v<parent>);
					
					std::size_t i = 0;
					auto scope = []<bool _comma, std::size_t _depth>(fake::mezz_t<_comma>, fake::mezz_t<_depth>){
						if constexpr(_comma)
							return decorate<map.comma>(","_v) + endl + indent<tab, _depth>();
						else
							return endl + indent<tab, _depth>();
					};
					
					if(_i++)
						_os << scope(fake::mezz_v<true>, fake::mezz_v<_layer>);
					
					constexpr fake::view_c auto quote = decorate<map.quote>("\""_v);
					constexpr fake::view_c auto char_quote = decorate<map.quote>("'"_v);
					constexpr fake::view_c auto json_quote = std::conditional_t<json, decltype(quote), fake::view<>>{};
					
					constexpr fake::view title = [](auto _name, auto _space, auto _json_quote, auto _ignore){
						if constexpr(json && _ignore.value)
							return ""_v;
						else
							return _json_quote + decorate<map.type>(type_name(fake::pack_v<type>)) + _space +
								decorate<map.member, context::index, _layer>(escape(_name)) + _json_quote +
								decorate<map.colon>(" : "_v);
					}(name, space, json_quote, fake::mezz_v<element || bare>);
					
					constexpr fake::view_c auto l = std::conditional_t<contain, fake::view_t<"[">, fake::view_t<"{">>{};
					constexpr fake::view_c auto r = std::conditional_t<contain, fake::view_t<"]">, fake::view_t<"}">>{};
					
					using array_adaptor = std::conditional_t<fake::array_c<type>, type, std::nullptr_t[]>;
					
					if constexpr(requires{fake::for_each<_ConfigToken>(_e, []<fake::view_c>(auto &&_e){});}){
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
					else if constexpr(fake::array_c<type> && !std::same_as<fake::array_value_t<array_adaptor>, char>){
						_os << title + decorate<map.bracket, context::bracket, _layer>(l) +
							scope(fake::mezz_v<false>, fake::mezz_v<_layer + 1>);
						
						for(auto &e : _e)
							_self.template operator()<fake::view_t<"">, _layer + 1, current>(_self, i, e);
						
						_os << scope(fake::mezz_v<false>, fake::mezz_v<_layer>) +
							decorate<map.bracket, context::bracket, _layer>(r);
					}
					else if constexpr(std::is_enum_v<type>){
						using underlying = std::underlying_type_t<type>;
						if constexpr(std::same_as<underlying, char>)
							_os << title + char_quote << escape(static_cast<char>(_e)) << char_quote;
						else
							_os << title << decorate<map.value>(static_cast<underlying>(_e));
					}
					else{
						if constexpr(requires{std::quoted(_e);})
							_os << title + quote << escape<fake::ensure(quote)>(_e) << quote;
						else if constexpr(std::same_as<type, char>)
							_os << title + char_quote << escape(_e) << char_quote;
						else
							_os << title << decorate<map.value>(_e);
					}
				};
				
				using type = std::remove_cvref_t<decltype(_stream.data)>;
				constexpr bool object = json && (enable_type || container(fake::pack_v<type>) == false);
				
				if constexpr(object)
					_os << decorate<map.bracket, context::bracket, 0x0>(fake::view_v<"{">) + endl + indent<tab, 0x1>();
				
				std::size_t index = 0;
				using status = fake::meta::array<fake::null_t>;
				impl.template operator()<fake::view<>, std::size_t{object}, status>(impl, index, _stream.data);
				
				if constexpr(object)
					_os << endl + decorate<map.bracket, context::bracket, 0x0>(fake::view_v<"}">);
				
				return _os;
			}
			
			static std::basic_istream<char>& istream(std::basic_istream<char> &_is, const stream &_stream){
				// recurring-lambda currying. 
				auto impl = [
					&_is,
					&init = _stream.init
				]<fake::view_c _View, std::size_t _layer, fake::meta::array_c _Status>(
					auto &_self,
					std::size_t &_i,
					auto &&_e
				){
					using namespace fake::literals;
					using type = std::remove_cvref_t<decltype(_e)>;
					using status = _Status;
					using parent = fake::meta::array_element_t<0, status>;
					using current = fake::meta::array_cat_t<fake::meta::array<type>, status>;
					constexpr bool sole = _View::empty() || enable_type == false;
					constexpr bool bare = _View::empty() && enable_type == false;
					constexpr fake::view_c auto space = std::conditional_t<sole, fake::view<>, fake::view<' '>>{};
					constexpr fake::view_c auto name = std::conditional_t<member, _View, fake::view<>>{};
					constexpr bool contain = json && container(fake::pack_v<type>);
					constexpr bool element = container(fake::pack_v<parent>);
					
					std::size_t i = 0;
					auto scope = []<bool _comma, std::size_t _depth>(fake::mezz_t<_comma>, fake::mezz_t<_depth>){
						if constexpr(_comma)
							return scoper<
								fake::ensure(decorate<map.comma>(","_v)),
								fake::ensure(endl),
								indenter<tab, _depth>{}
							>{};
						else
							return scoper<fake::ensure(endl), indenter<tab, _depth>{}>{};
					};
					
					if(_i++)
						_is >> scope(fake::mezz_v<true>, fake::mezz_v<_layer>);
					
					constexpr fake::view_c auto quote = decorate<map.quote>("\""_v);
					constexpr fake::view_c auto char_quote = decorate<map.quote>("'"_v);
					constexpr fake::view_c auto json_quote = std::conditional_t<json, decltype(quote), fake::view<>>{};
					
					constexpr auto title = [](auto _name, auto _space, auto _json_quote, auto _ignore, auto _type){
						using type = fake::take_t<decltype(_type){}>;
						if constexpr(json && _ignore.value)
							return fake::ensure(""_v);
						else if constexpr(_json_quote.empty())
							return scoper<
								fake::ensure(decorate<map.type>(type_name(fake::pack_v<type>))),
								fake::ensure(decorate<map.member, context::index, _layer>(escape(_name))),
								fake::ensure(decorate<map.colon>(":"_v))
							>{};
						else
							return scoper<
								fake::ensure(
									_json_quote + decorate<map.type>(type_name(fake::pack_v<type>)) + _space +
										decorate<map.member, context::index, _layer>(escape(_name)) + _json_quote
								),
								fake::ensure(decorate<map.colon>(":"_v))
							>{};
					}(name, space, json_quote, fake::mezz_v<element || bare>, fake::pack_v<type>);
					
					constexpr fake::view_c auto l = std::conditional_t<contain, fake::view_t<"[">, fake::view_t<"{">>{};
					constexpr fake::view_c auto r = std::conditional_t<contain, fake::view_t<"]">, fake::view_t<"}">>{};
					
					using array_adaptor = std::conditional_t<fake::array_c<type>, type, std::nullptr_t[]>;
					
					if constexpr(requires{fake::for_each<_ConfigToken>(_e, []<fake::view_c>(auto &&_e){});}){
						_is >> title >> fake::ensure(decorate<map.bracket, context::bracket, _layer>(l))
							>> scope(fake::mezz_v<false>, fake::mezz_v<_layer + 1>);
						
						constexpr auto terminator = fake::ensure(
							decorate<map.bracket, context::bracket, _layer>(r),
							scope(fake::mezz_v<false>, fake::mezz_v<_layer>)
						);
						
						if constexpr(fake::std_container_c<type> || fake::std_array_c<type>){
							constexpr auto value_type_v = []{
								if constexpr(requires{typename type::key_type; typename type::mapped_type;})
									return fake::pack_v<std::pair<typename type::key_type, typename type::mapped_type>>;
								else
									return fake::pack_v<typename type::value_type>;
							}();
							using value_type = fake::take_t<value_type_v>;
							
							if constexpr(requires{_e.clear();})
								_e.clear();
							
							for(std::size_t index = 0; terminator.try_match(_is) == false; index++){
								const auto make_element = [&_self, &i]{
									value_type value{};
									_self.template operator()<fake::view<>, _layer + 1, current>(_self, i, value);
									
									return value;
								};
								
								if constexpr(requires{_e.emplace(value_type{});}){
									_e.emplace(make_element());
								}
								else if constexpr(requires{_e.emplace_back(value_type{});}){
									_e.emplace_back(make_element());
								}
								else if constexpr(requires{std::tuple_size<type>::value;}){
									if(index >= std::tuple_size_v<type>)
										throw fake::exception::mismatch::make("match failed: [out of range]"_v, _is);
									_e[index] = make_element();
								}
								else{
									static_assert(sizeof(type*) == 0, "type matches 'for_each' but miss modifier.");
								}
							}
						}
						else if constexpr(
							fake::trait_c<type, std::unique_ptr> ||
							fake::trait_c<type, std::shared_ptr> ||
							fake::trait_c<type, std::optional>
						){
							_e.reset();
							
							if(terminator.try_match(_is) == false){
								if constexpr(requires{init.make(_e);})
									init.make(_e);
								else if constexpr(fake::trait_c<type, std::unique_ptr>)
									_e = std::make_unique<typename type::element_type>();
								else if(fake::trait_c<type, std::optional>)
									_e.emplace();
								
								fake::for_each<_ConfigToken>(
									_e,
									[&_self, &i]<fake::view_c _V>(auto &&_e){
										_self.template operator()<_V, _layer + 1, current>(_self, i, _e);
									}
								);
							}
						}
						else if constexpr(fake::trait_c<type, std::variant>){
							fake::for_each<_ConfigToken>(
								_e,
								[&_self, &i, &_e, &init]<fake::view_c _V>(auto &&_d){
									_self.template operator()<_V, _layer + 1, current>(_self, i, _d);
									if constexpr(_V{} == "index"_v){
										if(_d == std::variant_npos)
											return;
										[&_e, &init]<std::size_t... _index>(
											std::index_sequence<_index...>,
											std::size_t _alt
										){
											using function = void(*)(type&, init_t &_init);
											constexpr function inits[std::variant_size_v<type>] = {
												[](type &_v, init_t &_init){
													using element_t = std::variant_alternative_t<_index, type>;
													
													if constexpr(requires{init.template make<element_t>(_v);})
														_init.template make<element_t>(_v);
													else
														_v.template emplace<_index>();
												}...
											};
											if(_alt < std::variant_size_v<type>)
												inits[_alt](_e, init);
										}(std::make_index_sequence<std::variant_size_v<type>>(), _d);
									}
								}
							);
						}
						else{
							fake::for_each<_ConfigToken>(
								_e,
								[&_self, &i]<fake::view_c _V>(auto &&_e){
									_self.template operator()<_V, _layer + 1, current>(_self, i, _e);
								}
							);
						}
						
						_is >> terminator;
					}
					else if constexpr(fake::array_c<type> && !std::same_as<fake::array_value_t<array_adaptor>, char>){
						_is >> title >> fake::ensure(decorate<map.bracket, context::bracket, _layer>(l))
							>> scope(fake::mezz_v<false>, fake::mezz_v<_layer + 1>);
						
						for(auto &e : _e)
							_self.template operator()<fake::view_t<"">, _layer + 1, current>(_self, i, e);
						
						_is >> scope(fake::mezz_v<false>, fake::mezz_v<_layer>)
							>> fake::ensure(decorate<map.bracket, context::bracket, _layer>(r));
					}
					else if constexpr(std::is_enum_v<type>){
						using underlying = std::underlying_type_t<type>;
						
						underlying under;
						if constexpr(std::same_as<underlying, char>)
							_is >> title >> fake::ensure(char_quote) >> escape(under) >> fake::ensure(char_quote);
						else
							_is >> title >> decorate<map.value>(under);
						_e = static_cast<type>(under);
					}
					else{
						if constexpr(requires{std::quoted(_e);})
							_is >> title >> fake::ensure(quote) >> escape<fake::ensure(quote)>(_e)
								>> fake::ensure(quote);
						else if constexpr(std::same_as<type, char>)
							_is >> title >> fake::ensure(char_quote) >> escape(_e) >> fake::ensure(char_quote);
						else
							_is >> title >> decorate<map.value>(_e);
					}
				};
				
				using type = std::remove_cvref_t<decltype(_stream.data)>;
				constexpr bool object = json && (enable_type || container(fake::pack_v<type>) == false);
				
				if constexpr(object)
					_is >> fake::ensure(decorate<map.bracket, context::bracket, 0x0>(fake::view_v<"{">))
						>> fake::ensure(endl) >> indenter<tab, 0x1>{};
				
				std::size_t index = 0;
				using status = fake::meta::array<fake::null_t>;
				impl.template operator()<fake::view<>, std::size_t{object}, status>(impl, index, _stream.data);
				
				if constexpr(object)
					_is >> fake::ensure(endl)
						>> fake::ensure(decorate<map.bracket, context::bracket, 0x0>(fake::view_v<"}">));
				
				return _is;
			}
			
		private:
			friend std::basic_ostream<char>& operator<<(std::basic_ostream<char> &_os, const stream &_stream){
				return ostream(_os, _stream);
			}
			
			friend std::basic_istream<char>& operator>>(std::basic_istream<char> &_is, const stream &_stream){
				return istream(_is, _stream);
			}
			
		private:
			_Type &data;
			_Init &init;
		};
		
	private:
		template<typename>
		struct is_std_array final : std::false_type{};
		
		template<template<typename, std::size_t> typename _Template, typename _Type, std::size_t _size>
		requires std::same_as<_Template<_Type, _size>, std::array<_Type, _size>>
		struct is_std_array<_Template<_Type, _size>> final : std::true_type{};
		
		template<typename _Type>
		static constexpr bool is_std_array_v = is_std_array<std::remove_cvref_t<_Type>>::value;
		
		// register meta-implementations for token-based-cpo 'fake::type_stream' and 'fake::for_each' at compile-time. 
		template<typename _ConfigToken>
		static consteval auto inject() noexcept{
			using type_stream = fake::custom::type_stream;
			using for_each = fake::custom::for_each;
			
			constexpr auto vest = []{};
			
			type_stream::config::emplace_stream<vest, token_t<_ConfigToken>, type_stream::adaptor<stream>>();
			
			// 'fake::utilities'. 
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<fake::view_like<fake::view>>>(
				[](auto &&_e, const auto &_f){}
			);
			
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<fake::generic<fake::mate>>>(
				[](auto &&_e, const auto &_f){
					using type = std::remove_cvref_t<decltype(_e)>;
					using first = typename type::first_type;
					using second = typename type::second_type;
					
					if constexpr(!fake::mezz_c<first> && !fake::view_c<first> && !std::same_as<first, void>)
						_f.template operator()<fake::view_t<"first">>(_e.first);
					if constexpr(!fake::mezz_c<second> && !fake::view_c<second> && !std::same_as<second, void>)
						_f.template operator()<fake::view_t<"second">>(_e.second);
				}
			);
			
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<fake::generic<fake::flat>>>(
				[](auto &&_e, const auto &_f){
					[&]<std::size_t... _index>(std::index_sequence<_index...>){
						(_f.template operator()<decltype(fake::to_view_v<_index>)>(fake::get<_index>(_e)), ...);
					}(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<decltype(_e)>>>());
				}
			);
			
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<fake::generic<fake::query>>>(
				[](auto &&_e, const auto &_f){
					using query = std::remove_cvref_t<decltype(_e)>;
					using keys = typename query::first_types;
					[&]<std::size_t... _index>(std::index_sequence<_index...>){
						const auto each = [&](fake::pack_c auto _pack, fake::mezz_c auto _i){
							using key = fake::take_t<decltype(_pack){}>;
							_f.template operator()<decltype(fake::to_view_v<_i.value>)>(fake::get<key>(_e));
						};
						
						(each(fake::pack_v<std::tuple_element_t<_index, keys>>, fake::mezz_v<_index>), ...);
					}(std::make_index_sequence<fake::query_size_v<query>>());
				}
			);
			
			// 'std::utilities'. 
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
			
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<fake::generic<std::unique_ptr>>>(
				[](auto &&_e, const auto &_f){
					if(_e)
						_f.template operator()<fake::view_t<"value">>(*_e);
				}
			);
			
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<fake::generic<std::optional>>>(
				[](auto &&_e, const auto &_f){
					if(_e)
						_f.template operator()<fake::view_t<"value">>(*_e);
				}
			);
			
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<fake::generic<std::variant>>>(
				[](auto &&_e, const auto &_f){
					std::size_t index = _e.index();
					_f.template operator()<fake::view_t<"index">>(index);
					if(index != std::variant_npos)
						std::visit([&_f](auto &&_e){_f.template operator()<fake::view_t<"value">>(_e);}, _e);
				}
			);
			
			// 'std containers'. 
			constexpr auto visitor = [](auto &&_e, const auto &_f){
				for(auto &&e : _e)
					_f.template operator()<fake::view<>>(e);
			};
			
			using std_array_g = fake::pattern_t<std::array<int, 1>>;
			for_each::config::emplace_visitor<vest, token_t<_ConfigToken>, adaptor<std_array_g>>(visitor);
			using std_array_m = fake::mezz_t<[]<typename _T>(fake::type_package<_T>) requires is_std_array_v<_T> {}>;
			config::json::register_container<[]{}, _ConfigToken, std_array_m>();
			
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
					constexpr stream(_Type &_data): data(_data){}
					stream(const stream&) = delete;
					stream& operator=(const stream&) = delete;
					
					friend std::basic_ostream<char>& operator<<(std::basic_ostream<char> &_os, stream _view){
						return _os << color<_front, _back>() << _view.data << clear();
					}
					
					friend std::basic_istream<char>& operator>>(std::basic_istream<char> &_is, stream _view){
						return _is >> fake::ensure(color<_front, _back>()) >> _view.data >> fake::ensure(clear());
					}
					
				private:
					_Type &data;
				};
				
			}
			
			template<fake::colors _front, fake::colors _back = fake::colors::black>
			constexpr auto color(const fake::view_c auto &_view) noexcept{
				return detail::color<_front, _back>() + _view + detail::clear();
			}
			
			template<fake::colors _front, fake::colors _back = fake::colors::black>
			constexpr auto color(const auto &_data){return detail::stream<decltype(_data), _front, _back>{_data};}
			
			template<fake::colors _front, fake::colors _back = fake::colors::black>
			constexpr auto color(auto &_data){return detail::stream<decltype(_data), _front, _back>{_data};}
			
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
				
				template<fake::colors _color>
				static constexpr auto color(auto &_data) noexcept(noexcept(tool::html::color<_color>(_data)))
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

#endif /*__FAKE_CPO_TYPEFORMAT_H__*/ 
