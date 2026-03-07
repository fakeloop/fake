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

#include "push.h"

#include <array>
#include <string>
#include <memory>

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
		template<fake::gene_c _Gene> struct alias final{};
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
			
			template<auto _vest, typename _ConfigToken, fake::gene_c _Gene, fake::view_c _Alias>
			requires (
				instantiate_after_configure<_vest, _ConfigToken>() &&
				!std::same_as<_Gene, fake::generic<fake::container::null_t>>
			)
			static consteval auto set_template_alias(_Alias) noexcept{
				constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
				local.template emplace<[]{}>(fake::pack_v<alias<_Gene>>, fake::pack_v<_Alias>);
			}
			
			template<auto _vest, typename _ConfigToken, fake::gene_c _Gene, fake::view_c _Alias>
			requires (
				instantiate_after_configure<_vest, _ConfigToken>() &&
				std::same_as<_Gene, fake::generic<fake::container::null_t>>
			)
			static consteval auto set_template_alias(_Alias) noexcept{}
			
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
		template<typename>
		struct is_std_array final : std::false_type{};
		
		template<template<typename, std::size_t> typename _Template, typename _Type, std::size_t _size>
		requires std::same_as<_Template<_Type, _size>, std::array<_Type, _size>>
		struct is_std_array<_Template<_Type, _size>> final : std::true_type{};
		
		template<typename _Type>
		static constexpr bool is_std_array_v = is_std_array<std::remove_cvref_t<_Type>>::value;
		
		template<char _e>
		static consteval auto escape(fake::mezz_t<_e>) noexcept{
			constexpr std::array alt{'0', 'b', 'e', 'f', 'r', 't', 'n', 'v', '\\', '\'', '\"'};
			constexpr std::size_t index = []{
				constexpr std::array code{'\0', '\b', '\e', '\f', '\r', '\t', '\n', '\v', '\\', '\'', '\"'};
				for(std::size_t i = 0; i < code.size(); i++)
					if(code[i] == _e)
						return i;
				return ~std::size_t{};
			}();
			if constexpr(index == ~std::size_t{})
				return fake::view<_e>{};
			else
				return fake::view<'\\', alt[index]>{};
		}
		
		static consteval auto escape(fake::view_c auto _e) noexcept{
			using type = decltype(_e);
			using code = fake::mezz_t<std::array{'\0', '\b', '\e', '\f', '\r', '\t', '\n', '\v', '\\', '\'', '\"'}>;
			using alt = fake::mezz_t<std::array{'0', 'b', 'e', 'f', 'r', 't', 'n', 'v', '\\', '\'', '\"'}>;
			
			constexpr std::array string = []{
				constexpr std::size_t size = []{
					constexpr auto match = [](char _e){return std::ranges::count(code::value, _e) + 0x1;};
					std::size_t size = 0;
					for(std::size_t i = 0; i < type::size(); i++)
						size += match(type::data()[i]);
					return size;
				}();
				
				std::array<char, size> array{};
				for(std::size_t i = 0, j = 0; i < type::size(); i++){
					const char e = type::data()[i];
					const auto it = std::ranges::find(code::value, e);
					if(it == code::value.end())
						array[j++] = e;
					else
						array[j++] = '\\', array[j++] = alt::value[it - code::value.begin()];
				}
				
				return std::move(array);
			}();
			
			using view = fake::mezz_t<string>;
			
			return []<std::size_t... _index>(std::index_sequence<_index...>){
				return fake::view<view::value[_index]...>{};
			}(std::make_index_sequence<string.size()>());
		}
		
	private:
		static consteval auto core(fake::view_c auto _prefix) noexcept{
			return _prefix.template substr<_prefix.template find_last_of<":">() + 1>();
		}
		
		template<fake::view_c auto _prefix, fake::view_c auto _infix, fake::view_c auto _suffix>
		struct split_t final{
			static constexpr auto prefix = _prefix;
			static constexpr auto infix = _infix;
			static constexpr auto suffix = _suffix;
		};
		
		static consteval auto split(fake::view_c auto _origin) noexcept{
			struct result_t final{std::string_view prefix; std::string_view infix; std::string_view suffix;};
			
			constexpr result_t r = [](std::string_view _symbol) -> result_t {
				const std::size_t tail = _symbol.find_last_of('>');
				const bool is_template = tail + 1;
				const std::string_view suffix = is_template ? _symbol.substr(tail + 1) : std::string_view{""};
				std::size_t head = is_template ? tail : 0;
				if(is_template)
					for(std::size_t stack = 1; stack && head; head--)
						switch(_symbol[head - 1]){case '>': stack++; break; case '<': stack--; break;};
				const std::string_view prefix = is_template ? _symbol.substr(0, head) : _symbol;
				return {prefix, _symbol.substr(head, tail + 1 - head), suffix};
			}(_origin);
			
			namespace tool = fake::detail::view;
			
			constexpr fake::view_c auto prefix_v = fake::view_v<tool::string<r.prefix.size() + 1>{r.prefix.data()}>;
			constexpr fake::view_c auto infix_v = fake::view_v<tool::string<r.infix.size() + 1>{r.infix.data()}>;
			constexpr fake::view_c auto suffix_v = fake::view_v<tool::string<r.suffix.size() + 1>{r.suffix.data()}>;
			
			return split_t<prefix_v, infix_v, suffix_v>{};
		}
		
	private:
		template<typename _ConfigToken, auto _footprint, typename _Type>
		requires fake::meta::array_c<decltype(_footprint)> && fake::is_template_v<fake::bare_t<_Type>>
		struct template_name final{
		private:
			static constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
			using bracket_t = fake::take_t<local.template at<[]{}>(fake::pack_v<bracket>)>;
			static constexpr fake::view_c auto left = bracket_t::first_type::value;
			static constexpr fake::view_c auto right = bracket_t::second_type::value;
			
		public:
			static consteval auto view() noexcept{
				using bare = fake::bare_t<_Type>;
				using info = fake::template_info<bare>;
				using alias_type = fake::take_t<local.template at<[]{}>(fake::pack_v<alias<fake::pattern_t<bare>>>)>;
				constexpr bool cover = fake::view_c<alias_type>;
				constexpr auto result = type_name::split(fake::type_view(fake::pack_v<_Type>));
				constexpr fake::view_c auto prefix = std::conditional_t<cover, alias_type, decltype(result.prefix)>{};
				constexpr fake::view_c auto infix = result.infix;
				constexpr fake::view_c auto suffix = result.suffix;
				
				using mezz = fake::take_t<local.template at<[]{}>(fake::pack_v<trunc<fake::pattern_t<bare>>>)>;
				
				using max_t = fake::mezz_t<~std::size_t{}>;
				constexpr std::size_t size = std::conditional_t<std::same_as<mezz, fake::null_t>, max_t, mezz>::value;
				
				if constexpr(info::is_view_like && fake::mezz_c<bare>){
					return prefix + left + fake::view_v<"\'"> + escape(bare{}) + fake::view_v<"\'"> + right + suffix;
				}
				else if constexpr(info::is_view_like && fake::view_c<bare>){
					constexpr fake::view_c auto value = escape(info::self);
					return prefix + left + fake::view_v<"\'"> + value + fake::view_v<"\'"> + right + suffix;
				}
				else if constexpr(info::is_array_like){
					using type = typename info::remove_cvref_arg;
					return prefix + left + method<_ConfigToken, _footprint, type>() + fake::view_v<" ["> +
						fake::to_view_v<info::array_size> + fake::view_v<"]"> + right + suffix;
				}
				else{
					using tuple = typename info::remove_cvref_args;
					return prefix + []<std::size_t... _index>(std::index_sequence<_index...>){
						return (
							left + (
								(
									std::conditional_t<_index, fake::view_t<", ">, fake::view_t<"">>{} +
									method<_ConfigToken, _footprint, fake::element_index_t<_index, tuple>>()
								) + ... + right
							)
						);
					}(std::make_index_sequence<std::min(size, std::tuple_size_v<tuple>)>()) + suffix;
				}
			}
		};
		
		template<typename _ConfigToken, auto _footprint, fake::array_c _Type>
		requires fake::meta::array_c<decltype(_footprint)>
		struct array_name final{
			static consteval auto split() noexcept{
				using type = fake::array_value_t<_Type>;
				constexpr std::size_t size = fake::array_size_v<_Type>;
				constexpr fake::view_c auto prefix = type_name::method<_ConfigToken, _footprint, type>();
				constexpr fake::view_c auto suffix = fake::value_view(fake::mezz_v<size>);
				
				return split_t<prefix, fake::view_v<"">, fake::view_v<" ["> + suffix + fake::view_v<"]">>{};
			}
			
			static consteval auto view() noexcept{
				constexpr auto result = split();
				
				return result.prefix + result.suffix;
			}
		};
		
	private:
		static constexpr auto integral_transform = [](fake::pack_c auto _pack){
			using namespace fake::literals;
			using type = fake::take_t<decltype(_pack){}>;
			constexpr fake::view_c auto unsigned_v = std::conditional_t<
				std::is_unsigned_v<type>,
				fake::view_t<"u">,
				fake::view_t<"">
			>{};
			constexpr fake::view_c auto size_v = fake::value_view(fake::mezz_v<(sizeof(type) << 0x3)>);
			
			return unsigned_v + "int"_v + size_v + "_t"_v;
		};
		
	private:
		// register meta-implementations for token-based-cpo 'fake::custom::type_name' at compile-time. 
		template<auto, typename _ConfigToken>
		static consteval auto inject() noexcept{
			config::emplace_transform<[]{}, _ConfigToken, int8_t>(integral_transform);
			config::emplace_transform<[]{}, _ConfigToken, uint8_t>(integral_transform);
			config::emplace_transform<[]{}, _ConfigToken, int16_t>(integral_transform);
			config::emplace_transform<[]{}, _ConfigToken, uint16_t>(integral_transform);
			config::emplace_transform<[]{}, _ConfigToken, int16_t>(integral_transform);
			config::emplace_transform<[]{}, _ConfigToken, uint16_t>(integral_transform);
			config::emplace_transform<[]{}, _ConfigToken, int32_t>(integral_transform);
			config::emplace_transform<[]{}, _ConfigToken, uint32_t>(integral_transform);
			config::emplace_transform<[]{}, _ConfigToken, int64_t>(integral_transform);
			config::emplace_transform<[]{}, _ConfigToken, uint64_t>(integral_transform);
			
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
			
			if constexpr(requires{typename fake::vector<bool>::reference;})
				config::emplace_transform<[]{}, _ConfigToken, fake::vector<bool>::reference>(
					[](fake::pack_c auto _pack){return fake::view_v<"bool">;}
				);
			
			using unique_ptr_g = fake::generic<std::unique_ptr>;
			config::set_templace_trunc<[]{}, _ConfigToken, unique_ptr_g, 0x1>();
			
			using vector_g = fake::pattern_t<fake::vector<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, vector_g, 0x1>();
			
			using deque_g = fake::pattern_t<fake::deque<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, deque_g, 0x1>();
			
			using forward_list_g = fake::pattern_t<fake::forward_list<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, forward_list_g, 0x1>();
			
			using list_g = fake::pattern_t<fake::list<int>>;
			config::set_templace_trunc<[]{}, _ConfigToken, list_g, 0x1>();
			config::set_template_alias<[]{}, _ConfigToken, list_g>(fake::view_v<"std::list">);
			
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
		template<
			fake::view_c auto _entire,
			fake::view_c auto _core,
			fake::view_c auto _prefix,
			fake::view_c auto _parameters,
			fake::view_c auto _suffix
		>
		struct info_t final{
			static constexpr auto entire = _entire;
			static constexpr auto core = _core;
			static constexpr auto prefix = _prefix;
			static constexpr auto parameters = _parameters;
			static constexpr auto suffix = _suffix;
		};
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		requires (fake::meta::array_c<decltype(_footprint)>)
		inline static consteval auto info() noexcept{
			using type_t = fake::bare_t<_Type>;
			constexpr auto local = fake::take_v<configure.at<[]{}>(fake::pack_v<key<_ConfigToken>>)>;
			if constexpr(local.template contains<[]{}>(fake::pack_v<type_t>)){
				constexpr auto handler = fake::take_v<local.template at<[]{}>(fake::pack_v<type_t>)>;
				constexpr fake::view_c auto origin = handler(fake::pack_v<_Type>);
				constexpr auto result = split(origin);
				return info_t<origin, type_name::core(result.prefix), result.prefix, result.infix, result.suffix>{};
			}
			else if constexpr(fake::array_c<type_t>){
				constexpr auto array = array_name<_ConfigToken, _footprint, _Type>::split();
				constexpr fake::view_c auto entire = array.prefix + array.suffix;
				constexpr fake::view_c auto origin = array.prefix;
				constexpr auto result = split(origin);
				return info_t<entire, type_name::core(result.prefix), result.prefix, result.infix, array.suffix>{};
			}
			else if constexpr(fake::is_template_v<type_t> || is_std_array_v<type_t>){
				using generic_t = fake::pattern_t<type_t>;
				if constexpr(local.template contains<[]{}>(fake::pack_v<generic_t>)){
					constexpr auto handler = fake::take_v<local.template at<[]{}>(fake::pack_v<generic_t>)>;
					constexpr fake::view_c auto origin = handler(fake::pack_v<_Type>);
					constexpr auto result = split(origin);
					return info_t<origin, type_name::core(result.prefix), result.prefix, result.infix, result.suffix>{};
				}
				else{
					constexpr fake::view_c auto origin = template_name<_ConfigToken, _footprint, _Type>::view();
					constexpr auto result = split(origin);
					return info_t<origin, type_name::core(result.prefix), result.prefix, result.infix, result.suffix>{};
				}
			}
			else{
				constexpr fake::view_c auto origin = fake::type_view(fake::pack_v<_Type>);
				constexpr auto result = split(origin);
				return info_t<origin, type_name::core(result.prefix), result.prefix, result.infix, result.suffix>{};
			}
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
			if constexpr(local.template contains<[]{}>(fake::pack_v<type_t>)){
				return fake::take_v<local.template at<[]{}>(fake::pack_v<type_t>)>(fake::pack_v<_Type>);
			}
			else if constexpr(fake::array_c<type_t>){
				return array_name<_ConfigToken, _footprint, _Type>::view();
			}
			else if constexpr(fake::is_template_v<type_t> || is_std_array_v<type_t>){
				using generic_t = fake::pattern_t<type_t>;
				if constexpr(local.template contains<[]{}>(fake::pack_v<generic_t>))
					return fake::take_v<local.template at<[]{}>(fake::pack_v<generic_t>)>(fake::pack_v<_Type>);
				else
					return template_name<_ConfigToken, _footprint, _Type>::view();
			}
			else{
				return fake::type_view(fake::pack_v<_Type>);
			}
		}
	};
	
	namespace detail::type_name
	{
		
		template<typename _ConfigToken, auto _footprint, typename _Type>
		concept invocable_c = requires{
			requires fake::meta::array_c<decltype(_footprint)>;
			custom::type_name::method<_ConfigToken, _footprint, _Type>();
			custom::type_name::info<_ConfigToken, _footprint, _Type>();
		};
		
		// currying broker. 
		template<typename _ConfigToken, auto _footprint>
		requires fake::meta::array_c<decltype(_footprint)>
		struct broker
		{
		public:
			template<typename _Type>
			requires invocable_c<_ConfigToken, _footprint, _Type>
			inline consteval auto info(fake::type_package<_Type>) const{
				return custom::type_name::info<_ConfigToken, _footprint, _Type>();
			}
			
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
	
	// gcc bug workaround utility. 
	template<auto _vest, typename _ConfigToken, auto _fp = custom::type_name::config::footprint<_ConfigToken>(_vest)>
	requires fake::meta::array_c<decltype(_fp)>
	inline constexpr auto type_name_w = fake::type_name<_ConfigToken, _fp>;
	
}

#include "pop.h"

#endif /*__FAKE_CPO_TYPENAME_H__*/ 
