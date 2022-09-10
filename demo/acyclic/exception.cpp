#include "demo.h"

namespace demo
{
	
	// demonstrate of exception handle in a 'fake::acyclic' instance. 
	
	// 在 'fake::acyclic' 实例中进行异常处理的示例。 
	void exception()
	{
		print("function[", fake::symbol::self([]{}), "]:");
		
		// using the basic thread based scheduler. 
		
		// 使用基于线程的基础调度器。 
		using sched_t = thread_scheduler;
		sched_t sched;
		
		// declare the node tokens and the acyclic topology. 
		// the derivation relationship between tokens represents additional group relegation. 
		// 'fake::acyclic' could control the node behaviours by groups. 
		
		// 声明节点代号以及有向无环图拓扑。 
		// 代号间的继承关系代表额外的分组从属关系。 
		// 'fake::acyclic' 可以分组管理节点的行为。 
		struct x{}; struct y{};
		struct a{}; struct b : x{}; struct c : y{}; struct d{}; struct e{}; struct f{};
		using topology_t = fake::top::info<
			fake::top::meta<a, fake::top::results<b, c, d>, fake::top::args<>>,
			fake::top::meta<b, fake::top::results<e>, fake::top::args<a>>,
			fake::top::meta<c, fake::top::results<e>, fake::top::args<a>>,
			fake::top::meta<d, fake::top::results<e, f>, fake::top::args<a>>,
			fake::top::meta<e, fake::top::results<>, fake::top::args<b, c, d>>,
			fake::top::meta<f, fake::top::results<>, fake::top::args<d>>
		>;
		
		//       a      //
		//      /|\     //
		//     / | \    //
		//    /  |  \   //
		//  b:x c:y  d  //
		//    \  |  /|  //
		//     * * / |  //
		//      \|/  |  //
		//       e   f  //
		
		// [*]: throw. //
		
		// associate node tokens with node implementation. 
		// bind the topology meta data with the node implementation. 
		
		// 关联节点代号与节点实现。 
		// 将拓扑元数据和节点实现进行绑定。 
		auto invoke = fake::bind<topology_t>(
			fake::deliver(
				fake::pass<fake::acyclic::token::sched<void>>(std::ref(sched)),
				fake::pass<fake::acyclic::token::spare<x>>(
					[]{
						auto eptr = std::current_exception();
						try{if(eptr) std::rethrow_exception(eptr);}catch(int _e){print("exception_handler<x>: ", _e);}
					}
				),
				fake::pass<fake::acyclic::token::spare<y>>(
					[]{
						auto eptr = std::current_exception();
						try{if(eptr) std::rethrow_exception(eptr);}catch(int _e){print("exception_handler<y>: ", _e);}
					}
				),
				fake::pass<a>([]{return std::tuple(114, 514, 1919);}),
				fake::pass<b>([](auto _x){throw _x; return _x;}),
				fake::pass<c>([](auto _x){throw _x; return _x;}),
				fake::pass<d>([](auto _x){return std::make_tuple(_x, std::tuple());}),
				fake::pass<e>([](auto _x, auto _y, auto _z){print(_x + _y + _z);}),
				fake::pass<f>([]{print("node<f>");})
			)
		);
		
		// pass arguments to the entry nodes of the acyclic instance. 
		
		// 向有向无环图实例的入口节点传递参数。 
		auto delegate = invoke(fake::pass<a>());
		
		using coro_t = demo::coroutine_t;
		
		// register the acyclic completion event coroutine handle and launch the acyclic context. 
		
		// 为有向无环图完成事件注册协程句柄同时启动有向无环图上下文。 
		delegate.await_suspend([]()->coro_t{std::cout << "@done." << std::endl; co_return;}());
		
		// wait until all threads joined. 
		
		// 等待所有线程返回。 
		sched.wait();
	}
	
}
