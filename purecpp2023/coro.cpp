#include <iostream>
#include <list>
#include <functional>
#include <coroutine>

#include "view.h"

struct promise;
struct await_coro : std::coroutine_handle<promise>{
	using promise_type = struct promise;
	~await_coro(){destroy();}
	inline void await_suspend(auto _h);
	inline int await_resume() const;
	inline bool await_ready() const{return false;}
};

struct promise{
	promise(auto &&_this, std::reference_wrapper<std::list<std::function<void(int)>>> _tasks):
	capture(_this.__cap), /* careful, the use of name with '__' is a undefined behavior */
	tasks(_tasks.get()){
		std::cout << fake::symbol::string_view<decltype(_this)>() << std::endl;
		std::cout << "capture = " << capture << std::endl;
		std::cout << std::hex << std::size_t(std::addressof(_this)) << std::dec << std::endl << std::endl;
	}
	await_coro get_return_object(){return {await_coro::from_promise(*this)};}
	std::suspend_never initial_suspend() const noexcept{return {};}
	std::suspend_always final_suspend() const noexcept{return {};}
	void return_value(int _result) noexcept{result *= _result;}
	void unhandled_exception() const noexcept{}
	
	int capture;
	int resume{};
	int result = 1;
	std::list<std::function<void(int)>> &tasks;
};

inline void await_coro::await_suspend(auto _h){
	promise().resume = _h.promise().capture;
	std::cout << "awaiter = " << promise().capture << ", suspend = " << promise().resume << std::endl;
	promise().tasks.emplace_back([&res = promise().result, _h](int _res){res *= _res; _h();});
};

inline int await_coro::await_resume() const{
	std::cout << "awaiter = " << promise().capture << ", resume = " << promise().resume << std::endl;
	return promise().result;
}

int main(int _argc, char* _argv[])
{
	std::list<std::function<void(int)>> tasks;
	
	auto pms = [cap = 1919 /* careful, may hauling if been used in the coroutine */](auto _tasks) -> await_coro {
		co_return co_await [cap = 810 /* careful, may hauling */](auto _tasks) -> await_coro {
			co_return co_await [cap = 364 /* careful, may hauling */](auto _tasks) -> await_coro {
				co_return 2;
			}(_tasks);
		}(_tasks);
	}(std::ref(tasks));
	
	std::cout << "done = " << pms.done() << ", result = " << pms.promise().result << std::endl;
	
	while(tasks.size())
		tasks.front()(tasks.size() * 1816 - 1785), tasks.pop_front();
	
	std::cout << "done = " << pms.done() << ", result = " << pms.promise().result << std::endl;
	
	return 0;
}
