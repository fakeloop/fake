#include <iostream>

#include "view.h"
#include "meta.h"

int main(int _argc, char* _argv[])
{
	using namespace fake::literals;
	constexpr fake::meta::variant v = []{};
	constexpr fake::meta::unordered_map um = []{};
	using foobar = decltype(v = ""_v);
	
	struct mgr{}; struct yuh{};		struct diyusi{}; struct nel{};
	struct nyn{}; struct icg{};		struct sz{}; struct joker{};
	using dummy_foo = decltype(
		um.emplace(fake::pack_v<mgr>, fake::pack_v<yuh>),
		um.emplace(fake::pack_v<diyusi>, fake::pack_v<nel>),
		um.emplace(fake::pack_v<nyn>, fake::pack_v<icg>),
		um.emplace(fake::pack_v<sz>, fake::pack_v<joker>),
		v = fake::take_v<v([]{})> + fake::type_view(um.data([]{}))
	);
	struct kofji{};
	using dummy_bar = decltype(
		um[fake::pack_v<nyn>] = fake::pack_v<kofji>,
		v = fake::take_v<v([]{})> + "\n\n"_v + fake::type_view(um.data([]{})),
		v = fake::take_v<v([]{})> + "\n\n"_v + fake::type_view(um[fake::pack_v<mgr>]([]{}))
	);
	
	constexpr const char* result = fake::take_t<v([]{})>::data();
	std::cout << result << std::endl;
	
	return 0;
}