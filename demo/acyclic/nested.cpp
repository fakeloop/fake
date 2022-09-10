#include "demo.h"

namespace demo
{
	
	// demonstrate of nested 'fake::acyclic' instances. 
	
	// 将 'fake::acyclic' 实例进行嵌套的示例。 
	void nested()
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
		// 'fake::acyclic::token::dupli' means the node associated with the derived token would transmit its return 
		// value to the subsequent nodes by reference duplications. 
		// otherwise, if the return value is a tuple, then it would be distribute to each subsequent nodes by elements. 
		
		// 声明节点代号以及有向无环图拓扑。 
		// 代号间的继承关系代表额外的分组从属关系。 
		// 'fake::acyclic' 可以分组管理节点的行为。 
		// 'fake::acyclic::token::await' 意味着其派生代号所关联节点的返回结果将通过 'awaitable' 异步调用传递给后继节点。 
		// 'fake::acyclic::token::dupli' 意味着其派生代号所关联节点的返回结果将被以引用副本的方式传递给后继节点。 
		// 否则，如果返回值是一个 tuple，那么它将被逐元素地分发到各后继节点。 
		struct an{}; struct bn{}; struct cn{};
		using nest_t = fake::top::info<
			fake::top::meta<an, fake::top::results<bn, cn>, fake::top::args<>>,
			fake::top::meta<bn, fake::top::results<>, fake::top::args<an>>,
			fake::top::meta<cn, fake::top::results<>, fake::top::args<an>>
		>;
		struct a{}; struct b{}; struct c : fake::acyclic::token::await, fake::acyclic::token::dupli{}; struct d{};
		using topology_t = fake::top::info<
			fake::top::meta<a, fake::top::results<b, c>, fake::top::args<>>,
			fake::top::meta<b, fake::top::results<d>, fake::top::args<a>>,
			fake::top::meta<c, fake::top::results<d>, fake::top::args<a>>,
			fake::top::meta<d, fake::top::results<>, fake::top::args<b, c>>
		>;
		
		//     a      .->an     //
		//    / \    /  / \     //
		//   /   '->'  /   \    //
		//  b     c   bn    cn  //
		//   \   .<-.  \   /    //
		//    \ /    \  \ /     //
		//     d      '<-*      //
		
		// associate node tokens with node implementation. 
		// bind the topology meta data with the node implementation. 
		
		// 关联节点代号与节点实现。 
		// 将拓扑元数据和节点实现进行绑定。 
		auto nest = fake::bind<nest_t>(
			fake::deliver(
				fake::pass<fake::acyclic::token::sched<void>>(std::ref(sched)),
				fake::pass<an>([](int _x){print("an: ", _x); return std::tuple(_x + 1405, _x + 296);}),
				fake::pass<bn>([](auto _x){print("bn: ", _x); return _x;}),
				fake::pass<cn>([](auto _x){print("cn: ", _x); return _x;})
			)
		);
		
		// associate node tokens with node implementation. 
		// bind the topology meta data with the node implementation. 
		// the nested acyclic instance would behave as a 'awaiter'. 
		
		// 关联节点代号与节点实现。 
		// 将拓扑元数据和节点实现进行绑定。 
		// 内部嵌套的有向无环图对象，其行为同 'awaiter'。 
		auto invoke = fake::bind<topology_t>(
			fake::deliver(
				fake::pass<fake::acyclic::token::sched<void>>(std::ref(sched)),
				fake::pass<a>([](int _x){print("a: ", _x); return std::tuple(_x, _x + 400);}),
				fake::pass<b>([](auto _x){print("b: ", _x); return _x + 250;}),
				fake::pass<c>([n = std::move(nest)](auto _x){print("c: ", _x); return n(fake::pass<an>(_x));}),
				fake::pass<d>([](auto _x, auto _y, auto _z){print("d: ", _x, ", ", _y - _z - 216);})
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
	
} // namespace demo. 
