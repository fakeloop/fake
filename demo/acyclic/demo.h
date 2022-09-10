#include <iostream>
#include <vector>
#include <list>
#include <thread>

#include "acyclic.h"
#include "symbol.h"

namespace demo
{
	
	// print elements '_e'. 
	// if 'std::cout << _e' is valid, then prints with it. 
	// else, iterate '_e' as a container, prints the elements it contains. 
	
	// 打印若干元素 '_e'。 
	// 如果 'std::cout << _e'，则用其打印。 
	// 否则，将 '_e' 作为容器遍历，打印其中元素。 
	inline static constexpr auto print = [](const auto &..._e){
		(
			[](const auto &_x){
				if constexpr(requires{std::cout << _x;})
					std::cout << _x;
				else
					for(const auto &e : _x) std::cout << e;
			}(_e), ...
		);
		std::cout << std::endl;
	};
	
	// basic coroutine implementation. 
	// used to demonstrate the 'awaitable' interfaces of 'fake::acyclic' instance. 
	
	// 基础协程实现。 
	// 用以演示 'fake::acyclic' 实例的 'awaitable' 接口。 
	struct promise;
	struct coroutine_t : std::coroutine_handle<promise>{using promise_type = struct promise;};
	struct promise{
		coroutine_t get_return_object(){return {coroutine_t::from_promise(*this)};}
		std::suspend_always initial_suspend() const noexcept{return {};}
		std::suspend_never final_suspend() const noexcept{return {};}
		void return_void() const noexcept{}
		void unhandled_exception() const noexcept{}
	};
	
	// basic thread based scheduler implementation. 
	// schedule tasks by creating new threads. 
	// separate data accessment with spin lock 'fake::atomic::guard'. 
	
	// 基于线程的基础调度器实现。 
	// 通过创建新线程进行任务调度。 
	// 使用 'fake::atomic::guard' 自旋锁进行数据访问隔离。 
	struct thread_scheduler{
		void wait(){
			auto get_size = [this]{fake::atomic::read guard(mutex); return tasks.size();};
			while(get_size()){
				std::thread* ptr;
				{fake::atomic::read guard(mutex); ptr = &tasks.front();}
				{ptr->join(); fake::atomic::write guard(mutex); tasks.pop_front();}
			}
		}
		void execute(std::function<void()> &&_delegate){
			fake::atomic::write guard(mutex);
			tasks.emplace_back(std::move(_delegate));
		}
		fake::atomic::guard mutex;
		std::list<std::thread> tasks;
	};
	
	// basic timer 'awaitable' implementation. 
	// delay coroutine resuming by scheduling a lambda wrapper with thread sleep. 
	
	// 基础计时器 'awaitable' 实现。 
	// 通过调度包含线程休眠的 lambda 包装进行协程延迟唤起。 
	template<typename _Sched>
	struct timer{
		void await_suspend(std::coroutine_handle<> _h){
			sched.execute([_h, t = time]{std::this_thread::sleep_for(std::chrono::milliseconds{t}); _h();});
		}
		void await_resume() const{}
		bool await_ready() const{return false;}
		uint64_t time;
		_Sched &sched;
	};
	
	// 'awaitable' 'coroutine' implementation. 
	// both 'awaitable' interfaces and 'coroutine' interfaces are provided. 
	// the delegated coroutine handle from the caller would be resumed when the 'final_suspend()' of '*this' occurred. 
	
	// 'awaitable' 'coroutine' 实现。 
	// 同时提供 'awaitable' 和 'coroutine' 接口。 
	// 由调用者委托的协程句柄将在 '*this' 的 'final_suspend()' 发生时被唤起。 
	struct awaitable_coroutine_promise;
	struct awaitable_coroutine_t : std::coroutine_handle<awaitable_coroutine_promise>{
		using promise_type = struct awaitable_coroutine_promise;
		inline void await_suspend(std::coroutine_handle<> _h);
		inline int await_resume() const;
		inline bool await_ready() const{return false;}
	};
	struct awaitable_coroutine_promise{
		awaitable_coroutine_t get_return_object(){return {awaitable_coroutine_t::from_promise(*this)};}
		std::suspend_always initial_suspend() const noexcept{return {};}
		std::suspend_never final_suspend() const noexcept{handle(); return {};}
		void return_value(int _result) noexcept{result = _result;}
		void unhandled_exception() const noexcept{}
		
		std::coroutine_handle<> handle;
		int result;
	};
	inline void awaitable_coroutine_t::await_suspend(std::coroutine_handle<> _h){promise().handle = _h; resume();}
	inline int awaitable_coroutine_t::await_resume() const{return promise().result;}
	
}
