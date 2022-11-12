#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "type_format.h"

struct foo{
	int x;
	double y;
	std::unordered_set<int> s;
	std::unordered_map<std::string, int> m;
};

template<typename _Type>
struct bar{
	std::tuple<foo, std::string, std::string> t;
	std::vector<std::string_view> v;
	_Type z;
	_Type w;
	char s[2] = "S";
	char n[2] = "N";
	char e[0];
	char m[3] = "NN";
	char l[0];
};

struct foobar{
public:
	template<std::convertible_to<bar<std::string>> _Cookie>
	foobar(_Cookie &&_cookie): cookie(std::forward<_Cookie>(_cookie)){}
	
private:
	int password = 114514;
	std::string secret = "embarrassing fact";
	bar<std::string> cookie;
};

// steal private members. 
template<>
struct fake::tool::steal<[]{}, fake::tool::adaptor<foobar>, &foobar::password, &foobar::secret, &foobar::cookie>{
	// register meta-implementations for token-based-cpo 'fake::for_each<fake::cpo::format<...>>' at compile-time. 
	using token = fake::cpo::format<fake::io::token::pretty>;
	static_assert(fake::custom::for_each::config::emplace_visitor<[]{}, token, steal>());
};

int main(int _argc, char* _argv[])
{
	using namespace std::string_literals;
	
	bar<std::string> var{
		{{114, 514.0, {1919, 893}, {{"ya", 8}, {"ju", 10}}}, "MGR", "YUH"},
		{"SZ", "JOKER"},
		"DIYUSI"s,
		"NEL"s
	};
	
	foobar lost_owner{std::move(var)};
	
	std::cout << fake::io::pretty<>(lost_owner) << std::endl;
}
