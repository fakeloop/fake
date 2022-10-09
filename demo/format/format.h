#ifndef __FAKE_FORMAT_H__
#define __FAKE_FORMAT_H__

#include "view.h"
#include "meta.h"

namespace fake
{
	
	namespace custom
	{
		
		namespace tool::format
		{
			
			template<typename _Type, fake::mezz_c auto _constraint>
			concept concept_c = requires(fake::type_package<_Type> _e){requires _constraint.value(_e);};
			
		}
		
		struct format
		{
		private:
			static constexpr fake::meta::unordered_map configure = []{};
			
			template<typename _ConfigToken> struct key final{};
			template<typename _ConfigToken> struct guard final{};
			struct forbid final{};
			
		public:
			struct config{
			public:
				template<typename _ConfigToken>
				static consteval auto footprint(auto) noexcept{
					configure.emplace<[]{}>(fake::pack_v<guard<_ConfigToken>>, fake::pack_v<forbid>);
					using local_t = typename decltype(configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>))::type;
					
					if constexpr(std::same_as<local_t, fake::null_t>)
						return fake::meta::array<>{};
					else
						return local_t{}.data([]{});
				}
				
			public:
				template<auto, typename _ConfigToken, fake::view_c auto _formatter, typename _Lambda>
				requires std::same_as<
					typename decltype(configure.at<fake::pack_v<_Lambda>>(fake::pack_v<guard<_ConfigToken>>))::type,
					fake::null_t
				>
				static constexpr auto formatter(_Lambda &&_lambda) noexcept{
					using formatter_t = std::remove_cvref_t<decltype(_formatter)>;
					using lambda_t = std::remove_cvref_t<_Lambda>;
					using local_t = typename decltype(configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>))::type;
					
					if constexpr(std::same_as<local_t, fake::null_t>){
						constexpr fake::meta::unordered_map local = []{};
						using local_t = std::remove_cvref_t<decltype(local)>;
						configure.emplace<[]{}>(fake::pack_v<key<_ConfigToken>>, fake::pack_v<local_t>);
						
						local_t{}.template emplace<[]{}>(fake::pack_v<formatter_t>, fake::pack_v<lambda_t>);
					}
					else{
						local_t{}.template emplace<[]{}>(fake::pack_v<formatter_t>, fake::pack_v<lambda_t>);
					}
				}
			};
			
		private:
			template<std::size_t _value>
			struct index_t final{static constexpr std::size_t value = _value;};
			
			static constexpr fake::value_mezzanine forward_m = fake::mezz_v<
				[]<typename _Type>(fake::type_package<_Type>) consteval noexcept{
					return std::is_lvalue_reference_v<_Type> || std::is_reference_v<_Type> == false;
				}
			>;
			
			enum struct sequence_e{formatter, content};
			
			template<sequence_e _type, fake::view_c auto _view>
			struct sequence_t final{
				static constexpr auto type = _type;
				static constexpr auto view = _view;
			};
			
			template<std::size_t _key, std::size_t _value>
			struct pair_t{static constexpr std::size_t value = _value;};
			
			template<typename... _Base>
			struct mapping_t final : _Base...{};
			
			template<auto, typename...>
			struct stream;
			
			template<
				auto _footprint,
				typename _ConfigToken,
				fake::trait_c<fake::meta::array> _Sequence,
				fake::trait_c<mapping_t> _Mapping,
				tool::format::concept_c<forward_m>... _References
			>
			struct stream<_footprint, _ConfigToken, _Sequence, _Mapping, _References...> final{
			private:
				friend struct fake::custom::format;
				
				using sequence = _Sequence;
				using mapping = _Mapping;
				
				using reference_sequence = std::tuple<const std::remove_cvref_t<_References>&...>;
				constexpr stream(_References &&..._references) noexcept:
					references{std::forward<_References>(_references)...}{}
				
			private:
				template<std::size_t _key, std::size_t _value>
				static consteval std::size_t extract(const pair_t<_key, _value> &_pair) noexcept{return _pair.value;}
				
				static constexpr auto configure = format::configure;
				template<typename _Config> using key = format::key<_Config>;
				
				friend auto& operator<<(auto &_os, const stream &_stm){
					using local_t = typename decltype(configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>))::type;
					
					[&]<std::size_t... _Index>(std::index_sequence<_Index...>){
						auto pick = [&]<std::size_t _Key>(index_t<_Key>){
							if constexpr(requires{extract<_Key>(mapping{});}){
								using formatter = std::remove_const_t<
									decltype(fake::meta::array_element<_Key, sequence>::view)
								>;
								using lambda_t = typename decltype(
									local_t{}.template at<[]{}>(fake::pack_v<formatter>)
								)::type;
								
								if constexpr(std::same_as<lambda_t, fake::null_t>)
									_os << std::get<extract<_Key>(mapping{})>(_stm.references);
								else
									_os << lambda_t{}(std::get<extract<_Key>(mapping{})>(_stm.references));
							}
							else{
								_os << fake::meta::array_element<_Key, sequence>::view.data();
							}
						};
						
						(pick(index_t<_Index>{}), ...);
					}(std::make_index_sequence<sequence::size>());
					
					return _os;
				}
				
			private:
				reference_sequence references;
			};
			
			template<auto _footprint, typename _ConfigToken, fake::view_c _View, typename... _Args>
			static constexpr auto analyse_format(_Args &&..._args) noexcept{
				using view_t = _View;
				constexpr std::size_t size = sizeof...(_Args);
				constexpr std::size_t npos = view_t::npos;
				constexpr fake::meta::variant run = []{};
				constexpr fake::meta::variant error = []{};
				constexpr fake::meta::variant index = []{};
				constexpr fake::meta::variant left_esc = []{};
				constexpr fake::meta::variant right_esc = []{};
				constexpr fake::meta::variant offset = []{};
				constexpr fake::meta::vector sequence = []{};
				constexpr fake::meta::vector mapping = []{};
				
				index.template assign<[]{}>(fake::pack_v<fake::mezz_t<std::size_t{}>>);
				offset.template assign<[]{}>(fake::pack_v<fake::mezz_t<std::size_t{}>>);
				run.template assign<[]{}>(fake::pack_v<fake::mezz_t<true>>);
				error.template assign<[]{}>(fake::pack_v<fake::mezz_t<false>>);
				sequence.template emplace_back<[]{}>(fake::pack_v<sequence_t<sequence_e::content, fake::view_v<"">>>);
				
				constexpr auto force = fake::meta::for_x<
					[]<auto _left_esc = decltype(left_esc){}, auto _right_esc = decltype(right_esc){}>(auto){
						_left_esc.template assign<[]{}>(fake::pack_v<fake::mezz_t<view_t::template find<"{">()>>);
						_right_esc.template assign<[]{}>(fake::pack_v<fake::mezz_t<view_t::template find<"}">()>>);
					},
					[]<auto _run = decltype(run){}, auto _error = decltype(error){}>(auto){
						constexpr bool run = fake::take_t<_run([]{})>::value;
						constexpr bool error = fake::take_t<_error([]{})>::value;
						return run && error == false;
					},
					[]<
						auto _left_esc = decltype(left_esc){},
						auto _right_esc = decltype(right_esc){},
						auto _offset = decltype(offset){}
					>(auto){
						constexpr std::size_t offset = fake::take_t<_offset([]{})>::value;
						_left_esc.template assign<[]{}>(
							fake::pack_v<fake::mezz_t<view_t::template find<"{", offset>()>>
						);
						_right_esc.template assign<[]{}>(
							fake::pack_v<fake::mezz_t<view_t::template find<"}", offset>()>>
						);
					},
					[]{}
				>{
					[]<
						auto _run = decltype(run){},
						auto _error = decltype(error){},
						auto _index = decltype(index){},
						auto _left_esc = decltype(left_esc){},
						auto _right_esc = decltype(right_esc){},
						auto _offset = decltype(offset){},
						auto _sequence = decltype(sequence){},
						auto _mapping = decltype(mapping){}
					>(auto) consteval noexcept{
						constexpr std::size_t index = fake::take_t<_index([]{})>::value;
						constexpr std::size_t left_esc = fake::take_t<_left_esc([]{})>::value;
						constexpr std::size_t right_esc = fake::take_t<_right_esc([]{})>::value;
						constexpr std::size_t offset = fake::take_t<_offset([]{})>::value;
						constexpr std::size_t left_ape = view_t::template find<"{", left_esc + 1>();
						constexpr std::size_t right_ape = view_t::template find<"}", right_esc + 1>();
						constexpr std::size_t brace = view_t::template find<"}", left_esc + 1>();
						
						if constexpr(left_esc == npos && right_esc == npos){
							// [done]. 
							constexpr fake::view content = view_t::template substr<offset>();
							using back_t = typename decltype(_sequence.template back([]{}))::type;
							if constexpr(content.size()){
								if constexpr(back_t::type == sequence_e::content){
									using element_t = sequence_t<sequence_e::content, back_t::view + content>;
									_sequence.template pop_back([]{});
									_sequence.template emplace_back<[]{}>(fake::pack_v<element_t>);
								}
								else{
									using element_t = sequence_t<sequence_e::content, content>;
									_sequence.template emplace_back<[]{}>(fake::pack_v<element_t>);
								}
							}
							
							_run.template assign<[]{}>(fake::pack_v<fake::mezz_t<false>>);
						}
						else if constexpr(left_esc < right_esc && left_esc + 1 == left_ape){
							// [escape '{']. 
							constexpr fake::view content = view_t::template substr<offset, left_ape - offset>();
							using back_t = typename decltype(_sequence.template back([]{}))::type;
							if constexpr(back_t::type == sequence_e::content){
								using element_t = sequence_t<sequence_e::content, back_t::view + content>;
								_sequence.template pop_back([]{});
								_sequence.template emplace_back<[]{}>(fake::pack_v<element_t>);
							}
							else{
								using element_t = sequence_t<sequence_e::content, content>;
								_sequence.template emplace_back<[]{}>(fake::pack_v<element_t>);
							}
							
							_offset.template assign<[]{}>(fake::pack_v<fake::mezz_t<left_ape + 1>>);
						}
						else if constexpr(right_esc < left_esc && right_esc + 1 == right_ape){
							// [escape '}']. 
							constexpr fake::view content = view_t::template substr<offset, right_ape - offset>();
							using back_t = typename decltype(_sequence.template back([]{}))::type;
							if constexpr(back_t::type == sequence_e::content){
								using element_t = sequence_t<sequence_e::content, back_t::view + content>;
								_sequence.template pop_back([]{});
								_sequence.template emplace_back<[]{}>(fake::pack_v<element_t>);
							}
							else{
								using element_t = sequence_t<sequence_e::content, content>;
								_sequence.template emplace_back<[]{}>(fake::pack_v<element_t>);
							}
							
							_offset.template assign<[]{}>(fake::pack_v<fake::mezz_t<right_ape + 1>>);
						}
						else if constexpr(left_esc < right_esc && brace != npos){
							// [braces "{}"]. 
							constexpr fake::view content = view_t::template substr<offset, left_esc - offset>();
							constexpr fake::view format = view_t::template substr<left_esc, brace + 1 - left_esc>();
							using back_t = typename decltype(_sequence.template back([]{}))::type;
							if constexpr(content.size()){
								if constexpr(back_t::type == sequence_e::content){
									using element_t = sequence_t<sequence_e::content, back_t::view + content>;
									_sequence.template pop_back([]{});
									_sequence.template emplace_back<[]{}>(fake::pack_v<element_t>);
								}
								else{
									using element_t = sequence_t<sequence_e::content, content>;
									_sequence.template emplace_back<[]{}>(fake::pack_v<element_t>);
								}
							}
							if constexpr(_sequence.size([]{})){
								using front_t = typename decltype(_sequence.front([]{}))::type;
								if constexpr(front_t::type == sequence_e::content && front_t::view.size() == 0)
									_sequence.pop_front([]{});
							}
							_mapping.template emplace_back<[]{}>(fake::pack_v<pair_t<_sequence.size([]{}), index>>);
							using format_t = sequence_t<sequence_e::formatter, format>;
							_sequence.template emplace_back<[]{}>(fake::pack_v<format_t>);
							
							_offset.template assign<[]{}>(fake::pack_v<fake::mezz_t<brace + 1>>);
							_index.template assign<[]{}>(fake::pack_v<fake::mezz_t<index + 1>>);
						}
						else{
							// [parse error]. 
							_error.template assign<[]{}>(fake::pack_v<fake::mezz_t<true>>);
						}
					}
				};
				
				using mapping_type = fake::mimic_t<decltype(mapping.data([]{})), mapping_t>;
				
				if constexpr(fake::take_t<error([]{})>::value == false)
					return stream<_footprint, _ConfigToken, decltype(sequence.data([]{})), mapping_type, _Args...>{
						std::forward<_Args>(_args)...
					};
			}
			
		private:
			template<typename _ConfigToken, auto _footprint>
			friend struct format_broker;
			
			template<
				typename _ConfigToken,
				fake::view_c _Format,
				auto _footprint,
				typename... _Args
			>
			inline static constexpr auto method(_Args &&..._args){
				return analyse_format<_footprint, _ConfigToken, _Format>(std::forward<_Args>(_args)...);
			}
		};
		
		template<typename _ConfigToken, auto _footprint>
		struct format_broker
		{
		public:
			template<fake::view_c _Format, typename... _Args>
			requires requires(_Args ..._a){fake::custom::format::method<_ConfigToken, _Format, _footprint>(_a...);}
			inline constexpr auto operator()(_Format &&_fmt, _Args &&..._args) const{
				return fake::custom::format::method<_ConfigToken, _Format, _footprint>(std::forward<_Args>(_args)...);
			}
		};
		
	}
	
	template<typename _ConfigToken, auto _footprint = fake::custom::format::config::footprint<_ConfigToken>([]{})>
	inline constexpr fake::custom::format_broker<_ConfigToken, _footprint> format;
	
}

#endif /*__FAKE_FORMAT_H__*/ 
