#include <iostream>

#include "flat.h"
#include "view.h"

int main(int _argc, char* _argv[])
{
	using namespace fake::literals;
	
	constexpr fake::query_c auto query = fake::query{
		fake::mate{"T[0]"_v, 1919},
		fake::mate{"T[1]"_v, 114.514f},
		fake::mate{"T[2]"_v, 'y'}
	};
	
	using result = fake::query_rebind_t<decltype(query), "T[1]"_v, double>;
	constexpr result result_v{};
	
	std::cout << fake::type_view(query) << std::endl;
	std::cout << fake::type_view(result_v) << std::endl;
	std::cout << fake::type_view(result_v["T[1]"_v]) << std::endl;
	
	struct foo{};
	struct bar{};
	
	constexpr fake::query_c auto foobar = fake::query{
		fake::mate{foo{}, 810},
		fake::mate{bar{}, 364.531f},
	};
	
	static_assert(std::is_aggregate_v<std::remove_cvref_t<decltype(foobar)>>);
	
	std::cout << fake::type_view(foobar) << std::endl;
	constexpr auto fv = foobar[foo{}];
	std::cout << fv << " | " << foobar[bar{}] << std::endl;
	
	return 0;
}
