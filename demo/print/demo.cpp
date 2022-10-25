#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "demo.h"

// config_token: 'token::print'. 
namespace token{struct print{};}

// using 'demo::tool::adaptor<...>' & 'demo::tool::tempack<...>' & 'demo::tool::steal<...>'. 
using namespace demo::tool;

// register meta-implementations for token-based-cpo 'demo::for_each<config_token>' at compile-time. 
struct compile_time{
	friend consteval void inject() noexcept{
		demo::custom::for_each::config::emplace_visitor<[]{}, token::print, adaptor<tempack<std::pair>>>(
			[](auto &&_e, const auto &_f){_f(_e.first), _f(_e.second);}
		);
		demo::custom::for_each::config::emplace_visitor<[]{}, token::print, adaptor<tempack<std::tuple>>>(
			[](auto &&_e, const auto &_f){
				[&]<std::size_t... _Index>(std::index_sequence<_Index...>){
					(_f(std::get<_Index>(_e)), ...);
				}(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<decltype(_e)>>>());
			}
		);
		demo::custom::for_each::config::emplace_visitor<[]{}, token::print, adaptor<tempack<std::unordered_set>>>(
			[](auto &&_e, const auto &_f){for(auto &&e : _e) _f(e);}
		);
		demo::custom::for_each::config::emplace_visitor<[]{}, token::print, adaptor<tempack<std::unordered_map>>>(
			[](auto &&_e, const auto &_f){for(auto &&e : _e) _f(e);}
		);
		demo::custom::for_each::config::emplace_visitor<[]{}, token::print, adaptor<tempack<std::vector>>>(
			[](auto &&_e, const auto &_f){for(auto &&e : _e) _f(e);}
		);
	}
};

struct foo{
	int x;
	double y;
	std::unordered_set<int> s;
	std::unordered_map<std::string, int> m;
};

template<typename _Type>
struct bar{
	std::tuple<foo, std::string, std::string> t;
	std::vector<std::string> v;
	_Type z;
	_Type w;
};

struct coffer{
	private: int password = 114514;
	private: std::string secret = "embarrassing fact";
};

// steal private members. 
template<>
struct demo::tool::steal<adaptor<coffer>, &coffer::password, &coffer::secret>{
	// register meta-implementations for token-based-cpo 'demo::for_each<config_token>' at compile-time. 
	static_assert(demo::custom::for_each::config::emplace_visitor<[]{}, token::print, steal>());
};

int main(int _argc, char* _argv[])
{
	// recurring-lambda currying. 
	auto print = [](auto &&_var){
		static constexpr auto shrink_type = [](fake::pack_c auto _pack){
			using type = fake::take_t<decltype(_pack){}>;
			std::string name{fake::symbol::string_view<type>()};
			
			auto replace = [&](const std::string &_what, const std::string &_with){
				for(
					std::size_t pos = 0;
					name.npos != (pos = name.find(_what.data(), pos, _what.length()));
					pos += _with.length()
				)
					name.replace(pos, _what.length(), _with.data(), _with.length());
			};
			
			replace("std::__cxx11::basic_string<char> ", "std::string");
			replace("std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > ", "std::string");
			replace("std::__cxx11::basic_string<char>", "std::string");
			replace("std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >", "std::string");
			
			return name;
		};
		
		auto impl = [layer = std::size_t{}](auto &_self, std::size_t &_i, auto &&_e, std::string_view _v = "") mutable{
			using type = std::remove_cvref_t<decltype(_e)>;
			std::size_t i = 0;
			auto scope = [&](const char* _split = ""){
				std::cout << _split << std::endl;
				for(std::size_t i = 0; i < layer; i++)
					std::cout << ' ';
			};
			
			if(_i++)
				scope(",");
			
			if constexpr(requires{std::cout << _e;}){
				std::cout << shrink_type(fake::pack_v<type>) << " " << _v << ": " << _e;
			}
			else{
				std::cout << shrink_type(fake::pack_v<type>) << " " << _v << ": {", layer += 4, scope();
				demo::for_each<token::print>(_e, [&](auto &&_e, std::string_view _v = ""){_self(_self, i, _e, _v);});
				layer -= 4, scope(), std::cout << '}';
			}
		};
		
		std::size_t index = 0;
		impl(impl, index, std::forward<decltype(_var)>(_var));
		std::cout << std::endl;
	};
	
	using namespace std::string_literals;
	using bars = bar<std::string>;
	bars var{{{114, 514.0, {1919, 893}, {{"ya", 8}, {"ju", 10}}}, "MGR", "YUH"}, {"SZ", "JOKER"}, "DIYUSI"s, "NEL"s};
	coffer lost_owner{};
	
	print(var);
	print(std::vector<int>{364, 364, 531});
	print("dummy str");
	print(lost_owner);
	
	return 0;
}
