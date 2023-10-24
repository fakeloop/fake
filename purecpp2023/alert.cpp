#include <iostream>

#include "alert.h"

namespace demo
{
	
	template<typename _Type>
	consteval auto ub() noexcept{
		_Type* error = nullptr;
		if constexpr(std::is_integral_v<_Type> == false)
			error--;
		return fake::pack_v<_Type>;
	}
	
	void foo(auto ..._e) noexcept{
		using namespace fake::literals;
		
		constexpr auto each = [](auto _arg){
			if constexpr(std::is_integral_v<decltype(_arg)> == false){
				decltype(_arg)* x = 114514;
			}
		};
		
		
		constexpr auto alert = [](auto _arg){
			if constexpr(std::is_integral_v<decltype(_arg)> == false)
				fake::alert<"type: --- ["_v, fake::pack_v<decltype(_arg)>, "] ---"_v>;
		};
		
		// debug message via ill formed. 
		// (each(_e), ...);
		
		// debug message via undefined behavior in 'consteval'. 
		// (demo::ub<decltype(_e)>(), ...);
		
		// debug message via 'fake::alert'. 
		(alert(_e), ...);
	}
	
}

int main(int _argc, char* _argv[])
{
	using namespace fake::literals;
	
	struct bar{};
	
	demo::foo(int{}, bar{}, char{});
	
	return 0;
}
