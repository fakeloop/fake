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
	
	namespace detail
	{
		
		struct hash final{
			constexpr std::size_t operator()(const auto&) const noexcept{return 0;}
		};
		
	}
	
	namespace detail
	{
		
		template<typename _Element>
		static consteval auto vector_t() noexcept{
			using init_t = std::initializer_list<_Element>;
			using namespace std;
			if constexpr(requires{vector(init_t{});})
				return fake::gene_v<fake::template_info<decltype(vector(init_t{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using vector = typename decltype(detail::vector_t<_Element>())::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept vector_c = fake::trait_v<vector, _Type>;
	
	template<typename _Element>
	concept include_vector_c = container::null_c<vector<_Element>> == false;
	
	namespace detail
	{
		
		template<typename _Element>
		static consteval auto deque_t() noexcept{
			using init_t = std::initializer_list<_Element>;
			using namespace std;
			if constexpr(requires{deque(init_t{});})
				return fake::gene_v<fake::template_info<decltype(deque(init_t{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using deque = typename decltype(detail::deque_t<_Element>())::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept deque_c = fake::trait_v<deque, _Type>;
	
	template<typename _Element>
	concept include_deque_c = container::null_c<deque<_Element>> == false;
	
	namespace detail
	{
		
		template<typename _Element>
		static consteval auto forward_list_t() noexcept{
			using init_t = std::initializer_list<_Element>;
			using namespace std;
			if constexpr(requires{forward_list(init_t{});})
				return fake::gene_v<fake::template_info<decltype(forward_list(init_t{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using forward_list = typename decltype(detail::forward_list_t<_Element>())::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept forward_list_c = fake::trait_v<forward_list, _Type>;
	
	template<typename _Element>
	concept include_forward_list_c = container::null_c<forward_list<_Element>> == false;
	
	namespace detail
	{
		
		template<typename _Element>
		static consteval auto list_t() noexcept{
			using init_t = std::initializer_list<_Element>;
			using namespace std;
			if constexpr(requires{list(init_t{});})
				return fake::gene_v<fake::template_info<decltype(list(init_t{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using list = typename decltype(detail::list_t<_Element>())::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept list_c = fake::trait_v<list, _Type>;
	
	template<typename _Element>
	concept include_list_c = container::null_c<list<_Element>> == false;
	
	namespace detail
	{
		
		template<typename _Element>
		static consteval auto set_t() noexcept{
			using init_t = std::initializer_list<_Element>;
			using namespace std;
			if constexpr(requires{set(init_t{});})
				return fake::gene_v<fake::template_info<decltype(set(init_t{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using set = typename decltype(detail::set_t<_Element>())::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept set_c = fake::trait_v<set, _Type>;
	
	template<typename _Element>
	concept include_set_c = container::null_c<set<_Element>> == false;
	
	namespace detail
	{
		
		template<typename _Element>
		static consteval auto multiset_t() noexcept{
			using init_t = std::initializer_list<_Element>;
			using namespace std;
			if constexpr(requires{multiset(init_t{});})
				return fake::gene_v<fake::template_info<decltype(multiset(init_t{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using multiset = typename decltype(detail::multiset_t<_Element>())::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept multiset_c = fake::trait_v<multiset, _Type>;
	
	template<typename _Element>
	concept include_multiset_c = container::null_c<multiset<_Element>> == false;
	
	namespace detail
	{
		
		template<typename _Element>
		static consteval auto unordered_set_t() noexcept{
			using init_t = std::initializer_list<_Element>;
			using namespace std;
			if constexpr(requires{unordered_set(init_t{});})
				return fake::gene_v<fake::template_info<decltype(unordered_set(init_t{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using unordered_set = typename decltype(detail::unordered_set_t<_Element>())::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept unordered_set_c = fake::trait_v<unordered_set, _Type>;
	
	template<typename _Element>
	concept include_unordered_set_c = container::null_c<unordered_set<_Element>> == false;
	
	namespace detail
	{
		
		template<typename _Element>
		static consteval auto unordered_multiset_t() noexcept{
			using init_t = std::initializer_list<_Element>;
			using namespace std;
			if constexpr(requires{unordered_multiset(init_t{});})
				return fake::gene_v<fake::template_info<decltype(unordered_multiset(init_t{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Element, typename... _Args>
	using unordered_multiset = typename decltype(
			detail::unordered_multiset_t<_Element>()
		)::template type<_Element, _Args...>;
	
	template<typename _Type>
	concept unordered_multiset_c = fake::trait_v<unordered_multiset, _Type>;
	
	template<typename _Element>
	concept include_unordered_multiset_c = container::null_c<unordered_multiset<_Element>> == false;
	
	namespace detail
	{
		
		template<typename _Key, typename _Mapped>
		static consteval auto map_t() noexcept{
			using init_t = std::initializer_list<std::pair<const _Key, _Mapped>>;
			using namespace std;
			if constexpr(requires{map(init_t{});})
				return fake::gene_v<fake::template_info<decltype(map(init_t{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Key, typename _Mapped, typename... _Args>
	using map = typename decltype(detail::map_t<_Key, _Mapped>())::template type<_Key, _Mapped, _Args...>;
	
	template<typename _Type>
	concept map_c = fake::trait_v<map, _Type>;
	
	template<typename _Key, typename _Mapped>
	concept include_map_c = container::null_c<map<_Key, _Mapped>> == false;
	
	namespace detail
	{
		
		template<typename _Key, typename _Mapped>
		static consteval auto multimap_t() noexcept{
			using init_t = std::initializer_list<std::pair<const _Key, _Mapped>>;
			using namespace std;
			if constexpr(requires{multimap(init_t{});})
				return fake::gene_v<fake::template_info<decltype(multimap(init_t{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Key, typename _Mapped, typename... _Args>
	using multimap = typename decltype(detail::multimap_t<_Key, _Mapped>())::template type<_Key, _Mapped, _Args...>;
	
	template<typename _Type>
	concept multimap_c = fake::trait_v<multimap, _Type>;
	
	template<typename _Key, typename _Mapped>
	concept include_multimap_c = container::null_c<multimap<_Key, _Mapped>> == false;
	
	namespace detail
	{
		
		template<typename _Key, typename _Mapped>
		static consteval auto unordered_map_t() noexcept{
			using init_t = std::initializer_list<std::pair<const _Key, _Mapped>>;
			using namespace std;
			if constexpr(requires{unordered_map(init_t{}, 0, hash{});})
				return fake::gene_v<fake::template_info<decltype(unordered_map(init_t{}, 0, hash{}))>::template type>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Key, typename _Mapped, typename... _Args>
	using unordered_map = typename decltype(
			detail::unordered_map_t<_Key, _Mapped>()
		)::template type<_Key, _Mapped, _Args...>;
	
	template<typename _Type>
	concept unordered_map_c = fake::trait_v<unordered_map, _Type>;
	
	template<typename _Key, typename _Mapped>
	concept include_unordered_map_c = container::null_c<unordered_map<_Key, _Mapped>> == false;
	
	namespace detail
	{
		
		template<typename _Key, typename _Mapped>
		static consteval auto unordered_multimap_t() noexcept{
			using init_t = std::initializer_list<std::pair<const _Key, _Mapped>>;
			using namespace std;
			if constexpr(requires{unordered_multimap(init_t{}, 0, hash{});})
				return fake::gene_v<
					fake::template_info<decltype(unordered_multimap(init_t{}, 0, hash{}))>::template type
				>;
			else
				return fake::gene_v<fake::container::null_t>;
		}
		
	}
	
	template<typename _Key, typename _Mapped, typename... _Args>
	using unordered_multimap = typename decltype(
			detail::unordered_multimap_t<_Key, _Mapped>()
		)::template type<_Key, _Mapped, _Args...>;
	
	template<typename _Type>
	concept unordered_multimap_c = fake::trait_v<unordered_multimap, _Type>;
	
	template<typename _Key, typename _Mapped>
	concept include_unordered_multimap_c = container::null_c<unordered_multimap<_Key, _Mapped>> == false;
	
}

#endif /*__FAKE_CONTAINER_H__*/ 
