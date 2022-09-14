#include <iostream>

#include "view.h"

namespace demo
{
	
	template<typename _Type>
	void print(_Type &&_e){
		if constexpr(std::is_same_v<std::remove_cvref_t<_Type>, char>)
			_e ? std::cout << "'" << _e << "'" << std::endl : std::cout << "'\\0'" << std::endl;
		else if constexpr(std::is_same_v<std::remove_cvref_t<_Type>, bool>)
			std::cout << std::boolalpha << _e << std::endl;
		else if constexpr(requires{std::cout << _e;})
			std::cout << _e << std::endl;
		else if constexpr(fake::view_c<_Type>)
			std::cout << _e.data() << std::endl;
	}
	
}

int main(int _argc, char* _argv[])
{
	using namespace fake::literals;
	
	constexpr auto begin = "NYN"_v;
	constexpr auto end = "ICG"_v;
	constexpr auto middle = " & "_v;
	
	constexpr auto cat = begin + middle + end;
	constexpr auto dup = fake::make_view<"Cookie: ">() + fake::make_view<cat.data()>();
	
	constexpr auto mgr = "Homo's songbird MGR is the best aneki."_v;
	
	demo::print(cat);
	demo::print(dup);
	demo::print(fake::symbol::string_view<decltype(cat.data())>());
	demo::print("- - - - - - - -");
	demo::print(mgr);
	demo::print(mgr.substr<16, 3>());
	demo::print("- - - - - - - -");
	demo::print(mgr.front());
	demo::print(mgr.back());
	demo::print(""_v.front());
	demo::print(""_v.back());
	demo::print("- - - - - - - -");
	demo::print(""_v.empty());
	demo::print(mgr.empty());
	demo::print("- - - - - - - -");
	demo::print(mgr.starts_with<"Homo">());
	demo::print(mgr.starts_with<"MGR">());
	demo::print(mgr.ends_with<"aneki.">());
	demo::print(mgr.ends_with<"MGR">());
	demo::print("- - - - - - - -");
	demo::print(mgr.starts_with<"Homo"_v>());
	demo::print(mgr.starts_with<"MGR"_v>());
	demo::print(mgr.ends_with<"aneki."_v>());
	demo::print(mgr.ends_with<"MGR"_v>());
	demo::print("- - - - - - - -");
	demo::print(mgr.contains<"MGR">());
	demo::print(mgr.contains<"SNNN">());
	demo::print(mgr.contains<"MGR"_v>());
	demo::print(mgr.contains<"SNNN"_v>());
	demo::print("- - - - - - - -");
	demo::print(mgr.find<"MGR">());
	demo::print(mgr.find<"SNNN">());
	demo::print(mgr.find<"MGR"_v>());
	demo::print(mgr.find<"SNNN"_v>());
	demo::print("- - - - - - - -");
	demo::print(mgr.rfind<"MGR">());
	demo::print(mgr.rfind<"SNNN">());
	demo::print(mgr.rfind<"MGR"_v>());
	demo::print(mgr.rfind<"SNNN"_v>());
	demo::print("- - - - - - - -");
	demo::print(mgr.find_first_of<"MGR">());
	demo::print(mgr.find_first_of<"YUH">());
	demo::print(mgr.find_first_of<"MGR"_v>());
	demo::print(mgr.find_first_of<"YUH"_v>());
	demo::print("- - - - - - - -");
	demo::print(mgr.find_last_of<"MGR">());
	demo::print(mgr.find_last_of<"YUH">());
	demo::print(mgr.find_last_of<"MGR"_v>());
	demo::print(mgr.find_last_of<"YUH"_v>());
	demo::print("- - - - - - - -");
	demo::print(mgr.find_first_not_of<"MGR">());
	demo::print(mgr.find_first_not_of<"YUH">());
	demo::print(mgr.find_first_not_of<"MGR"_v>());
	demo::print(mgr.find_first_not_of<"YUH"_v>());
	demo::print("- - - - - - - -");
	demo::print(mgr.find_last_not_of<"MGR">());
	demo::print(mgr.find_last_not_of<"YUH">());
	demo::print(mgr.find_last_not_of<"MGR"_v>());
	demo::print(mgr.find_last_not_of<"YUH"_v>());
	demo::print("- - - - - - - -");
	demo::print("YZY"_v == "YZY"_v);
	demo::print("NYN"_v == "ICG"_v);
	demo::print("YZY"_v != "YZY"_v);
	demo::print("NYN"_v != "ICG"_v);
	demo::print("YZY"_v < "YZY"_v);
	demo::print("NYN"_v < "ICG"_v);
	demo::print("YZY"_v > "YZY"_v);
	demo::print("NYN"_v > "ICG"_v);
	
	return 0;
}
