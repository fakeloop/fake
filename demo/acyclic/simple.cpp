#include "demo.h"

namespace demo
{
	
	// basic 'fake::acyclic' demonstrate with inline scheduler. 
	
	// 内联调度器下的基础 'fake::acyclic' 示例。 
	void simple()
	{
		print("function[", fake::symbol::self([]{}), "]:");
		
		// demonstrate inline scheduler, invokes delegated tasks in-place. 
		
		// 演示内联调度器，就地调用委托任务。 
		struct sched_t{void execute(std::function<void()> &&_delegate){_delegate();}};
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
				fake::pass<b>([](auto _x){print(_x); return (_x << 1) + _x + 377;}),
				fake::pass<c>([](auto _x){print(_x); return _x;})
			)
		);
		
		// pass arguments to the entry nodes of the acyclic instance. 
		
		// 向有向无环图实例的入口节点传递参数。 
		auto delegate = invoke(fake::pass<a>(114));
		
		using coro_t = demo::coroutine_t;
		
		// register the acyclic completion event coroutine handle and launch the acyclic context. 
		
		// 为有向无环图完成事件注册协程句柄同时启动有向无环图上下文。 
		delegate.await_suspend([]()->coro_t{std::cout << "@done." << std::endl; co_return;}());
	}
	
}
