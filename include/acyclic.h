#ifndef __FAKE_ACYCLIC_H__
#define __FAKE_ACYCLIC_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include <concepts>
#include <coroutine>
#include <exception>
#include <algorithm>

#include "atomic_guard.h"
#include "tuple.h"
#include "functor_info.h"

namespace fake{template<typename _Token = fake::null_t> constexpr auto pass(auto&&...);}
namespace fake::custom::tool::acyclic{struct executor_t;}

namespace fake::acyclic::concepts
{
	
	template<typename _Type>
	concept awaiter_c = requires(_Type _t, std::coroutine_handle<> _handle){
		{_t.await_ready()} -> std::convertible_to<bool>;
		requires
			requires{{_t.await_suspend(_handle)} -> std::same_as<void>;} ||
			requires{{_t.await_suspend(_handle)} -> std::convertible_to<bool>;} ||
			requires{{_t.await_suspend(_handle)} -> std::convertible_to<std::coroutine_handle<>>;};
		{_t.await_resume()};
	};
	
	template<typename _Token>
	concept token_with_promise_c = requires{typename _Token::promise_type;};
	
	template<typename _Awaitable>
	concept awaitable_to_awaiter_via_member_operator_co_await_c = requires(_Awaitable _a){
		requires awaiter_c<decltype(_a.operator co_await())>;
	};
	
	template<typename _Awaitable>
	concept awaitable_to_awaiter_via_global_operator_co_await_c = requires(_Awaitable _a){
		requires awaiter_c<decltype(operator co_await(static_cast<_Awaitable&&>(_a)))>;
	};
	
	template<typename _Awaitable>
	concept awaitable_to_awaiter_via_as_is_c = requires{
		requires awaiter_c<_Awaitable>;
	};
	
	template<typename _Type>
	concept awaitable_c = bool(
		awaitable_to_awaiter_via_member_operator_co_await_c<_Type> ^
		awaitable_to_awaiter_via_global_operator_co_await_c<_Type> |
		awaitable_to_awaiter_via_as_is_c<_Type>
	);
	
	template<typename _Token, typename _Expr, typename _Promise>
	concept expr_to_awaitable_via_promise_c = requires(_Promise _p){
		requires
			requires(_Expr &_e){awaitable_c<decltype(_p.await_transform(_e))>;} ||
			requires(_Expr &&_e){awaitable_c<decltype(_p.await_transform(_e))>;} ||
			requires(_Expr &_e){
				requires _Token::scheduler_inject;
				awaitable_c<decltype(_p.await_transform(fake::pass<fake::custom::tool::acyclic::executor_t>(_e)))>;
			} ||
			requires(_Expr &&_e){
				requires _Token::scheduler_inject;
				awaitable_c<decltype(_p.await_transform(fake::pass<fake::custom::tool::acyclic::executor_t>(_e)))>;
			};
	};
	
	template<typename _Expr>
	concept expr_to_awaitable_via_as_is_c = requires{
		requires awaitable_c<_Expr>;
	};
	
	template<typename _Type, typename _Token>
	concept co_await_expr_c = requires{
		requires expr_to_awaitable_via_promise_c<_Token, _Type, typename _Token::promise_type>;
	} || expr_to_awaitable_via_as_is_c<_Type>;
	
	template<typename _Scheduler, typename _Token, typename _Task>
	concept meta_scheduler_c = requires(_Scheduler _scheduler, _Task _task){
		_scheduler.template execute<_Token>(_task);
	};
	
	template<typename _Scheduler, typename _Task>
	concept trivial_scheduler_c = requires(_Scheduler _scheduler, _Task _task){
		_scheduler.execute(_task);
	};
	
	template<typename _Type>
	concept scheduler_c = meta_scheduler_c<_Type, fake::type_package<void>, void()> ||
		trivial_scheduler_c<_Type, void()>;
	
	template<typename _Token>
	concept token_with_execution_c = requires{typename _Token::execute_type;};
	
	template<typename _Expr, typename _Transform>
	concept expr_to_scheduler_via_transform_c = requires(_Expr _e, _Transform _t){
		requires scheduler_c<decltype(_t.scheduler_transform(_e))> ||
			scheduler_c<decltype(_t.scheduler_transform(static_cast<_Expr&&>(_e)))>;
	};
	
	template<typename _Expr>
	concept expr_to_scheduler_via_as_is_c = requires{
		requires scheduler_c<_Expr>;
	};
	
	template<typename _Type, typename _Token>
	concept scheduler_expr_c = requires{
		requires expr_to_scheduler_via_transform_c<_Type, typename _Token::execute_type>;
	} || expr_to_scheduler_via_as_is_c<_Type>;
	
	template<typename _Type>
	concept promise_c = requires(_Type _t){
		{_t.initial_suspend()} -> co_await_expr_c<_Type>;
		{_t.final_suspend()} -> co_await_expr_c<_Type>;
		{_t.unhandled_exception()};
	};
	
}

namespace fake::acyclic::token
{
	
	// special token demonstrates an 'scheduler' which schedules a delegated '_task'. 
	// scheduler concepts: 'scheduler->execute<token_t>(_task)'. 
	// usage: 
	//     'auto callee = fake::deliver(fake::pass<fake::acyclic::token::sched<foo>>(std::ref(baka)), ...)'. 
	//     'auto acyclic = fake::bind<...>(callee, ...)'. 
	//     * for a privately associated scheduler, it would invoke the 'lambda' binds with token 'foo'. 
	//     * the 'lambda' may also bind with tokens like 'struct woo : foo{}'. 
	template<typename _Token>
	struct sched{using schedule_type = _Token;};
	
	// one shall suspend the 'pc'. 
	struct await{};
	
	// one should not be scheduled inline. 
	struct split{};
	
	// one shall keep its argument fixed. 
	// alternatively, one shall be invoked via 'std::apply(lambda, var)', in which [decltype(var) = std::tuple<...>]. 
	struct fixed{};
	
	// one shall keep its result fixed. 
	// alternatively, one's results shall be distributed to subsequent nodes. 
	struct dupli{};
	
	// one shall suspend the 'pc' and its 'awaitable' shall be parsed into 'awaiter' via '_Promise'. 
	template<concepts::promise_c _Promise, bool _Inject = false>
	struct adapt : fake::acyclic::token::await{
		using promise_type = _Promise;
		static constexpr bool scheduler_inject = _Inject;
	};
	
	// special token demonstrates a default exception handler which may process a associated unhandled exception. 
	// the unhandled exception is associated to the 'lambda' binds with the token '_Token'. 
	// usage: 
	//     'auto callee = fake::deliver(fake::pass<fake::acyclic::token::spare<foo>>([](){...}), ...)'. 
	//     'auto acyclic = fake::bind<...>(callee, ...)'. 
	//     * a 'handler' would handle the unhandled exception thrown by the 'lambda' binds with token 'foo'. 
	//     * 'lambda' may also bind with tokens like 'struct woo : foo{}'. 
	template<typename _Token>
	struct spare{using handle_type = _Token;};
	
}

namespace fake
{
	
	template<typename _Token, typename _Value, typename _Origin, typename _Tuple>
	requires std::is_class_v<_Token>
	struct pass_t final
	{
		using token_t = std::remove_cvref_t<_Token>;
		using value_t = _Value;
		using origin_t = _Origin;
		static constexpr bool is_tuple = _Tuple::value;
		
		template<fake::pack_c _Pack, typename _Type>
		requires fake::ref_c<_Type> && std::same_as<std::add_lvalue_reference_t<fake::remove_ref_t<_Type>>, value_t>
		constexpr pass_t(_Pack, _Type &&_type): value(fake::remove_ref_v(std::forward<_Type>(_type))){}
		
		template<fake::pack_c _Pack, typename _Type>
		requires std::same_as<std::remove_cvref_t<_Type>, value_t>
		constexpr pass_t(_Pack, _Type &&_type): value(std::forward<_Type>(_type)){}
		
		template<fake::pack_c _Pack, fake::array_c _Type>
		requires std::same_as<std::remove_cvref_t<_Type>, value_t>
		constexpr pass_t(_Pack, _Type &&_type){std::ranges::copy(_type, value);}
		
		template<fake::pack_c _Pack, typename... _Type>
		requires std::same_as<fake::tuple::make_tuple_t<_Type...>, value_t>
		constexpr pass_t(_Pack, _Type &&..._type): value(std::make_tuple(std::forward<_Type>(_type)...)){}
		
		std::add_const_t<std::add_lvalue_reference_t<value_t>> get() const &{return value;}
		std::add_rvalue_reference_t<value_t> get() const &&{return std::move(value);}
		std::add_lvalue_reference_t<value_t> get() &{return value;}
		std::add_rvalue_reference_t<value_t> get() &&{return std::move(value);}
		
		template<typename... _Args>
		requires std::invocable<_Value, _Args...>
		constexpr decltype(auto) operator()(_Args &&..._args){return value(std::forward<_Args>(_args)...);}
		
	private:
		template<typename, typename, typename, typename>
		friend struct pass_t;
		
		value_t value;
	};
	
	template<fake::pack_c _Pack, typename _Type>
	requires fake::ref_c<_Type>
	pass_t(_Pack, _Type&&) -> pass_t<
		std::remove_cvref_t<typename _Pack::type>,
		std::add_lvalue_reference_t<fake::remove_ref_t<_Type>>,
		std::add_lvalue_reference_t<fake::remove_ref_t<_Type>>,
		std::false_type
	>;
	
	template<fake::pack_c _Pack, typename _Type>
	pass_t(_Pack, _Type&&) -> pass_t<
		std::remove_cvref_t<typename _Pack::type>,
		std::remove_cvref_t<_Type>,
		_Type,
		std::false_type
	>;
	
	template<fake::pack_c _Pack, typename... _Type>
	pass_t(_Pack, _Type&&...) -> pass_t<
		std::remove_cvref_t<typename _Pack::type>,
		fake::tuple::make_tuple_t<_Type...>,
		fake::tuple::make_tuple_t<_Type...>,
		std::true_type
	>;
	
	template<typename _Type>
	concept pass_c = fake::trait_v<pass_t, std::remove_cvref_t<_Type>>;
	
	template<typename _Token = fake::null_t>
	constexpr auto pass(auto &&..._type){
		return pass_t(fake::pack_v<_Token>, std::forward<decltype(_type)>(_type)...);
	}
	
	template<typename _Token>
	constexpr auto pass(pass_c auto &&_pass){
		using type = std::remove_cvref_t<decltype(_pass)>;
		if constexpr(type::is_tuple){
			constexpr std::size_t size = std::tuple_size_v<typename type::value_t>;
			
			return []<std::size_t... _Index>(auto &&_tuple, std::index_sequence<_Index...>){
				return pass<_Token>(std::get<_Index>(std::forward<decltype(_tuple)>(_tuple))...);
			}(std::forward<decltype(_pass)>(_pass).get(), std::make_index_sequence<size>());
		}
		else{
			return pass<_Token>(std::forward<decltype(_pass)>(_pass).get());
		}
	}
	
	namespace custom
	{
		
		struct deliver final
		{
			template<pass_c... _Passes>
			constexpr auto operator()(_Passes &&..._passes) const{
				return pack<std::remove_cvref_t<_Passes>...>(std::forward<_Passes>(_passes)...);
			}
			
		private:
			template<pass_c... _Passes>
			struct pack{
				using storage_t = std::tuple<std::remove_cvref_t<_Passes>...>;
				
				template<typename... _Args>
				requires std::conjunction_v<std::is_same<std::remove_cvref_t<_Args>, _Passes>...>
				constexpr pack(_Args &&..._args): storage(std::forward<_Args>(_args)...){}
				
			public:
				storage_t storage;
			};
		};
		
		template<typename _Type>
		concept pack_c = fake::trait_v<deliver::pack, std::remove_cvref_t<_Type>>;
		
	}
	
	constexpr custom::deliver deliver{};
	
	namespace execution::topology
	{
		
		template<typename... _Tokens>
		requires std::same_as<fake::tuple::shrink_t<std::tuple<_Tokens...>>, std::tuple<_Tokens...>> &&
			(std::is_class_v<_Tokens> && ...)
		struct results final{};
		
		template<typename... _Tokens>
		requires std::same_as<fake::tuple::shrink_t<std::tuple<_Tokens...>>, std::tuple<_Tokens...>> &&
			(std::is_class_v<_Tokens> && ...)
		struct args final{};
		
		template<typename _Type>
		concept results_c = fake::trait_v<results, std::remove_cvref_t<_Type>>;
		
		template<typename _Type>
		concept args_c = fake::trait_v<args, std::remove_cvref_t<_Type>>;
		
		template<typename _From, typename _To>
		requires (
			(std::same_as<_From, void> || std::is_class_v<_From>) &&
			(std::same_as<_To, void> || std::is_class_v<_To>)
		)
		struct observer final{
			using from = _From;
			using to = _To;
		};
		
		template<typename _Type>
		concept observer_c = fake::trait_v<observer, std::remove_cvref_t<_Type>>;
		
		template<typename, typename ,typename>
		struct meta final{};
		
		enum struct meta_e : std::size_t{meta_top, meta_obs, meta_obj};
		
		template<typename _Token, results_c _Results, args_c _Args>
		requires std::is_class_v<_Token>
		struct meta<_Token, _Results, _Args> final{
			static constexpr meta_e mode = meta_e::meta_top;
			using token = _Token;
			using results = _Results;
			using args = _Args;
		};
		
		template<typename _Token, results_c _Results, observer_c _Observer>
		requires std::is_class_v<_Token>
		struct meta<_Token, _Results, _Observer> final{
			static constexpr meta_e mode = meta_e::meta_obs;
			using token = _Token;
			using results = _Results;
			using observer = _Observer;
		};
		
		template<typename _Token, observer_c _Observer, observer_c _Object>
		requires std::is_class_v<_Token>
		struct meta<_Token, _Observer, _Object> final{
			static constexpr meta_e mode = meta_e::meta_obj;
			using token = _Token;
			using observer = _Observer;
			using object = _Object;
		};
		
		template<typename _Type>
		concept meta_c = fake::trait_v<meta, std::remove_cvref_t<_Type>>;
		
		template<meta_c... _Meta> struct info final{};
		
		template<typename _Type>
		concept info_c = fake::trait_v<info, std::remove_cvref_t<_Type>>;
		
		namespace detail
		{
			
			template<fake::tuple_c _Sequence>
			consteval fake::pack_c auto make_info(){
				constexpr std::size_t size = std::tuple_size_v<_Sequence>;
				
				if constexpr(size == 0)
					return fake::pack_v<info<>>;
				else if constexpr(size == 1)
					return fake::pack_v<info<meta<std::tuple_element_t<0, _Sequence>, results<>, args<>>>>;
				else
					return []<std::size_t... _Index>(std::index_sequence<_Index...>){
						return fake::pack_v<
							info<
								meta<
									std::tuple_element_t<0, _Sequence>,
									results<std::tuple_element_t<1, _Sequence>>,
									args<>
								>,
								meta<
									std::tuple_element_t<_Index + 1, _Sequence>,
									results<std::tuple_element_t<_Index + 2, _Sequence>>,
									args<std::tuple_element_t<_Index, _Sequence>>
								>
								...,
								meta<
									std::tuple_element_t<size - 1, _Sequence>,
									results<>,
									args<std::tuple_element_t<size - 2, _Sequence>>
								>
							>
						>;
					}(std::make_index_sequence<size - 2>());
			}
			
			template<info_c... _Parallel>
			consteval fake::pack_c auto parallel_info(){
				return []<meta_c... _Meta>(fake::type_package<std::tuple<_Meta...>>){
					return fake::pack_v<info<_Meta...>>;
				}(fake::tuple::concat_v<fake::tuple::make_t<_Parallel>...>);
			}
			
		}
		
		template<typename... _Tokens>
		using sequence = fake::take_t<detail::make_info<std::tuple<std::remove_cvref_t<_Tokens>...>>()>;
		
		template<info_c... _Parallel>
		using parallel = fake::take_t<detail::parallel_info<std::remove_cvref_t<_Parallel>...>()>;
		
	}
	
	namespace top = execution::topology;
	
	namespace exception
	{
		
		namespace detail::acyclic
		{
			
			struct recur : public std::exception{
				const char* what() const noexcept override{return "'acyclic::recur_inline_invoke()' previous error";}
			};
			
		}
		
		struct acyclic : public std::exception
		{
			enum struct invoke_interface : std::size_t{
				await_suspend,
				await_resume,
				await_aspect
			};
			
			const char* what() const noexcept override{
				switch(interface_type){
					case invoke_interface::await_suspend:
						return "'delegate.await_suspend(_handle)' not ready";
					break;
					case invoke_interface::await_resume:
						return "'delegate.await_resume()' not ready";
					break;
					case invoke_interface::await_aspect:
						return "'delegate.await_aspect()' invoked after scheduling";
					break;
				};
				
				return "unknown exception";
			}
			
			acyclic(invoke_interface _interface): interface_type(_interface){}
			
			const invoke_interface interface_type;
		};
		
	}
	
	namespace custom
	{
		
		template<execution::topology::info_c, execution::topology::info_c, template<typename...> typename>
		struct acyclic;
		
		namespace tool::acyclic
		{
			
			namespace detail
			{
				
				template<typename>
				struct helper{};
				
				template<template<typename...> typename _Type, typename... _Types>
				struct helper<_Type<_Types...>>
				{
					template<typename _Lambda, typename _Merge>
					static consteval auto for_each(_Lambda &&_lambda, _Merge &&_merge){
						return _merge(_lambda(fake::pack_v<_Types>)...);
					}
				};
				
			}
			
			template<typename _Type>
			consteval auto for_each(auto &&_lambda, auto &&_merge)
			{
				return detail::helper<_Type>::for_each(
					std::forward<decltype(_lambda)>(_lambda),
					std::forward<decltype(_merge)>(_merge)
				);
			}
			
			template<typename _Type> struct promise;
			
			template<typename _Type>
			struct coroutine : std::coroutine_handle<promise<_Type>>{
				using promise_type = struct promise<_Type>;
				auto get() const{return std::coroutine_handle<promise<_Type>>::promise().get();}
			};
			
			template<typename _Type>
			struct promise{
				coroutine<_Type> get_return_object(){return {coroutine<_Type>::from_promise(*this)};}
				std::suspend_never initial_suspend() const noexcept{return {};}
				std::suspend_never final_suspend() const noexcept{return {};}
				template<typename _Value>
				std::suspend_always yield_value(_Value &&_value) noexcept{
					value.emplace(std::forward<_Value>(_value));
					return {};
				}
				std::suspend_always yield_value(const std::nullopt_t &_null) noexcept{
					value.reset();
					return {};
				}
				
				void return_void() const noexcept{}
				void unhandled_exception() const{
					auto eptr = std::current_exception();
					if(eptr)
						std::rethrow_exception(eptr);
				}
				auto get() const{return value.value();}
				
			private:
				std::optional<_Type> value;
			};
			
			template<>
			struct promise<void>{
				coroutine<void> get_return_object(){return {coroutine<void>::from_promise(*this)};}
				std::suspend_never initial_suspend() const noexcept{return {};}
				std::suspend_never final_suspend() const noexcept{return {};}
				std::suspend_always yield_value(const std::nullopt_t &_null) noexcept{return {};}
				
				void return_void() const noexcept{}
				void unhandled_exception() const{
					auto eptr = std::current_exception();
					if(eptr)
						std::rethrow_exception(eptr);
				}
				void get() const{}
			};
			
			struct executor_t{
				template<typename _Token>
				static void execute(std::convertible_to<std::function<void()>> auto &&_delegate){
					std::forward<decltype(_delegate)>(_delegate)();
				}
				
				static void execute(std::convertible_to<std::function<void()>> auto &&_delegate){
					std::forward<decltype(_delegate)>(_delegate)();
				}
			};
			
			constexpr executor_t executor;
			
			// carry 'fake::functor_info'. 
			template<execution::topology::meta_c _Meta, typename _Info>
			struct functor final{
				using meta = _Meta;
				// could be awaiter, result or tuple. 
				using info = _Info;
			};
			
			template<typename _Token, typename _Type, fake::atomic::ultimatum::value_type _Value>
			struct relay final{
				using token_t = _Token;
				using type_t = _Type;
				static constexpr fake::atomic::ultimatum::value_type depend = _Value;
				std::optional<type_t> storage;
				fake::atomic::ultimatum flag{depend};
			};
			
			template<typename _Token, fake::atomic::ultimatum::value_type _Value>
			struct relay<_Token, void, _Value> final{
				using token_t = _Token;
				using type_t = void;
				static constexpr fake::atomic::ultimatum::value_type depend = _Value;
				fake::atomic::ultimatum flag{depend};
			};
			
			template<typename _Depend, typename _Relay, typename _Functors, typename _Entries, typename _Resume>
			struct deduction final{
				using depend_t = _Depend;
				using relay_t = _Relay;
				using functors_t = _Functors;
				using entries_t = _Entries;
				using resume_t = _Resume;
			};
			
			template<execution::topology::meta_c _Meta, typename _Deducer>
			struct aspection final{
				using meta = _Meta;
				using deducer = _Deducer;
				typename deducer::relay_t vars;
			};
			
			template<typename _Deducer, fake::tuple_c _Aspections>
			struct deduce final{
				using deducer = _Deducer;
				using aspects = _Aspections;
			};
			
			struct guard_t{
				guard_t(auto _handle, auto &_work, auto &_atomic): handle(_handle), work(_work), atomic(_atomic){
					work.produce();
				}
				
				template<typename _Guard>
				requires std::same_as<guard_t, std::remove_cvref_t<_Guard>>
				guard_t(_Guard &&_guard): handle(_guard.handle), work(_guard.work), atomic(_guard.atomic){
					static_assert(
						std::is_rvalue_reference_v<decltype(_guard)>,
						"\e[33;40merror<fake::acyclic::guard_t>: copy constructor instantiated.\e[0m"
					);
					_guard.valid = false;
					valid = true;
				}
				
				~guard_t(){
					if(valid && work.consume() == fake::atomic::ultimatum::signal::trigger){
						atomic.produce();
						
						if(handle)
							handle();
					}
				}
				
			private:
				bool valid = true;
				std::coroutine_handle<> handle;
				fake::atomic::ultimatum &work;
				fake::atomic::ultimatum &atomic;
			};
			
			template<typename _Relay, typename _Resume>
			struct instance_t final{
			private:
				using relay_t = _Relay;
				using resume_t = _Resume;
				
				struct callee_t final{
					relay_t &vars;
					std::coroutine_handle<> &handle;
					std::coroutine_handle<> &self;
					fake::atomic::ultimatum &work;
					fake::atomic::ultimatum &use;
					fake::atomic::ultimatum &invoke_await_suspend;
					fake::atomic::ultimatum &invoke_await_resume;
				};
				
				struct aspect_t final{
					std::coroutine_handle<> &handle;
					fake::atomic::ultimatum &delegate;
					fake::atomic::ultimatum &preempt;
					fake::atomic::ultimatum &work;
					fake::atomic::ultimatum &atomic;
				};
				
				instance_t(
					relay_t &_vars,
					std::coroutine_handle<> &_callee_handle,
					std::coroutine_handle<> &_self,
					fake::atomic::ultimatum &_callee_work,
					fake::atomic::ultimatum &_use,
					fake::atomic::ultimatum &_suspend,
					fake::atomic::ultimatum &_resume,
					std::coroutine_handle<> &_aspect_handle,
					fake::atomic::ultimatum &_delegate,
					fake::atomic::ultimatum &_preempt,
					fake::atomic::ultimatum &_aspect_work,
					fake::atomic::ultimatum &_atomic
				) noexcept:
					callee(_vars, _callee_handle, _self, _callee_work, _use, _suspend, _resume),
					aspect(_aspect_handle, _delegate, _preempt, _aspect_work, _atomic)
				{}
				
			public:
				instance_t(const instance_t &_e) noexcept: callee(_e.callee), aspect(_e.aspect){}
				instance_t(instance_t &&_e) noexcept: callee(_e.callee), aspect(_e.aspect){}
				~instance_t(){}
				
				constexpr bool await_ready() const noexcept{return false;}
				
				void await_aspect(std::coroutine_handle<> _handle) const{
					if(aspect.delegate.consume() == fake::atomic::ultimatum::signal::overflow)
						throw fake::exception::acyclic{fake::exception::acyclic::invoke_interface::await_aspect};
					aspect.handle = _handle;
					aspect.handle = [](auto _handle, auto &_use, auto _self)->tool::acyclic::coroutine<void>{
						_use.produce();
						
						co_yield std::nullopt;
						
						if(_handle)
							_handle();
						
						if(_use.consume() == fake::atomic::ultimatum::signal::trigger)
							_self();
					}(_handle, callee.use, callee.self);
				}
				
				void await_suspend(std::coroutine_handle<> _handle) const{
					if(callee.invoke_await_suspend.consume() == fake::atomic::ultimatum::signal::overflow)
						throw fake::exception::acyclic{fake::exception::acyclic::invoke_interface::await_suspend};
					
					aspect.delegate.consume();
					aspect.preempt.produce();
					aspect.work.produce();
					callee.handle = [](
						auto _handle,
						auto &_preempt,
						auto &_work,
						auto &_aspcet,
						auto &_use,
						auto _self
					)->tool::acyclic::coroutine<void>{
						_use.produce();
						
						co_yield std::nullopt;
						
						// 'await_resume()' would be invoked in '_handle()' by design. 
						if(_handle)
							_handle();
						
						if(
							_preempt.consume() == fake::atomic::ultimatum::signal::trigger &&
							_work.consume() == fake::atomic::ultimatum::signal::trigger &&
							_aspcet
						)
							_aspcet();
						
						if(_use.consume() == fake::atomic::ultimatum::signal::trigger)
							_self();
					}(_handle, aspect.preempt, aspect.work, aspect.handle, callee.use, callee.self);
					
					guard_t guard_callee{callee.handle, callee.work, callee.invoke_await_resume};
					guard_t guard_aspect{aspect.handle, aspect.work, aspect.atomic};
					
					callee.self();
				}
				
				// should NOT be invoked directly. 
				// would be invoked inside of the invocation the '_handle' registered via 'await_suspend()' by design. 
				inline decltype(auto) await_resume() const{
					if(callee.invoke_await_resume.consume() == fake::atomic::ultimatum::signal::overflow)
						throw fake::exception::acyclic{fake::exception::acyclic::invoke_interface::await_resume};
					
					return [&vars = callee.vars]<typename... _Vars>(
						fake::type_package<std::tuple<_Vars...>>
					)->decltype(auto){
						return std::forward_as_tuple(std::move(std::get<_Vars>(vars).storage.value())...);
					}(fake::pack_v<resume_t>);
				}
				
			private:
				template<execution::topology::info_c, execution::topology::info_c, template<typename...> typename>
				friend struct fake::custom::acyclic;
				
				callee_t callee;
				aspect_t aspect;
			};
			
		}
		
		template<
			execution::topology::info_c _Topology,
			execution::topology::info_c _Decorate,
			template<typename...> typename _Coroutine
		>
		struct acyclic final
		{
			template<pack_c _Callee, pack_c _Aspect>
			struct delegate final{
			private:
				template<typename... _Types>
				using coroutine_t = _Coroutine<_Types...>;
				
				struct callee_t{
					using type_t = std::remove_cvref_t<_Callee>;
					using origin_t = _Callee;
					using storage_t = typename type_t::storage_t;
					static constexpr std::size_t size = std::tuple_size_v<storage_t>;
				};
				
				struct aspect_t{
					using type_t = std::remove_cvref_t<_Aspect>;
					using origin_t = _Aspect;
					using storage_t = typename type_t::storage_t;
					static constexpr std::size_t size = std::tuple_size_v<storage_t>;
				};
				
				template<fake::tuple_c _Tuple>
				static consteval auto normalize_aspect() noexcept{
					return fake::tuple::transform_v<
						_Tuple,
						[]<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
							if constexpr(_Meta::mode == execution::topology::meta_e::meta_obs)
								return fake::pack_v<
									execution::topology::meta<
										typename _Meta::token,
										typename _Meta::results,
										execution::topology::args<>
									>
								>;
							else
								return fake::pack_v<_Meta>;
						}
					>;
				}
				
				using tuple_top = fake::tuple::make_t<std::remove_cvref_t<_Topology>>;
				using tuple_dec = fake::tuple::make_t<std::remove_cvref_t<_Decorate>>;
				using tuple_asp = fake::take_t<normalize_aspect<tuple_dec>()>;
				
				template<typename _Type>
				static decltype(auto) select_scheduler(auto &_storage){
					using storage_t = std::remove_cvref_t<decltype(_storage)>;
					
					using private_sched_t = fake::tuple::select_if_t<
						storage_t,
						[]<pass_c _Pass>(fake::type_package<_Pass>){
							using token_t = typename _Pass::token_t;
							
							constexpr bool is_sched_token = requires(token_t _e){
								requires fake::branch{
									[](fake::acyclic::token::sched<void>){return false;},
									[]<typename _Token>(fake::acyclic::token::sched<_Token>){return true;},
									[](...){return false;}
								}(_e);
							};
							
							if constexpr(is_sched_token)
								return std::derived_from<_Type, typename token_t::schedule_type>;
							else
								return false;
						}
					>;
					
					using default_sched_t = fake::tuple::select_if_t<
						storage_t,
						[]<pass_c _Pass>(fake::type_package<_Pass>){
							using token_t = typename _Pass::token_t;
							
							return requires(token_t _e){
								requires fake::branch{
									[](fake::acyclic::token::sched<void>){return true;},
									[](...){return false;}
								}(_e);
							};
						}
					>;
					
					constexpr std::size_t private_size = std::tuple_size_v<private_sched_t>;
					constexpr std::size_t default_size = std::tuple_size_v<default_sched_t>;
					
					static_assert(
						private_size || default_size,
						"\e[33;40merror<fake::acyclic>: selecting scheduler failed.\e[0m"
					);
					
					if constexpr(private_size){
						constexpr auto select_v = []<pass_c ... _Pass>(fake::type_package<std::tuple<_Pass...>>){
							return decltype(
								fake::branch{
									[](...){return fake::pack_v<void>;},
									[](typename _Pass::token_t::schedule_type){return fake::pack_v<_Pass>;}...
								}(std::declval<_Type>())
							){};
						}(fake::pack_v<private_sched_t>);
						
						using select_t = fake::take_t<select_v>;
						
						static_assert(
							std::is_same_v<select_t, void> == false,
							"\e[33;40merror<fake::acyclic>: the selection result of scheduler is ambiguous.\e[0m"
						);
						
						using expr_t = typename std::remove_cvref_t<decltype(std::get<select_t>(_storage))>::value_t;
						
						static_assert(
							fake::acyclic::concepts::scheduler_expr_c<expr_t, typename select_t::token_t>,
							"\e[33;40merror<fake::acyclic>: 'scheduler_expr_c<expr, token>' is NOT satisfaction.\e[0m"
						);
						
						return std::get<select_t>(_storage);
					}
					else if constexpr(default_size){
						static_assert(
							default_size < 2,
							"\e[33;40merror<fake::acyclic>: the selection result of scheduler is ambiguous.\e[0m"
						);
						
						using select_t = std::tuple_element_t<0, default_sched_t>;
						using expr_t = typename std::remove_cvref_t<decltype(std::get<select_t>(_storage))>::value_t;
						
						static_assert(
							fake::acyclic::concepts::scheduler_expr_c<expr_t, typename select_t::token_t>,
							"\e[33;40merror<fake::acyclic>: 'scheduler_expr_c<expr, token>' is NOT satisfaction.\e[0m"
						);
						
						return std::get<select_t>(_storage);
					}
				}
				
				template<typename _Type>
				static decltype(auto) select_exception_handler(auto &_storage){
					using storage_t = std::remove_cvref_t<decltype(_storage)>;
					
					using private_handler_t = fake::tuple::select_if_t<
						storage_t,
						[]<pass_c _Pass>(fake::type_package<_Pass>){
							using token_t = typename _Pass::token_t;
							
							constexpr bool is_spare_token = fake::branch{
								[](fake::type_package<fake::acyclic::token::spare<void>>){return false;},
								[]<typename _Token>(fake::type_package<fake::acyclic::token::spare<_Token>>){
									return true;
								},
								[]<typename _Any>(fake::type_package<_Any>){return false;}
							}(fake::pack_v<token_t>);
							
							if constexpr(is_spare_token)
								return std::derived_from<_Type, typename token_t::handle_type>;
							else
								return false;
						}
					>;
					
					using default_handler_t = fake::tuple::select_if_t<
						storage_t,
						[]<pass_c _Pass>(fake::type_package<_Pass>){
							using token_t = typename _Pass::token_t;
							
							return fake::branch{
								[](fake::type_package<fake::acyclic::token::spare<void>>){return true;},
								[]<typename _Any>(fake::type_package<_Any>){return false;}
							}(fake::pack_v<token_t>);
						}
					>;
					
					constexpr std::size_t private_size = std::tuple_size_v<private_handler_t>;
					constexpr std::size_t default_size = std::tuple_size_v<default_handler_t>;
					
					if constexpr(private_size){
						constexpr auto select_v = []<pass_c ... _Pass>(fake::type_package<std::tuple<_Pass...>>){
							return decltype(
								fake::branch{
									[](...){return fake::pack_v<void>;},
									[](typename _Pass::token_t::handle_type){return fake::pack_v<_Pass>;}...
								}(std::declval<_Type>())
							){};
						}(fake::pack_v<private_handler_t>);
						
						using select_t = fake::take_t<select_v>;
						
						static_assert(
							std::is_same_v<select_t, void> == false,
							"\e[33;40merror<fake::acyclic>: "
							"the selection result of exception handler is ambiguous.\e[0m"
						);
						
						return std::get<select_t>(_storage);
					}
					else if constexpr(default_size == 1){
						static_assert(
							default_size < 2,
							"\e[33;40merror<fake::acyclic>: "
							"the selection result of exception handler is ambiguous.\e[0m"
						);
						
						using select_t = std::tuple_element_t<0, default_handler_t>;
						return std::get<select_t>(_storage);
					}
				}
				
				template<typename _Type>
				static decltype(auto) dispatch_exception(auto &_storage){
					return select_exception_handler<_Type>(_storage)();
				}
				
				// remove inline functors from topology. 
				template<fake::tuple_c _Graph>
				static consteval auto deduce_inline() noexcept{
					return fake::tuple::scale_v<
						_Graph,
						[]<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
							using results_t = typename _Meta::results;
							// being not 'await' and having only one result node, may inlined, check other conditions. 
							// 'split' may not be inlined as well. 
							if constexpr(
								std::derived_from<typename _Meta::token, fake::acyclic::token::await> == false &&
								std::derived_from<typename _Meta::token, fake::acyclic::token::split> == false &&
								fake::element_size_v<results_t> == 1
							){
								using result_t = fake::element_index_t<0, results_t>;
								
								constexpr auto pick = []<execution::topology::meta_c _Result>(
									fake::type_package<_Result>
								){
									return std::is_same_v<result_t, typename _Result::token>;
								};
								
								// having only one arg node, may inlined, check other conditions. 
								using args_t = typename fake::tuple::find_if_t<_Graph, pick>::args;
								if constexpr(fake::element_size_v<args_t> == 1){
									using arg_t = fake::element_index_t<0, args_t>;
									
									using storage_t = std::add_lvalue_reference_t<
										std::conditional_t<
											std::is_same_v<tuple_top, _Graph>,
											typename callee_t::storage_t,
											typename aspect_t::storage_t
										>
									>;
									
									using arg_scheduler_t = decltype(
										select_scheduler<arg_t>(std::declval<storage_t>())
									);
									
									using result_scheduler_t = decltype(
										select_scheduler<result_t>(std::declval<storage_t>())
									);
									
									// apply the inline lopping if the pair of nodes shares the same scheduler. 
									if constexpr(std::is_same_v<arg_scheduler_t, result_scheduler_t>)
										return fake::pack_v<std::tuple<>>;
									else
										return fake::pack_v<std::tuple<typename _Meta::token>>;
								}
								else{
									return fake::pack_v<std::tuple<typename _Meta::token>>;
								}
							}
							else{
								return fake::pack_v<std::tuple<typename _Meta::token>>;
							}
						}
					>;
				}
				
				// pick entries from topology. 
				template<fake::tuple_c _Graph>
				static consteval auto deduce_entries() noexcept{
					return fake::tuple::scale_v<
						_Graph,
						[]<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
							if constexpr(fake::element_size_v<typename _Meta::args> == 0)
								return fake::pack_v<std::tuple<typename _Meta::token>>;
							else
								return fake::pack_v<std::tuple<>>;
						}
					>;
				}
				
				// pick results from topology. 
				template<fake::tuple_c _Graph>
				static consteval auto deduce_results() noexcept{
					return fake::tuple::scale_v<
						_Graph,
						[]<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
							if constexpr(fake::element_size_v<typename _Meta::results> == 0)
								return fake::pack_v<std::tuple<typename _Meta::token>>;
							else
								return fake::pack_v<std::tuple<>>;
						}
					>;
				}
				
				// reduce by tokens from 'std::tuple<meta>'. 
				template<execution::topology::meta_c... _Metas, typename... _Tokens>
				static consteval auto meta_reduce(
					fake::type_package<std::tuple<_Metas...>>,
					fake::type_package<std::tuple<_Tokens...>>
				) noexcept{
					constexpr auto erase = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
						constexpr auto contains = []<typename _Token>(fake::type_package<_Token>){
							return std::is_same_v<typename _Meta::token, _Token>;
						};
						return fake::tuple::contains_v<std::tuple<_Tokens...>, contains>;
					};
					return fake::tuple::erase_if_v<std::tuple<_Metas...>, erase>;
				}
				
				template<fake::acyclic::concepts::awaiter_c _Awaiter, typename _Result>
				struct awaiter_info{using awaiter_t = _Awaiter; using result_t = _Result;};
				
				template<typename _Expr, typename _Token>
				requires std::derived_from<_Token, fake::acyclic::token::await> &&
					fake::acyclic::concepts::co_await_expr_c<_Expr, _Token>
				static consteval auto deduce_await() noexcept{
					namespace concepts = fake::acyclic::concepts;
					
					constexpr auto awaitable = []<typename _Ex, typename _To>(
						fake::type_package<_Ex>,
						fake::type_package<_To>
					){
						using token_t = _To;
						using expr_t = _Ex;
						
						if constexpr(concepts::token_with_promise_c<token_t>){
							using promise_t = token_t::promise_type;
							
							if constexpr(concepts::expr_to_awaitable_via_promise_c<token_t, expr_t, promise_t>){
								if constexpr(requires{token_t::scheduler_inject;}){
									if constexpr(token_t::scheduler_inject)
										return fake::pack_v<
											decltype(
												std::declval<promise_t>().await_transform(
													fake::pass<tool::acyclic::executor_t>(std::declval<expr_t>())
												)
											)
										>;
									else
										return fake::pack_v<
											decltype(std::declval<promise_t>().await_transform(std::declval<expr_t>()))
										>;
								}
								else{
									return fake::pack_v<
										decltype(std::declval<promise_t>().await_transform(std::declval<expr_t>()))
									>;
								}
							}
							else if constexpr(concepts::expr_to_awaitable_via_as_is_c<expr_t>){
								return fake::pack_v<expr_t>;
							}
						}
						else if constexpr(concepts::expr_to_awaitable_via_as_is_c<expr_t>){
							return fake::pack_v<expr_t>;
						}
					};
					
					using awaitable_t = fake::take_t<awaitable(fake::pack_v<_Expr>, fake::pack_v<_Token>)>;
					
					constexpr auto awaiter = []<concepts::awaitable_c _Aw>(fake::type_package<_Aw>){
						using awaitable_t = _Aw;
						
						if constexpr(concepts::awaitable_to_awaiter_via_member_operator_co_await_c<awaitable_t>)
							return pack_v<decltype(std::declval<awaitable_t>().operator co_await())>;
						else if constexpr(concepts::awaitable_to_awaiter_via_global_operator_co_await_c<awaitable_t>)
							return pack_v<
								decltype(operator co_await(static_cast<awaitable_t&&>(std::declval<awaitable_t>)))
							>;
						else if constexpr(concepts::awaitable_to_awaiter_via_as_is_c<awaitable_t>)
							return pack_v<awaitable_t>;
					};
					
					using awaiter_t = fake::take_t<awaiter(fake::pack_v<awaitable_t>)>;
					
					if constexpr(requires{typename _Token::promise_type;} && requires{_Token::scheduler_inject;})
						if constexpr(_Token::scheduler_inject)
							static_assert(
								requires(awaiter_t &_awaiter, tool::acyclic::executor_t &_executor){
									_awaiter.await_inject(_executor);
								},
								"\e[33;40merror<fake::acyclic>: 'scheduler_inject' token, awaiter without method.\e[0m"
							);
					
					return awaiter_info<awaiter_t, decltype(std::declval<awaiter_t>().await_resume())>{};
				}
				
				template<fake::tuple_c _Graph, typename _TokenArg, typename _TokenNode>
				static consteval std::size_t deduce_distribute_index() noexcept{
					constexpr auto meta = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
						return std::is_same_v<typename _Meta::token, _TokenArg>;
					};
					using meta_t = fake::tuple::find_if_t<_Graph, meta>;
					
					constexpr auto result_to_tuple = []<typename... _Result>(
						fake::type_package<execution::topology::results<_Result...>>
					){
						return fake::pack_v<std::tuple<_Result...>>;
					};
					using results_t = fake::take_t<result_to_tuple(fake::pack_v<typename meta_t::results>)>;
					
					constexpr auto index = []<typename _Token>(fake::type_package<_Token>){
						return std::is_same_v<_Token, _TokenNode>;
					};
					return fake::tuple::match_index_t<results_t, index>::value;
				}
				
				// transform parameters. 
				template<typename _Node>
				static decltype(auto) transform(auto &&_arg) noexcept{
					using node_t = _Node;
					using param_t = decltype(_arg);
					constexpr bool fixed_v = std::derived_from<node_t, fake::acyclic::token::fixed>;
					
					if constexpr(fake::tuple_c<param_t> && fixed_v == false)
						return std::forward<decltype(_arg)>(_arg);
					else
						return std::forward_as_tuple(std::forward<decltype(_arg)>(_arg));
				};
				
				template<typename _From, typename _To>
				static consteval auto select_aspects() noexcept{
					constexpr auto filter = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
						if constexpr(_Meta::mode == execution::topology::meta_e::meta_obs){
							using from_t = typename _Meta::observer::from;
							using to_t = typename _Meta::observer::to;
							
							return (std::is_same_v<from_t, void> || std::derived_from<_From, from_t>) &&
								(std::is_same_v<to_t, void> || std::derived_from<_To, to_t>);
						}
						else{
							return false;
						}
					};
					
					return fake::tuple::select_if_v<tuple_dec, filter>;
				}
				
				template<typename _From, typename _To, typename _Type>
				static consteval auto deduce_aspect() noexcept{
					constexpr auto filter = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
						if constexpr(_Meta::mode == execution::topology::meta_e::meta_obs){
							using from_t = typename _Meta::observer::from;
							using to_t = typename _Meta::observer::to;
							
							return (std::is_same_v<from_t, void> || std::derived_from<_From, from_t>) &&
								(std::is_same_v<to_t, void> || std::derived_from<_To, to_t>);
						}
						else{
							return false;
						}
					};
					
					using filter_t = fake::tuple::select_if_t<tuple_dec, filter>;
					
					constexpr auto scale = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
						// aspect node. 
						using node_t = typename _Meta::token;
						using meta_t = execution::topology::meta<
							node_t,
							typename _Meta::observer,
							execution::topology::observer<_From, _To>
						>;
						
						constexpr auto query_pass = []<pass_c _Pass>(fake::type_package<_Pass>){
							return std::is_same_v<typename _Pass::token_t, node_t>;
						};
						using pass_t = fake::tuple::find_if_t<typename aspect_t::storage_t, query_pass>;
						using lambda_t = typename pass_t::value_t;
						
						constexpr auto broker = []<typename _Arg>(_Arg &&_arg)->decltype(auto){
							return transform<node_t>(std::forward<_Arg>(_arg));
						};
						
						using transform_t = decltype(broker);
						
						// remove functors return 'void'. 
						using arg_tuple_t = std::tuple<_Type>;
						constexpr auto remove_void = []<typename _ArgType>(fake::type_package<_ArgType>){
							return std::is_same_v<_ArgType, void> == false;
						};
						using shrink_tuple_t = fake::tuple::select_if_t<arg_tuple_t, remove_void>;
						constexpr auto arg_broker = []<typename _ArgType>(fake::type_package<_ArgType>){
							return fake::pack_v<std::invoke_result_t<transform_t, _ArgType&>>;
						};
						
						// there is a layer of 'std::tuple<...>' wrapper comes from 'transform_t'
						using args_t = fake::tuple::concat_t<
							std::tuple<fake::type_package<meta_t>>,
							fake::tuple::scale_t<shrink_tuple_t, arg_broker>
						>;
						
						constexpr bool applicable = fake::applicable_c<lambda_t, args_t>;
						
						static_assert(
							applicable,
							"\e[33;40merror<fake::acyclic>: aspect node functor invocation failed.\e[0m"
						);
						
						if constexpr(applicable){
							constexpr auto add_ref = []<typename _Arg>(fake::type_package<_Arg>){
								return fake::pack_v<std::add_lvalue_reference_t<_Arg>>;
							};
							using args_ref_t = fake::tuple::transform_t<args_t, add_ref>;
							using retn_t = fake::apply_result_t<lambda_t, args_ref_t>;
							constexpr auto tier = []<typename... _Args>(fake::type_package<std::tuple<_Args...>>){
								return fake::pack_v<retn_t(fake::type_package<meta_t>, _Args&...)>;
							};
							using func_t = fake::take_t<tier(fake::pack_v<args_t>)>;
							
							return fake::pack_v<std::tuple<tool::acyclic::functor<meta_t, fake::functor_info<func_t>>>>;
						}
						else{
							return fake::pack_v<std::tuple<>>;
						}
					};
					
					return fake::tuple::scale_v<filter_t, scale>;
				}
				
				// 'std::pair<...>' could not form instance with void. 
				template<typename _First, typename _Second>
				struct dummy_pair{
					using first_type = _First;
					using second_type = _Second;
				};
				
				template<
					fake::tuple_c _Graph,
					typename... _Functors,
					typename _Token,
					execution::topology::results_c _Results,
					typename... _Args
				>
				static consteval auto deduce_trace(
					fake::type_package<std::tuple<_Functors...>>,
					fake::type_package<execution::topology::meta<_Token, _Results, execution::topology::args<_Args...>>>
				) noexcept{
					// self meta for pack a new 'tool::acyclic::functor<meta, info, aspects>' if 'info' deducible. 
					using meta_t = execution::topology::meta<_Token, _Results, execution::topology::args<_Args...>>;
					
					// query info of each args passes. 
					constexpr auto rely = []<typename _Arg>(fake::type_package<_Arg>){
						constexpr auto query = []<typename _Functor>(fake::type_package<_Functor>){
							return std::is_same_v<typename _Functor::meta::token, _Arg>;
						};
						return fake::tuple::find_if_v<std::tuple<_Functors...>, query>;
					};
					using raw_t = fake::tuple::transform_t<std::tuple<_Args...>, rely>;
					
					// check if every query result deduced. 
					constexpr auto invalid = []<typename _Functor>(fake::type_package<_Functor>){
						return std::is_same_v<_Functor, fake::null_t>;
					};
					constexpr bool deducible = fake::tuple::contains_v<raw_t, invalid> == false;
					
					// all 'fake::functor_info' of args deduced. 
					if constexpr(deducible){
						using query_t = raw_t;
						
						constexpr auto result = tool::acyclic::for_each<raw_t>(
							[]<typename _Func>(fake::type_package<_Func>){
								// extract every return type from each args. 
								using node_t = _Token;
								using arg_node_t = typename _Func::meta::token;
								using arg_result_t = typename _Func::info::retn;
								
								// parse type of 'awaiter->await_resume()'. 
								using param_t = fake::take_t<
									[]{
										if constexpr(std::derived_from<arg_node_t, fake::acyclic::token::await>)
											return fake::pack_v<
												typename decltype(deduce_await<arg_result_t, arg_node_t>())::result_t
											>;
										else
											return fake::pack_v<arg_result_t>;
									}()
								>;
								
								// parse type of 'std::tuple_element<...>'. 
								if constexpr(
									std::derived_from<arg_node_t, fake::acyclic::token::dupli> ||
									fake::is_tuple_v<param_t> == false
								){
									if constexpr(std::is_same_v<tuple_top, _Graph>){
										constexpr auto aspects = deduce_aspect<arg_node_t, node_t, param_t>();
										
										return fake::pack_v<dummy_pair<param_t, fake::take_t<aspects>>>;
									}
									else{
										return fake::pack_v<dummy_pair<param_t, std::tuple<>>>;
									}
								}
								else{
									constexpr std::size_t size = fake::element_size_v<typename _Func::meta::results>;
									static_assert(
										size == std::tuple_size_v<param_t>,
										"\e[33;40merror<fake::acyclic>: "
										"distributed results do NOT match the number of subsequent nodes.\e[0m"
									);
									
									constexpr std::size_t index = deduce_distribute_index<_Graph, arg_node_t, node_t>();
									using param_t = std::tuple_element_t<index, param_t>;
									
									if constexpr(std::is_same_v<tuple_top, _Graph>){
										constexpr auto aspects = deduce_aspect<arg_node_t, node_t, param_t>();
										
										return fake::pack_v<dummy_pair<param_t, fake::take_t<aspects>>>;
									}
									else{
										return fake::pack_v<dummy_pair<param_t, std::tuple<>>>;
									}
								}
							},
							[]<typename... _Params>(fake::type_package<_Params>...){
								using node_t = _Token;
								
								// make gcc happy. 
								// the deduction of type 'pass_t' would better NOT be applied outside this lambda. 
								// capturing the type 'pass_t' into this lambda may cause a segmentation fault of gcc. 
								constexpr auto query_pass = []<pass_c _Pass>(fake::type_package<_Pass>){
									return std::is_same_v<typename _Pass::token_t, node_t>;
								};
								using pass_t = fake::tuple::find_if_t<
									std::conditional_t<
										std::is_same_v<tuple_top, _Graph>,
										typename callee_t::storage_t,
										typename aspect_t::storage_t
									>,
									query_pass
								>;
								
								// merge each previous return type as args. 
								// then deduce the invoke result of current pass. 
								using lambda_t = typename pass_t::value_t;
								
								constexpr auto broker = []<typename _Arg>(_Arg &&_arg)->decltype(auto){
									return transform<node_t>(std::forward<_Arg>(_arg));
								};
								
								using transform_t = decltype(broker);
								
								// remove functors return 'void'. 
								using args_tuple_t = std::tuple<typename _Params::first_type...>;
								constexpr auto remove_void = []<typename _ArgType>(fake::type_package<_ArgType>){
									return std::is_same_v<_ArgType, void> == false;
								};
								using shrink_tuple_t = fake::tuple::select_if_t<args_tuple_t, remove_void>;
								
								constexpr auto args_broker = []<typename _ArgType>(fake::type_package<_ArgType>){
									return fake::pack_v<std::invoke_result_t<transform_t, _ArgType&>>;
								};
								constexpr auto split_v = fake::tuple::transform_v<shrink_tuple_t, args_broker>;
								
								// there is a layer of 'std::tuple<...>' wrapper comes from 'transform_t'
								using args_t = fake::tuple::scale_t<shrink_tuple_t, args_broker>;
								
								constexpr bool applicable = fake::applicable_c<lambda_t, args_t>;
								
								static_assert(
									applicable,
									"\e[33;40merror<fake::acyclic>: node functor invocation failed.\e[0m"
								);
								
								if constexpr(applicable){
									using retn_t = fake::apply_result_t<lambda_t, args_t>;
									
									constexpr auto func_broker = []<typename... _ArgType>(
										fake::type_package<std::tuple<_ArgType...>>
									){
										return fake::pack_v<retn_t(_ArgType...)>;
									};
									
									using func_t = decltype(func_broker(split_v));
									
									if constexpr(std::is_same_v<tuple_top, _Graph>){
										using aspect_t = fake::tuple::concat_t<typename _Params::second_type...>;
										
										return fake::pack_v<std::pair<func_t, aspect_t>>;
									}
									else{
										return fake::pack_v<std::pair<func_t, std::tuple<>>>;
									}
								}
								else{
									return fake::pack_v<std::pair<fake::type_package<fake::null_t>, std::tuple<>>>;
								}
							}
						);
						
						using delegate_t = typename fake::take_t<result>::first_type::type;
						using aspect_t = typename fake::take_t<result>::second_type;
						
						// record the deduced 'tool::acyclic::functor<meta, info, aspects>'. 
						if constexpr(std::is_same_v<delegate_t, fake::null_t>)
							return fake::pack_v<std::tuple<>>;
						else
							return fake::pack_v<
								fake::tuple::concat_t<
									std::tuple<tool::acyclic::functor<meta_t, fake::functor_info<delegate_t>>>,
									aspect_t
								>
							>;
					} // NOT all 'fake::functor_info' of args deduced yet. 
					else{
						return fake::pack_v<std::tuple<>>;
					}
				}
				
				// deduce 'tool::acyclic::functor<meta, fake::functor_info, std::tuple>' from init status. 
				template<fake::tuple_c _Graph, typename... _Functors, typename... _Observers>
				static consteval auto deduce_functor(
					fake::type_package<std::tuple<_Functors...>> _collect,
					fake::type_package<std::tuple<_Observers...>> _observe,
					fake::type_package<std::tuple<>> _pending
				) noexcept{
					return std::pair{_collect, _observe};
				}
				
				template<
					fake::tuple_c _Graph,
					typename... _Functors,
					typename... _Observers,
					execution::topology::meta_c... _Metas
				>
				static consteval auto deduce_functor(
					fake::type_package<std::tuple<_Functors...>> _collect,
					fake::type_package<std::tuple<_Observers...>> _observe,
					fake::type_package<std::tuple<_Metas...>> _pending
				) noexcept{
					using collect_t = std::tuple<_Functors...>;
					using observe_t = std::tuple<_Observers...>;
					using pending_t = std::tuple<_Metas...>;
					
					constexpr auto trace = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
						return deduce_trace<_Graph>(fake::pack_v<collect_t>, fake::pack_v<_Meta>);
					};
					
					using addition_t = fake::tuple::scale_t<pending_t, trace>;
					
					constexpr auto filter_collect = []<typename _Functor>(fake::type_package<_Functor>){
						return _Functor::meta::mode == execution::topology::meta_e::meta_top;
					};
					
					using filter_collect_t = fake::tuple::select_if_t<addition_t, filter_collect>;
					constexpr auto collect = fake::tuple::concat_v<collect_t, filter_collect_t>;
					
					constexpr auto filter_observe = []<typename _Functor>(fake::type_package<_Functor>){
						return _Functor::meta::mode == execution::topology::meta_e::meta_obj;
					};
					
					using filter_observe_t = fake::tuple::select_if_t<addition_t, filter_observe>;
					constexpr auto observe = fake::tuple::concat_v<observe_t, filter_observe_t>;
					
					static_assert(
						std::tuple_size_v<addition_t> || std::is_same_v<tuple_dec, _Graph>,
						"\e[33;40merror<fake::acyclic>: node functor does NOT exist.\e[0m"
					);
					
					constexpr auto trans = []<typename _Functor>(fake::type_package<_Functor>){
						return fake::pack_v<typename _Functor::meta::token>;
					};
					
					constexpr auto remain = meta_reduce(
						fake::pack_v<pending_t>,
						fake::tuple::transform_v<addition_t, trans>
					);
					
					static_assert(
						std::tuple_size_v<fake::take_t<remain>> < std::tuple_size_v<pending_t> ||
						std::is_same_v<tuple_dec, _Graph>,
						"\e[33;40merror<fake::acyclic>: logic error, nothing reduced.\e[0m"
					);
					
					if constexpr(
						std::tuple_size_v<addition_t> &&
						std::tuple_size_v<fake::take_t<remain>> < std::tuple_size_v<pending_t>
					)
						return deduce_functor<_Graph>(collect, observe, remain);
					else
						return std::pair{_collect, _observe};
				}
				
				template<fake::tuple_c _Graph, typename... _Tokens>
				static consteval auto deduce_init(fake::type_package<std::tuple<_Tokens...>>) noexcept{
					constexpr auto init = []<typename _Token>(fake::type_package<_Token>){
						constexpr auto query_meta = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
							return std::is_same_v<typename _Meta::token, _Token>;
						};
						
						using meta_t = fake::tuple::find_if_t<_Graph, query_meta>;
						using token_t = _Token;
						
						constexpr auto query_pass = []<pass_c _Pass>(fake::type_package<_Pass>){
							return std::is_same_v<typename _Pass::token_t, _Token>;
						};
						
						using callee_storage_t = typename callee_t::storage_t;
						using functor_t = typename fake::tuple::find_if_t<callee_storage_t, query_pass>::value_t;
						
						static_assert(
							fake::deducible_c<fake::functor_info, functor_t>,
							"\e[33;40merror<fake::acyclic>: "
							"lambda functor of entry node is NOT deducible.\e[0m"
						);
						
						return fake::pack_v<tool::acyclic::functor<meta_t, fake::functor_info<functor_t>>>;
					};
					
					return fake::tuple::transform_v<std::tuple<_Tokens...>, init>;
				}
				
				template<execution::topology::meta_c... _Metas, typename... _Functors>
				static consteval auto deduce_relay(
					fake::type_package<std::tuple<_Metas...>>,
					fake::type_package<std::tuple<_Functors...>>
				) noexcept{
					using metas_t = std::tuple<_Metas...>;
					using functors_t = std::tuple<_Functors...>;
					
					constexpr auto vars = []<typename _Token, typename... _Result, typename... _Arg>(
						fake::type_package<
							execution::topology::meta<
								_Token,
								execution::topology::results<_Result...>,
								execution::topology::args<_Arg...>
							>
						>
					){
						constexpr auto pick = []<
							execution::topology::meta_c _Meta,
							typename _Info
						>(fake::type_package<tool::acyclic::functor<_Meta, _Info>>){
							using node_t = typename _Meta::token;
							using result_t = typename _Info::retn;
							
							if constexpr(std::is_same_v<node_t, _Token>){
								if constexpr(std::derived_from<node_t, fake::acyclic::token::await>){
									using origin_t = typename decltype(deduce_await<result_t, node_t>())::result_t;
									using type_t = fake::remove_rvalue_reference_t<origin_t>;
									
									return fake::pack_v<
										std::tuple<tool::acyclic::relay<node_t, type_t, sizeof...(_Arg)>>
									>;
								}
								else{
									using type_t = fake::remove_rvalue_reference_t<result_t>;
									
									return fake::pack_v<
										std::tuple<tool::acyclic::relay<node_t, type_t, sizeof...(_Arg)>>
									>;
								}
							}
							else{
								return fake::pack_v<std::tuple<>>;
							}
						};
						
						return fake::tuple::scale_v<functors_t, pick>;
					};
					
					return fake::tuple::scale_v<metas_t, vars>;
				}
				
				// find all tokens which could be shrink inlined. 
				template<
					fake::tuple_c _Graph,
					typename _Token,
					typename... _Results,
					typename... _Args,
					typename... _Inlines
				>
				static consteval auto deduce_rely(
					fake::type_package<
						execution::topology::meta<
							_Token,
							execution::topology::results<_Results...>,
							execution::topology::args<_Args...>
						>
					>,
					fake::type_package<std::tuple<_Inlines...>>
				) noexcept{
					// self meta for pack a new 'tool::acyclic::functor<meta, info>' if 'info' deducible. 
					using meta_t = execution::topology::meta<
						_Token,
						execution::topology::results<_Results...>,
						execution::topology::args<_Args...>
					>;
					using inline_t = std::tuple<_Inlines...>;
					using results_t = std::tuple<_Results...>;
					using args_t = std::tuple<_Args...>;
					
					constexpr auto replace_results = []<typename _Result>(fake::type_package<_Result>){
						constexpr auto exist = []<typename _Type>(fake::type_package<_Type>){
							return std::is_same_v<_Type, _Result>;
						};
						if constexpr(fake::tuple::contains_v<inline_t, exist>){
							return fake::pack_v<std::tuple<_Result>>;
						}
						else{
							constexpr auto deep = []<typename _Type, typename... _DeepResult, typename... _DeepArg>(
								fake::type_package<
									execution::topology::meta<
										_Type,
										execution::topology::results<_DeepResult...>,
										execution::topology::args<_DeepArg...>
									>
								>
							){
								if constexpr(std::is_same_v<_Type, _Result>)
									return fake::pack_v<std::tuple<_DeepResult...>>;
								else
									return fake::pack_v<std::tuple<>>;
							};
							return fake::pack_v<fake::tuple::scale_t<_Graph, deep>>;
						}
					};
					using recur_results_t = fake::tuple::scale_t<results_t, replace_results>;
					
					constexpr auto replace_args = []<typename _Arg>(fake::type_package<_Arg>){
						constexpr auto exist = []<typename _Type>(fake::type_package<_Type>){
							return std::is_same_v<_Type, _Arg>;
						};
						if constexpr(fake::tuple::contains_v<inline_t, exist>){
							return fake::pack_v<std::tuple<_Arg>>;
						}
						else{
							constexpr auto deep = []<typename _Type, typename... _DeepResult, typename... _DeepArg>(
								fake::type_package<
									execution::topology::meta<
										_Type,
										execution::topology::results<_DeepResult...>,
										execution::topology::args<_DeepArg...>
									>
								>
							){
								if constexpr(std::is_same_v<_Type, _Arg>)
									return fake::pack_v<std::tuple<_DeepArg...>>;
								else
									return fake::pack_v<std::tuple<>>;
							};
							return fake::pack_v<fake::tuple::scale_t<_Graph, deep>>;
						}
					};
					using recur_args_t = fake::tuple::scale_t<args_t, replace_args>;
					
					constexpr auto pack = []<typename... _Result, typename... _Arg>(
						fake::type_package<std::tuple<_Result...>>,
						fake::type_package<std::tuple<_Arg...>>
					){
						return fake::pack_v<
							execution::topology::meta<
								_Token,
								execution::topology::results<_Result...>,
								execution::topology::args<_Arg...>
							>
						>;
					}(fake::tuple::shrink_v<recur_results_t>, fake::tuple::shrink_v<recur_args_t>);
					using pack_t = fake::take_t<pack>;
					
					if constexpr(std::is_same_v<pack_t, meta_t>)
						return fake::pack_v<meta_t>;
					else
						return deduce_rely<_Graph>(pack, fake::pack_v<inline_t>);
				}
				
				// deduce 'tool::acyclic::functor<meta, fake::functor_info>' from init status. 
				template<fake::tuple_c _Graph, typename... _Inlines>
				static consteval auto deduce_depend(fake::type_package<std::tuple<_Inlines...>>) noexcept{
					using inline_t = std::tuple<_Inlines...>;
					
					constexpr auto pick = []<typename _Token>(fake::type_package<_Token>){
						constexpr auto meta = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
							return std::is_same_v<typename _Meta::token, _Token>;
						};
						
						return deduce_rely<_Graph>(fake::tuple::find_if_v<_Graph, meta>, fake::pack_v<inline_t>);
					};
					
					return fake::tuple::transform_v<inline_t, pick>;
				}
				
				// collect relay points of the result nodes. 
				// pass them as a result of 'std::forward_as_tuple(...)'. 
				template<fake::tuple_c _Relays, fake::tuple_c _Results>
				static consteval auto deduce_resume(fake::type_package<_Relays>, fake::type_package<_Results>) noexcept{
					constexpr auto scale = []<typename _Token>(fake::type_package<_Token>){
						using var_t = fake::tuple::find_if_t<
							_Relays,
							[]<typename _Var>(fake::type_package<_Var>){
								return std::is_same_v<typename _Var::token_t, _Token>;
							}
						>;
						
						static_assert(
							std::is_same_v<var_t, fake::null_t> == false,
							"\e[33;40merror<fake::acyclic>: logic error, invoke result does NOT exist.\e[0m"
						);
						
						if constexpr(std::is_same_v<var_t, fake::null_t> == false){
							if constexpr(std::is_same_v<typename var_t::type_t, void>)
								return fake::pack_v<std::tuple<>>;
							else
								return fake::pack_v<std::tuple<var_t>>;
						}
					};
					
					return fake::tuple::scale_v<_Results, scale>;
				}
				
				static consteval bool strict_meta_enum_valid() noexcept{
					constexpr auto top = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
						return _Meta::mode != execution::topology::meta_e::meta_top;
					};
					
					constexpr auto dec = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
						return _Meta::mode == execution::topology::meta_e::meta_obj;
					};
					
					constexpr bool top_valid = fake::tuple::contains_v<tuple_top, top> == false;
					constexpr bool dec_valid = fake::tuple::contains_v<tuple_top, dec> == false;
					
					return top_valid && dec_valid;
				}
				
				static consteval bool strict_nodes_unique() noexcept{
					constexpr auto tokens = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
						return fake::pack_v<typename _Meta::token>;
					};
					
					using top_tokens_t = fake::tuple::transform_t<tuple_top, tokens>;
					using top_shrink_t = fake::tuple::shrink_t<top_tokens_t>;
					constexpr bool top_unique = std::tuple_size_v<top_tokens_t> == std::tuple_size_v<top_shrink_t>;
					
					using dec_tokens_t = fake::tuple::transform_t<tuple_dec, tokens>;
					using dec_shrink_t = fake::tuple::shrink_t<dec_tokens_t>;
					constexpr bool dec_unique = std::tuple_size_v<dec_tokens_t> == std::tuple_size_v<dec_shrink_t>;
					
					return top_unique && dec_unique;
				}
				
				static consteval bool strict_nodes_match() noexcept{
					constexpr auto match = [](auto _pack){
						using analysis_t = fake::take_t<_pack>;
						constexpr auto unpaired_args = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
							constexpr auto miss = []<typename _Arg>(fake::type_package<_Arg>){
								constexpr auto query = []<execution::topology::meta_c _Query>(
									fake::type_package<_Query>
								){
									return std::is_same_v<_Arg, typename _Query::token>;
								};
								
								using results_t = typename fake::tuple::find_if_t<analysis_t, query>::results;
								
								constexpr auto contains = []<typename _Result>(fake::type_package<_Result>){
									return std::is_same_v<typename _Meta::token, _Result>;
								};
								
								return fake::tuple::contains_v<fake::tuple::make_t<results_t>, contains> == false;
							};
							
							return fake::tuple::contains_v<fake::tuple::make_t<typename _Meta::args>, miss>;
						};
						
						constexpr bool args_match = fake::tuple::contains_v<analysis_t, unpaired_args> == false;
						
						constexpr auto unpaired_results = []<execution::topology::meta_c _Meta>(
							fake::type_package<_Meta>
						){
							constexpr auto miss = []<typename _Result>(fake::type_package<_Result>){
								constexpr auto query = []<execution::topology::meta_c _Query>(
									fake::type_package<_Query>
								){
									return std::is_same_v<_Result, typename _Query::token>;
								};
								
								using args_t = typename fake::tuple::find_if_t<analysis_t, query>::args;
								
								constexpr auto contains = []<typename _Arg>(fake::type_package<_Arg>){
									return std::is_same_v<typename _Meta::token, _Arg>;
								};
								
								return fake::tuple::contains_v<fake::tuple::make_t<args_t>, contains> == false;
							};
							
							return fake::tuple::contains_v<fake::tuple::make_t<typename _Meta::results>, miss>;
						};
						
						constexpr bool results_match = fake::tuple::contains_v<analysis_t, unpaired_results> == false;
						
						return args_match && results_match;
					};
					
					return match(fake::pack_v<tuple_top>) && match(fake::pack_v<tuple_asp>);
				}
				
				static consteval bool strict_acyclic() noexcept{
					constexpr auto recur = []<fake::tuple_c _Metas>(auto _self, fake::type_package<_Metas>){
						// invoker: dfs<remain, empty, empty>(dfs, ...). 
						constexpr auto dfs = []<
							fake::tuple_c _MetaSet,
							fake::tuple_c _OpenSet,
							execution::topology::meta_c... _Current
						>(
							auto _self,
							fake::type_package<std::tuple<_MetaSet, _OpenSet>> _origin,
							fake::type_package<std::tuple<_Current...>>
						){
							using meta_t = _MetaSet;
							using open_t = _OpenSet;
							
							if constexpr(sizeof...(_Current)){
								using iterate_t = std::tuple<_Current...>;
								using current_t = std::tuple_element_t<0, iterate_t>;
								
								constexpr auto current = []<typename _Token>(fake::type_package<_Token>){
									return std::is_same_v<current_t, _Token>;
								};
								
								if constexpr(fake::tuple::contains_v<open_t, current>){
									return fake::pack_v<fake::null_t>;
								}
								else{
									constexpr auto remain_v = fake::tuple::erase_front_v<iterate_t>;
									using meta_t = fake::take_t<_self(_self, _origin, remain_v)>;
									
									if constexpr(std::is_same_v<meta_t, fake::null_t>){
										return fake::pack_v<fake::null_t>;
									}
									else if constexpr(fake::tuple::contains_v<meta_t, current>){
										using open_t = fake::tuple::emplace_front_t<open_t, current_t>;
										
										return [_self, current]<typename... _Results>(
											fake::type_package<execution::topology::results<_Results...>>
										){
											constexpr auto scale = []<typename _Token>(fake::type_package<_Token>){
												constexpr auto query = []<execution::topology::meta_c _Meta>(
													fake::type_package<_Meta>
												){
													return std::is_same_v<_Token, typename _Meta::token>;
												};
												
												using query_t = fake::tuple::find_if_t<meta_t, query>;
												
												return fake::pack_v<
													std::conditional_t<
														std::is_same_v<query_t, fake::null_t>,
														std::tuple<>,
														std::tuple<query_t>
													>
												>;
											};
											
											using recur_t = fake::tuple::scale_t<std::tuple<_Results...>, scale>;
											constexpr auto recur_v = fake::pack_v<recur_t>;
											
											if constexpr(std::tuple_size_v<recur_t>){
												// regular node. 
												constexpr auto state_v = fake::pack_v<std::tuple<meta_t, open_t>>;
												
												// do NOT use 'fake::take_t<_self(...)>', '_self' is NOT 'constexpr'. 
												// or it may cause an ICE of gcc. 
												constexpr auto self = decltype(_self){};
												using meta_t = fake::take_t<self(self, state_v, recur_v)>;
												
												if constexpr(std::is_same_v<meta_t, fake::null_t>)
													return fake::pack_v<fake::null_t>;
												else
													return fake::tuple::erase_if_v<meta_t, current>;
											}
											else{
												// leaf node. 
												return fake::tuple::erase_if_v<meta_t, current>;
											}
										}(fake::pack_v<typename current_t::results>);
									}
									else{
										return fake::pack_v<meta_t>;
									}
								}
							}
							else{
								return fake::pack_v<meta_t>;
							}
						};
						
						using meta_t = _Metas;
						
						if constexpr(std::tuple_size_v<meta_t>){
							constexpr auto state_v = fake::pack_v<std::tuple<meta_t, std::tuple<>>>;
							using root_t = std::tuple<std::tuple_element_t<0, meta_t>>;
							using meta_t = fake::take_t<dfs(dfs, state_v, fake::pack_v<root_t>)>;
							
							if constexpr(std::is_same_v<meta_t, fake::null_t>)
								return false;
							else
								return _self(_self, fake::pack_v<meta_t>);
						}
						else{
							return true;
						}
					};
					
					return recur(recur, fake::pack_v<tuple_top>) && recur(recur, fake::pack_v<tuple_asp>);
				}
				
				template<typename = void>
				requires fake::clamour_c<
					strict_meta_enum_valid(),
					[](auto _b){
						static_assert(
							_b,
							"\e[33;40merror<fake::acyclic>: there are topology metas with invalid enum.\e[0m"
						);
					}
				> &&
				fake::clamour_c<
					strict_nodes_unique(),
					[](auto _b){
						static_assert(
							_b,
							"\e[33;40merror<fake::acyclic>: the topology tokens are NOT unique.\e[0m"
						);
					}
				> &&
				fake::clamour_c<
					strict_nodes_match(),
					[](auto _b){
						static_assert(
							_b,
							"\e[33;40merror<fake::acyclic>: "
							"the 'arguments' do NOT match against the 'results' in topology.\e[0m"
						);
					}
				> &&
				fake::clamour_c<
					strict_acyclic(),
					[](auto _b){
						static_assert(
							_b,
							"\e[33;40merror<fake::acyclic>: there are some loops in the topology.\e[0m"
						);
					}
				>
				static consteval auto deduce() noexcept{
					constexpr auto empty_v = fake::pack_v<std::tuple<>>;
					
					constexpr auto entries = deduce_entries<tuple_top>();
					constexpr auto results = deduce_results<tuple_top>();
					constexpr auto initial = deduce_init<tuple_top>(entries);
					constexpr auto reduce = meta_reduce(fake::pack_v<tuple_top>, entries);
					constexpr auto functors = deduce_functor<tuple_top>(initial, empty_v, reduce);
					constexpr auto inlined = deduce_inline<tuple_top>();
					constexpr auto depend = deduce_depend<tuple_top>(inlined);
					constexpr auto archive = deduce_relay(depend, functors.first);
					constexpr auto resume = deduce_resume(archive, results);
					
					using depend_t = typename std::remove_cvref_t<decltype(depend)>::type;
					using relay_t = typename std::remove_cvref_t<decltype(archive)>::type;
					using functors_t = typename std::remove_cvref_t<decltype(functors.first)>::type;
					using entries_t = typename std::remove_cvref_t<decltype(entries)>::type;
					using results_t = typename std::remove_cvref_t<decltype(results)>::type;
					using resume_t = typename std::remove_cvref_t<decltype(resume)>::type;
					
					constexpr auto iterate = []<typename _Functor>(fake::type_package<_Functor>){
						constexpr auto empty_v = fake::pack_v<std::tuple<>>;
						
						using functor_t = _Functor;
						using meta_t = typename _Functor::meta;
						using info_t = typename _Functor::info;
						
						constexpr auto subsequence = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
							return _Meta::mode == execution::topology::meta_e::meta_top;
						};
						
						constexpr auto metas = []<typename _DeduceFunctor>(fake::type_package<_DeduceFunctor>){
							using node_t = typename _DeduceFunctor::meta::token;
							using info_t = typename _DeduceFunctor::info;
							constexpr auto query = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
								return std::is_same_v<node_t, typename _Meta::token>;
							};
							
							using meta_t = fake::tuple::find_if_t<tuple_dec, query>;
							if constexpr(meta_t::mode == execution::topology::meta_e::meta_obs)
								return fake::pack_v<
									tool::acyclic::functor<
										execution::topology::meta<
											node_t,
											typename meta_t::results,
											execution::topology::args<>
										>,
										info_t
									>
								>;
							else
								return fake::pack_v<tool::acyclic::functor<meta_t, info_t>>;
						};
						
						constexpr auto trans = []<typename _DeduceFunctor>(fake::type_package<_DeduceFunctor>){
							return fake::pack_v<typename _DeduceFunctor::meta>;
						};
						
						constexpr auto entries = fake::pack_v<std::tuple<typename meta_t::token>>;
						constexpr auto initial = fake::pack_v<std::tuple<functor_t>>;
						constexpr auto reduce = fake::tuple::select_if_v<tuple_dec, subsequence>;
						constexpr auto functors = deduce_functor<tuple_dec>(initial, empty_v, reduce);
						using functors_t = fake::tuple::transform_t<fake::take_t<functors.first>, metas>;
						using tuple_ass = fake::tuple::transform_t<functors_t, trans>;
						constexpr auto results = deduce_results<tuple_ass>();
						constexpr auto inlined = deduce_inline<tuple_ass>();
						constexpr auto depend = deduce_depend<tuple_ass>(inlined);
						constexpr auto archive = deduce_relay(depend, fake::pack_v<functors_t>);
						constexpr auto resume = deduce_resume(archive, results);
						
						using depend_t = typename std::remove_cvref_t<decltype(depend)>::type;
						using relay_t = typename std::remove_cvref_t<decltype(archive)>::type;
						using entries_t = typename std::remove_cvref_t<decltype(entries)>::type;
						using results_t = typename std::remove_cvref_t<decltype(results)>::type;
						using resume_t = typename std::remove_cvref_t<decltype(resume)>::type;
						
						return fake::pack_v<
							tool::acyclic::aspection<
								meta_t,
								tool::acyclic::deduction<depend_t, relay_t, functors_t, entries_t, resume_t>
							>
						>;
					};
					using aspections = fake::tuple::transform_t<fake::take_t<functors.second>, iterate>;
					
					return fake::pack_v<
						tool::acyclic::deduce<
							tool::acyclic::deduction<depend_t, relay_t, functors_t, entries_t, resume_t>,
							aspections
						>
					>;
				}
				
			public:
				template<pack_c _C, pack_c _A>
				requires std::same_as<std::remove_cvref_t<_C>, typename callee_t::type_t> &&
					std::same_as<std::remove_cvref_t<_A>, typename aspect_t::type_t>
				constexpr delegate(_C &&_c, _A &&_a): callee(std::forward<_C>(_c)), aspect{std::forward<_A>(_a)}{}
				
			private:
				template<pass_c... _Params>
				static consteval bool strict_args_unique() noexcept{
					constexpr auto tokens = []<pass_c _Pass>(fake::type_package<_Pass>){
						return fake::pack_v<typename _Pass::token_t>;
					};
					
					using tokens_t = fake::tuple::transform_t<std::tuple<_Params...>, tokens>;
					using shrink_t = fake::tuple::shrink_t<tokens_t>;
					
					return std::tuple_size_v<tokens_t> == std::tuple_size_v<shrink_t>;
				}
				
				template<typename _Params, typename _Entries>
				static consteval auto strict_args_match_entries() noexcept{
					constexpr std::size_t entry_count = std::tuple_size_v<_Entries>;
					
					using param_tuple_t = _Params;
					constexpr std::size_t param_count = std::tuple_size_v<param_tuple_t>;
					
					if constexpr(entry_count == param_count){
						constexpr auto iterate = []<typename _Entry>(fake::type_package<_Entry>){
							constexpr auto find = []<pass_c _Find>(fake::type_package<_Find>){
								return std::is_same_v<_Entry, typename _Find::token_t>;
							};
							
							using arg_t = fake::tuple::find_if_t<param_tuple_t, find>;
							
							if constexpr(std::is_same_v<arg_t, fake::null_t>){
								return false;
							}
							else{
								// extract 'lambda' with 'token == _Entry'. 
								constexpr auto query_pass = []<pass_c _Pass>(fake::type_package<_Pass>){
									return std::is_same_v<typename _Pass::token_t, typename arg_t::token_t>;
								};
								
								using pass_t = fake::tuple::find_if_t<typename callee_t::storage_t, query_pass>;
								
								if constexpr(std::is_same_v<pass_t, fake::null_t>){
									return false;
								}
								else{
									// concept deduce 'lambda(arg)' or 'std::apply(lambda, tuple<args...>)'. 
									if constexpr(arg_t::is_tuple){
										return []<typename... Elements>(fake::type_package<std::tuple<Elements...>>){
											return std::is_invocable_v<pass_t, Elements...>;
										}(fake::pack_v<typename arg_t::value_t>);
									}
									else{
										return std::is_invocable_v<pass_t, typename arg_t::value_t>;
									}
								}
							}
						};
						
						using valid_tuple_t = fake::tuple::select_if_t<_Entries, iterate>;
						constexpr std::size_t valid_count = std::tuple_size_v<valid_tuple_t>;
						
						return entry_count == valid_count;
					}
					else{
						return false;
					}
				}
				
				template<typename _Params>
				static consteval auto strict_args_match_implementations() noexcept{
					using params_t = _Params;
					using storage_t = callee_t::storage_t;
					
					constexpr auto miss = []<pass_c _Param>(fake::type_package<_Param>){
						using node_t = typename _Param::token_t;
						
						constexpr auto query = []<pass_c _Node>(fake::type_package<_Node>){
							return std::is_same_v<node_t, typename _Node::token_t>;
						};
						
						using value_t = decltype(transform<node_t>(std::declval<_Param&>().get()));
						
						return fake::applicable_c<fake::tuple::find_if_t<storage_t, query>, value_t> == false;
					};
					
					return fake::tuple::contains_v<params_t, miss> == false;
				}
				
			private:
				// 'deduce_wrapper_t' is a work-around wrapper to make gcc happy. 
				// nude 'decltype(deduce())' would not occur an instantiation of 'deduce()' => return value. 
				// nude 'fake::take_t<deduce()>' would not occur it neither. 
				// it would cause a memory leak without a instance of the compile-time 'static constexpr' object. 
				// it is due to the dangling of the compile-time reference of the lambda in the 'requires' statement. 
				// the dangling, aka the '_lambda' in 'fake::clamour_c<_condition, _lambda>', may occur a 'ice' of gcc. 
				// which is a segmentation fault in most conditions. 
				struct deduce_wrapper_t final{static constexpr auto deduce_v = deduce();};
				
				using deduce_t = typename fake::take_t<deduce_wrapper_t::deduce_v>::deducer;
				
			private:
				enum struct invoke_e : bool{entry, propagate};
				
				// '_Token' is the token of the current node. 
				// '_storage' is the container of all delegates. 
				// '_args...' are the parameters of the current delegate, may be a 'pass_t<...>' or some 'relay<...>'. 
				template<
					fake::tuple_c _Graph,
					typename _Deduce,
					typename _Token,
					invoke_e _Invoke,
					fake::tuple_c _Voids = std::tuple<>
				>
				static decltype(auto) recur_inline_invoke(auto &_storage, auto &_aspect, auto &&..._args){
					using node_t = _Token;
					using storage_t = std::conditional_t<
						std::is_same_v<tuple_top, _Graph>,
						typename callee_t::storage_t,
						typename aspect_t::storage_t
					>;
					
					constexpr auto inlined = []<typename _Node>(fake::type_package<_Node>){
						constexpr auto meta = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
							return std::is_same_v<typename _Meta::token, _Node>;
						};
						
						using prevs_t = typename fake::tuple::find_if_t<_Graph, meta>::args;
						
						if constexpr(fake::element_size_v<prevs_t> == 1){
							using prev_t = fake::element_index_t<0, prevs_t>;
							
							constexpr auto find_prev_var = []<typename _Var>(fake::type_package<_Var>){
								return std::is_same_v<typename _Var::token_t, prev_t>;
							};
							
							using prev_var_t = fake::tuple::find_if_t<typename _Deduce::relay_t, find_prev_var>;
							
							return std::is_same_v<prev_var_t, fake::null_t>;
						}
						else{
							return false;
						}
					};
					
					constexpr bool is_inlined = inlined(fake::pack_v<node_t>);
					
					constexpr auto distribute = []<typename _Arg>(
						auto &&_value,
						auto &_broker,
						fake::type_package<_Arg>
					)->decltype(auto){
						constexpr auto functor = []<typename _Functor>(fake::type_package<_Functor>){
							return std::is_same_v<typename _Functor::meta::token, _Arg>;
						};
						
						using retn_t = typename fake::tuple::find_if_t<
							typename _Deduce::functors_t,
							functor
						>::info::retn;
						
						// parse type of 'awaiter->await_resume()'. 
						using result_t = fake::take_t<
							[]{
								if constexpr(std::derived_from<_Arg, fake::acyclic::token::await>)
									return fake::pack_v<
										typename decltype(deduce_await<retn_t, _Arg>())::result_t
									>;
								else
									return fake::pack_v<retn_t>;
							}()
						>;
						
						constexpr auto observe = []<typename _Prev = _Arg>(
							auto &&_value,
							auto &_broker
						)->decltype(auto){
							if constexpr(std::is_same_v<tuple_top, _Graph>){
								using broker_t = std::remove_cvref_t<decltype(_broker)>;
								using ref_t = std::add_lvalue_reference_t<std::remove_cvref_t<decltype(_value)>>;
								
								constexpr auto query = []<typename _Aspection>(fake::type_package<_Aspection>){
									using object_t = typename _Aspection::meta::object;
									using from_t = typename object_t::from;
									using to_t = typename object_t::to;
									
									return std::is_same_v<from_t, _Prev> && std::is_same_v<to_t, node_t>;
								};
								
								constexpr auto aspections_v = fake::tuple::select_if_v<
									typename broker_t::aspects_t,
									query
								>;
								
								[]<typename... _Aspections>(
									ref_t _value,
									auto &_broker,
									fake::type_package<std::tuple<_Aspections...>>
								){
									constexpr auto make_pass = []<typename _Meta, typename _Ref = ref_t>(
										ref_t _value,
										fake::type_package<_Meta>
									){
										using ref_t = _Ref;
										using node_t = typename _Meta::token;
										
										using tuple_t = std::remove_cvref_t<decltype(transform<node_t>(_value))>;
										
										return []<std::size_t... _Index>(
											auto &&_value,
											std::index_sequence<_Index...>
										){
											return fake::pass<node_t>(
												fake::pack_v<_Meta>,
												std::ref(
													static_cast<
														std::add_lvalue_reference_t<
															std::remove_cvref_t<
																std::tuple_element_t<_Index, tuple_t>
															>
														>
													>(std::get<_Index>(std::forward<decltype(_value)>(_value)))
												)
												...
											);
										}(
											transform<node_t>(_value),
											std::make_index_sequence<std::tuple_size_v<tuple_t>>()
										);
									};
									
									(
										(
											invoke_params<tuple_asp, typename _Aspections::deducer>(
												_broker.aspect,
												_broker,
												std::get<_Aspections>(_broker.aspects).vars,
												fake::pass<fake::acyclic::token::sched<void>>(tool::acyclic::executor),
												_broker.handle,
												_broker.work,
												_broker.atomic,
												make_pass(_value, fake::pack_v<typename _Aspections::meta>)
											),
											[](auto &_broker){
												constexpr auto trigger_e = fake::atomic::ultimatum::signal::trigger;
												
												if(
													_broker.observes.consume() == trigger_e &&
													_broker.preempt.consume() == trigger_e &&
													_broker.work.consume() == trigger_e &&
													_broker.handle
												)
													_broker.handle();
											}(_broker)
										),
										...
									);
								}(static_cast<ref_t>(_value), _broker, aspections_v);
							}
							
							return std::forward<decltype(_value)>(_value);
						};
						
						constexpr bool dupli_v = std::derived_from<_Arg, fake::acyclic::token::dupli>;
						
						if constexpr(fake::is_tuple_v<result_t> == false || dupli_v)
							return observe(std::forward<decltype(_value)>(_value), _broker);
						else
							return observe(
								std::get<deduce_distribute_index<_Graph, _Arg, node_t>()>(
									std::forward<decltype(_value)>(_value)
								),
								_broker
							);
					};
					
					constexpr auto observe_void = []<typename _Prev>(auto &_broker, fake::type_package<_Prev>){
						using prev_t = _Prev;
						
						if constexpr(std::is_same_v<tuple_top, _Graph>){
							using broker_t = std::remove_cvref_t<decltype(_broker)>;
							
							constexpr auto query = []<typename _Aspection>(fake::type_package<_Aspection>){
								using object_t = typename _Aspection::meta::object;
								using from_t = typename object_t::from;
								using to_t = typename object_t::to;
								
								return std::is_same_v<from_t, prev_t> && std::is_same_v<to_t, node_t>;
							};
							
							constexpr auto aspections_v = fake::tuple::select_if_v<
								typename broker_t::aspects_t,
								query
							>;
							
							[]<typename... _Aspections>(
								auto &_broker,
								fake::type_package<std::tuple<_Aspections...>>
							){
								(
									(
										invoke_params<tuple_asp, typename _Aspections::deducer>(
											_broker.aspect,
											_broker,
											std::get<_Aspections>(_broker.aspects).vars,
											fake::pass<fake::acyclic::token::sched<void>>(tool::acyclic::executor),
											_broker.handle,
											_broker.work,
											_broker.atomic,
											fake::pass<typename _Aspections::meta::token>(
												fake::pack_v<typename _Aspections::meta>
											)
										),
										[](auto &_broker){
											constexpr auto trigger_e = fake::atomic::ultimatum::signal::trigger;
											
											if(
												_broker.observes.consume() == trigger_e &&
												_broker.preempt.consume() == trigger_e &&
												_broker.work.consume() == trigger_e &&
												_broker.handle
											)
												_broker.handle();
										}(_broker)
									),
									...
								);
							}(_broker, aspections_v);
						}
					};
					
					constexpr bool has_exception_handler = std::is_same_v<
						decltype(select_exception_handler<node_t>(_storage)),
						void
					> == false;
					
					if constexpr(is_inlined){
						constexpr auto meta = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
							return std::is_same_v<typename _Meta::token, node_t>;
						};
						
						using prev_t = fake::element_index_t<0, typename fake::tuple::find_if_t<_Graph, meta>::args>;
						
						constexpr auto query = []<pass_c _Pass>(fake::type_package<_Pass>){
							return std::is_same_v<typename _Pass::token_t, node_t>;
						};
						
						using delegate_t = fake::tuple::find_if_t<storage_t, query>;
						
						using recur_result_t = decltype(
							recur_inline_invoke<_Graph, _Deduce, prev_t, _Invoke>(
								_storage,
								_aspect,
								std::forward<decltype(_args)>(_args)...
							)
						);
						
						if constexpr(std::is_same_v<recur_result_t, void>){
							observe_void(_aspect, fake::pack_v<prev_t>);
							
							if constexpr(has_exception_handler){
								try{
									recur_inline_invoke<_Graph, _Deduce, prev_t, _Invoke>(
										_storage,
										_aspect,
										std::forward<decltype(_args)>(_args)...
									);
									
									return std::get<delegate_t>(_storage)();
								}
								catch(const fake::exception::detail::acyclic::recur&){
									// do nothing, fall through and throw again. 
								}
								catch(...){
									dispatch_exception<node_t>(_storage);
								}
								
								throw fake::exception::detail::acyclic::recur{};
							}
							else{
								recur_inline_invoke<_Graph, _Deduce, prev_t, _Invoke>(
									_storage,
									_aspect,
									std::forward<decltype(_args)>(_args)...
								);
								
								return std::get<delegate_t>(_storage)();
							}
						}
						else{
							if constexpr(has_exception_handler){
								try{
									return std::apply(
										std::get<delegate_t>(_storage),
										transform<node_t>(
											distribute(
												recur_inline_invoke<_Graph, _Deduce, prev_t, _Invoke>(
													_storage,
													_aspect,
													std::forward<decltype(_args)>(_args)...
												),
												_aspect,
												fake::pack_v<prev_t>
											)
										)
									);
								}
								catch(const fake::exception::detail::acyclic::recur&){
									// do nothing, fall through and throw again. 
								}
								catch(...){
									dispatch_exception<node_t>(_storage);
								}
								
								throw fake::exception::detail::acyclic::recur{};
							}
							else{
								return std::apply(
									std::get<delegate_t>(_storage),
									transform<node_t>(
										distribute(
											recur_inline_invoke<_Graph, _Deduce, prev_t, _Invoke>(
												_storage,
												_aspect,
												std::forward<decltype(_args)>(_args)...
											),
											_aspect,
											fake::pack_v<prev_t>
										)
									)
								);
							}
						}
					}
					else{
						constexpr auto query = []<pass_c _Pass>(fake::type_package<_Pass>){
							return std::is_same_v<typename _Pass::token_t, node_t>;
						};
						
						using delegate_t = fake::tuple::find_if_t<storage_t, query>;
						
						if constexpr(_Invoke == invoke_e::entry){
							if constexpr(has_exception_handler){
								try{
									return std::apply(
										std::get<delegate_t>(_storage),
										std::tuple_cat(transform<node_t>(std::forward<decltype(_args)>(_args).get())...)
									);
								}
								catch(const fake::exception::detail::acyclic::recur&){
									// do nothing, fall through and throw again. 
								}
								catch(...){
									dispatch_exception<node_t>(_storage);
								}
								
								throw fake::exception::detail::acyclic::recur{};
							}
							else{
								return std::apply(
									std::get<delegate_t>(_storage),
									std::tuple_cat(transform<node_t>(std::forward<decltype(_args)>(_args).get())...)
								);
							}
						}
						else{
							[]<typename... _Prev>(auto _obs, auto &_broker, fake::type_package<std::tuple<_Prev...>>){
								(_obs(_broker, fake::pack_v<_Prev>), ...);
							}(observe_void, _aspect, fake::pack_v<_Voids>);
							
							if constexpr(has_exception_handler){
								try{
									return std::apply(
										std::get<delegate_t>(_storage),
										std::tuple_cat(
											transform<node_t>(
												distribute(
													std::forward<decltype(_args)>(_args).storage.value(),
													_aspect,
													fake::pack_v<typename std::remove_cvref_t<decltype(_args)>::token_t>
												)
											)...
										)
									);
								}
								catch(const fake::exception::detail::acyclic::recur&){
									// do nothing, fall through and throw again. 
								}
								catch(...){
									dispatch_exception<node_t>(_storage);
								}
								
								throw fake::exception::detail::acyclic::recur{};
							}
							else{
								return std::apply(
									std::get<delegate_t>(_storage),
									std::tuple_cat(
										transform<node_t>(
											distribute(
												std::forward<decltype(_args)>(_args).storage.value(),
												_aspect,
												fake::pack_v<typename std::remove_cvref_t<decltype(_args)>::token_t>
											)
										)...
									)
								);
							}
						}
					}
				}
				
				template<typename _Token, typename _Scheduler>
				static constexpr auto expr_to_awaiter(auto &&_expr){
					namespace concepts = fake::acyclic::concepts;
					using scheduler_t = _Scheduler;
					
					constexpr auto awaitable = []<typename _Ex>(_Ex &&_ex)->decltype(auto){
						using token_t = _Token;
						using expr_t = _Ex;
						
						if constexpr(concepts::token_with_promise_c<token_t>){
							using promise_t = token_t::promise_type;
							
							if constexpr(concepts::expr_to_awaitable_via_promise_c<token_t, expr_t, promise_t>){
								if constexpr(requires{token_t::scheduler_inject;}){
									if constexpr(token_t::scheduler_inject)
										return promise_t{}.await_transform(
											fake::pass<scheduler_t>(std::forward<_Ex>(_ex))
										);
									else
										return promise_t{}.await_transform(std::forward<_Ex>(_ex));
								}
								else{
									return promise_t{}.await_transform(std::forward<_Ex>(_ex));
								}
							}
							else if constexpr(concepts::expr_to_awaitable_via_as_is_c<expr_t>){
								return std::forward<_Ex>(_ex);
							}
						}
						else if constexpr(concepts::expr_to_awaitable_via_as_is_c<expr_t>){
							return std::forward<_Ex>(_ex);
						}
					};
					
					constexpr auto awaiter = []<concepts::awaitable_c _Aw>(_Aw &&_aw)->decltype(auto){
						using awaitable_t = _Aw;
						
						if constexpr(concepts::awaitable_to_awaiter_via_member_operator_co_await_c<awaitable_t>)
							return std::forward<_Aw>(_aw).operator co_await();
						else if constexpr(concepts::awaitable_to_awaiter_via_global_operator_co_await_c<awaitable_t>)
							return operator co_await(std::forward<_Aw>(_aw));
						else if constexpr(concepts::awaitable_to_awaiter_via_as_is_c<awaitable_t>)
							return std::forward<_Aw>(_aw);
					};
					
					// there may be a rvalue reference of a temporary materialization. 
					// aka the instance of the return value of 'awaitable()'. 
					// it must be passed to 'awaiter()' inplace, or it would cause a reference dangling. 
					// the temporary materialization would be destructed after the 'return' statement. 
					// so this function aka 'expr_to_awaiter()' should NOT returns 'decltype(auto)'. 
					return awaiter(awaitable(std::forward<decltype(_expr)>(_expr)));
				}
				
				static constexpr decltype(auto) expr_to_scheduler(pass_c auto &_expr){
					namespace concepts = fake::acyclic::concepts;
					using token_t = std::remove_cvref_t<decltype(_expr)>::token_t;
					
					constexpr auto scheduler = []<typename _Ex>(_Ex &&_ex)->decltype(auto){
						using expr_t = _Ex;
						
						if constexpr(concepts::token_with_execution_c<token_t>){
							using execution_t = token_t::execute_type;
							
							if constexpr(concepts::expr_to_scheduler_via_transform_c<expr_t, execution_t>)
								return execution_t{}.scheduler_transform(std::forward<_Ex>(_ex));
							else if constexpr(concepts::expr_to_scheduler_via_as_is_c<expr_t>)
								return std::forward<_Ex>(_ex);
						}
						else if constexpr(concepts::expr_to_scheduler_via_as_is_c<expr_t>)
							return std::forward<_Ex>(_ex);
					};
					
					return scheduler(_expr.get());
				}
				
				struct invoker_t final{
					callee_t::type_t callee;
					typename deduce_t::relay_t vars;
					std::coroutine_handle<> handle;
					std::coroutine_handle<> self;
					fake::atomic::ultimatum work{0};
					fake::atomic::ultimatum use{0};
					
					fake::atomic::ultimatum invoke_await_suspend{0};
					fake::atomic::ultimatum invoke_await_resume{0};
				};
				
				struct broker_t final{
					using aspects_t = typename fake::take_t<deduce_wrapper_t::deduce_v>::aspects;
					static constexpr std::size_t size = std::tuple_size_v<aspects_t>;
					
					aspect_t::type_t aspect;
					aspects_t aspects;
					std::coroutine_handle<> handle;
					fake::atomic::ultimatum work{0};
					fake::atomic::ultimatum observes{size};
					
					// preempt a 'this->work.consume()' invocation. 
					// it would be preempted by the done event of 'delegate' and the triggering of 'this->observes'. 
					fake::atomic::ultimatum preempt{0};
					
					// consumed when the invocation of the acyclic. 
					fake::atomic::ultimatum delegate{1};
					
					// 'atomic.produce()' would be invoked when each aspect acyclic done. 
					fake::atomic::ultimatum atomic{0};
				};
				
				template<typename _Node>
				static constexpr auto coroutine_await(
					fake::acyclic::concepts::awaiter_c auto &_awaiter,
					std::convertible_to<std::coroutine_handle<>> auto &_coro,
					auto &&_sched
				){
					using suspend_t = decltype(_awaiter.await_suspend(_coro));
					constexpr bool suspend_void = std::same_as<suspend_t, void>;
					constexpr bool suspend_bool = std::convertible_to<suspend_t, bool>;
					constexpr bool suspend_coro = std::convertible_to<suspend_t, std::coroutine_handle<>>;
					
					if(_awaiter.await_ready()){
						_coro();
					}
					else{
						if constexpr(requires{typename _Node::promise_type;} && requires{_Node::scheduler_inject;})
							if constexpr(_Node::scheduler_inject)
								_awaiter.await_inject(std::forward<decltype(_sched)>(_sched));
						
						if constexpr(suspend_void){
							_awaiter.await_suspend(_coro);
						}
						else if constexpr(suspend_bool){
							if(bool(_awaiter.await_suspend(_coro)) == false)
								_coro();
						}
						else if constexpr(suspend_coro){
							_awaiter.await_suspend(_coro).resume();
						}
					}
				}
				
				template<typename _Group, typename _Deduce>
				static constexpr auto invoke_params(
					auto &_deli,
					broker_t &_aspect,
					typename _Deduce::relay_t &_vars,
					pass_c auto _sched,
					std::coroutine_handle<> _handle,
					fake::atomic::ultimatum &_work,
					fake::atomic::ultimatum &_atomic,
					auto &&_arg
				){
					using arg_t = std::remove_cvref_t<decltype(_arg)>;
					using inline_t = typename arg_t::token_t;
					
					constexpr auto recur_var = []<typename _Rec>(auto _self, fake::type_package<_Rec>){
						constexpr auto find_var = []<typename _Var>(fake::type_package<_Var>){
							return std::is_same_v<typename _Var::token_t, _Rec>;
						};
						
						using recur_t = fake::tuple::find_if_t<typename _Deduce::relay_t, find_var>;
						
						if constexpr(std::is_same_v<recur_t, fake::null_t>){
							constexpr auto pick = []<execution::topology::meta_c _Meta>(
								fake::type_package<_Meta>
							){
								return std::is_same_v<typename _Meta::token, _Rec>;
							};
							
							using results_t = typename fake::tuple::find_if_t<_Group, pick>::results;
							
							static_assert(
								fake::element_size_v<results_t> == 1,
								"\e[33;40merror<fake::acyclic>: "
								"logic error, unexpected inlined results number.\e[0m"
							);
							
							return _self(_self, fake::pack_v<fake::element_index_t<0, results_t>>);
						}
						else{
							return fake::pack_v<recur_t>;
						}
					};
					
					using var_t = fake::take_t<recur_var(recur_var, fake::pack_v<inline_t>)>;
					using node_t = typename var_t::token_t;
					
					constexpr bool has_exception_handler = std::is_same_v<
						decltype(select_exception_handler<node_t>(_deli.storage)),
						void
					> == false;
					
					using scheduler_t = decltype(expr_to_scheduler(_sched));
					
					auto callback = [
						&storage = _deli.storage,
						&var = std::get<var_t>(_vars),
						arg = std::forward<decltype(_arg)>(_arg),
						&deli = _deli,
						&aspect = _aspect,
						&vars = _vars,
						handle = _handle,
						&work = _work,
						&atomic = _atomic
					]() mutable{
						if constexpr(std::derived_from<node_t, fake::acyclic::token::await>){
							using ref_t = std::reference_wrapper<
								std::remove_cvref_t<
									decltype(
										expr_to_awaiter<node_t, scheduler_t>(
											recur_inline_invoke<_Group, _Deduce, node_t, invoke_e::entry>(
												storage,
												aspect,
												arg
											)
										)
									)
								>
							>;
							
							if constexpr(std::is_same_v<typename var_t::type_t, void>){
								coroutine_t<ref_t> coro = [](
									auto _await,
									auto &_deli,
									auto &_aspect,
									auto &_vars,
									auto _handle,
									auto &_work,
									auto &_atomic
								)->coroutine_t<ref_t>{
									tool::acyclic::guard_t guard{_handle, _work, _atomic};
									
									co_yield std::ref(_await);
									
									if constexpr(has_exception_handler){
										try{
											_await.await_resume();
											
											propagate<_Group, _Deduce, node_t>(
												_deli,
												_aspect,
												_vars,
												_handle,
												_work,
												_atomic
											);
										}
										catch(...){
											dispatch_exception<node_t>(_deli.storage);
										}
									}
									else{
										_await.await_resume();
										
										propagate<_Group, _Deduce, node_t>(
											_deli,
											_aspect,
											_vars,
											_handle,
											_work,
											_atomic
										);
									}
								}(
									expr_to_awaiter<node_t, scheduler_t>(
										recur_inline_invoke<_Group, _Deduce, node_t, invoke_e::entry>(
											storage,
											aspect,
											arg
										)
									),
									deli,
									aspect,
									vars,
									handle,
									work,
									atomic
								);
								
								auto ref = coro.get();
								
								coroutine_await<node_t>(
									ref.get(),
									coro,
									expr_to_scheduler(select_scheduler<node_t>(storage))
								);
							}
							else{
								coroutine_t<ref_t> coro = [](
									auto _await,
									auto &_var,
									auto &_deli,
									auto &_aspect,
									auto &_vars,
									auto _handle,
									auto &_work,
									auto &_atomic
								)->coroutine_t<ref_t>{
									tool::acyclic::guard_t guard{_handle, _work, _atomic};
									
									co_yield std::ref(_await);
									
									if constexpr(has_exception_handler){
										try{
											_var.storage.emplace(_await.await_resume());
											
											propagate<_Group, _Deduce, node_t>(
												_deli,
												_aspect,
												_vars,
												_handle,
												_work,
												_atomic
											);
										}
										catch(...){
											dispatch_exception<node_t>(_deli.storage);
										}
									}
									else{
										_var.storage.emplace(_await.await_resume());
										
										propagate<_Group, _Deduce, node_t>(
											_deli,
											_aspect,
											_vars,
											_handle,
											_work,
											_atomic
										);
									}
								}(
									expr_to_awaiter<node_t, scheduler_t>(
										recur_inline_invoke<_Group, _Deduce, node_t, invoke_e::entry>(
											storage,
											aspect,
											arg
										)
									),
									var,
									deli,
									aspect,
									vars,
									handle,
									work,
									atomic
								);
								
								auto ref = coro.get();
								
								coroutine_await<node_t>(
									ref.get(),
									coro,
									expr_to_scheduler(select_scheduler<node_t>(storage))
								);
							}
						}
						else{
							if constexpr(std::is_same_v<typename var_t::type_t, void>)
								recur_inline_invoke<_Group, _Deduce, node_t, invoke_e::entry>(storage, aspect, arg);
							else
								var.storage.emplace(
									recur_inline_invoke<_Group, _Deduce, node_t, invoke_e::entry>(
										storage,
										aspect,
										arg
									)
								);
							
							propagate<_Group, _Deduce, node_t>(deli, aspect, vars, handle, work, atomic);
						}
					};
					
					if constexpr(fake::acyclic::concepts::meta_scheduler_c<scheduler_t, node_t, void()>){
						if constexpr(has_exception_handler){
							try{
								expr_to_scheduler(_sched).template execute<node_t>(
									[
										&storage = _deli.storage,
										callback = std::move(callback),
										guard = tool::acyclic::guard_t{_handle, _work, _atomic}
									]() mutable{
										try{callback();}
										catch(const fake::exception::detail::acyclic::recur&){}
										catch(...){dispatch_exception<node_t>(storage);}
									}
								);
							}
							catch(...){
								dispatch_exception<node_t>(_deli.storage);
							}
						}
						else{
							expr_to_scheduler(_sched).template execute<node_t>(
								[
									callback = std::move(callback),
									guard = tool::acyclic::guard_t{_handle, _work, _atomic}
								]() mutable{
									callback();
								}
							);
						}
					}
					else{
						if constexpr(has_exception_handler){
							try{
								expr_to_scheduler(_sched).execute(
									[
										&storage = _deli.storage,
										callback = std::move(callback),
										guard = tool::acyclic::guard_t{_handle, _work, _atomic}
									]() mutable{
										try{callback();}
										catch(const fake::exception::detail::acyclic::recur&){}
										catch(...){dispatch_exception<node_t>(storage);}
									}
								);
							}
							catch(...){
								dispatch_exception<node_t>(_deli.storage);
							}
						}
						else{
							expr_to_scheduler(_sched).execute(
								[
									callback = std::move(callback),
									guard = tool::acyclic::guard_t{_handle, _work, _atomic}
								]() mutable{
									callback();
								}
							);
						}
					}
				}
				
				template<fake::tuple_c _Graph, typename _Deduce, typename _Token>
				static auto propagate(
					auto &_deli,
					broker_t &_aspect,
					typename _Deduce::relay_t &_vars,
					std::coroutine_handle<> _handle,
					fake::atomic::ultimatum &_work,
					fake::atomic::ultimatum &_atomic
				){
					using token_t = _Token;
					
					// current state: 
					// the invocation of the 'token_t' marked node is done, the result has been storaged into '_vars'. 
					
					// for all nodes following the current node, count down the 'relay->flag' by one. 
					// invocation occurs if the atmoic 'flag' returns 'fake::atomic::ultimatum::signal::trigger'. 
					// 'propagate' will schedule the ready node on its associated scheduler. 
					// if the default scheduler is NOT offered, every node should carries a private scheduler. 
					// this assertion should be guaranteed by the concepts of 'this->opertor()'. 
					
					constexpr auto find_current = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
						return std::is_same_v<typename _Meta::token, token_t>;
					};
					
					using current_t = fake::tuple::find_if_t<typename _Deduce::depend_t, find_current>;
					
					static_assert(
						std::is_same_v<current_t, fake::null_t> == false,
						"\e[33;40merror<fake::acyclic>: logic error, node NOT found in 'propagete()'.\e[0m"
					);
					
					constexpr auto iterate = []<typename... _Results>(
						auto &_deli,
						broker_t &_aspect,
						typename _Deduce::relay_t &_vars,
						std::coroutine_handle<> _handle,
						fake::atomic::ultimatum &_work,
						fake::atomic::ultimatum &_atomic,
						execution::topology::results<_Results...>
					){
						constexpr auto element = []<typename _Result>(
							auto &_deli,
							broker_t &_aspect,
							typename _Deduce::relay_t &_vars,
							pass_c auto _sched,
							std::coroutine_handle<> _handle,
							fake::atomic::ultimatum &_work,
							fake::atomic::ultimatum &_atomic,
							fake::type_package<_Result>
						){
							using node_t = _Result;
							constexpr auto find_node = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
								return std::is_same_v<typename _Meta::token, node_t>;
							};
							
							constexpr auto find_var = []<typename _Var>(fake::type_package<_Var>){
								return std::is_same_v<typename _Var::token_t, node_t>;
							};
							
							using raw_t = typename fake::tuple::find_if_t<typename _Deduce::depend_t, find_node>::args;
							using var_t = fake::tuple::find_if_t<typename _Deduce::relay_t, find_var>;
							
							constexpr auto arg_tuple = []<typename... _Args>(execution::topology::args<_Args...>){
								return fake::pack_v<std::tuple<_Args...>>;
							};
							
							using raw_tuple = fake::take_t<arg_tuple(raw_t{})>;
							
							constexpr auto remove = []<typename _Arg>(fake::type_package<_Arg>){
								constexpr auto find_relay = []<typename _Relay>(fake::type_package<_Relay>){
									return std::is_same_v<typename _Relay::token_t, _Arg>;
								};
								
								using find_t = fake::tuple::find_if_t<typename _Deduce::relay_t, find_relay>;
								
								return fake::pack_v<
									std::conditional_t<
										std::is_same_v<typename find_t::type_t, void>,
										std::tuple<>,
										std::tuple<typename find_t::token_t>
									>
								>;
							};
							
							using args_t = fake::tuple::scale_t<raw_tuple, remove>;
							
							constexpr auto pick_void = []<typename _Arg>(fake::type_package<_Arg>){
								constexpr auto find_relay = []<typename _Relay>(fake::type_package<_Relay>){
									return std::is_same_v<typename _Relay::token_t, _Arg>;
								};
								
								using find_t = fake::tuple::find_if_t<typename _Deduce::relay_t, find_relay>;
								
								return fake::pack_v<
									std::conditional_t<
										std::is_same_v<typename find_t::type_t, void>,
										std::tuple<typename find_t::token_t>,
										std::tuple<>
									>
								>;
							};
							
							using voids_t = fake::tuple::scale_t<raw_tuple, pick_void>;
							
							constexpr bool has_exception_handler = std::is_same_v<
								decltype(select_exception_handler<node_t>(_deli.storage)),
								void
							> == false;
							
							using scheduler_t = decltype(expr_to_scheduler(_sched));
							
							auto callback = [
								&storage = _deli.storage,
								&var = std::get<var_t>(_vars),
								&deli = _deli,
								&aspect = _aspect,
								&vars = _vars,
								handle = _handle,
								&work = _work,
								&atomic = _atomic
							]<typename _ArgsType = args_t, typename _VoidsType = voids_t>() mutable{
								// pass the type 'args_t' through the gcc type scope state machine to make it happy. 
								using args_t = _ArgsType;
								
								constexpr auto invoke = []<typename... _Args, fake::tuple_c _Voids = _VoidsType>(
									auto &_storage,
									auto &_aspect,
									auto &_vars,
									fake::type_package<std::tuple<_Args...>>
								)->decltype(auto){
									return recur_inline_invoke<_Graph, _Deduce, node_t, invoke_e::propagate, _Voids>(
										_storage,
										_aspect,
										std::get<
											fake::tuple::find_if_t<
												typename _Deduce::relay_t,
												[]<typename _Arg>(fake::type_package<_Arg>){
													return std::is_same_v<typename _Arg::token_t, _Args>;
												}
											>
										>(_vars)...
									);
								};
								
								if constexpr(std::derived_from<node_t, fake::acyclic::token::await>){
									using ref_t = std::reference_wrapper<
										std::remove_cvref_t<
											decltype(
												expr_to_awaiter<node_t, scheduler_t>(
													invoke(storage, aspect, vars, fake::pack_v<args_t>)
												)
											)
										>
									>;
									
									if constexpr(std::is_same_v<typename var_t::type_t, void>){
										coroutine_t<ref_t> coro = [](
											auto _await,
											auto &_deli,
											auto &_aspect,
											auto &_vars,
											auto _handle,
											auto &_work,
											auto &_atomic
										)->coroutine_t<ref_t>{
											tool::acyclic::guard_t guard{_handle, _work, _atomic};
											
											co_yield std::ref(_await);
											
											if constexpr(has_exception_handler){
												try{
													_await.await_resume();
													
													propagate<_Graph, _Deduce, node_t>(
														_deli,
														_aspect,
														_vars,
														_handle,
														_work,
														_atomic
													);
												}
												catch(...){
													dispatch_exception<node_t>(_deli.storage);
												}
											}
											else{
												_await.await_resume();
												
												propagate<_Graph, _Deduce, node_t>(
													_deli,
													_aspect,
													_vars,
													_handle,
													_work,
													_atomic
												);
											}
										}(
											expr_to_awaiter<node_t, scheduler_t>(
												invoke(storage, aspect, vars, fake::pack_v<args_t>)
											),
											deli,
											aspect,
											vars,
											handle,
											work,
											atomic
										);
										
										auto ref = coro.get();
										
										coroutine_await<node_t>(
											ref.get(),
											coro,
											expr_to_scheduler(select_scheduler<node_t>(storage))
										);
									}
									else{
										coroutine_t<ref_t> coro = [](
											auto _await,
											auto &_var,
											auto &_deli,
											auto &_aspect,
											auto &_vars,
											auto _handle,
											auto &_work,
											auto &_atomic
										)->coroutine_t<ref_t>{
											tool::acyclic::guard_t guard{_handle, _work, _atomic};
											
											co_yield std::ref(_await);
											
											if constexpr(has_exception_handler){
												try{
													_var.storage.emplace(_await.await_resume());
													
													propagate<_Graph, _Deduce, node_t>(
														_deli,
														_aspect,
														_vars,
														_handle,
														_work,
														_atomic
													);
												}
												catch(...){
													dispatch_exception<node_t>(_deli.storage);
												}
											}
											else{
												_var.storage.emplace(_await.await_resume());
												
												propagate<_Graph, _Deduce, node_t>(
													_deli,
													_aspect,
													_vars,
													_handle,
													_work,
													_atomic
												);
											}
										}(
											expr_to_awaiter<node_t, scheduler_t>(
												invoke(storage, aspect, vars, fake::pack_v<args_t>)
											),
											var,
											deli,
											aspect,
											vars,
											handle,
											work,
											atomic
										);
										
										auto ref = coro.get();
										
										coroutine_await<node_t>(
											ref.get(),
											coro,
											expr_to_scheduler(select_scheduler<node_t>(storage))
										);
									}
								}
								else{
									if constexpr(std::is_same_v<typename var_t::type_t, void>)
										invoke(storage, aspect, vars, fake::pack_v<args_t>);
									else
										var.storage.emplace(invoke(storage, aspect, vars, fake::pack_v<args_t>));
									
									propagate<_Graph, _Deduce, node_t>(deli, aspect, vars, handle, work, atomic);
								}
							};
							
							if(std::get<var_t>(_vars).flag.consume() == fake::atomic::ultimatum::signal::trigger){
								if constexpr(fake::acyclic::concepts::meta_scheduler_c<scheduler_t, node_t, void()>){
									if constexpr(has_exception_handler){
										try{
											expr_to_scheduler(_sched).template execute<node_t>(
												[
													&storage = _deli.storage,
													callback = std::move(callback),
													guard = tool::acyclic::guard_t{_handle, _work, _atomic}
												]() mutable{
													try{callback();}
													catch(const fake::exception::detail::acyclic::recur&){}
													catch(...){dispatch_exception<node_t>(storage);}
												}
											);
										}
										catch(...){
											dispatch_exception<node_t>(_deli.storage);
										}
									}
									else{
										expr_to_scheduler(_sched).template execute<node_t>(
											[
												callback = std::move(callback),
												guard = tool::acyclic::guard_t{_handle, _work, _atomic}
											]() mutable{
												callback();
											}
										);
									}
								}
								else{
									if constexpr(has_exception_handler){
										try{
											expr_to_scheduler(_sched).execute(
												[
													&storage = _deli.storage,
													callback = std::move(callback),
													guard = tool::acyclic::guard_t{_handle, _work, _atomic}
												]() mutable{
													try{callback();}
													catch(const fake::exception::detail::acyclic::recur&){}
													catch(...){dispatch_exception<node_t>(storage);}
												}
											);
										}
										catch(...){
											dispatch_exception<node_t>(_deli.storage);
										}
									}
									else{
										expr_to_scheduler(_sched).execute(
											[
												callback = std::move(callback),
												guard = tool::acyclic::guard_t{_handle, _work, _atomic}
											]() mutable{
												callback();
											}
										);
									}
								}
							}
						};
						
						(
							element(
								_deli,
								_aspect,
								_vars,
								select_scheduler<_Results>(_deli.storage),
								_handle,
								_work,
								_atomic,
								fake::pack_v<_Results>
							), ...
						);
					};
					
					iterate(_deli, _aspect, _vars, _handle, _work, _atomic, typename current_t::results{});
				}
				
				template<pass_c... _Params>
				static auto invocation(auto &&_callee, auto &&_aspect, _Params &&..._params){
					using relay_t = typename deduce_t::relay_t;
					using resume_t = typename deduce_t::resume_t;
					
					coroutine_t<tool::acyclic::instance_t<relay_t, resume_t>> coro = []<pass_c... _Args>(
						auto &&_callee,
						auto &&_aspect,
						_Args ..._args
					)->coroutine_t<tool::acyclic::instance_t<relay_t, resume_t>>{
						invoker_t callee = {std::forward<decltype(_callee)>(_callee)};
						broker_t aspect = {std::forward<decltype(_aspect)>(_aspect)};
						
						tool::acyclic::instance_t<relay_t, resume_t> yield{
							callee.vars,
							callee.handle,
							callee.self,
							callee.work,
							callee.use,
							callee.invoke_await_suspend,
							callee.invoke_await_resume,
							aspect.handle,
							aspect.delegate,
							aspect.preempt,
							aspect.work,
							aspect.atomic
						};
						
						callee.invoke_await_suspend.produce();
						
						co_yield yield;
						
						if constexpr(broker_t::size == 0)
							if(
								aspect.preempt.consume() == fake::atomic::ultimatum::signal::trigger &&
								aspect.work.consume() == fake::atomic::ultimatum::signal::trigger &&
								aspect.handle
							)
								aspect.handle();
						
						{
							tool::acyclic::guard_t guard_callee{callee.handle, callee.work, callee.invoke_await_resume};
							tool::acyclic::guard_t guard_aspect{aspect.handle, aspect.work, aspect.atomic};
							
							(
								invoke_params<tuple_top, deduce_t>(
									callee.callee,
									aspect,
									callee.vars,
									select_scheduler<typename _Args::token_t>(callee.callee.storage),
									callee.handle,
									callee.work,
									callee.invoke_await_resume,
									std::forward<_Args>(_args)
								), ...
							);
						}
						
						co_yield std::nullopt;
					}(
						std::forward<decltype(_callee)>(_callee),
						std::forward<decltype(_aspect)>(_aspect),
						std::forward<_Params>(_params)...
					);
					
					tool::acyclic::instance_t<relay_t, resume_t> instance = coro.get();
					instance.callee.self = coro;
					
					return instance;
				}
				
			public:
				template<pass_c... _Params>
				requires fake::clamour_c<
					strict_args_unique<std::remove_cvref_t<_Params>...>(),
					[](auto _b){
						static_assert(
							_b,
							"\e[33;40merror<fake::acyclic::operator()>: the tokens of arguments are NOT unique.\e[0m"
						);
					}
				> &&
				fake::clamour_c<
					strict_args_match_entries<std::tuple<_Params...>, typename deduce_t::entries_t>(),
					[](auto _b){
						static_assert(
							_b,
							"\e[33;40merror<fake::acyclic::operator()>: "
							"arguments do NOT match against the entries.\e[0m"
						);
					}
				> &&
				fake::clamour_c<
					strict_args_match_implementations<std::tuple<_Params...>>(),
					[](auto _b){
						static_assert(
							_b,
							"\e[33;40merror<fake::acyclic::operator()>: "
							"arguments are NOT invocable by the implementations.\e[0m"
						);
					}
				>
				auto operator()(_Params &&..._params) const &{
					return invocation(callee, aspect, std::forward<_Params>(_params)...);
				}
				
				template<pass_c... _Params>
				requires fake::clamour_c<
					strict_args_unique<std::remove_cvref_t<_Params>...>(),
					[](auto _b){
						static_assert(
							_b,
							"\e[33;40merror<fake::acyclic::operator()>: the tokens of arguments are NOT unique.\e[0m"
						);
					}
				> &&
				fake::clamour_c<
					strict_args_match_entries<std::tuple<_Params...>, typename deduce_t::entries_t>(),
					[](auto _b){
						static_assert(
							_b,
							"\e[33;40merror<fake::acyclic::operator()>: "
							"arguments do NOT match against the entries.\e[0m"
						);
					}
				> &&
				fake::clamour_c<
					strict_args_match_implementations<std::tuple<_Params...>>(),
					[](auto _b){
						static_assert(
							_b,
							"\e[33;40merror<fake::acyclic::operator()>: "
							"arguments are NOT invocable by the implementations.\e[0m"
						);
					}
				>
				auto operator()(_Params &&..._params) &&{
					return invocation(std::move(callee), std::move(aspect), std::forward<_Params>(_params)...);
				}
				
			private:
				callee_t::type_t callee;
				aspect_t::type_t aspect;
			};
			
		private:
			template<pass_c... _Params>
			static consteval bool strict_bind_unique(fake::type_package<std::tuple<_Params...>>) noexcept{
				constexpr auto tokens = []<pass_c _Pass>(fake::type_package<_Pass>){
					return fake::pack_v<typename _Pass::token_t>;
				};
				
				using tokens_t = fake::tuple::transform_t<std::tuple<_Params...>, tokens>;
				using shrink_t = fake::tuple::shrink_t<tokens_t>;
				
				return std::tuple_size_v<tokens_t> == std::tuple_size_v<shrink_t>;
			}
			
			template<pass_c... _Params>
			static consteval bool strict_bind_implementation(fake::type_package<std::tuple<_Params...>>) noexcept{
				using metas_t = fake::tuple::concat_t<fake::tuple::make_t<_Topology>, fake::tuple::make_t<_Decorate>>;
				using passes_t = std::tuple<_Params...>;
				
				constexpr auto miss = []<execution::topology::meta_c _Meta>(fake::type_package<_Meta>){
					constexpr auto contains = []<pass_c _Pass>(fake::type_package<_Pass>){
						return std::is_same_v<typename _Meta::token, typename _Pass::token_t>;
					};
					
					return fake::tuple::contains_v<passes_t, contains> == false;
				};
				
				return fake::tuple::contains_v<metas_t, miss> == false;
			}
			
		public:
			template<pack_c _C, pack_c _A = decltype(fake::deliver())>
			requires fake::clamour_c<
				strict_bind_unique(fake::pack_v<typename std::remove_cvref_t<_C>::storage_t>),
				[](auto _b){
					static_assert(
						_b,
						"\e[33;40merror<fake::acyclic::bind()>: "
						"the tokens of the acyclic implementations are NOT unique.\e[0m"
					);
				}
			> &&
			fake::clamour_c<
				strict_bind_unique(fake::pack_v<typename std::remove_cvref_t<_A>::storage_t>),
				[](auto _b){
					static_assert(
						_b,
						"\e[33;40merror<fake::acyclic::bind()>: the tokens of the aspects packs are NOT unique.\e[0m"
					);
				}
			> &&
			fake::clamour_c<
				strict_bind_implementation(
					fake::tuple::concat_v<
						typename std::remove_cvref_t<_C>::storage_t,
						typename std::remove_cvref_t<_A>::storage_t
					>
				),
				[](auto _b){
					static_assert(
						_b,
						"\e[33;40merror<fake::acyclic::bind()>: some tokens are bound without implementation.\e[0m"
					);
				}
			>
			static constexpr auto bind(_C &&_c, _A &&_a = fake::deliver()){
				return delegate<_C, _A>{std::forward<_C>(_c), std::forward<_A>(_a)};
			}
		};
		
	}
	
	template<
		execution::topology::info_c _Topology,
		execution::topology::info_c _Decorate = fake::top::info<>,
		template<typename...> typename _Coroutine = custom::tool::acyclic::coroutine
	>
	auto bind(auto &&_e){
		using acyclic_t = custom::acyclic<_Topology, _Decorate, _Coroutine>;
		return acyclic_t::bind(std::forward<decltype(_e)>(_e));
	}
	
	template<
		execution::topology::info_c _Topology,
		execution::topology::info_c _Decorate = fake::top::info<>,
		template<typename...> typename _Coroutine = custom::tool::acyclic::coroutine
	>
	auto bind(auto &&_e, auto &&_f){
		using acyclic_t = custom::acyclic<_Topology, _Decorate, _Coroutine>;
		return acyclic_t::bind(std::forward<decltype(_e)>(_e), std::forward<decltype(_f)>(_f));
	}
	
}

#endif /*__FAKE_ACYCLIC_H__*/ 
