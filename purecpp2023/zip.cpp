#include <iostream>

#include "tuple.h"
#include "view.h"

int main(int _argc, char* _argv[]){
	using indices = std::tuple<fake::mezz_t<0>, fake::mezz_t<1>, fake::mezz_t<2>, fake::mezz_t<3>>;
	using keys = std::tuple<int, float, char>;
	using values = std::tuple<unsigned int, double, char*>;
	
	{
		using zip = fake::tuple::zip_t<indices, keys, values>;
		std::cout << fake::symbol::string_view<zip>() << std::endl;
		[]<std::size_t... _index>(std::index_sequence<_index...>){
			((std::cout << fake::symbol::string_view<std::tuple_element_t<_index, zip>>() << std::endl), ...);
		}(std::make_index_sequence<std::tuple_size_v<zip>>());
	}
	
	{
		using zip = fake::tuple::zip_t<keys, values>;
		std::cout << fake::symbol::string_view<zip>() << std::endl;
		[]<std::size_t... _index>(std::index_sequence<_index...>){
			((std::cout << fake::symbol::string_view<std::tuple_element_t<_index, zip>>() << std::endl), ...);
		}(std::make_index_sequence<std::tuple_size_v<zip>>());
	}
	
	return 0;
}
