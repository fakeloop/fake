#include <iostream>
#include "format.h"

namespace demo::format
{
	
	namespace token{struct name{};}
	
	namespace{
		struct register_t{
		private:
			static consteval void inject() noexcept{
				fake::custom::format::config::formatter<[]{}, token::name, fake::view_v<"{float}">>(
					[](const auto &_value){return std::to_string(_value) + "f";}
				);
				fake::custom::format::config::formatter<[]{}, token::name, fake::view_v<"{[]}">>(
					[](const auto &_value){return "[" + std::to_string(_value) + "]";}
				);
				fake::custom::format::config::formatter<[]{}, token::name, fake::view_v<"{upper}">>(
					[](const auto &_value){std::string buffer = _value; for(auto &e : buffer) e -= 0x20; return buffer;}
				);
			}
		};
	}
	
	void name(){
		using namespace fake::literals;
		std::cout << fake::format<token::name>("yuh: {float}, {[]}, {upper}."_v, 114.514f, 1919, "yaju") << std::endl;
		// the following invocation is ill-formed since the register interface has been disabled by the previous call. 
		// fake::custom::format::config::formatter<[]{}, token::name, "{dummy}"_v>([](const auto&){});
	}
	
}
