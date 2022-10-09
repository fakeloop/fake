#include <iostream>
#include "format.h"
#include "utility.h"

namespace demo::format
{
	
	namespace token{struct color{};}
	
	namespace{
		struct register_t{
		private:
			static consteval void inject() noexcept{
				fake::custom::format::config::formatter<[]{}, token::color, fake::view_v<"{red}">>(
					[](const auto &_value){return fake::color<fake::colors::red>(std::to_string(_value));}
				);
				fake::custom::format::config::formatter<[]{}, token::color, fake::view_v<"{green}">>(
					[](const auto &_value){return fake::color<fake::colors::green>(std::to_string(_value));}
				);
				fake::custom::format::config::formatter<[]{}, token::color, fake::view_v<"{blue}">>(
					[](const auto &_value){return fake::color<fake::colors::blue>(std::string_view(_value));}
				);
			}
		};
	}
	
	void color(){
		using namespace fake::literals;
		std::cout << fake::format<token::color>("mgr: {red}, {green}, {blue}."_v, 114.514f, 1919, "yaju") << std::endl;
	}
	
}
