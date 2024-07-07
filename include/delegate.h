#ifndef __FAKE_DELEGATE_H__
#define __FAKE_DELEGATE_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "push.h"

#include <any>
#include <bit>
#include <functional>
#include <unordered_map>
#include <set>
#include <vector>

#include "functor_info.h"
#include "tuple.h"
#include "utility.h"

namespace fake
{
	
	namespace detail::delegate
	{
		
		template<typename>
		struct index final{static std::size_t current;};
		
		template<typename _Type>
		std::size_t index<_Type>::current = 0;
		
	}
	
	template<typename>
	struct delegate;
	
	template<typename _Result, typename... _Args>
	struct delegate<_Result(_Args...)>{
		using function_t = std::function<_Result(_Args...)>;
		using result_type = typename function_t::result_type;
		
		template<std::convertible_to<function_t> _Functor>
		std::size_t operator=(_Functor &&_functor){
			delegates.emplace(detail::delegate::index<void>::current, std::forward<_Functor>(_functor));
			return detail::delegate::index<void>::current++;
		}
		
		std::size_t size() const{
			return delegates.size();
		}
		
		template<typename... _Params>
		std::size_t amount() const{
			if constexpr(std::invocable<function_t, _Params...>)
				return size();
			else
				return 0;
		}
		
		std::size_t amount(const auto &..._args) const{
			return amount<decltype(_args)...>();
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		requires (std::same_as<_Retn, void> == false)
		std::vector<std::pair<std::size_t, std::optional<result_type>>> cast(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			std::vector<std::pair<std::size_t, std::optional<result_type>>> retn;
			retn.reserve(delegates.size());
			
			for(const auto &[id, functor] : delegates)
				retn.emplace_back(id, functor(_args...));
			
			return retn;
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		requires std::same_as<_Retn, void>
		void cast(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			for(const auto &[id, functor] : delegates)
				functor(_args...);
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		requires (std::same_as<_Retn, void> == false)
		std::vector<std::pair<std::size_t, result_type>> pick(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			std::vector<std::pair<std::size_t, result_type>> retn;
			retn.reserve(delegates.size());
			
			for(const auto &[id, functor] : delegates)
				retn.emplace_back(id, functor(_args...));
			
			return retn;
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		requires std::same_as<_Retn, void>
		void pick(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			for(const auto &[id, functor] : delegates)
				functor(_args...);
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		result_type mono(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			if(delegates.size() != 0x1)
				throw std::runtime_error{(fake::view_v<"fake::delegate::mono(): delegate is NOT unique.">).data()};
			
			return delegates.begin()->second(std::forward<decltype(_args)>(_args)...);
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		result_type rash(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			if(delegates.size() != 0x1)
				throw std::runtime_error{(fake::view_v<"fake::delegate::rash(): delegate is NOT unique.">).data()};
			
			return delegates.begin()->second(std::forward<decltype(_args)>(_args)...);
		}
		
		bool operator()(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			if(delegates.empty())
				return false;
			
			for(const auto &[id, functor] : delegates)
				functor(_args...);
			
			return true;
		}
		
		std::size_t erase(std::size_t _index){return delegates.erase(_index);}
		
		operator bool() const{return delegates.size();}
		
	private:
		std::unordered_map<std::size_t, function_t> delegates;
	};
	
	template<std::same_as<void> _Type>
	struct delegate<_Type>{
		struct function_t final{
			std::function<std::any(const std::any&)> function;
			fake::signet_t type;
			fake::signet_t result;
		};
		
		template<fake::tuple_c>
		struct valid final : std::false_type{};
		
		template<typename... _Types>
		requires (
			(
				(std::is_lvalue_reference_v<_Types> && std::is_const_v<std::remove_reference_t<_Types>>) ||
				std::is_reference_v<_Types> == false
			) && ...
		)
		struct valid<std::tuple<_Types...>> final : std::true_type{};
		
		template<fake::functor_c _Function>
		requires valid<typename fake::functor_info<std::remove_cvref_t<_Function>>::tuple>::value
		std::size_t operator=(_Function &&_functor){
			using info_t = fake::functor_info<std::remove_cvref_t<_Function>>;
			using retn_t = std::remove_cvref_t<typename info_t::retn>;
			using args_t = fake::tuple::transform_t<
				typename info_t::tuple,
				[](fake::pack_c auto _e){
					using type = std::remove_cvref_t<fake::take_t<decltype(_e){}>>;
					return fake::pack_v<std::add_lvalue_reference_t<std::add_const_t<type>>>;
				}
			>;
			using bare_t = fake::tuple::transform_t<
				typename info_t::tuple,
				[](fake::pack_c auto _e){return fake::pack_v<std::remove_cvref_t<fake::take_t<decltype(_e){}>>>;}
			>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<retn_t>);
			
			delegates.emplace(
				detail::delegate::index<void>::current,
				function_t{
					[f = std::forward<_Function>(_functor)](const std::any &_args) -> std::any {
						if constexpr(std::same_as<retn_t, void>){
							std::apply(f, std::any_cast<const args_t&>(_args));
							return {};
						}
						else{
							return std::apply(f, std::any_cast<const args_t&>(_args));
						}
					},
					fake::signet_v<arguments>,
					fake::signet_v<result>
				}
			);
			parameters[fake::signet_v<arguments>].emplace(detail::delegate::index<void>::current);
			
			return detail::delegate::index<void>::current++;
		}
		
		std::size_t size() const{
			return delegates.size();
		}
		
		template<typename... _Params>
		std::size_t amount() const{
			using bare_t = std::tuple<std::remove_cvref_t<_Params>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			
			if(parameters.contains(fake::signet_v<arguments>) == false)
				return 0x0;
			
			return parameters.at(fake::signet_v<arguments>).size();
		}
		
		std::size_t amount(const auto &..._args) const{
			return amount<decltype(_args)...>();
		}
		
		template<typename _Retn>
		requires (std::same_as<_Retn, void> == false) && std::same_as<std::remove_cvref_t<_Retn>, _Retn>
		std::vector<std::pair<std::size_t, std::optional<_Retn>>> cast(const auto &..._args) const{
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			
			if(parameters.contains(fake::signet_v<arguments>) == false)
				return {};
			
			const auto &broadcast = parameters.at(fake::signet_v<arguments>);
			
			std::vector<std::pair<std::size_t, std::optional<_Retn>>> retn;
			retn.reserve(broadcast.size());
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const std::size_t e : broadcast){
				const args_t refs = std::tie(_args...);
				const auto &delegate = delegates.at(e);
				
				if(delegate.result == signature)
					retn.emplace_back(e, std::any_cast<_Retn>(delegate.function(std::make_any<const args_t&>(refs))));
				else
					delegate.function(std::make_any<const args_t&>(refs)), retn.emplace_back(e, std::nullopt);
			}
			
			return retn;
		}
		
		template<std::same_as<void> _Retn>
		std::vector<std::pair<std::size_t, bool>> cast(const auto &..._args) const{
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			
			if(parameters.contains(fake::signet_v<arguments>) == false)
				return {};
			
			const auto &broadcast = parameters.at(fake::signet_v<arguments>);
			
			std::vector<std::pair<std::size_t, bool>> retn;
			retn.reserve(broadcast.size());
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const std::size_t e : broadcast){
				const args_t refs = std::tie(_args...);
				const auto &delegate = delegates.at(e);
				
				delegate.function(std::make_any<const args_t&>(refs));
				retn.emplace_back(e, delegate.result == signature);
			}
			
			return retn;
		}
		
		template<typename _Retn>
		requires (std::same_as<_Retn, void> == false) && std::same_as<std::remove_cvref_t<_Retn>, _Retn>
		std::vector<std::pair<std::size_t, _Retn>> pick(const auto &..._args) const{
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			
			if(parameters.contains(fake::signet_v<arguments>) == false)
				return {};
			
			const auto &broadcast = parameters.at(fake::signet_v<arguments>);
			
			std::vector<std::pair<std::size_t, _Retn>> retn;
			
			std::size_t reserve = 0;
			for(const fake::signet_t signature{fake::signet_v<result>}; const std::size_t e : broadcast)
				if(delegates.at(e).result == signature)
					reserve++;
			
			retn.reserve(reserve);
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const std::size_t e : broadcast){
				const args_t refs = std::tie(_args...);
				const auto &delegate = delegates.at(e);
				
				if(delegate.result == signature)
					retn.emplace_back(e, std::any_cast<_Retn>(delegate.function(std::make_any<const args_t&>(refs))));
			}
			
			return retn;
		}
		
		template<std::same_as<void> _Retn>
		std::vector<std::size_t> pick(const auto &..._args) const{
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<bool>);
			
			if(parameters.contains(fake::signet_v<arguments>) == false)
				return {};
			
			const auto &broadcast = parameters.at(fake::signet_v<arguments>);
			
			std::vector<std::size_t> retn;
			
			std::size_t reserve = 0;
			for(const fake::signet_t signature{fake::signet_v<result>}; const std::size_t e : broadcast)
				if(delegates.at(e).result == signature)
					reserve++;
			
			retn.reserve(reserve);
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const std::size_t e : broadcast){
				const args_t refs = std::tie(_args...);
				const auto &delegate = delegates.at(e);
				
				if(delegate.result == signature)
					delegate.function(std::make_any<const args_t&>(refs)), retn.emplace_back(e);
			}
			
			return retn;
		}
		
		template<typename _Retn>
		requires std::constructible_from<_Retn> && std::same_as<std::remove_cvref_t<_Retn>, _Retn> ||
			std::same_as<_Retn, void>
		_Retn mono(const auto &..._args) const{
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			constexpr fake::view_c auto error = fake::view_v<"fake::delegate::mono<"> + result + fake::view_v<">()">;
			
			if(parameters.contains(fake::signet_v<arguments>) == false){
				if constexpr(std::same_as<_Retn, void>)
					return;
				else
					return {};
			}
			
			const auto &broadcast = parameters.at(fake::signet_v<arguments>);
			
			std::size_t reserve = 0;
			for(const fake::signet_t signature{fake::signet_v<result>}; const std::size_t e : broadcast)
				if(delegates.at(e).result == signature)
					reserve++;
			
			if(reserve != 0x1)
				throw std::runtime_error{(error + fake::view_v<": delegate is NOT unique.">).data()};
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const std::size_t e : broadcast){
				const args_t refs = std::tie(_args...);
				const auto &delegate = delegates.at(e);
				
				if(delegate.result == signature){
					if constexpr(std::same_as<_Retn, void>)
						return void(delegate.function(std::make_any<const args_t&>(refs)));
					else
						return std::any_cast<_Retn>(delegate.function(std::make_any<const args_t&>(refs)));
				}
			}
			
			throw std::runtime_error{(error + fake::view_v<": logic error unreachable.">).data()};
		}
		
		template<typename _Retn>
		requires std::same_as<std::remove_cvref_t<_Retn>, _Retn>
		_Retn rash(const auto &..._args) const{
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			constexpr fake::view_c auto error = fake::view_v<"fake::delegate::rash<"> + result + fake::view_v<">()">;
			
			if(parameters.contains(fake::signet_v<arguments>) == false)
				throw std::runtime_error{(error + fake::view_v<": delegate does NOT exist.">).data()};;
			
			const auto &broadcast = parameters.at(fake::signet_v<arguments>);
			
			std::size_t reserve = 0;
			for(const fake::signet_t signature{fake::signet_v<result>}; const std::size_t e : broadcast)
				if(delegates.at(e).result == signature)
					reserve++;
			
			if(reserve != 0x1)
				throw std::runtime_error{(error + fake::view_v<": delegate is NOT unique.">).data()};
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const std::size_t e : broadcast){
				const args_t refs = std::tie(_args...);
				const auto &delegate = delegates.at(e);
				
				if(delegate.result == signature){
					if constexpr(std::same_as<_Retn, void>)
						return void(delegate.function(std::make_any<const args_t&>(refs)));
					else
						return std::any_cast<_Retn>(delegate.function(std::make_any<const args_t&>(refs)));
				}
			}
			
			throw std::runtime_error{(error + fake::view_v<": logic error unreachable.">).data()};
		}
		
		bool operator()(const auto &..._args) const{
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			
			if(parameters.contains(fake::signet_v<arguments>) == false)
				return false;
			
			for(const std::size_t e : parameters.at(fake::signet_v<arguments>)){
				const args_t refs = std::tie(_args...);
				delegates.at(e).function(std::make_any<const args_t&>(refs));
			}
			
			return true;
		}
		
		std::size_t erase(std::size_t _index){
			if(delegates.contains(_index) == false)
				return 0;
			
			const fake::signet_t type = delegates[_index].type;
			
			parameters[type].erase(_index);
			if(parameters[type].empty())
				parameters.erase(type);
			
			return delegates.erase(_index);
		}
		
		operator bool() const{return delegates.size();}
		
	private:
		std::unordered_map<std::size_t, function_t> delegates;
		std::unordered_map<fake::signet_t, std::set<std::size_t>> parameters;
	};
	
}

#include "pop.h"

#endif /*__FAKE_DELEGATE_H__*/ 
