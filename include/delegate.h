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
#include <memory>
#include <functional>
#include <unordered_map>
#include <set>
#include <vector>

#include "functor_info.h"
#include "detect.h"
#include "tuple.h"
#include "utility.h"

namespace fake
{
	
	namespace tool::delegate
	{
		
		struct shared_ptr final{
			template<typename _Delegate> using type = std::shared_ptr<_Delegate>;
		};
		
		struct pointer final{
			template<typename _Delegate> using type = std::add_pointer_t<_Delegate>;
		};
		
		struct shared_ptrs final{
			template<typename _Delegate> using type = std::vector<std::shared_ptr<_Delegate>>;
		};
		
		struct pointers final{
			template<typename _Delegate> using type = std::vector<std::add_pointer_t<_Delegate>>;
		};
		
	}
	
	namespace detail::delegate
	{
		
		template<typename>
		struct index final{static std::size_t current;};
		
		template<typename _Type>
		std::size_t index<_Type>::current = 0x1;
		
		template<typename _Type>
		concept policy_c = requires(_Type &_e){{_e.policy} -> fake::like<fake::view_t<"fake::delegate::policy">>;};
		
		template<typename _Type>
		concept object_c = requires{
			requires policy_c<_Type> == false;
			requires std::same_as<_Type, std::remove_cvref_t<_Type>>;
		} || requires{
			requires policy_c<_Type>;
			requires std::same_as<typename _Type::object_type, std::remove_cvref_t<typename _Type::object_type>>;
		};
		
		template<typename _Type>
		concept parent_c = std::same_as<typename _Type::transform_type, fake::tool::delegate::shared_ptr> ||
			std::same_as<typename _Type::transform_type, fake::tool::delegate::pointer> ||
			std::same_as<typename _Type::transform_type, fake::tool::delegate::shared_ptrs> ||
			std::same_as<typename _Type::transform_type, fake::tool::delegate::pointers>;
		
		inline constexpr fake::creed_c auto policy_d = fake::just_d<[]<policy_c>(){}>;
		inline constexpr fake::creed_c auto object_d = fake::just_d<[]<object_c>(){}>;
		inline constexpr fake::creed_c auto parent_d = fake::just_d<[]<parent_c>(){}>;
		
		template<typename _Type, fake::creed_c auto _creed>
		concept mix_c = _creed(fake::pazz_v<_Type>);
		
		template<typename _Object>
		requires std::same_as<_Object, std::remove_cvref_t<_Object>>
		struct reference{
		public:
			reference(std::nullopt_t _nullopt) noexcept: host{_nullopt}{}
			reference(_Object* _host) noexcept: host{*_host}{}
			reference(_Object &_host) noexcept: host{_host}{}
			
		public:
			std::optional<std::reference_wrapper<_Object>> host;
		};
		
		template<parent_c _Policy, typename _Delegate>
		requires std::same_as<_Delegate, std::remove_cvref_t<_Delegate>>
		struct resource{
		private:
			static constexpr bool shared = std::same_as<typename _Policy::transform_type, tool::delegate::shared_ptr>;
			static constexpr bool pointer = std::same_as<typename _Policy::transform_type, tool::delegate::pointer>;
			static constexpr bool shareds = std::same_as<typename _Policy::transform_type, tool::delegate::shared_ptrs>;
			static constexpr bool pointers = std::same_as<typename _Policy::transform_type, tool::delegate::pointers>;
			
			using storage_type = typename _Policy::transform_type::template type<const _Delegate>;
			
		private:
			struct dummy final{using value_type = fake::null_t;};
			
			using shared_argument = typename std::conditional_t<shareds, storage_type, dummy>::value_type;
			
		public:
			static constexpr bool multiple = shareds || pointers;
			
		public:
			resource() noexcept requires multiple{}
			resource() noexcept requires (multiple == false): source{nullptr}{}
			explicit resource(storage_type _e) noexcept requires shared: source{std::move(_e)}{}
			explicit resource(shared_argument _e) requires shareds: source{std::move(_e)}{}
			resource(storage_type _e) noexcept requires multiple: source{std::move(_e)}{}
			resource(const _Delegate* _e) noexcept requires pointer: source{_e}{}
			resource(const _Delegate* _e) requires pointers: source{_e}{}
			resource(const _Delegate &_e) noexcept requires pointer: source{std::addressof(_e)}{}
			resource(const _Delegate &_e) requires pointers: source{std::addressof(_e)}{}
			
		public:
			storage_type source;
		};
		
		// base. 
		template<typename, typename, typename>
		struct base;
		
		// [non-policy] [non-hosted] [non-parent] [_Retn(_Args...)]. 
		template<typename _Delegate, typename _Result, typename... _Args>
		struct base<_Delegate, void, _Result(_Args...)>{
			static constexpr bool policy = false;
			static constexpr bool object = false;
			static constexpr bool parent = false;
			
			using function_t = std::function<_Result(_Args...)>;
			using function_ref_t = function_t;
			using function_cref_t = function_t;
			using storage_t = function_t;
		};
		
		// [policy] [hosted] [parent] [_Retn(_Args...)]. 
		template<
			typename _Delegate,
			mix_c<policy_d && object_d && parent_d> _Policy,
			typename _Result,
			typename... _Args
		>
		struct base<_Delegate, _Policy, _Result(_Args...)> :
			public reference<typename _Policy::object_type>, public resource<_Policy, _Delegate>{
		private:
			using reference_type = reference<typename _Policy::object_type>;
			using parent_type = resource<_Policy, _Delegate>;
			using object_type = typename _Policy::object_type;
			
		public:
			static constexpr bool policy = true;
			static constexpr bool object = true;
			static constexpr bool parent = true;
			
			using function_t = std::function<_Result(_Args...)>;
			using function_ref_t = std::function<_Result(object_type&, _Args...)>;
			using function_cref_t = std::function<_Result(const object_type&, _Args...)>;
			using storage_t = std::variant<function_t, function_ref_t, function_cref_t>;
			
		public:
			base(auto &&_reference) noexcept
			requires std::constructible_from<reference_type, decltype(_reference)>:
			reference_type{std::forward<decltype(_reference)>(_reference)},
			parent_type{}
			{}
			
			base(auto &&_reference, auto &&_parent)
			noexcept(std::is_nothrow_constructible_v<parent_type, decltype(_parent)>)
			requires
				std::constructible_from<reference_type, decltype(_reference)> &&
				std::constructible_from<parent_type, decltype(_parent)>:
			reference_type{std::forward<decltype(_reference)>(_reference)},
			parent_type{std::forward<decltype(_parent)>(_parent)}
			{}
		};
		
		// [policy] [hosted] [non-parent] [_Retn(_Args...)]. 
		template<
			typename _Delegate, mix_c<policy_d && object_d && !parent_d> _Policy,
			typename _Result,
			typename... _Args
		>
		struct base<_Delegate, _Policy, _Result(_Args...)> :
			public reference<typename _Policy::object_type>{
		private:
			using reference_type = reference<typename _Policy::object_type>;
			using object_type = typename _Policy::object_type;
			
		public:
			static constexpr bool policy = true;
			static constexpr bool object = true;
			static constexpr bool parent = false;
			
			using function_t = std::function<_Result(_Args...)>;
			using function_ref_t = std::function<_Result(object_type&, _Args...)>;
			using function_cref_t = std::function<_Result(const object_type&, _Args...)>;
			using storage_t = std::variant<function_t, function_ref_t, function_cref_t>;
			
		public:
			using reference_type::reference_type;
		};
		
		// [policy] [non-hosted] [parent] [_Retn(_Args...)]. 
		template<
			typename _Delegate, mix_c<policy_d && !object_d && parent_d> _Policy,
			typename _Result,
			typename... _Args
		>
		struct base<_Delegate, _Policy, _Result(_Args...)> :
			public resource<_Policy, _Delegate>{
		private:
			using parent_type = resource<_Policy, _Delegate>;
			
		public:
			static constexpr bool policy = true;
			static constexpr bool object = false;
			static constexpr bool parent = true;
			
			using function_t = std::function<_Result(_Args...)>;
			using function_ref_t = function_t;
			using function_cref_t = function_t;
			using storage_t = function_t;
			
		public:
			using parent_type::parent_type;
		};
		
		// [policy] [non-hosted] [non-parent] [_Retn(_Args...)]. 
		template<
			typename _Delegate, mix_c<policy_d && !object_d && !parent_d> _Policy,
			typename _Result,
			typename... _Args
		>
		struct base<_Delegate, _Policy, _Result(_Args...)>{
			static constexpr bool policy = true;
			static constexpr bool object = false;
			static constexpr bool parent = false;
			
			using function_t = std::function<_Result(_Args...)>;
			using function_ref_t = function_t;
			using function_cref_t = function_t;
			using storage_t = function_t;
		};
		
		// [non-policy] [hosted] [non-parent] [_Retn(_Args...)]. 
		template<typename _Delegate, mix_c<!policy_d && object_d> _Object, typename _Result, typename... _Args>
		struct base<_Delegate, _Object, _Result(_Args...)> :
			public reference<_Object>{
		private:
			using reference_type = reference<_Object>;
			
		public:
			static constexpr bool policy = false;
			static constexpr bool object = true;
			static constexpr bool parent = false;
			
			using function_t = std::function<_Result(_Args...)>;
			using function_ref_t = std::function<_Result(_Object&, _Args...)>;
			using function_cref_t = std::function<_Result(const _Object&, _Args...)>;
			using storage_t = std::variant<function_t, function_ref_t, function_cref_t>;
			
		public:
			using reference_type::reference_type;
		};
		
		// [non-policy] [non-hosted] [non-parent] [complie-time signature]. 
		template<typename _Delegate>
		struct base<_Delegate, void, void>{
			static constexpr bool policy = false;
			static constexpr bool object = false;
			static constexpr bool parent = false;
			
			using object_type = void;
			using function_t = std::function<std::any(const std::any&)>;
			using function_ref_t = function_t;
			using function_cref_t = function_t;
			using storage_t = function_t;
		};
		
		// [policy] [hosted] [parent] [complie-time signature]. 
		template<typename _Delegate, mix_c<policy_d && object_d && parent_d> _Policy>
		struct base<_Delegate, _Policy, void> :
			public reference<typename _Policy::object_type>, public resource<_Policy, _Delegate>{
		private:
			using reference_type = reference<typename _Policy::object_type>;
			using parent_type = resource<_Policy, _Delegate>;
			
		public:
			static constexpr bool policy = true;
			static constexpr bool object = true;
			static constexpr bool parent = true;
			
			using object_type = typename _Policy::object_type;
			using function_t = std::function<std::any(const std::any&)>;
			using function_ref_t = std::function<std::any(object_type&, const std::any&)>;
			using function_cref_t = std::function<std::any(const object_type&, const std::any&)>;
			using storage_t = std::variant<function_t, function_ref_t, function_cref_t>;
			
		public:
			base(auto &&_reference) noexcept
			requires std::constructible_from<reference_type, decltype(_reference)>:
			reference_type{std::forward<decltype(_reference)>(_reference)},
			parent_type{}
			{};
			
			base(auto &&_reference, auto &&_parent)
			noexcept(std::is_nothrow_constructible_v<parent_type, decltype(_parent)>)
			requires
				std::constructible_from<reference_type, decltype(_reference)> &&
				std::constructible_from<parent_type, decltype(_parent)>:
			reference_type{std::forward<decltype(_reference)>(_reference)},
			parent_type{std::forward<decltype(_parent)>(_parent)}
			{}
		};
		
		// [policy] [hosted] [non-parent] [complie-time signature]. 
		template<typename _Delegate, mix_c<policy_d && object_d && !parent_d> _Policy>
		struct base<_Delegate, _Policy, void> :
			public reference<typename _Policy::object_type>{
		private:
			using reference_type = reference<typename _Policy::object_type>;
			
		public:
			static constexpr bool policy = true;
			static constexpr bool object = true;
			static constexpr bool parent = false;
			
			using object_type = typename _Policy::object_type;
			using function_t = std::function<std::any(const std::any&)>;
			using function_ref_t = std::function<std::any(object_type&, const std::any&)>;
			using function_cref_t = std::function<std::any(const object_type&, const std::any&)>;
			using storage_t = std::variant<function_t, function_ref_t, function_cref_t>;
			
		public:
			using reference_type::reference_type;
		};
		
		// [policy] [non-hosted] [parent] [complie-time signature]. 
		template<typename _Delegate, mix_c<policy_d && !object_d && parent_d> _Policy>
		struct base<_Delegate, _Policy, void> :
			public resource<_Policy, _Delegate>{
		private:
			using parent_type = resource<_Policy, _Delegate>;
			
		public:
			static constexpr bool policy = true;
			static constexpr bool object = false;
			static constexpr bool parent = true;
			
			using object_type = void;
			using function_t = std::function<std::any(const std::any&)>;
			using function_ref_t = function_t;
			using function_cref_t = function_t;
			using storage_t = function_t;
			
		public:
			using parent_type::parent_type;
		};
		
		// [policy] [non-hosted] [non-parent] [complie-time signature]. 
		template<typename _Delegate, mix_c<policy_d && !object_d && !parent_d> _Policy>
		struct base<_Delegate, _Policy, void>{
			static constexpr bool policy = true;
			static constexpr bool object = false;
			static constexpr bool parent = false;
			
			using object_type = void;
			using function_t = std::function<std::any(const std::any&)>;
			using function_ref_t = function_t;
			using function_cref_t = function_t;
			using storage_t = function_t;
		};
		
		// [non-policy] [hosted] [non-parent] [complie-time signature]. 
		template<typename _Delegate, mix_c<!policy_d && object_d> _Object>
		struct base<_Delegate, _Object, void> :
			public reference<_Object>{
		private:
			using reference_type = reference<_Object>;
			
		public:
			static constexpr bool policy = false;
			static constexpr bool object = true;
			static constexpr bool parent = false;
			
			using object_type = _Object;
			using function_t = std::function<std::any(const std::any&)>;
			using function_ref_t = std::function<std::any(_Object&, const std::any&)>;
			using function_cref_t = std::function<std::any(const _Object&, const std::any&)>;
			using storage_t = std::variant<function_t, function_ref_t, function_cref_t>;
			
		public:
			using reference_type::reference_type;
		};
		
		inline constexpr fake::creed_c auto arg_d = !fake::reference_d ||
			fake::lvalue_reference_d && (fake::remove_reference_p <<= fake::const_d);
		
		template<typename _Type>
		concept arg_c = mix_c<_Type, arg_d>;
		
		enum struct valid_e : uint8_t{invalid, isolate, ref, cref};
		
		template<typename _Object, fake::tuple_c>
		struct valid final : std::false_type{
			static constexpr valid_e branch = valid_e::invalid;
		};
		
		template<typename _Object>
		struct valid<_Object, std::tuple<>> final : std::true_type{
			static constexpr valid_e branch = valid_e::isolate;
			using ref = void;
			using args = std::tuple<>;
		};
		
		template<typename... _Types>
		requires (arg_c<_Types> && ...)
		struct valid<void, std::tuple<_Types...>> final : std::true_type{
			static constexpr valid_e branch = valid_e::isolate;
			using ref = void;
			using args = std::tuple<_Types...>;
		};
		
		template<mix_c<!fake::void_d> _Object, typename... _Tails>
		requires (arg_c<_Tails> && ...)
		struct valid<_Object, std::tuple<_Object&, _Tails...>> final : std::true_type{
			static constexpr valid_e branch = valid_e::ref;
			using ref = _Object&;
			using args = std::tuple<_Tails...>;
		};
		
		template<mix_c<!fake::void_d> _Object, typename... _Tails>
		requires (arg_c<_Tails> && ...)
		struct valid<_Object, std::tuple<const _Object&, _Tails...>> final : std::true_type{
			static constexpr valid_e branch = valid_e::cref;
			using ref = const _Object&;
			using args = std::tuple<_Tails...>;
		};
		
		template<mix_c<!fake::void_d> _Object, typename... _Types>
		requires (arg_c<_Types> && ...)
		struct valid<_Object, std::tuple<_Types...>> final : std::true_type{
			static constexpr valid_e branch = valid_e::isolate;
			using ref = void;
			using args = std::tuple<_Types...>;
		};
		
		template<typename _Type, typename _Delegate>
		concept emplace_c = (
			fake::trait_c<std::remove_cvref_t<_Type>, std::shared_ptr> ||
			std::is_pointer_v<decltype(_Delegate::source)> ||
			requires{requires std::is_pointer_v<typename decltype(_Delegate::source)::value_type>;}
		) && (
			requires(_Type _e, _Delegate &_d){_d.source.emplace_back(std::forward<decltype(_e)>(_e));} ||
			requires(_Type _e, _Delegate &_d){_d.source = std::forward<decltype(_e)>(_e);} ||
			requires(_Type _e, _Delegate &_d){_d.source = &_e;}
		);
		
	}
	
	enum struct delegate_e : uint8_t{assign, amount, cast, pick, seek, mono, rash, invoke, enum_size};
	
	enum struct delegate_handle : std::size_t{invalid};
	
	// policy type example: 
	
	/*************************************************************************\
	
	struct policy final
	{
		static constexpr fake::view_c auto policy = "fake::delegate::policy"_v;
		
		using object_type = some_object_type;
		using transform_type = fake::tool::delegate::shared_ptrs;
	};
	
	\*************************************************************************/
	
	template<typename, typename = void, template<typename> typename = fake::pack_t>
	struct delegate;
	
	template<typename _Result, typename... _Args, typename _Policy, template<typename> typename _Aspect>
	requires std::same_as<std::remove_cvref_t<_Policy>, _Policy>
	struct delegate<_Result(_Args...), _Policy, _Aspect> :
		protected detail::delegate::base<delegate<_Result(_Args...), _Policy, _Aspect>, _Policy, _Result(_Args...)>{
	private:
		using base_type = detail::delegate::base<delegate, _Policy, _Result(_Args...)>;
		
		using function_t = typename base_type::function_t;
		using function_ref_t = typename base_type::function_ref_t;
		using function_cref_t = typename base_type::function_cref_t;
		
		using result_type = typename function_t::result_type;
		using storage_t = typename base_type::storage_t;
		
		static constexpr bool object = base_type::object;
		static constexpr bool parent = base_type::parent;
		
		static constexpr fake::view_c auto report = fake::type_view(fake::pack_v<_Result(_Args...)>);
		
	public:
		using base_type::base_type;
		using policy_type = _Policy;
		
	public:
		[[nodiscard]] bool contains(fake::delegate_handle _id) const requires parent{
			if(delegate::id == _id)
				return true;
			
			if constexpr(base_type::multiple){
				for(const auto &e : delegate::source)
					if(e && e->contains(_id))
						return true;
				
				return false;
			}
			else{
				return delegate::source && delegate::source->contains(_id);
			}
		}
		
		bool attach(fake::detail::delegate::emplace_c<delegate> auto &&_e) requires parent{
			if constexpr(requires{{_e.id} -> std::convertible_to<fake::delegate_handle>;})
				if(delegate::contains(_e.id))
					return false;
			
			if constexpr(requires{{_e->id} -> std::convertible_to<fake::delegate_handle>;})
				if(delegate::contains(_e->id))
					return false;
			
			if constexpr(base_type::multiple){
				if constexpr(std::same_as<std::remove_cvref_t<decltype(_e)>, delegate>)
					delegate::source.emplace_back(&std::forward<decltype(_e)>(_e));
				else
					delegate::source.emplace_back(std::forward<decltype(_e)>(_e));
			}
			else{
				if constexpr(std::same_as<std::remove_cvref_t<decltype(_e)>, delegate>)
					delegate::source = &_e;
				else
					delegate::source = std::forward<decltype(_e)>(_e);
			}
			
			return true;
		}
		
		bool detach(fake::delegate_handle _id) requires parent{
			if constexpr(base_type::multiple)
				return std::erase_if(delegate::source, [_id](const auto &_e){return _e->id == _id;});
			else if(delegate::source && delegate::source->id == _id)
				return bool(std::exchange(delegate::source, nullptr));
			else
				return false;
		}
		
	private:
		template<typename _Functor>
		[[nodiscard]] std::size_t assign(auto _in_place, _Functor &&_functor){
			struct message : fake::pazz_t<_Functor>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::false_type;
				using invoke = std::false_type;
				using aspect = fake::mezz_t<delegate_e::assign>;
			};
			
			if constexpr(object == false)
				delegates.emplace(
					detail::delegate::index<void>::current,
					std::make_shared<storage_t>(std::forward<_Functor>(_functor))
				);
			else
				delegates.emplace(
					detail::delegate::index<void>::current,
					std::make_shared<storage_t>(_in_place, std::forward<_Functor>(_functor))
				);
			
			return detail::delegate::index<void>::current++;
		}
		
	public:
		template<std::convertible_to<function_t> _Functor>
		std::size_t operator=(_Functor &&_functor){
			return delegate::assign(std::in_place_type<function_t>, std::forward<_Functor>(_functor));
		}
		
		template<std::convertible_to<function_ref_t> _Functor>
		requires object
		std::size_t operator=(_Functor &&_functor){
			return delegate::assign(std::in_place_type<function_ref_t>, std::forward<_Functor>(_functor));
		}
		
		template<std::convertible_to<function_cref_t> _Functor>
		requires object
		std::size_t operator=(_Functor &&_functor){
			return delegate::assign(std::in_place_type<function_cref_t>, std::forward<_Functor>(_functor));
		}
		
	private:
		[[nodiscard]] std::size_t source_size() const requires parent{
			if constexpr(base_type::multiple)
				return std::ranges::fold_left(
					delegate::source | std::views::transform([](const auto &_e){return _e ? _e->size() : 0x0;}),
					std::size_t{},
					std::plus<std::size_t>{}
				);
			else if(delegate::source)
				return delegate::source->size();
			else
				return std::size_t{};
		}
		
	public:
		[[nodiscard]] std::size_t size() const{
			if constexpr(parent)
				return delegate::source_size() + delegates.size();
			else
				return delegates.size();
		}
		
		template<typename... _Params>
		[[nodiscard]] std::size_t amount() const{
			struct message : fake::pazz_t<std::size_t(_Params...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::false_type;
				using invoke = std::false_type;
				using aspect = fake::mezz_t<delegate_e::amount>;
			};
			
			if constexpr(std::invocable<function_t, _Params...>)
				return delegate::size();
			else
				return 0x0;
		}
		
		[[nodiscard]] std::size_t amount(const auto &..._args) const{
			return delegate::amount<decltype(_args)...>();
		}
		
	private:
		template<typename... _Params>
		auto visit(storage_t &_storage, _Params &&..._args) const{
			using namespace fake::literals;
			
			if constexpr(object){
				return fake::visit(
					_storage,
					[&_args...](function_t &_f){
						return _f(std::forward<_Params>(_args)...);
					},
					[this, &_args...](function_ref_t &_f){
						if(base_type::host.has_value() == false)
							throw std::runtime_error{("fake::delegate<"_v + report + ">(): pure proto type."_v).data()};
						
						return _f(base_type::host->get(), std::forward<_Params>(_args)...);
					},
					[this, &_args...](function_cref_t &_f){
						if(base_type::host.has_value() == false)
							throw std::runtime_error{("fake::delegate<"_v + report + ">(): pure proto type."_v).data()};
						
						return _f(base_type::host->get(), std::forward<_Params>(_args)...);
					}
				);
			}
			else{
				return _storage(std::forward<_Params>(_args)...);
			}
		}
		
	public:
		template<std::same_as<result_type> _Retn = result_type>
		requires (std::same_as<_Retn, void> == false)
		std::vector<std::pair<std::size_t, std::optional<result_type>>> cast(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::false_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::cast>;
			};
			
			std::vector<std::pair<std::size_t, std::optional<result_type>>> retn;
			retn.reserve(delegate::size());
			
			for(const auto &[id, funcptr] : delegate::local())
				retn.emplace_back(id, delegate::visit(*funcptr, std::forward<decltype(_args)>(_args)...));
			
			return retn;
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		requires std::same_as<_Retn, void>
		void cast(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::false_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::cast>;
			};
			
			for(const auto &[id, funcptr] : delegate::local())
				delegate::visit(*funcptr, std::forward<decltype(_args)>(_args)...);
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		requires (std::same_as<_Retn, void> == false)
		std::vector<std::pair<std::size_t, result_type>> pick(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::false_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::pick>;
			};
			
			std::vector<std::pair<std::size_t, result_type>> retn;
			retn.reserve(delegate::size());
			
			for(const auto &[id, funcptr] : delegate::local())
				retn.emplace_back(id, delegate::visit(*funcptr, std::forward<decltype(_args)>(_args)...));
			
			return retn;
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		requires std::same_as<_Retn, void>
		void pick(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::false_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::pick>;
			};
			
			for(const auto &[id, funcptr] : delegate::local())
				delegate::visit(*funcptr, std::forward<decltype(_args)>(_args)...);
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		std::optional<result_type> seek(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			using namespace fake::literals;
			
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::false_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::seek>;
			};
			
			if(const std::size_t size = delegate::size(); size == 0x0)
				return std::nullopt;
			else if(size != 0x1)
				throw std::runtime_error{
					("fake::delegate::seek<"_v + report + ">(): delegate is NOT unique."_v).data()
				};
			
			return delegate::visit(delegate::unique(), std::forward<decltype(_args)>(_args)...);
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		result_type mono(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			using namespace fake::literals;
			
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::false_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::mono>;
			};
			
			if(const std::size_t size = delegate::size(); size == 0uz){
				if constexpr(std::same_as<_Retn, void>)
					return;
				else
					return {};
			}
			else if(size != 0x1){
				throw std::runtime_error{
					("fake::delegate::mono<"_v + report + ">(): delegate is NOT unique."_v).data()
				};
			}
			
			return delegate::visit(delegate::unique(), std::forward<decltype(_args)>(_args)...);
		}
		
		template<std::same_as<result_type> _Retn = result_type>
		result_type rash(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			using namespace fake::literals;
			
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::false_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::rash>;
			};
			
			if(delegate::size() != 0x1)
				throw std::runtime_error{
					("fake::delegate::rash<"_v + report + ">(): delegate is NOT unique."_v).data()
				};
			
			return delegate::visit(delegate::unique(), std::forward<decltype(_args)>(_args)...);
		}
		
		bool operator()(auto &&..._args) const
		requires std::invocable<function_t, decltype(_args)...>{
			struct message : fake::pazz_t<bool(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::false_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::invoke>;
			};
			
			if(delegate::size() == 0)
				return false;
			
			for(const auto &[id, funcptr] : delegate::local())
				delegate::visit(*funcptr, std::forward<decltype(_args)>(_args)...);
			
			return true;
		}
		
		std::size_t erase(std::size_t _index){return delegates.erase(_index);}
		
		operator bool() const{return delegate::size();}
		
	private:
		[[nodiscard]] std::vector<std::pair<std::size_t, std::shared_ptr<storage_t>>> local() const{
			std::vector<std::pair<std::size_t, std::shared_ptr<storage_t>>> snap;
			
			snap.reserve(delegate::size());
			
			std::ranges::copy(delegates, std::back_inserter(snap));
			
			if constexpr(parent){
				if constexpr(base_type::multiple){
					for(const auto &e : delegate::source)
						if(e)
							std::ranges::copy(e->local(), std::back_inserter(snap));
				}
				else if(delegate::source){
					std::ranges::copy(delegate::source->local(), std::back_inserter(snap));
				}
			}
			
			return std::move(snap);
		}
		
	private:
		[[nodiscard]] std::optional<std::reference_wrapper<storage_t>> source_unique() const requires parent{
			if(delegates.size())
				return *delegates.begin()->second;
			
			if constexpr(base_type::multiple){
				for(const auto &e : delegate::source)
					if(e)
						if(auto opt = e->source_unique())
							return std::move(opt);
			}
			else if(delegate::source){
				if(auto opt = delegate::source->source_unique())
					return std::move(opt);
			}
			
			return std::nullopt;
		}
		
		[[nodiscard]] storage_t& unique() const{
			if constexpr(parent)
				return delegate::source_unique()->get();
			else
				return *delegates.begin()->second;
		}
		
	public:
		const fake::delegate_handle id = static_cast<fake::delegate_handle>(detail::delegate::index<void>::current++);
		
	private:
		std::unordered_map<std::size_t, std::shared_ptr<storage_t>> delegates;
	};
	
	template<std::same_as<void> _Type, typename _Policy, template<typename> typename _Aspect>
	requires std::same_as<std::remove_cvref_t<_Policy>, _Policy>
	struct delegate<_Type, _Policy, _Aspect> :
		protected detail::delegate::base<delegate<_Type, _Policy, _Aspect>, _Policy, void>{
	private:
		using base_type = detail::delegate::base<delegate, _Policy, void>;
		
		using object_type = typename base_type::object_type;
		
		using function_t = typename base_type::function_t;
		using function_ref_t = typename base_type::function_ref_t;
		using function_cref_t = typename base_type::function_cref_t;
		
		using storage_t = typename base_type::storage_t;
		
		static constexpr bool object = base_type::object;
		static constexpr bool parent = base_type::parent;
		
		struct package_t final{
			std::shared_ptr<storage_t> funcptr;
			fake::signet_t type; // signature of the types of args, the self reference fo host is excluded. 
			fake::signet_t result;
		};
		
	public:
		using base_type::base_type;
		using policy_type = _Policy;
		
	public:
		[[nodiscard]] bool contains(fake::delegate_handle _id) const requires parent{
			if(delegate::id == _id)
				return true;
			
			if constexpr(base_type::multiple){
				for(const auto &e : delegate::source)
					if(e && e->contains(_id))
						return true;
				
				return false;
			}
			else{
				return delegate::source && delegate::source->contains(_id);
			}
		}
		
		bool attach(fake::detail::delegate::emplace_c<delegate> auto &&_e) requires parent{
			if constexpr(requires{{_e.id} -> std::convertible_to<fake::delegate_handle>;})
				if(delegate::contains(_e.id))
					return false;
			
			if constexpr(requires{{_e->id} -> std::convertible_to<fake::delegate_handle>;})
				if(delegate::contains(_e->id))
					return false;
			
			if constexpr(base_type::multiple){
				if constexpr(std::same_as<std::remove_cvref_t<decltype(_e)>, delegate>)
					delegate::source.emplace_back(&std::forward<decltype(_e)>(_e));
				else
					delegate::source.emplace_back(std::forward<decltype(_e)>(_e));
			}
			else{
				if constexpr(std::same_as<std::remove_cvref_t<decltype(_e)>, delegate>)
					delegate::source = &_e;
				else
					delegate::source = std::forward<decltype(_e)>(_e);
			}
			
			return true;
		}
		
		bool detach(fake::delegate_handle _id) requires parent{
			if constexpr(base_type::multiple)
				return std::erase_if(delegate::source, [_id](const auto &_e){return _e->id == _id;});
			else if(delegate::source && delegate::source->id == _id)
				return bool(std::exchange(delegate::source, nullptr));
			else
				return false;
		}
		
	private:
		template<typename _Retn, typename _Ref, fake::tuple_c _Args, typename _Functor>
		[[nodiscard]] std::size_t assign(auto _in_place, _Functor &&_functor){
			struct message : fake::pazz_t<_Functor>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::false_type;
				using aspect = fake::mezz_t<delegate_e::assign>;
			};
			
			using retn_t = std::remove_cvref_t<_Retn>;
			using args_t = fake::tuple::transform_t<
				_Args,
				[](fake::pack_c auto _e){
					using type = std::remove_cvref_t<fake::take_t<decltype(_e){}>>;
					return fake::pack_v<std::add_lvalue_reference_t<std::add_const_t<type>>>;
				}
			>;
			using bare_t = fake::tuple::transform_t<
				_Args,
				[](fake::pack_c auto _e){return fake::pack_v<std::remove_cvref_t<fake::take_t<decltype(_e){}>>>;}
			>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<retn_t>);
			
			if constexpr(object == false)
				delegates.emplace(
					detail::delegate::index<void>::current,
					package_t{
						std::make_shared<storage_t>(
							[f = std::forward<_Functor>(_functor)](const std::any &_args) -> std::any {
								if constexpr(std::same_as<retn_t, void>){
									std::apply(f, std::any_cast<const args_t&>(_args));
									return {};
								}
								else{
									return std::apply(f, std::any_cast<const args_t&>(_args));
								}
							}
						),
						fake::signet_v<arguments>,
						fake::signet_v<result>
					}
				);
			else if constexpr(std::same_as<_Ref, void>) // same as 'valid::branch == valid_e::isolate'. 
				delegates.emplace(
					detail::delegate::index<void>::current,
					package_t{
						std::make_shared<storage_t>(
							_in_place,
							[f = std::forward<_Functor>(_functor)](const std::any &_args) -> std::any {
								if constexpr(std::same_as<retn_t, void>){
									std::apply(f, std::any_cast<const args_t&>(_args));
									return {};
								}
								else{
									return std::apply(f, std::any_cast<const args_t&>(_args));
								}
							}
						),
						fake::signet_v<arguments>,
						fake::signet_v<result>
					}
				);
			else
				delegates.emplace(
					detail::delegate::index<void>::current,
					package_t{
						std::make_shared<storage_t>(
							_in_place,
							[f = std::forward<_Functor>(_functor)](_Ref _host, const std::any &_args) -> std::any {
								if constexpr(std::same_as<retn_t, void>){
									std::apply(
										f,
										std::tuple_cat(std::tie(_host), std::any_cast<const args_t&>(_args))
									);
									return {};
								}
								else{
									return std::apply(
										f,
										std::tuple_cat(std::tie(_host), std::any_cast<const args_t&>(_args))
									);
								}
							}
						),
						fake::signet_v<arguments>,
						fake::signet_v<result>
					}
				);
			
			parameters[fake::signet_v<arguments>].emplace(detail::delegate::index<void>::current);
			
			return detail::delegate::index<void>::current++;
		}
		
	public:
		template<fake::functor_c _Functor>
		requires detail::delegate::valid<
			object_type,
			typename fake::functor_info<std::remove_cvref_t<_Functor>>::tuple
		>::value
		std::size_t operator=(_Functor &&_functor){
			using info_t = fake::functor_info<std::remove_cvref_t<_Functor>>;
			using valid_t = detail::delegate::valid<object_type, typename info_t::tuple>;
			using retn_t = typename info_t::retn;
			using ref_t = typename valid_t::ref;
			using args_t = typename valid_t::args;
			
			if constexpr(valid_t::branch == detail::delegate::valid_e::isolate)
				return delegate::assign<retn_t, ref_t, args_t>(
					std::in_place_type<function_t>,
					std::forward<_Functor>(_functor)
				);
			if constexpr(valid_t::branch == detail::delegate::valid_e::ref)
				return delegate::assign<retn_t, ref_t, args_t>(
					std::in_place_type<function_ref_t>,
					std::forward<_Functor>(_functor)
				);
			if constexpr(valid_t::branch == detail::delegate::valid_e::cref)
				return delegate::assign<retn_t, ref_t, args_t>(
					std::in_place_type<function_cref_t>,
					std::forward<_Functor>(_functor)
				);
		}
		
	private:
		[[nodiscard]] std::size_t source_size() const requires parent{
			if constexpr(base_type::multiple)
				return std::ranges::fold_left(
					delegate::source | std::views::transform([](const auto &_e){return _e ? _e->size() : 0x0;}),
					std::size_t{},
					std::plus<std::size_t>{}
				);
			else if(delegate::source)
				return delegate::source->size();
			else
				return std::size_t{};
		}
		
	public:
		[[nodiscard]] std::size_t size() const{
			if constexpr(parent)
				return delegate::source_size() + delegates.size();
			else
				return delegates.size();
		}
		
	private:
		[[nodiscard]] std::size_t source_amount(const fake::signet_t &_s) const requires parent{
			if constexpr(base_type::multiple)
				return std::ranges::fold_left(
					delegate::source | std::views::transform([_s](const auto &_e){return _e ? _e->amount(_s) : 0x0;}),
					std::size_t{},
					std::plus<std::size_t>{}
				);
			else if(delegate::source)
				return delegate::source->amount(_s);
			else
				return 0x0;
		}
		
		[[nodiscard]] std::size_t amount(const fake::signet_t &_signet) const{
			if constexpr(parent){
				if(parameters.contains(_signet) == false)
					return delegate::source_amount(_signet);
				else
					return delegate::source_amount(_signet) + parameters.at(_signet).size();
			}
			else{
				if(parameters.contains(_signet) == false)
					return 0x0;
				
				return parameters.at(_signet).size();
			}
		}
		
	public:
		template<typename... _Params>
		[[nodiscard]] std::size_t amount() const{
			struct message : fake::pazz_t<std::size_t(_Params...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::false_type;
				using aspect = fake::mezz_t<delegate_e::amount>;
			};
			
			using bare_t = std::tuple<std::remove_cvref_t<_Params>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			
			return delegate::amount(fake::signet_v<arguments>);
		}
		
		[[nodiscard]] std::size_t amount(const auto &..._args) const{
			struct message : fake::pazz_t<std::size_t(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::false_type;
				using aspect = fake::mezz_t<delegate_e::amount>;
			};
			
			return delegate::amount<decltype(_args)...>();
		}
		
	private:
		template<typename _Retn, typename _Args>
		auto visit(storage_t &_storage, const fake::tuple_c auto &_refs) const{
			if constexpr(object){
				using namespace fake::literals;
				
				static constexpr fake::view_c auto report = []<typename... _Types>(
					fake::type_package<std::tuple<_Types...>>
				){
					return fake::type_view(fake::pack_v<_Retn(_Types...)>);
				}(fake::pack_v<_Args>);
				
				return fake::visit(
					_storage,
					[&_refs](function_t &_f){
						if constexpr(std::same_as<_Retn, void>)
							_f(std::make_any<const _Args&>(_refs));
						else
							return std::any_cast<_Retn>(_f(std::make_any<const _Args&>(_refs)));
					},
					[this, &_refs](function_ref_t &_f){
						if(base_type::host.has_value() == false)
							throw std::runtime_error{("fake::delegate<"_v + report + ">(): pure proto type."_v).data()};
						
						if constexpr(std::same_as<_Retn, void>)
							_f(base_type::host->get(), std::make_any<const _Args&>(_refs));
						else
							return std::any_cast<_Retn>(_f(base_type::host->get(), std::make_any<const _Args&>(_refs)));
					},
					[this, &_refs](function_cref_t &_f){
						if(base_type::host.has_value() == false)
							throw std::runtime_error{("fake::delegate<"_v + report + ">(): pure proto type."_v).data()};
						
						if constexpr(std::same_as<_Retn, void>)
							_f(base_type::host->get(), std::make_any<const _Args&>(_refs));
						else
							return std::any_cast<_Retn>(_f(base_type::host->get(), std::make_any<const _Args&>(_refs)));
					}
				);
			}
			else if constexpr(std::same_as<_Retn, void>){
				_storage(std::make_any<const _Args&>(_refs));
			}
			else{
				return std::any_cast<_Retn>(_storage(std::make_any<const _Args&>(_refs)));
			}
		}
		
	public:
		template<typename _Retn>
		requires (std::same_as<_Retn, void> == false) && std::same_as<std::remove_cvref_t<_Retn>, _Retn>
		std::vector<std::pair<std::size_t, std::optional<_Retn>>> cast(const auto &..._args) const{
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::cast>;
			};
			
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			
			if(delegate::amount<decltype(_args)...>() == 0)
				return {};
			
			const std::vector<std::pair<std::size_t, package_t>> broadcast = delegate::local(fake::signet_v<arguments>);
			
			std::vector<std::pair<std::size_t, std::optional<_Retn>>> retn;
			retn.reserve(broadcast.size());
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast){
				const args_t refs = std::tie(_args...);
				
				if(storage.result == signature)
					retn.emplace_back(id, delegate::visit<_Retn, args_t>(*storage.funcptr, refs));
				else
					delegate::visit<void, args_t>(*storage.funcptr, refs), retn.emplace_back(id, std::nullopt);
			}
			
			return retn;
		}
		
		template<std::same_as<void> _Retn>
		std::vector<std::pair<std::size_t, bool>> cast(const auto &..._args) const{
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::cast>;
			};
			
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			
			if(delegate::amount<decltype(_args)...>() == 0)
				return {};
			
			const std::vector<std::pair<std::size_t, package_t>> broadcast = delegate::local(fake::signet_v<arguments>);
			
			std::vector<std::pair<std::size_t, bool>> retn;
			retn.reserve(broadcast.size());
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast){
				const args_t refs = std::tie(_args...);
				
				delegate::visit<void, args_t>(*storage.funcptr, refs);
				retn.emplace_back(id, storage.result == signature);
			}
			
			return retn;
		}
		
		template<typename _Retn>
		requires (std::same_as<_Retn, void> == false) && std::same_as<std::remove_cvref_t<_Retn>, _Retn>
		std::vector<std::pair<std::size_t, _Retn>> pick(const auto &..._args) const{
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::pick>;
			};
			
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			
			if(delegate::amount<decltype(_args)...>() == 0)
				return {};
			
			const std::vector<std::pair<std::size_t, package_t>> broadcast = delegate::local(fake::signet_v<arguments>);
			
			std::vector<std::pair<std::size_t, _Retn>> retn;
			
			std::size_t reserve = 0;
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast)
				if(storage.result == signature)
					reserve++;
			
			retn.reserve(reserve);
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast){
				const args_t refs = std::tie(_args...);
				
				if(storage.result == signature)
					retn.emplace_back(id, delegate::visit<_Retn, args_t>(*storage.funcptr, refs));
			}
			
			return retn;
		}
		
		template<std::same_as<void> _Retn>
		std::vector<std::size_t> pick(const auto &..._args) const{
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::pick>;
			};
			
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<bool>);
			
			if(delegate::amount<decltype(_args)...>() == 0)
				return {};
			
			const std::vector<std::pair<std::size_t, package_t>> broadcast = delegate::local(fake::signet_v<arguments>);
			
			std::vector<std::size_t> retn;
			
			std::size_t reserve = 0;
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast)
				if(storage.result == signature)
					reserve++;
			
			retn.reserve(reserve);
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast){
				const args_t refs = std::tie(_args...);
				
				if(storage.result == signature)
					delegate::visit<void, args_t>(*storage.funcptr, refs), retn.emplace_back(id);
			}
			
			return retn;
		}
		
		template<typename _Retn>
		requires std::constructible_from<_Retn> && std::same_as<std::remove_cvref_t<_Retn>, _Retn> ||
			std::same_as<_Retn, void>
		std::conditional_t<std::same_as<_Retn, void>, bool, std::optional<_Retn>> seek(const auto &..._args) const{
			using namespace fake::literals;
			
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::seek>;
			};
			
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto report = fake::type_view(fake::pack_v<_Retn(decltype(_args)...)>);
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			constexpr fake::view_c auto error = "fake::delegate::seek<"_v + report + ">()"_v;
			
			if(delegate::amount<decltype(_args)...>() == 0){
				if constexpr(std::same_as<_Retn, void>)
					return false;
				else
					return std::nullopt;
			}
			
			const std::vector<std::pair<std::size_t, package_t>> broadcast = delegate::local(fake::signet_v<arguments>);
			
			std::size_t reserve = 0;
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast)
				if(storage.result == signature)
					reserve++;
			
			if(reserve != 0x1)
				throw std::runtime_error{(error + ": delegate is NOT unique."_v).data()};
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast){
				const args_t refs = std::tie(_args...);
				
				if(storage.result == signature){
					if constexpr(std::same_as<_Retn, void>)
						return delegate::visit<void, args_t>(*storage.funcptr, refs), true;
					else
						return delegate::visit<_Retn, args_t>(*storage.funcptr, refs);
				}
			}
			
			throw std::runtime_error{(error + ": logic error unreachable."_v).data()};
		}
		
		template<typename _Retn>
		requires std::constructible_from<_Retn> && std::same_as<std::remove_cvref_t<_Retn>, _Retn> ||
			std::same_as<_Retn, void>
		_Retn mono(const auto &..._args) const{
			using namespace fake::literals;
			
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::mono>;
			};
			
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto report = fake::type_view(fake::pack_v<_Retn(decltype(_args)...)>);
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			constexpr fake::view_c auto error = "fake::delegate::mono<"_v + report + ">()"_v;
			
			if(delegate::amount<decltype(_args)...>() == 0){
				if constexpr(std::same_as<_Retn, void>)
					return;
				else
					return {};
			}
			
			const std::vector<std::pair<std::size_t, package_t>> broadcast = delegate::local(fake::signet_v<arguments>);
			
			std::size_t reserve = 0;
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast)
				if(storage.result == signature)
					reserve++;
			
			if(reserve != 0x1)
				throw std::runtime_error{(error + ": delegate is NOT unique."_v).data()};
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast){
				const args_t refs = std::tie(_args...);
				
				if(storage.result == signature)
					return delegate::visit<_Retn, args_t>(*storage.funcptr, refs);
			}
			
			throw std::runtime_error{(error + ": logic error unreachable."_v).data()};
		}
		
		template<typename _Retn>
		requires std::same_as<std::remove_cvref_t<_Retn>, _Retn>
		_Retn rash(const auto &..._args) const{
			using namespace fake::literals;
			
			struct message : fake::pazz_t<_Retn(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::rash>;
			};
			
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto report = fake::type_view(fake::pack_v<_Retn(decltype(_args)...)>);
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			constexpr fake::view_c auto result = fake::type_view(fake::pack_v<_Retn>);
			constexpr fake::view_c auto error = "fake::delegate::rash<"_v + report + ">()"_v;
			
			if(delegate::amount<decltype(_args)...>() == 0)
				throw std::runtime_error{(error + ": delegate does NOT exist."_v).data()};
			
			const std::vector<std::pair<std::size_t, package_t>> broadcast = delegate::local(fake::signet_v<arguments>);
			
			std::size_t reserve = 0;
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast)
				if(storage.result == signature)
					reserve++;
			
			if(reserve != 0x1)
				throw std::runtime_error{(error + ": delegate is NOT unique."_v).data()};
			
			for(const fake::signet_t signature{fake::signet_v<result>}; const auto &[id, storage] : broadcast){
				const args_t refs = std::tie(_args...);
				
				if(storage.result == signature)
					return delegate::visit<_Retn, args_t>(*storage.funcptr, refs);
			}
			
			throw std::runtime_error{(error + ": logic error unreachable."_v).data()};
		}
		
		bool operator()(const auto &..._args) const{
			struct message : fake::pazz_t<bool(decltype(_args)...)>, _Aspect<message>{
				using delegate = fake::pazz_t<delegate>;
				using dynamic = std::true_type;
				using invoke = std::true_type;
				using aspect = fake::mezz_t<delegate_e::invoke>;
			};
			
			using args_t = std::tuple<decltype(_args)...>;
			using bare_t = std::tuple<std::remove_cvref_t<decltype(_args)>...>;
			constexpr fake::view_c auto arguments = fake::type_view(fake::pack_v<bare_t>);
			
			if(delegate::amount<decltype(_args)...>() == 0)
				return false;
			
			for(const auto &[id, storage] : delegate::local(fake::signet_v<arguments>)){
				const args_t refs = std::tie(_args...);
				delegate::visit<void, args_t>(*storage.funcptr, refs);
			}
			
			return true;
		}
		
		std::size_t erase(std::size_t _index){
			if(delegates.contains(_index) == false)
				return 0x0;
			
			const fake::signet_t type = delegates.at(_index).type;
			
			auto &parameter = parameters.at(type);
			
			parameter.erase(_index);
			
			if(parameter.empty())
				parameters.erase(type);
			
			return delegates.erase(_index);
		}
		
		operator bool() const{return delegate::size();}
		
	private:
		[[nodiscard]] std::vector<std::pair<std::size_t, package_t>> local(const fake::signet_t &_signet) const{
			std::vector<std::pair<std::size_t, package_t>> snap;
			
			snap.reserve(delegate::amount(_signet));
			
			if constexpr(parent){
				if(const auto it = parameters.find(_signet); it != parameters.end()){
					const auto transform = [this](std::size_t _e){return *delegates.find(_e);};
					
					std::ranges::copy(it->second | std::views::transform(transform), std::back_inserter(snap));
				}
			}
			else{
				const auto &broadcast = parameters.at(_signet);
				const auto transform = [this](std::size_t _e){return *delegates.find(_e);};
				
				std::ranges::copy(broadcast | std::views::transform(transform), std::back_inserter(snap));
			}
			
			if constexpr(parent){
				if constexpr(base_type::multiple){
					for(const auto &e : delegate::source)
						if(e)
							std::ranges::copy(e->local(_signet), std::back_inserter(snap));
				}
				else if(delegate::source){
					std::ranges::copy(delegate::source->local(_signet), std::back_inserter(snap));
				}
			}
			
			return std::move(snap);
		}
		
	public:
		const fake::delegate_handle id = static_cast<fake::delegate_handle>(detail::delegate::index<void>::current++);
		
	private:
		std::unordered_map<std::size_t, package_t> delegates;
		std::unordered_map<fake::signet_t, std::set<std::size_t>> parameters;
	};
	
	template<typename>
	struct is_delegate : std::false_type{};
	
	template<typename _Type, typename _Policy, template<typename> typename _Aspect>
	struct is_delegate<fake::delegate<_Type, _Policy, _Aspect>> : std::true_type{};
	
	template<typename _Type>
	constexpr bool is_delegate_v = is_delegate<std::remove_cvref_t<_Type>>::value;
	
	template<typename _Type>
	concept delegate_c = is_delegate_v<_Type>;
	
	struct storage
	{
	public:
		[[nodiscard]] bool contains(fake::view_c auto _key) const{
			constexpr fake::signet_t signet = fake::signet(_key);
			
			return data.contains(signet);
		}
		
	public:
		template<typename _Type>
		_Type& emplace(fake::view_c auto _key, auto &&..._args)
		requires std::same_as<_Type, std::remove_cvref_t<_Type>> && requires(std::any &_any){
			_any.emplace<_Type>(std::forward<decltype(_args)>(_args)...);
		}{
			using namespace fake::literals;
			
			constexpr fake::signet_t signet = fake::signet(_key);
			
			if(data.contains(signet) == false) [[likely]]{
				const auto [it, result] = data.try_emplace(signet);
				
				return it->second.template emplace<_Type>(std::forward<decltype(_args)>(_args)...);
			}
			
			throw std::invalid_argument{("error: fake::storage exist key: '"_v + _key + "'"_v).data()};
		}
		
		void erase(fake::view_c auto _key){
			constexpr fake::signet_t signet = fake::signet(_key);
			
			data.erase(signet);
		}
		
	public:
		template<typename _Type>
		requires std::same_as<_Type, std::remove_cvref_t<_Type>>
		_Type& ref(fake::view_c auto _key){
			using namespace fake::literals;
			
			constexpr fake::signet_t signet = fake::signet(_key);
			
			if(auto it = data.find(signet); it != data.end()) [[likely]]
				return std::any_cast<_Type&>(it->second);
			
			throw std::invalid_argument{("error: fake::storage miss key: '"_v + _key + "'"_v).data()};
		}
		
		template<typename _Type>
		requires std::same_as<_Type, std::remove_cvref_t<_Type>>
		const _Type& ref(fake::view_c auto _key) const{
			using namespace fake::literals;
			
			constexpr fake::signet_t signet = fake::signet(_key);
			
			if(auto it = data.find(signet); it != data.end()) [[likely]]
				return std::any_cast<const _Type&>(it->second);
			
			throw std::invalid_argument{("error: fake::storage miss key: '"_v + _key + "'"_v).data()};
		}
		
	private:
		std::unordered_map<fake::signet_t, std::any> data;
	};
	
}

#include "pop.h"

#endif /*__FAKE_DELEGATE_H__*/ 
