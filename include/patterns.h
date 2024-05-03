#ifndef __FAKE_PATTERN_H__
#define __FAKE_PATTERN_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "push.h"

#include <optional>
#include <type_traits>

namespace fake::patterns
{
	
	namespace tool
	{
		
		template<typename _Type>
		concept plug_c = std::is_class_v<_Type> && std::is_reference_v<_Type> == false;
		
		template<typename _Type, typename... _Types>
		concept member_c = (std::same_as<_Type, _Types> || ...);
		
		template<typename, std::size_t>
		struct element{};
		
		template<typename _T, std::size_t _i>
		static consteval void select(element<_T, _i>) noexcept{}
		
		template<typename... _Types>
		struct unique;
		
		template<typename... _Types, std::size_t... _index>
		struct unique<std::index_sequence<_index...>, _Types...> : element<_Types, _index>...{};
		
		template<typename... _Types>
		concept unique_c = requires(tool::unique<std::index_sequence_for<_Types...>, _Types...> _e){
			(tool::select<_Types>(_e), ...);
		};
		
		enum struct phase_e : std::size_t{init, query, quit};
		
	}
	
	template<tool::plug_c... _Plugs>
	requires tool::unique_c<_Plugs...>
	struct singleton final{
	private:
		static constexpr std::size_t size = sizeof...(_Plugs);
		
		static constexpr bool noexcept_init = (std::is_nothrow_constructible_v<_Plugs> && ...);
		static constexpr bool noexcept_quit = (std::is_nothrow_destructible_v<_Plugs> && ...);
		
	public:
		template<tool::member_c<_Plugs...> _Plug>
		static _Plug& plug(){
			return singleton::get<_Plug, tool::phase_e::query>().value();
		}
		
		static constexpr auto instance(){[[maybe_unused]] const auto &dummy = self; return []{};}
		
		// for executable to inject into library. 
		static void inject(void (*_dynamic)(std::size_t, void*)) noexcept{
			std::array<void*, size> array{reinterpret_cast<void*>(&plug<_Plugs>())...};
			_dynamic(size, reinterpret_cast<void*>(&array));
		}
		
		// for dynamic library to reference to the singletons from host. 
		static void reference(std::size_t _size, void* _array) noexcept{
			const std::array<void*, size> &array = *reinterpret_cast<std::array<void*, size>*>(_array);
			[&array]<std::size_t... _index>(std::index_sequence<_index...>){
				(singleton::pointer(reinterpret_cast<_Plugs*>(array[_index])), ...);
			}(std::make_index_sequence<size>());
		}
		
	private:
		singleton() noexcept(noexcept_init){(singleton::get<_Plugs, tool::phase_e::init>(), ...);};
		~singleton() noexcept(noexcept_quit){char _{}; ((singleton::get<_Plugs, tool::phase_e::quit>(), _) = ...);};
		
		singleton(const singleton&) = delete;
		singleton& operator=(const singleton&) = delete;
		
	private:
		template<typename _Plug>
		static std::optional<_Plug>& storage(){static std::optional<_Plug> data; return data;}
		
		template<typename _Plug>
		static std::optional<_Plug*> pointer(_Plug* _ptr = nullptr){
			static std::optional<_Plug*> data = _ptr ? std::optional<_Plug*>{_ptr} : std::optional<_Plug*>{};
			return data;
		}
		
		template<typename _Plug, tool::phase_e _phase_e>
		static std::optional<_Plug>& get(){
			if constexpr(_phase_e == tool::phase_e::init){
				if constexpr(requires{_Plug{typename _Plug::token{}};})
					singleton::storage<_Plug>().emplace(typename _Plug::token{});
				else
					singleton::storage<_Plug>().emplace();
			}
			if constexpr(_phase_e == tool::phase_e::quit)
				singleton::storage<_Plug>().reset();
			return singleton::storage<_Plug>();
		}
		
	private:
		static const singleton& local(){static singleton data; return data;}
		
	private:
		static const singleton &self;
	};
	
	template<tool::plug_c... _Plugs>
	requires tool::unique_c<_Plugs...>
	const singleton<_Plugs...>& singleton<_Plugs...>::self = singleton::local();
	
	struct intact final{
		intact() = default;
		constexpr intact(const intact &_copy) = delete;
		constexpr inline intact(intact &&_move) noexcept{_move.valid_v = false;};
		intact& operator=(const intact &_copy) = delete;
		intact& operator=(intact &&_move) noexcept{_move.valid_v = false; return *this;};
		constexpr operator bool() const noexcept{return valid_v;}
		
	private:
		bool valid_v = true;
	};
	
}

#include "pop.h"

#endif /*__FAKE_PATTERN_H__*/ 
