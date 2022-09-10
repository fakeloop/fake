#include "demo.h"

namespace demo
{
	
	// 'fake::acyclic' instance may invoke 'awaitable' object in an asynchronous context. 
	
	// 'fake::acyclic' 实例可以在异步上下文中调用 'awaitable' 对象。 
	void awaiter()
	{
		print("function[", fake::symbol::self([]{}), "]:");
		
		// using the basic thread based scheduler. 
		
		// 使用基于线程的基础调度器。 
		using sched_t = thread_scheduler;
		sched_t sched;
		
		// declare the node tokens and the acyclic topology. 
		// the derivation relationship between tokens represents additional group relegation. 
		// 'fake::acyclic' could control the node behaviours by groups. 
		// 'fake::acyclic::token::await' means the node associated with the derived token would transmit its return 
		// value to the subsequent nodes via an asynchronous 'awaitable' schedule. 
		
		// 声明节点代号以及有向无环图拓扑。 
		// 代号间的继承关系代表额外的分组从属关系。 
		// 'fake::acyclic' 可以分组管理节点的行为。 
		// 'fake::acyclic::token::await' 意味着其派生代号所关联节点的返回结果将通过 'awaitable' 异步调用传递给后继节点。 
		struct a{}; struct b : fake::acyclic::token::await{}; struct c{};
		using sequence_t = fake::top::sequence<a, b, c>;
		
		// associate node tokens with node implementation. 
		// bind the topology meta data with the node implementation. 
		
		// 关联节点代号与节点实现。 
		// 将拓扑元数据和节点实现进行绑定。 
		auto invoke = fake::bind<sequence_t>(
			fake::deliver(
				fake::pass<fake::acyclic::token::sched<void>>(std::ref(sched)),
				fake::pass<a>([](int _x){print(_x); return _x + 400;}),
				fake::pass<b>([&sched](auto _x){print(_x); return timer{1000, sched};}),
				fake::pass<c>([]{print("awake.");})
			)
		);
		
		// pass arguments to the entry nodes of the acyclic instance. 
		
		// 向有向无环图实例的入口节点传递参数。 
		auto delegate = invoke(fake::pass<a>(114));
		
		using coro_t = demo::coroutine_t;
		
		// register the acyclic completion event coroutine handle and launch the acyclic context. 
		
		// 为有向无环图完成事件注册协程句柄同时启动有向无环图上下文。 
		delegate.await_suspend([]()->coro_t{std::cout << "@done." << std::endl; co_return;}());
		
		// wait until all threads joined. 
		
		// 等待所有线程返回。 
		sched.wait();
	}
	
}
