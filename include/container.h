#ifndef __FAKE_CONTAINER_H__
#define __FAKE_CONTAINER_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "push.h"

#include <initializer_list>

#include "traits.h"

namespace fake
{
	
	namespace container
	{
		template<typename...>
		struct null_t{};
		
		template<typename _Type>
		concept null_c = fake::trait_v<null_t, std::remove_cvref_t<_Type>>;
		
	}
	
	namespace detail::container
	{
		
		template<typename _Hash, typename _Type>
		concept hash_c = requires(const _Type &_e, _Hash &_hash){{_hash(_e)} -> std::same_as<std::size_t>;};
		
	}
	
	namespace detail::container
	{
		
		template<typename _Element>
		static consteval auto vector_t() noexcept{
			if constexpr(std::same_as<_Element, void>){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::remove_cvref_t<_Element>>;
				using namespace std;
				if constexpr(requires{vector(init{});})
					return fake::gene_v<fake::template_info<decltype(vector(init{}))>::template type>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using vector = typename decltype(
		detail::container::vector_t<_Element>()
	)::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept vector_c = fake::trait_v<vector, std::remove_cvref_t<_Type>>;
	
	template<typename _Element>
	concept include_vector_c = container::null_c<vector<_Element>> == false;
	
	namespace detail::container
	{
		
		template<typename _Element>
		static consteval auto deque_t() noexcept{
			if constexpr(std::same_as<_Element, void>){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::remove_cvref_t<_Element>>;
				using namespace std;
				if constexpr(requires{deque(init{});})
					return fake::gene_v<fake::template_info<decltype(deque(init{}))>::template type>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using deque = typename decltype(
		detail::container::deque_t<_Element>()
	)::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept deque_c = fake::trait_v<deque, std::remove_cvref_t<_Type>>;
	
	template<typename _Element>
	concept include_deque_c = container::null_c<deque<_Element>> == false;
	
	namespace detail::container
	{
		
		template<typename _Element>
		static consteval auto forward_list_t() noexcept{
			if constexpr(std::same_as<_Element, void>){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::remove_cvref_t<_Element>>;
				using namespace std;
				if constexpr(requires{forward_list(init{});})
					return fake::gene_v<fake::template_info<decltype(forward_list(init{}))>::template type>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using forward_list = typename decltype(
		detail::container::forward_list_t<_Element>()
	)::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept forward_list_c = fake::trait_v<forward_list, std::remove_cvref_t<_Type>>;
	
	template<typename _Element>
	concept include_forward_list_c = container::null_c<forward_list<_Element>> == false;
	
	namespace detail::container
	{
		
		template<typename _Element>
		static consteval auto list_t() noexcept{
			if constexpr(std::same_as<_Element, void>){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::remove_cvref_t<_Element>>;
				using namespace std;
				if constexpr(requires{list(init{});})
					return fake::gene_v<fake::template_info<decltype(list(init{}))>::template type>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using list = typename decltype(
		detail::container::list_t<_Element>()
	)::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept list_c = fake::trait_v<list, std::remove_cvref_t<_Type>>;
	
	template<typename _Element>
	concept include_list_c = container::null_c<list<_Element>> == false;
	
	namespace detail::container
	{
		
		template<typename _Element>
		static consteval auto set_t() noexcept{
			if constexpr(std::same_as<_Element, void>){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::remove_cvref_t<_Element>>;
				using namespace std;
				if constexpr(requires{set(init{});})
					return fake::gene_v<fake::template_info<decltype(set(init{}))>::template type>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using set = typename decltype(
		detail::container::set_t<_Element>()
	)::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept set_c = fake::trait_v<set, std::remove_cvref_t<_Type>>;
	
	template<typename _Element>
	concept include_set_c = container::null_c<set<_Element>> == false;
	
	namespace detail::container
	{
		
		template<typename _Element>
		static consteval auto multiset_t() noexcept{
			if constexpr(std::same_as<_Element, void>){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::remove_cvref_t<_Element>>;
				using namespace std;
				if constexpr(requires{multiset(init{});})
					return fake::gene_v<fake::template_info<decltype(multiset(init{}))>::template type>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using multiset = typename decltype(
		detail::container::multiset_t<_Element>()
	)::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept multiset_c = fake::trait_v<multiset, std::remove_cvref_t<_Type>>;
	
	template<typename _Element>
	concept include_multiset_c = container::null_c<multiset<_Element>> == false;
	
	namespace detail::container
	{
		
		template<typename _Element, typename _Hash>
		static consteval auto unordered_set_t() noexcept{
			if constexpr(std::same_as<_Element, void> || detail::container::hash_c<_Hash, _Element> == false){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::remove_cvref_t<_Element>>;
				using namespace std;
				if constexpr(requires{unordered_set(init{}, 0, _Hash{});})
					return fake::gene_v<
						fake::template_info<decltype(unordered_set(init{}, 0, _Hash{}))>::template type
					>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Element, typename _Hash = std::hash<_Element>, typename... _Args>
	using unordered_set = typename decltype(
		detail::container::unordered_set_t<_Element, _Hash>()
	)::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept unordered_set_c = fake::trait_v<unordered_set, std::remove_cvref_t<_Type>>;
	
	template<typename _Element>
	concept include_unordered_set_c = container::null_c<unordered_set<_Element>> == false;
	
	namespace detail::container
	{
		
		template<typename _Element, typename _Hash>
		static consteval auto unordered_multiset_t() noexcept{
			if constexpr(std::same_as<_Element, void> || detail::container::hash_c<_Hash, _Element> == false){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::remove_cvref_t<_Element>>;
				using namespace std;
				if constexpr(requires{unordered_multiset(init{}, 0, _Hash{});})
					return fake::gene_v<
						fake::template_info<decltype(unordered_multiset(init{}, 0, _Hash{}))>::template type
					>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Element, typename _Hash = std::hash<_Element>, typename... _Args>
	using unordered_multiset = typename decltype(
		detail::container::unordered_multiset_t<_Element, _Hash>()
	)::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept unordered_multiset_c = fake::trait_v<unordered_multiset, std::remove_cvref_t<_Type>>;
	
	template<typename _Element>
	concept include_unordered_multiset_c = container::null_c<unordered_multiset<_Element>> == false;
	
	namespace detail::container
	{
		
		template<typename _Key, typename _Mapped>
		static consteval auto map_t() noexcept{
			if constexpr(std::same_as<_Key, void> || std::same_as<_Mapped, void>){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::pair<const _Key, _Mapped>>;
				using namespace std;
				if constexpr(requires{map(init{});})
					return fake::gene_v<fake::template_info<decltype(map(init{}))>::template type>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Key, typename _Mapped, typename... _Args>
	using map = typename decltype(
		detail::container::map_t<_Key, _Mapped>()
	)::template type<_Key, _Mapped, _Args...>;
	
	template<typename _Type>
	concept map_c = fake::trait_v<map, std::remove_cvref_t<_Type>>;
	
	template<typename _Key, typename _Mapped>
	concept include_map_c = container::null_c<map<_Key, _Mapped>> == false;
	
	namespace detail::container
	{
		
		template<typename _Key, typename _Mapped>
		static consteval auto multimap_t() noexcept{
			if constexpr(std::same_as<_Key, void> || std::same_as<_Mapped, void>){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::pair<const _Key, _Mapped>>;
				using namespace std;
				if constexpr(requires{multimap(init{});})
					return fake::gene_v<fake::template_info<decltype(multimap(init{}))>::template type>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Key, typename _Mapped, typename... _Args>
	using multimap = typename decltype(
		detail::container::multimap_t<_Key, _Mapped>()
	)::template type<_Key, _Mapped, _Args...>;
	
	template<typename _Type>
	concept multimap_c = fake::trait_v<multimap, std::remove_cvref_t<_Type>>;
	
	template<typename _Key, typename _Mapped>
	concept include_multimap_c = container::null_c<multimap<_Key, _Mapped>> == false;
	
	namespace detail::container
	{
		
		template<typename _Key, typename _Mapped, typename _Hash>
		static consteval auto unordered_map_t() noexcept{
			if constexpr(
				std::same_as<_Key, void> ||
				std::same_as<_Mapped, void> ||
				detail::container::hash_c<_Hash, _Key> == false
			){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::pair<const _Key, _Mapped>>;
				using namespace std;
				if constexpr(requires{unordered_map(init{}, 0, _Hash{});})
					return fake::gene_v<
						fake::template_info<decltype(unordered_map(init{}, 0, _Hash{}))>::template type
					>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Key, typename _Mapped, typename _Hash = std::hash<_Key>, typename... _Args>
	using unordered_map = typename decltype(
		detail::container::unordered_map_t<_Key, _Mapped, _Hash>()
	)::template type<_Key, _Mapped, _Args...>;
	
	template<typename _Type>
	concept unordered_map_c = fake::trait_v<unordered_map, std::remove_cvref_t<_Type>>;
	
	template<typename _Key, typename _Mapped>
	concept include_unordered_map_c = container::null_c<unordered_map<_Key, _Mapped>> == false;
	
	namespace detail::container
	{
		
		template<typename _Key, typename _Mapped, typename _Hash>
		static consteval auto unordered_multimap_t() noexcept{
			if constexpr(
				std::same_as<_Key, void> ||
				std::same_as<_Mapped, void> ||
				detail::container::hash_c<_Hash, _Key> == false
			){
				return fake::gene_v<fake::container::null_t>;
			}
			else{
				using init = std::initializer_list<std::pair<const _Key, _Mapped>>;
				using namespace std;
				if constexpr(requires{unordered_multimap(init{}, 0, _Hash{});})
					return fake::gene_v<
						fake::template_info<decltype(unordered_multimap(init{}, 0, _Hash{}))>::template type
					>;
				else
					return fake::gene_v<fake::container::null_t>;
			}
		}
		
	}
	
	template<typename _Key, typename _Mapped, typename _Hash = std::hash<_Key>, typename... _Args>
	using unordered_multimap = typename decltype(
		detail::container::unordered_multimap_t<_Key, _Mapped, _Hash>()
	)::template type<_Key, _Mapped, _Args...>;
	
	template<typename _Type>
	concept unordered_multimap_c = fake::trait_v<unordered_multimap, std::remove_cvref_t<_Type>>;
	
	template<typename _Key, typename _Mapped>
	concept include_unordered_multimap_c = container::null_c<unordered_multimap<_Key, _Mapped>> == false;
	
}

namespace fake
{
	
	template<typename _Type>
	concept std_container_c = fake::vector_c<_Type> ||
		fake::deque_c<_Type> ||
		fake::forward_list_c<_Type> ||
		fake::list_c<_Type> ||
		fake::set_c<_Type> ||
		fake::multiset_c<_Type> ||
		fake::unordered_set_c<_Type> ||
		fake::unordered_multiset_c<_Type> ||
		fake::map_c<_Type> ||
		fake::multimap_c<_Type> ||
		fake::unordered_map_c<_Type> ||
		fake::unordered_multimap_c<_Type>;
	
}

#include "pop.h"

#endif /*__FAKE_CONTAINER_H__*/ 
