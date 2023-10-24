#include <iostream>

#include "tuple.h"
#include "view.h"

int main(int _argc, char* _argv[])
{
	constexpr auto has_foo = [](fake::pack_c auto _pack){return requires(fake::take_t<_pack.echo> _e){_e.foo();};};
	constexpr auto has_bar = [](fake::pack_c auto _pack){return requires(fake::take_t<_pack.echo> _e){_e.bar();};};
	
	constexpr auto each = [](fake::pack_c auto _state, auto _index, fake::pack_c auto _type){
		using state = fake::take_t<_state.echo>;
		if constexpr(state::second_type::value){
			using op = typename state::first_type;
			constexpr auto detector = fake::take_t<_type.echo>::value;
			
			return fake::pack_v<std::pair<op, fake::mezz_t<detector(fake::pack_v<op>)>>>;
		}
		else{
			return fake::null_t{};
		}
	};
	
	{
		struct duck{void foo(); void bar();};
		
		using constraint = std::tuple<fake::mezz_t<has_foo>, fake::mezz_t<has_bar>>;
		using result = fake::tuple::for_each_t<constraint, std::pair<duck, fake::mezz_t<true>>, each>;
		
		std::cout << fake::symbol::string_view<duck>() << ": valid = " << result::second_type::value << std::endl;
	}
	
	{
		struct duck{void foo(int); void bar();}; // invalid arg type in member function 'foo'. 
		
		using constraint = std::tuple<fake::mezz_t<has_foo>, fake::mezz_t<has_bar>>;
		using result = fake::tuple::for_each_t<constraint, std::pair<duck, fake::mezz_t<true>>, each>;
		
		std::cout << fake::symbol::string_view<duck>() << ": valid = " << result::second_type::value << std::endl;
	}
	
	return 0;
}
