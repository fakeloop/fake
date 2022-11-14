#ifndef __FAKE_TYPE_MAP_H__
#define __FAKE_TYPE_MAP_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include <type_traits>
#include <tuple>

#include "is_valid.h"

namespace fake::counter
{
	
	namespace detail
	{
		
		using default_t = fake::sfinae<0>;
		using existed_t = fake::sfinae<1>;
		
		template<typename _Key, std::size_t _Index>
		struct flag
		{
			// self-contained poison-pill. 
			friend constexpr auto anonymous(flag<_Key, _Index>) noexcept;
		};
		
		template<typename _Key, std::size_t _Index>
		struct record
		{
			friend constexpr auto anonymous(flag<_Key, _Index>) noexcept{
				return std::size_t{};
			}
		};
		
		template<typename _Lambda, typename _Key, std::size_t _Index = std::size_t{}>
		constexpr std::size_t reader(default_t, flag<_Key, _Index> = {}) noexcept
		{
			return _Index;
		}
		
		template<
			typename _Lambda,
			typename _Key,
			std::size_t _Index = std::size_t{},
			std::size_t = anonymous(flag<_Key, _Index>{})
		>
		constexpr std::size_t reader(
			existed_t,
			flag<_Key, _Index> = {},
			std::size_t _Value = reader<_Lambda>(existed_t{}, flag<_Key, _Index + 1>{})
		) noexcept
		{
			return _Value;
		}
		
		template<
			typename _Key,
			typename _Lambda,
			std::size_t _Index = detail::reader<_Lambda, _Key>(existed_t{})
		>
		constexpr std::size_t current() noexcept
		{
			return _Index;
		}
		
		template<
			typename _Key,
			typename _Lambda,
			std::size_t _Index = detail::reader<_Lambda, _Key>(existed_t{}),
			typename = decltype(detail::record<_Key, _Index>{})
		>
		constexpr std::size_t increase() noexcept
		{
			return _Index;
		}
		
	}
	
	template<typename _Key, typename _Lambda>
	constexpr std::size_t current(_Lambda&&) noexcept
	{
		return detail::current<_Key, _Lambda>();
	}
	
	template<typename _Key, typename _Lambda>
	constexpr std::size_t increase(_Lambda&&) noexcept
	{
		return detail::increase<_Key, _Lambda>();
	}
	
}

namespace fake::type_map
{
	
	namespace detail
	{
		
		using default_t = fake::sfinae<0>;
		using existed_t = fake::sfinae<1>;
		
		template<typename _Key, std::size_t _Index>
		struct flag{
			// self-contained poison-pill. 
			friend constexpr auto anonymous(flag<_Key, _Index>) noexcept;
		};
		
		template<typename _Key, std::size_t _Index, typename _TypeName>
		struct record{
			friend constexpr auto anonymous(flag<_Key, _Index>) noexcept{
				return fake::pack_v<_TypeName>;
			}
		};
		
		template<typename _Lambda, typename _Key, std::size_t _Index>
		constexpr bool queryer(default_t) noexcept{
			return false;
		}
		
		template<typename _Lambda, typename _Key, std::size_t _Index>
		requires requires{anonymous(flag<_Key, _Index>{});}
		constexpr bool queryer(existed_t) noexcept{
			return true;
		}
		
		template<typename _Lambda, typename _Key, std::size_t _Index>
		constexpr auto reader(default_t) noexcept{
			return fake::pack_v<fake::null_t>;
		}
		
		template<typename _Lambda, typename _Key, std::size_t _Index>
		requires requires{anonymous(flag<_Key, _Index>{});}
		constexpr auto reader(existed_t) noexcept{
			return anonymous(flag<_Key, _Index>{});
		}
		
		template<typename _Lambda, typename _Key, std::size_t _Index = std::size_t{}>
		constexpr std::size_t counter(default_t, flag<_Key, _Index> = {}) noexcept{
			return _Index;
		}
		
		template<
			typename _Lambda,
			typename _Key,
			std::size_t _Index = std::size_t{},
			typename = decltype(anonymous(flag<_Key, _Index>{}))
		>
		constexpr std::size_t counter(
			existed_t,
			flag<_Key, _Index> = {},
			std::size_t _Value = counter<_Lambda>(existed_t{}, flag<_Key, _Index + 1>{})
		) noexcept{
			return _Value;
		}
		
		template<typename _Lambda, typename _Key, std::size_t _Index, typename _TypeName>
		constexpr auto writer(default_t) noexcept{
			return fake::pack_v<fake::null_t>;
		}
		
		template<typename _Lambda, typename _Key, std::size_t _Index, typename _TypeName>
		requires (queryer<_Lambda, _Key, _Index>(existed_t{}) == false)
		constexpr auto writer(existed_t) noexcept{
			sizeof(record<_Key, _Index, _TypeName>);
			return fake::pack_v<_TypeName>;
		}
		
		template<typename _Key, std::size_t _Index, typename _Lambda>
		requires requires{fake::type_map::detail::queryer<_Lambda, _Key, _Index>(existed_t{});}
		constexpr bool has() noexcept{
			return fake::type_map::detail::queryer<_Lambda, _Key, _Index>(existed_t{});
		}
		
		template<typename _Key, std::size_t _Index, typename _Lambda>
		requires requires{fake::type_map::detail::reader<_Lambda, _Key, _Index>(existed_t{});}
		constexpr auto read() noexcept{
			return fake::type_map::detail::reader<_Lambda, _Key, _Index>(existed_t{});
		}
		
		template<typename _Key, std::size_t _Index, typename _TypeName, typename _Lambda>
		requires requires{fake::type_map::detail::writer<_Lambda, _Key, _Index, _TypeName>(existed_t{});}
		constexpr auto write() noexcept{
			return fake::type_map::detail::writer<_Lambda, _Key, _Index, _TypeName>(existed_t{});
		}
		
		template<typename _Key, typename _Lambda>
		requires requires{fake::type_map::detail::counter<_Lambda, _Key>(existed_t{});}
		constexpr std::size_t size() noexcept{
			return fake::type_map::detail::counter<_Lambda, _Key>(existed_t{});
		}
		
	}
	
	template<typename _Key, std::size_t _Index>
	constexpr bool has(auto _lambda) noexcept{
		return fake::type_map::detail::has<_Key, _Index, decltype(_lambda)>();
	}
	
	template<typename _Key, std::size_t _Index>
	constexpr auto read(auto _lambda) noexcept{
		return fake::type_map::detail::read<_Key, _Index, decltype(_lambda)>();
	}
	
	template<typename _Key, std::size_t _Index, typename _TypeName>
	constexpr auto write(auto _lambda) noexcept{
		return fake::type_map::detail::write<_Key, _Index, _TypeName, decltype(_lambda)>();
	}
	
	template<typename _Key>
	constexpr std::size_t size(auto _lambda) noexcept{
		return fake::type_map::detail::size<_Key, decltype(_lambda)>();
	}
	
}

namespace fake::cpo
{
	
	// emplace trait. 
	struct override{};
	struct final{};
	
	// method policy. 
	struct relax{}; // forbid a registered 'cpo' with token 'special' to fail in an invocation. 
	struct strict{}; // allow a registered 'cpo' with token 'special' to fail in an invocation. 
	
	namespace detail
	{
		
		using default_t = fake::type_map::detail::default_t;
		using existed_t = fake::type_map::detail::existed_t;
		
		template<typename _Type, typename _Trait>
		struct storage_package{using type = _Type; using trait = _Trait;};
		
		template<typename _Token, typename _Policy, typename _Method, std::size_t _Index>
		constexpr auto target_implement() noexcept
		{
			constexpr auto valid_method = _Method{};
			if constexpr(_Index){
				using storage = typename decltype(
					fake::type_map::detail::reader<_Method, _Token, _Index - 1>(existed_t{})
				)::type;
				constexpr bool has_method = valid_method(storage{});
				static_assert(
					std::is_same_v<_Policy, relax> || std::is_same_v<storage, fake::null_t> || has_method,
					"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
					"\nerror<fake::cpo::method>:\n"
					"\tcondition 'strict': current method requirement policy is 'strict'.\n"
					"\tcondition 'method': associated 'cpo' for a special type is registered.\n"
					"\twhen both of 'strict' and 'method' are true, associated 'cpo' invocation failed.\n"
					"\t<ctrl + f> to find the token type associated with type 'special' in compiler outputs:\n"
					"\taka template parameter [_Token] in 'fake::cpo::detail::target_implement()'.\n"
					"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				);
				
				if constexpr(!(std::is_same_v<_Policy, relax> || std::is_same_v<storage, fake::null_t> || has_method)){
					struct type_t{};
					constexpr type_t type;
					storage aka = type;
				}
				
				if constexpr(has_method)
					return fake::pack_v<typename storage::type>;
				else
					return target_implement<_Token, _Policy, _Method, _Index - 1>();
			}
			else{
				return fake::pack_v<fake::null_t>;
			}
		}
		
		template<typename _Lambda, typename _Special, typename _General, typename _Policy, typename _Method>
		constexpr auto extract_implement() noexcept
		{
			constexpr std::size_t special_size = fake::type_map::detail::counter<_Lambda, _Special>(existed_t{});
			constexpr std::size_t generic_size = fake::type_map::detail::counter<_Lambda, _General>(existed_t{});
			
			using special_result = typename decltype(
				target_implement<_Special, _Policy, _Method, special_size>()
			)::type;
			
			if constexpr(std::is_same_v<special_result, fake::null_t>){
				using generic_result = typename decltype(
					target_implement<_General, relax, _Method, generic_size>()
				)::type;
				
				return fake::pack_v<generic_result>;
			}
			else{
				return fake::pack_v<special_result>;
			}
		}
		
		template<
			typename _Special,
			typename _General,
			typename _Policy,
			typename _Lambda,
			typename _Method = decltype(fake::is_valid(std::declval<_Lambda>())),
			typename _Result = decltype(extract_implement<_Lambda, _Special, _General, _Policy, _Method>())
		>
		constexpr auto method() noexcept
		{
			return _Result{};
		}
		
		template<typename _CurrentTrait>
		constexpr bool valid_override() noexcept
		{
			constexpr bool valid = std::is_same_v<_CurrentTrait, final> == false;
			static_assert(
				valid,
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				"\nerror<fake::cpo::emplace>: could NOT override a interface after 'final' implementation.\n"
				"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			);
			
			return true;
		}
		
		template<
			typename _Token,
			typename _TypeName,
			typename _Trait = override,
			typename _Lambda,
			std::size_t _Index = fake::type_map::detail::counter<_Lambda, _Token>(existed_t{}),
			typename _CurrentTrait = typename std::conditional_t<
				bool(_Index),
				typename decltype(
					fake::type_map::detail::reader<_Lambda, _Token, _Index - 1>(existed_t{})
				)::type,
				storage_package<fake::null_t, override>
			>::trait,
			typename _Size = std::enable_if_t<valid_override<_CurrentTrait>(), std::size_t>,
			typename _Result = decltype(
				fake::type_map::detail::writer<
					_Lambda,
					_Token,
					_Size{_Index},
					storage_package<_TypeName, _Trait>
				>(existed_t{})
			)
		>
		constexpr auto emplace() noexcept
		{
			return _Result{};
		}
		
		template<typename _Token, typename _Method, std::size_t _Index, bool _Exist = false>
		constexpr auto shrink_implement() noexcept
		{
			if constexpr(_Index){
				using storage = typename decltype(
					fake::type_map::detail::reader<_Method, _Token, _Index - 1>(existed_t{})
				)::type;
				constexpr bool has_method = _Method{}(storage{});
				static_assert(
					_Exist == false || has_method == false,
					"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
					"\nerror<fake::cpo::shrink>: each type could have at most ONE shrinker.\n"
					"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				);
				if constexpr(has_method && _Exist == false){
					shrink_implement<_Token, _Method, _Index - 1, true>();
					return fake::pack_v<typename storage::type>;
				}
				else{
					return shrink_implement<_Token, _Method, _Index - 1, _Exist>();
				}
			}
			else{
				return fake::pack_v<fake::null_t>;
			}
		}
		
		template<typename _Token, typename _Condition>
		constexpr auto shrink() noexcept
		{
			return decltype(
				shrink_implement<
					_Token,
					decltype(fake::is_valid(std::declval<_Condition>())),
					fake::type_map::detail::counter<_Condition, _Token>(existed_t{})
				>()
			){};
		}
		
		template<typename _Token, typename _Lambda, std::size_t _Index>
		constexpr auto assert_implement() noexcept
		{
			if constexpr(_Index){
				using storage = typename decltype(
					fake::type_map::detail::reader<_Lambda, _Token, _Index - 1>(existed_t{})
				)::type;
				
				constexpr auto assert = fake::is_valid(
					[](auto _p)->decltype(decltype(_p)::type::type::assert()){}
				);
				constexpr auto alert = fake::is_valid(
					[](auto _p)->decltype(decltype(_p)::type::type::alert(std::bool_constant<true>{})){}
				);
				constexpr bool has_assert = assert(fake::pack_v<storage>);
				constexpr bool has_alert = alert(fake::pack_v<storage>);
				
				static_assert(
					has_assert && has_alert,
					"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
					"\nerror<fake::cpo::assert>: for registered implementations:\n"
					"\tshould have 'impl::alert(impl::assert() == false)'.\n"
					"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
				);
				
				if constexpr(has_assert && has_alert){
					constexpr bool result = storage::type::assert();
					storage::type::alert(std::bool_constant<result>{});
					
					return result && assert_implement<_Token, _Lambda, _Index - 1>();
				}
				else{
					return false;
				}
			}
			else{
				return true;
			}
		}
		
		template<typename _Token, typename _Lambda>
		constexpr auto assert() noexcept
		{
			return assert_implement<
				_Token,
				_Lambda,
				fake::type_map::detail::counter<_Lambda, _Token>(existed_t{})
			>();
		}
		
	}
	
	template<typename _Special, typename _General, typename _Policy = strict, typename _Lambda>
	constexpr auto method(_Lambda &&) noexcept
	{
		static_assert(
			std::is_same_v<_Policy, relax> || std::is_same_v<_Policy, strict>,
			"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			"\nerror<fake::cpo::method()>:\n"
			"\ttemplate argument '_Policy' should be one of these types:\n"
			"\t'fake::cpo::relax' or 'fake::cpo::strict'.\n"
			"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
		);
		return detail::method<_Special, _General, _Policy, _Lambda>();
	}
	
	template<typename _Token, typename _TypeName, typename _Trait = override, typename _Lambda>
	constexpr auto emplace(_Lambda &&) noexcept
	{
		static_assert(
			std::is_same_v<_Trait, override> || std::is_same_v<_Trait, final>,
			"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			"\nerror<fake::cpo::emplace()>:\n"
			"\ttemplate argument '_Trait' should be one of these types:\n"
			"\t'fake::cpo::override' or 'fake::cpo::final'.\n"
			"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
		);
		return detail::emplace<_Token, _TypeName, _Trait, _Lambda>();
	}
	
	namespace custom
	{
		
		struct shrink
		{
			template<typename _Implement>
			static constexpr auto emplace(_Implement) noexcept{
				fake::cpo::emplace<target, _Implement>([]{});
			}
			
			template<typename _Package>
			constexpr auto operator()(_Package _package) const noexcept{
				constexpr auto cpo = shrink::method<target>(
					[](auto _p)->decltype(decltype(_p)::type::shrink(_package)){}
				);
				if constexpr(std::is_same_v<typename decltype(cpo)::type, fake::null_t>)
					return _package;
				else
					return decltype(cpo)::type::shrink(_package);
			}
			
		private:
			struct target{};
			
			template<typename _Target, typename _Condition>
			static constexpr auto method(_Condition&&) noexcept{
				return detail::shrink<_Target, _Condition>();
			}
		};
		
		template<typename _Type>
		struct assert
		{
			template<typename _Implement>
			static constexpr void emplace(_Implement) noexcept{
				fake::cpo::emplace<target, _Implement>([]{});
			}
			
			template<typename = void>
			constexpr auto operator()() const noexcept{
				return assert::method<target>([]{});
			}
			
		private:
			struct target{};
			
			template<typename _Target, typename _Condition>
			static constexpr auto method(_Condition&&) noexcept{
				return detail::assert<_Target, _Condition>();
			}
		};
		
	}
	
	constexpr custom::shrink shrink;
	template<typename _Type> constexpr custom::assert<_Type> assert;
	
}

namespace fake::meta::tool
{
	
	struct token{};
	
	template<typename _key, auto _lambda = []{}, typename _Lambda = decltype(_lambda)>
	constexpr auto refresh(token, _key) noexcept{return fake::pack_v<_Lambda>;}
	
}

#endif /*__FAKE_TYPE_MAP_H__*/ 
