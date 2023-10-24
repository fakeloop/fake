#include <iostream>

#include "alert.h"
#include "tuple.h"
#include "view.h"

int main(int _argc, char* _argv[]){
	using namespace fake::literals;
	
	constexpr auto each = [](fake::pack_c auto _state, auto _index, fake::pack_c auto _type){
		using state = fake::take_t<_state.echo>;
		using type = fake::take_t<_type.echo>;
		// fake::alert<"state: ### ["_v, fake::pack_v<state>, "] ###"_v>;
		// fake::alert<"type: ### ["_v, fake::pack_v<type>, "] ###"_v>;
		
		if constexpr(std::is_integral_v<type>)
			return fake::pack_v<fake::tuple::emplace_front_t<state, std::make_unsigned_t<type>>>;
		else
			return fake::pack_v<fake::tuple::emplace_front_t<state, type>>;
	};
	
	using origin = std::tuple<int, float, char>;
	using result = fake::tuple::for_each_t<origin, std::tuple<>, each>;
	
	std::cout << fake::symbol::string_view<result>() << std::endl;
	
	return 0;
}
