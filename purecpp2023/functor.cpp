#include <iostream>
#include <memory>

#include "flat.h"
#include "tuple.h"
#include "functor_info.h"
#include "view.h"

int main(int _argc, char* _argv[])
{
	using namespace fake::literals;
	
	constexpr auto explore = [](fake::pack_c auto _state, auto _index, fake::pack_c auto _type){
		using state = fake::take_t<_state.echo>;
		using type = fake::take_t<_type.echo>;
		
		constexpr auto self = state{}["recur"_v].value;
		constexpr std::size_t index = state{}["index"_v].value;
		constexpr bool isarg = state{}["isarg"_v].value;
		using argti = std::remove_cvref_t<decltype(state{}[fake::view_v<"argti">])>; // avoid literals. 
		using adorn = std::remove_cvref_t<decltype(state{}[fake::view_v<"adorn">])>; // make gcc 12.x.x happy. 
		
		constexpr auto next = [](fake::pack_c auto _state, fake::mezz_c auto _node){
			using state = fake::take_t<_state.echo>;
			using split = std::remove_cvref_t<decltype(state{}["split"_v])>;
			using concat = fake::query_rebind_t<state, "split"_v, fake::tuple::push_back_t<split, decltype(_node)>>;
			using result = fake::query_rebind_t<concat, "index"_v, fake::mezz_t<state{}["index"_v].value + 1>>;
			return fake::pack_v<result>;
		};
		constexpr auto push = [](fake::pack_c auto _state, fake::mezz_c auto _isarg){
			using tmp = fake::query_rebind_t<fake::take_t<_state.echo>, "isarg"_v, fake::mezz_t<_isarg.value>>;
			return fake::pack_v<fake::query_rebind_t<tmp, "argti"_v, std::tuple<>>>;
		};
		constexpr auto pop = [](fake::pack_c auto _state, fake::mezz_c auto _isarg){
			using tmp = fake::query_rebind_t<fake::take_t<_state.echo>, "isarg"_v, fake::mezz_t<isarg>>;
			return fake::pack_v<fake::query_rebind_t<tmp, "argti"_v, argti>>;
		};
		constexpr auto append = [](fake::pack_c auto _state, fake::mezz_c auto _isarg, fake::mezz_c auto _index){
			using state = fake::take_t<_state.echo>;
			using cat_t = fake::tuple::push_back_t<argti, fake::mezz_t<_index.value>>;
			return fake::pack_v<std::conditional_t<_isarg.value, fake::query_rebind_t<state, "argti"_v, cat_t>, state>>;
		};
		
		constexpr fake::view_c auto prefix = "using t"_v;
		
		if constexpr(fake::functor_c<type>){
			using info = fake::functor_info<type>;
			using retn = typename info::retn;
			using args = typename info::tuple;
			
			using retn_t = fake::take_t<self(push(_state, fake::mezz_v<false>), 0, fake::pack_v<retn>)>;
			using push_t = fake::take_t<push(fake::pack_v<retn_t>, fake::mezz_v<true>)>;
			using args_t = fake::tuple::for_each_t<args, push_t, self>;
			using next_t = fake::take_t<pop(fake::pack_v<args_t>, fake::mezz_v<isarg>)>;
			
			constexpr std::size_t index = next_t{}["index"_v].value;
			constexpr std::size_t retn_i = retn_t{}["index"_v].value - 1;
			
			using ti = std::remove_cvref_t<decltype(args_t{}["argti"_v])>;
			
			constexpr fake::view_c auto retn_v = prefix + fake::to_view<index>() + " = t"_v + fake::to_view<retn_i>();
			constexpr fake::view_c auto args_v = []<std::size_t... _i>(std::index_sequence<_i...>){
				return ((", t"_v.substr<_i ? 0 : 2>() + fake::to_view<std::tuple_element_t<_i, ti>::value>()) + ...);
			}(std::make_index_sequence<std::tuple_size_v<args>>());
			constexpr fake::view_c auto concat = next_t{}["codes"_v] + retn_v + "(*)("_v + args_v + ");\n"_v;
			constexpr auto node = [](fake::pack_c auto _e){
				using e = fake::take_t<_e.echo>;
				return []<std::size_t... _i>(std::index_sequence<_i...>, fake::pack_c auto _e){
					constexpr std::size_t retn_i = retn_t{}["index"_v].value - 2;
					return adorn::value(
						"function"_v,
						std::tuple_element_t<retn_i, e>::value(_e),
						std::tuple_element_t<std::tuple_element_t<_i, ti>::value - 1, e>::value(_e)...
					);
				}(std::make_index_sequence<std::tuple_size_v<args>>(), _e);
			};
			
			using iter_t = fake::take_t<append(fake::pack_v<next_t>, fake::mezz_v<isarg>, fake::mezz_v<index>)>;
			return next(fake::pack_v<fake::query_rebind_t<iter_t, "codes"_v, decltype(concat)>>, fake::mezz_v<node>);
		}
		else if constexpr(std::is_pointer_v<type>){
			using next_t = fake::take_t<self(_state, 0, fake::pack_v<std::remove_pointer_t<type>>)>;
			constexpr std::size_t index = next_t{}["index"_v].value;
			constexpr fake::view_c auto code = prefix + fake::to_view<index>() + " = t"_v +
				fake::to_view<index - 1>() + "*;\n"_v;
			constexpr fake::view_c auto concat = next_t{}["codes"_v] + code;
			constexpr auto node = [](fake::pack_c auto _e){
				using e = fake::take_t<_e.echo>;
				constexpr std::size_t index = next_t{}["index"_v].value;
				return adorn::value("pointer"_v, std::tuple_element_t<index - 2, e>::value(_e));
			};
			
			using iter_t = fake::take_t<append(fake::pack_v<next_t>, fake::mezz_v<isarg>, fake::mezz_v<index>)>;
			return next(fake::pack_v<fake::query_rebind_t<iter_t, "codes"_v, decltype(concat)>>, fake::mezz_v<node>);
		}
		else{
			constexpr fake::view_c auto code = prefix + fake::to_view<index>() + " = "_v +
				fake::type_view(fake::pack_v<type>) + ";\n"_v;
			constexpr fake::view_c auto concat = state{}["codes"_v] + code;
			constexpr auto node = [](fake::pack_c auto _e){return adorn::value("type"_v, fake::pack_v<type>);};
			
			using iter_t = fake::take_t<append(fake::pack_v<state>, fake::mezz_v<isarg>, fake::mezz_v<index>)>;
			return next(fake::pack_v<fake::query_rebind_t<iter_t, "codes"_v, decltype(concat)>>, fake::mezz_v<node>);
		}
	};
	
	constexpr auto adorn = [](fake::view_c auto _tag, fake::pack_c auto _type, fake::pack_c auto ..._args){
		if constexpr(_tag == "function"_v)
			return fake::pack_v<std::function<fake::take_t<_type.echo>(fake::take_t<_args.echo>...)>>;
		if constexpr(_tag == "pointer"_v)
			return fake::pack_v<std::shared_ptr<fake::take_t<_type.echo>>>;
		if constexpr(_tag == "type"_v)
			return _type;
	};
	
	constexpr fake::query_c auto state = fake::query{
		// state attributes. 
		fake::mate{"recur"_v, fake::mezz_v<explore>},
		fake::mate{"index"_v, fake::mezz_v<std::size_t{1}>},
		fake::mate{"isarg"_v, fake::mezz_v<false>},
		fake::mate{"argti"_v, std::tuple{}},
		
		// results. 
		fake::mate{"codes"_v, ""_v},
		fake::mate{"split"_v, std::tuple{}},
		
		// aspect. 
		fake::mate{"adorn"_v, fake::mezz_v<adorn>}
	};
	
	using unko = short int (* (** (*)(const int (&)[6], short int* (*)(char, const int (&)[7]), short int*))[5])(char);
	// using unko = short int (* (* (*)(const int (&)[4], short int (*)(char)))[3])(char);
	
	constexpr fake::query_c auto result = fake::take_t<explore(fake::pack_v<decltype(state)>, 0, fake::pack_v<unko>)>{};
	constexpr std::string_view codes = result["codes"_v].data();
	using split = std::remove_cvref_t<decltype(result["split"_v])>;
	using replace = fake::take_t<std::tuple_element_t<std::tuple_size_v<split> - 1, split>::value(fake::pack_v<split>)>;
	
	std::cout << codes << "<aka>: " << std::endl;
	std::cout << fake::type_view(fake::pack_v<unko>) << std::endl;
	std::cout << fake::type_view(fake::pack_v<replace>) << std::endl;
	
	return 0;
}
