#include "demo.h"

namespace demo
{
	
	// an 'aspect' is an observation point of the data transmission form a previous node to a subsequent node. 
	// 'fake::acyclic' provides automatic aspect observation controlled by compile-time meta data 'fake::top::info'. 
	
	// '切面' 是前后节点间数据传递过程的观测点。 
	// 'fake::acyclic' 提供自动切面观测, 其通过编译期元数据 'fake::top::info' 进行控制。 
	void aspect()
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
		struct x{}; struct y{}; struct z{}; struct p{};
		struct a : p{}; struct b : x{}; struct c : x{}; struct d : y{}; struct e : z{}; struct f{};
		using topology_t = fake::top::info<
			fake::top::meta<a, fake::top::results<c>, fake::top::args<>>,
			fake::top::meta<b, fake::top::results<e>, fake::top::args<>>,
			fake::top::meta<c, fake::top::results<e>, fake::top::args<a>>,
			fake::top::meta<d, fake::top::results<e, f>, fake::top::args<>>,
			fake::top::meta<e, fake::top::results<>, fake::top::args<b, c, d>>,
			fake::top::meta<f, fake::top::results<>, fake::top::args<d>>
		>;
		
		// declare the aspcect or node tokens and the acyclic topology. 
		// 'fake::acyclic::token::dupli' means the node associated with the derived token would transmit its return 
		// value to the subsequent nodes by reference duplications. 
		// otherwise, if the return value is a tuple, then it would be distribute to each subsequent nodes by elements. 
		// 'fake::top::observer<src, dst>' refers to the observation of the data transmission from 'src' to 'dst'. 
		// the template parameter of 'void' means to match any node token. 
		
		// 声明切面或节点代号以及有向无环图拓扑。 
		// 'fake::acyclic::token::dupli' 意味着其派生代号所关联节点的返回结果将被以引用副本的方式传递给后继节点。 
		// 否则，如果返回值是一个 tuple，那么它将被逐元素地分发到各后继节点。 
		// 'fake::top::observer<src, dst>' 代表观测从 'src' 到 'dst' 的数据传递。 
		// 模板参数 'void' 意味着匹配任何节点代号。 
		struct m{}; struct n{}; struct j : fake::acyclic::token::dupli{}; struct k{};
		using aspect_t = fake::top::info<
			fake::top::meta<m, fake::top::results<n>, fake::top::observer<x, z>>,
			fake::top::meta<n, fake::top::results<>, fake::top::args<m>>,
			
			fake::top::meta<j, fake::top::results<k>, fake::top::observer<y, void>>,
			fake::top::meta<k, fake::top::results<>, fake::top::args<j>>
		>;
		
		// node 'c:x' and node 'e:z' would NOT be invoked, because the previous node 'a:p' would raise a exception. 
		// 'b:x => e:z', 'c:x => e:z' and 'd:y => e:z' data transmission would NOT occurs, as well as the aspect event. 
		
		// 节点 'c:x' 和 'e:z' 不会被执行，因为其前驱节点 'a:p' 会发出一个异常。 
		// 'b:x => e:z'， 'c:x => e:z' 和 'd:y => e:z' 的数据传递不会发生，其切面事件亦然。 
		
		//      a:p      //
		//       |       //
		//       *       //
		//       |       //
		//  b:x c:x d:y  //
		//    \  |  /|   //
		//     @ @ @ @   //
		//      \|/  |   //
		//      e:z  f   //
		
		// associate node tokens with node implementation. 
		
		// 关联节点代号与节点实现。 
		auto topology = fake::deliver(
			fake::pass<fake::acyclic::token::sched<void>>(std::ref(sched)),
			fake::pass<fake::acyclic::token::spare<p>>(
				[]{
					auto eptr = std::current_exception();
					try{if(eptr) std::rethrow_exception(eptr);}catch(int _e){print("exception_handler<p>: ", _e);}
				}
			),
			fake::pass<a>([]{throw 364; return 931;}),
			fake::pass<b>([](int _x){return _x;}),
			fake::pass<c>([](auto _x){return _x;}),
			fake::pass<d>([](int _x){return std::make_tuple(_x, std::tuple(1919, 810));}),
			fake::pass<e>([](auto ..._e){print("e:" + ((" " + std::to_string(_e)) + ...));}),
			fake::pass<f>([](auto ..._f){print("f:" + ((" " + std::to_string(_f)) + ...));})
		);
		
		// the parameter 'auto _info' is the complie-time meta data of the observed nodes. 
		
		// 'auto _info' 参数是被观测对象的编译期元数据。 
		auto aspect = fake::deliver(
			fake::pass<fake::acyclic::token::sched<void>>(std::ref(sched)),
			fake::pass<m>(
				[](auto _info, auto &_obs){
					print("m: ", _obs++);
					print(fake::symbol::string_view<decltype(_info)>());
					return _obs;
				}
			),
			fake::pass<n>([](auto _copy){print("n: ", _copy);}),
			
			fake::pass<j>(
				[](auto _info, auto &..._obs){
					print("j:" + ((" " + std::to_string(_obs)) + ...));
					print(fake::symbol::string_view<fake::take_t<_info>>());
					return std::tuple(_obs = -_obs...);
				}
			),
			fake::pass<k>([](auto ..._copy){print("k:" + ((" " + std::to_string(_copy)) + ...));})
		);
		
		// bind the topology meta data with the node implementation. 
		
		// 将拓扑元数据和节点实现进行绑定。 
		auto invoke = fake::bind<topology_t, aspect_t>(std::move(topology), std::move(aspect));
		
		// pass arguments to the entry nodes of the acyclic instance. 
		
		// 向有向无环图实例的入口节点传递参数。 
		auto delegate = invoke(fake::pass<a>(), fake::pass<b>(114), fake::pass<d>(514));
		
		using coro_t = demo::coroutine_t;
		
		// register the aspect completion event coroutine handle. 
		
		// 为切面完成事件注册协程句柄。 
		delegate.await_aspect([]()->coro_t{std::cout << "@aspect done." << std::endl; co_return;}());
		
		// register the acyclic completion event coroutine handle and launch the acyclic context. 
		
		// 为有向无环图完成事件注册协程句柄同时启动有向无环图上下文。 
		delegate.await_suspend([]()->coro_t{std::cout << "@done." << std::endl; co_return;}());
		
		// wait until all threads joined. 
		
		// 等待所有线程返回。 
		sched.wait();
	}
	
}
