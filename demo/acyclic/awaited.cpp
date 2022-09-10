#include "demo.h"

namespace demo
{
	
	// 'fake::acyclic' context may behave like a 'awaitable'. 
	
	// 'fake::acyclic' 上下文的行为可以与 'awaitable' 一致。 
	void awaited()
	{
		print("function[", fake::symbol::self([]{}), "]:");
		
		// using the basic thread based scheduler. 
		
		// 使用基于线程的基础调度器。 
		using sched_t = thread_scheduler;
		sched_t sched;
		
		// declare the node tokens and the acyclic topology. 
		
		// 声明节点代号以及有向无环图拓扑。 
		struct a{}; struct b{}; struct c{};
		using sequence_t = fake::top::sequence<a, b, c>;
		
		// associate node tokens with node implementation. 
		// bind the topology meta data with the node implementation. 
		
		// 关联节点代号与节点实现。 
		// 将拓扑元数据和节点实现进行绑定。 
		auto invoke = fake::bind<sequence_t>(
			fake::deliver(
				fake::pass<fake::acyclic::token::sched<void>>(std::ref(sched)),
				fake::pass<a>([](int _x){print(_x); return _x + 400;}),
				fake::pass<b>([](auto _x){print(_x); return _x + 296;}),
				fake::pass<c>([](auto _x){print("awake."); return _x + 1109;})
			)
		);
		
		using coro_t = demo::coroutine_t;
		
		// schedule a basic corotine, and launch the acyclic context as a 'awaitable' in it. 
		
		// 调度一个基础协程，并在其中将有向无环图上下文作为 'awaitable' 启动。 
		sched.execute(
			[ivk = std::move(invoke)]()->coro_t{
				std::apply(print, co_await ivk(fake::pass<a>(114)));
				std::cout << "@done." << std::endl;
			}()
		);
		
		// wait until all threads joined. 
		
		// 等待所有线程返回。 
		sched.wait();
	}
	
}
