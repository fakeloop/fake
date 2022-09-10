#include "demo.h"

namespace demo
{
	
	// basic 'fake::acyclic' demonstrate with sequential scheduler. 
	
	// 顺序调度器下的基础 'fake::acyclic' 示例。 
	void sequence()
	{
		print("function[", fake::symbol::self([]{}), "]:");
		
		// demonstrate scheduler, invokes delegated tasks sequentially. 
		
		// 演示调度器，顺序调用委托任务。 
		struct sched_t{
			void run(){while(tasks.size()) tasks.front()(), tasks.pop_front();}
			void execute(std::function<void()> &&_delegate){tasks.emplace_back(std::move(_delegate));}
			std::list<std::function<void()>> tasks;
		};
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
				fake::pass<a>([](int _x, float _y){print(_x + _y); return _x + _y + 400;}),
				fake::pass<b>([](auto _x){print(_x--); return std::make_tuple(std::tuple(_x * 2 + 893, _x + 297));}),
				fake::pass<c>([](auto _x, auto _y){print(_x), print(_y);})
			)
		);
		
		// pass arguments to the entry nodes of the acyclic instance. 
		
		// 向有向无环图实例的入口节点传递参数。 
		auto delegate = invoke(fake::pass<a>(50, 64.0f));
		
		using coro_t = demo::coroutine_t;
		
		// register the acyclic completion event coroutine handle and launch the acyclic context. 
		
		// 为有向无环图完成事件注册协程句柄同时启动有向无环图上下文。 
		delegate.await_suspend([]()->coro_t{std::cout << "@done." << std::endl; co_return;}());
		
		// schedule acyclic context in synchronous sequentially. 
		
		// 同步顺序调度有向无环图上下文. 
		sched.run();
	}
	
}
