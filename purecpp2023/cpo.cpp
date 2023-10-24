#include "view.h"

namespace demo
{
	
	template<fake::view_c auto _view, typename _Type>
	struct facet final{};
	
	template<fake::view_c auto _view, typename _Type>
	inline constexpr facet<_view, _Type> facet_v;
	
	template<typename _Type>
	struct is_facet final : std::false_type{};
	
	template<fake::view_c auto _view, typename _Type>
	struct is_facet<facet<_view, _Type>> final : std::true_type{};
	
	template<typename _Type>
	inline constexpr bool is_facet_v = is_facet<_Type>::value;
	
	template<typename _Type>
	concept facet_c = is_facet_v<std::remove_cvref_t<_Type>>;
	
	template<typename _Facet, typename... _Args>
	struct facet_result;
	
	template<typename _Facet, typename... _Args>
	using facet_result_t = typename facet_result<_Facet, _Args...>::type;
	
	namespace detail
	{
		
		struct facet_invoke final{
			constexpr auto operator()(facet_c auto _facet, auto &&..._args) const ->
			facet_result_t<decltype(_facet), decltype(_args)...>;
		};
		
		namespace poison
		{
			
			inline constexpr auto facet_invoke(facet_c auto, auto &&..._args) requires (0 > sizeof...(_args));
			
		}
		
	}
	
	inline namespace cpo
	{
		
		inline constexpr detail::facet_invoke facet_invoke;
		
	}
	
	constexpr auto detail::facet_invoke::operator()(facet_c auto _facet, auto &&..._args) const ->
	facet_result_t<decltype(_facet), decltype(_args)...>{
		using detail::poison::facet_invoke;
		return facet_invoke(_facet, std::forward<decltype(_args)>(_args)...);
	}
	
	namespace detail
	{
		
		struct invalid final{};
		
		template<typename _Facet, typename... _Args>
		struct facet_result final{
		private:
			static consteval fake::pack_c auto result(){
				using detail::poison::facet_invoke;
				if constexpr(
					requires{facet_invoke(std::declval<_Facet>(), std::forward<_Args>(std::declval<_Args>())...);}
				)
					return fake::pack_v<
						decltype(facet_invoke(std::declval<_Facet>(), std::forward<_Args>(std::declval<_Args>())...))
					>;
				else
					return fake::pack_v<invalid>;
			}
			
		public:
			using type = fake::take_t<facet_result::result()>;
		};
		
	}
	
	template<typename _Facet, typename... _Args>
	requires (std::same_as<typename detail::facet_result<_Facet, _Args...>::type, detail::invalid> == false)
	struct facet_result<_Facet, _Args...> final{
		using type = typename detail::facet_result<_Facet, _Args...>::type;
	};
	
}

#include <iostream>

namespace libx
{
	
	using namespace fake::literals;
	
	struct hello_t{
		auto operator()(fake::pack_c auto _pack, auto &&..._args) const ->
		demo::facet_result_t<demo::facet<"libx::hello"_v, fake::take_t<decltype(_pack){}>>, decltype(_args)...>{
			constexpr demo::facet_c auto facet = demo::facet_v<"libx::hello"_v, fake::take_t<_pack.echo>>;
			return demo::facet_invoke(facet, std::forward<decltype(_args)>(_args)...);
		}
	};
	
	inline constexpr hello_t hello;
	
	auto greeting(auto &&_e, auto &&_arg) ->
	std::invoke_result_t<hello_t, fake::type_package<std::remove_cvref_t<decltype(_e)>>, decltype(_e), decltype(_arg)>{
		constexpr fake::pack_c auto pack = fake::pack_v<std::remove_cvref_t<decltype(_e)>>;
		return hello(pack, std::forward<decltype(_e)>(_e), std::forward<decltype(_arg)>(_arg));
	}
	
}

namespace user
{
	
	using namespace fake::literals;
	
	struct bar final{
		int e;
	};
	
	
}

namespace user
{
	
	auto facet_invoke(demo::facet<"libx::hello"_v, bar>, const auto &_this, std::string_view _sv){
		std::cout << fake::self_view([]{}) << ":" << std::endl;
		std::cout << _this.e << _sv << std::endl;
		return "1919810"_v;
	}
	
}

int main(int _argc, char* _argv[])
{
	[](auto...){
		using namespace fake::literals;
		
		using bar = fake::type_t<decltype([]{}), user::bar>;
		
		// query type. 
		if constexpr(requires{libx::greeting(bar{114}, "514");}){
			const auto result = libx::greeting(bar{114}, "514");
			
			std::cout << fake::self_view([]{}) << ":" << std::endl;
			std::cout << fake::type_view(result) << std::endl;
		}
		
		struct dummy final{};
		if constexpr(requires{libx::greeting(dummy{}, "531");})
			libx::greeting(dummy{}, "4545");
	}();
	
	return 0;
}
