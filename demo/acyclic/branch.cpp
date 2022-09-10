#include "demo.h"

namespace demo
{
	
	// demonstrate of a 'fake::acyclic' instance with diamond shaped topology. 
	
	// 演示菱形拓扑结构的 'fake::acyclic' 实例。 
	void branch()
	{
		print("function[", fake::symbol::self([]{}), "]:");
		
		// using the basic thread based scheduler. 
		
		// 使用基于线程的基础调度器。 
		using sched_t = thread_scheduler;
		sched_t sched;
		
		// declare the node tokens and the acyclic topology. 
		
		// 声明节点代号以及有向无环图拓扑。 
		struct a{}; struct b{}; struct c{}; struct d{};
		using topology_t = fake::top::info<
			fake::top::meta<a, fake::top::results<b, c>, fake::top::args<>>,
			fake::top::meta<b, fake::top::results<d>, fake::top::args<a>>,
			fake::top::meta<c, fake::top::results<d>, fake::top::args<a>>,
			fake::top::meta<d, fake::top::results<>, fake::top::args<b, c>>
		>;
		
		//     a     //
		//    / \    //
		//   /   \   //
		//  b     c  //
		//   \   /   //
		//    \ /    //
		//     d     //
		
		// view reference to a sub-range of a 'std::vector'. 
		
		// 引用 'std::vector' 内某一子区间的 view。 
		struct view{
			using it = std::vector<char>::iterator;
			it it_begin, it_end;
			it begin() const{return it_begin;}
			it end() const{return it_end;}
			std::size_t size() const{return it_end - it_begin;}
		};
		
		// associate node tokens with node implementation. 
		// bind the topology meta data with the node implementation. 
		
		// 关联节点代号与节点实现。 
		// 将拓扑元数据和节点实现进行绑定。 
		auto invoke = fake::bind<topology_t>(
			fake::deliver(
				fake::pass<fake::acyclic::token::sched<void>>(std::ref(sched)),
				fake::pass<a>(
					[](std::vector<char> &_x){
						auto begin = _x.begin(), middle = begin + _x.size() / 2, end = _x.end();
						return std::tuple(view{begin, middle}, view{middle, end});
					}
				),
				fake::pass<b>(
					[](const auto &_x){
						for(auto &e : _x)
							std::cout << e, e = '@', std::this_thread::yield();
						return _x.size();
					}
				),
				fake::pass<c>(
					[](const auto &_x){
						for(auto &e : _x)
							std::cout << e, e = '$', std::this_thread::yield();
						return _x.size();
					}
				),
				fake::pass<d>(
					[](auto _x, auto _y){
						std::cout << std::endl;
						print(_x + _y);
					}
				)
			)
		);
		
		// initialize the vector. 
		
		// 初始化 vector。 
		std::vector<char> vec;
		for(int i = 0; i < 20; i++) vec.emplace_back('*');
		for(int i = 0; i < 20; i++) vec.emplace_back('#');
		
		// pass arguments to the entry nodes of the acyclic instance. 
		
		// 向有向无环图实例的入口节点传递参数。 
		auto delegate = invoke(fake::pass<a>(std::ref(vec)));
		
		using coro_t = demo::coroutine_t;
		
		// register the acyclic completion event coroutine handle and launch the acyclic context. 
		
		// 为有向无环图完成事件注册协程句柄同时启动有向无环图上下文。 
		delegate.await_suspend([]()->coro_t{std::cout << "@done." << std::endl; co_return;}());
		
		// wait until all threads joined. 
		
		// 等待所有线程返回。 
		sched.wait();
		
		// print the modified vector. 
		
		// 打印改动后的 vector。 
		print(vec);
	}
	
}
