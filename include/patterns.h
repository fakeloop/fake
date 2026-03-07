#ifndef __FAKE_PATTERN_H__
#define __FAKE_PATTERN_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include "push.h"

#include "detect.h"

namespace fake::patterns
{
	
	namespace tool
	{
		
		template<typename _Type>
		concept plug_c = std::is_class_v<_Type> && std::is_reference_v<_Type> == false;
		
		template<typename _Type, typename... _Types>
		concept member_c = (std::same_as<_Type, _Types> || ...);
		
		template<typename, std::size_t>
		struct element{};
		
		template<typename _T, std::size_t _i>
		static consteval void select(element<_T, _i>) noexcept{}
		
		template<typename... _Types>
		struct unique;
		
		template<typename... _Types, std::size_t... _index>
		struct unique<std::index_sequence<_index...>, _Types...> : element<_Types, _index>...{};
		
		template<typename... _Types>
		concept unique_c = requires(tool::unique<std::index_sequence_for<_Types...>, _Types...> _e){
			(tool::select<_Types>(_e), ...);
		};
		
		enum struct phase_e : std::size_t{init, query, quit};
		
	}
	
	template<tool::plug_c... _Plugs>
	requires tool::unique_c<_Plugs...>
	struct singleton final{
	private:
		static constexpr std::size_t size = sizeof...(_Plugs);
		
		template<typename _Plug>
		static consteval bool nothrow_constructible() noexcept{
			if constexpr(requires{_Plug{typename _Plug::token{}};})
				return std::is_nothrow_constructible_v<_Plug, typename _Plug::token>;
			else
				return std::is_nothrow_constructible_v<_Plug>;
		}
		
		static constexpr bool noexcept_init = (singleton::nothrow_constructible<_Plugs>() && ...);
		static constexpr bool noexcept_quit = (std::is_nothrow_destructible_v<_Plugs> && ...);
		
		static constexpr uint64_t signature = fake::type_view(fake::pack_v<fake::flat<_Plugs...>>).hash();
		
	public:
		template<tool::member_c<_Plugs...> _Plug>
		[[nodiscard]] static bool ready() noexcept{
			return singleton::storage<_Plug>().has_value();
		}
		
		template<tool::member_c<_Plugs...> _Plug>
		static _Plug& plug(){
			return singleton::get<_Plug, tool::phase_e::query>().value();
		}
		
		template<tool::member_c<_Plugs...> _Plug>
		static _Plug& rash(){
			return singleton::pry<_Plug>().value();
		}
		
		static constexpr auto instance(){[[maybe_unused]] const auto &dummy = self; return []{};}
		
		// for executable to inject into library. 
		static void inject(void (*_dynamic)(uint64_t, void*)) noexcept{
			std::array<void*, size> array{reinterpret_cast<void*>(&plug<_Plugs>())...};
			_dynamic(signature, reinterpret_cast<void*>(&array));
		}
		
	private:
		singleton() noexcept(noexcept_init){(singleton::get<_Plugs, tool::phase_e::init>(), ...);};
		~singleton() noexcept(noexcept_quit){char e{}; ((singleton::get<_Plugs, tool::phase_e::quit>(), e) = ...);};
		
		singleton(const singleton&) = delete;
		singleton& operator=(const singleton&) = delete;
		
	private:
		template<typename _Plug>
		static std::optional<_Plug>& storage(){static std::optional<_Plug> data; return data;}
		
		template<typename _Plug, tool::phase_e _phase_e>
		static std::optional<_Plug>& get(){
			if constexpr(_phase_e == tool::phase_e::init)
				if(auto &opt = singleton::storage<_Plug>(); opt.has_value() == false)
					if constexpr(requires{_Plug{typename _Plug::token{}};})
						opt.emplace(typename _Plug::token{});
					else
						opt.emplace();
			if constexpr(_phase_e == tool::phase_e::quit)
				singleton::storage<_Plug>().reset();
			return singleton::storage<_Plug>();
		}
		
		template<typename _Plug>
		static std::optional<_Plug>& pry(){
			if(auto &opt = singleton::storage<_Plug>(); opt.has_value() == false)
				if constexpr(requires{_Plug{typename _Plug::token{}};})
					opt.emplace(typename _Plug::token{});
				else
					opt.emplace();
			return singleton::storage<_Plug>();
		}
		
	private:
		static const singleton& local(){static singleton data; return data;}
		
	private:
		static const singleton &self;
	};
	
	template<tool::plug_c... _Plugs>
	requires tool::unique_c<_Plugs...>
	const singleton<_Plugs...>& singleton<_Plugs...>::self = singleton::local();
	
	template<tool::plug_c... _Plugs>
	requires tool::unique_c<_Plugs...>
	struct gueston final{
	private:
		static constexpr std::size_t size = sizeof...(_Plugs);
		
		static constexpr uint64_t signature = fake::type_view(fake::pack_v<fake::flat<_Plugs...>>).hash();
		
	public:
		template<tool::member_c<_Plugs...> _Plug>
		[[nodiscard]] static bool ready() noexcept{
			return gueston::pointer<_Plug>().has_value();
		}
		
		template<tool::member_c<_Plugs...> _Plug>
		static _Plug& plug(){
			if(const auto ptr = gueston::pointer<_Plug>().value_or(nullptr))
				return *ptr;
			else
				throw std::bad_optional_access{};
		}
		
		static constexpr auto instance(){[[maybe_unused]] const auto &dummy = self; return []{};}
		
		// for dynamic library to reference to the singletons from host. 
		static void reference(uint64_t _signature, void* _array) noexcept{
			if(signature != _signature)
				return;
			
			const std::array<void*, size> &array = *reinterpret_cast<std::array<void*, size>*>(_array);
			[&array]<std::size_t... _index>(std::index_sequence<_index...>){
				(gueston::pointer(reinterpret_cast<_Plugs*>(array[_index])), ...);
			}(std::make_index_sequence<size>());
		}
		
	private:
		gueston() noexcept = default;
		~gueston() noexcept = default;
		
		gueston(const gueston&) = delete;
		gueston& operator=(const gueston&) = delete;
		
	private:
		template<typename _Plug>
		static std::optional<_Plug*> pointer(_Plug* _ptr = nullptr){
			static std::optional<_Plug*> data = std::nullopt;
			if(_ptr) [[unlikely]]
				data.emplace(_ptr);
			return data;
		}
		
	private:
		static const gueston& local(){static gueston data; return data;}
		
	private:
		static const gueston &self;
	};
	
	template<tool::plug_c... _Plugs>
	requires tool::unique_c<_Plugs...>
	const gueston<_Plugs...>& gueston<_Plugs...>::self = gueston::local();
	
	enum struct intact_e : std::size_t{origin, duplicate, both, neither, moveonly};
	
	template<fake::patterns::intact_e _policy>
	struct intact;
	
	template<>
	struct intact<fake::patterns::intact_e::moveonly> final{
		intact() = default;
		constexpr intact(const intact &_copy) = delete;
		constexpr inline intact(intact &&_move) noexcept{_move.valid_v = false;}
		intact& operator=(const intact &_copy) = delete;
		intact& operator=(intact &&_move) noexcept{_move.valid_v = false; return *this;}
		constexpr operator bool() const noexcept{return valid_v;}
		
	private:
		bool valid_v = true;
	};
	
	template<>
	struct intact<fake::patterns::intact_e::neither> final{
		intact() = default;
		constexpr intact(const intact &_copy) noexcept{valid_v = _copy.valid_v = false;}
		constexpr inline intact(intact &&_move) noexcept{_move.valid_v = false;}
		intact& operator=(const intact &_copy) noexcept{valid_v = _copy.valid_v = false; return *this;}
		intact& operator=(intact &&_move) noexcept{_move.valid_v = false; return *this;}
		constexpr operator bool() const noexcept{return valid_v;}
		
	private:
		mutable bool valid_v = true;
	};
	
	template<fake::patterns::intact_e _policy>
	requires (_policy != fake::patterns::intact_e::neither)
	struct intact<_policy> final{
		intact() = default;
		constexpr intact(const intact &_copy) noexcept{
			valid_v = _policy != fake::patterns::intact_e::origin;
			_copy.valid_v = _policy != fake::patterns::intact_e::duplicate;
		}
		constexpr inline intact(intact &&_move) noexcept{_move.valid_v = false;}
		intact& operator=(const intact &_copy) noexcept{
			valid_v = _policy != fake::patterns::intact_e::origin;
			_copy.valid_v = _policy != fake::patterns::intact_e::duplicate;
			 return *this;
		}
		intact& operator=(intact &&_move) noexcept{_move.valid_v = false; return *this;}
		constexpr operator bool() const noexcept{return valid_v;}
		
	private:
		mutable bool valid_v = true;
	};
	
	namespace detail
	{
		
		template<typename _Registry>
		struct odr{
		private:
			static void impl() noexcept{
				using registry = _Registry;
				using type = typename _Registry::type;
				
				if constexpr(requires{registry::delegate.template operator()<type>();})
					registry::delegate.template operator()<type>();
				else if constexpr(requires{registry::delegate(fake::pazz_v<type>);})
					registry::delegate(fake::pazz_v<type>);
				else if constexpr(requires{registry::delegate(fake::pack_v<type>);})
					registry::delegate(fake::pack_v<type>);
				else if constexpr(requires{registry::delegate();})
					registry::delegate();
			}
			
		public:
			inline static auto use = []{return impl(), true;}();
		};
		
		template<typename _Registry>
		using facet = decltype([]{odr<_Registry>::use;}, odr<_Registry>{});
		
	}
	
	template<typename _Type, auto _delegate>
	struct registry : detail::facet<registry<_Type, _delegate>>{
		using type = _Type;
		static constexpr auto delegate = _delegate;
	};
	
	namespace detail::builder
	{
		
		struct dummy final{};
		
		template<typename _Type>
		concept detector_c = fake::creed_c<_Type> || std::same_as<_Type, fake::null_t>;
		
		template<typename _Type>
		concept functor_c = fake::mezz_c<_Type> && requires{{_Type::value()} -> fake::detect::as<!fake::same_d<void>>;};
		
		template<typename _Type>
		struct take final{using type = _Type;};
		
		template<typename _Type>
		requires fake::pack_c<_Type> || fake::pazz_c<_Type>
		struct take<_Type> final{using type = typename _Type::type;};
		
	}
	
	template<fake::view_c _Key, typename _Config, detail::builder::detector_c _Creed = fake::null_t>
	requires std::same_as<_Config, std::remove_cvref_t<_Config>>
	struct arg final{
		using config = _Config;
		
	public:
		static constexpr fake::view_c auto key = _Key{};
		static constexpr bool functor = detail::builder::functor_c<config>;
		static constexpr bool immediate = functor == false;
		static constexpr bool invalid = fake::pack_c<config> || fake::pazz_c<config> || functor;
		static constexpr bool valid = invalid == false;
		static constexpr bool element = true;
		static constexpr bool reference = false;
		
		static constexpr _Creed detector = {};
		static constexpr bool specific = fake::null_c<detector>;
		static constexpr bool abstract = specific == false;
		
	public:
		using storage = config;
		
	public:
		[[nodiscard]] constexpr arg(_Key, _Config _value) noexcept: value{std::move(_value)}{}
		[[nodiscard]] constexpr arg(_Key, _Config _value, _Creed) noexcept: value{std::move(_value)}{}
		
	public:
		config value;
	};
	
	template<fake::view_c _Key, typename _Config, detail::builder::detector_c _Creed = fake::null_t>
	requires std::same_as<_Config, std::remove_cvref_t<_Config>>
	struct ref final{
		using config = _Config;
		
	public:
		static constexpr fake::view_c auto key = _Key{};
		static constexpr bool functor = detail::builder::functor_c<config>;
		static constexpr bool immediate = functor == false;
		static constexpr bool invalid = fake::pack_c<config> || fake::pazz_c<config> || functor;
		static constexpr bool valid = invalid == false;
		static constexpr bool element = false;
		static constexpr bool reference = true;
		
		static constexpr _Creed detector = {};
		static constexpr bool specific = fake::null_c<detector>;
		static constexpr bool abstract = specific == false;
		
	private:
		using type = typename detail::builder::take<config>::type;
		
	public:
		using storage = std::conditional_t<
			functor,
			config,
			std::conditional_t<invalid, fake::pack_t<const type&>, const config&>
		>;
		
	public:
		[[nodiscard]] constexpr ref(_Key, _Config _value) noexcept: value{std::move(_value)}{}
		[[nodiscard]] constexpr ref(_Key, _Config _value, _Creed) noexcept: value{std::move(_value)}{}
		
	public:
		config value;
	};
	
	namespace detail::builder
	{
		
		template<typename _Type, bool _yield, bool _reference>
		struct branch final{using type = _Type;};
		
		template<detail::builder::functor_c _Type>
		struct branch<_Type, true, true> final{using type = const std::remove_cvref_t<decltype(_Type::value())>&;};
		
		template<detail::builder::functor_c _Type>
		struct branch<_Type, true, false> final{using type = std::remove_cvref_t<decltype(_Type::value())>;};
		
		template<typename _Type, bool _reference>
		requires fake::pack_c<_Type> || fake::pazz_c<_Type>
		struct branch<_Type, true, _reference> final{using type = typename _Type::type;};
		
		template<typename _Type>
		concept arg_c = fake::trait_c<_Type, fake::patterns::arg> || fake::trait_c<_Type, fake::patterns::ref>;
		
		template<detail::builder::arg_c _Arg, typename _Yield = std::bool_constant<_Arg::valid>, typename _Shift = void>
		requires (
			_Yield::value ||
			fake::pack_c<typename _Arg::config> ||
			fake::pazz_c<typename _Arg::config> ||
			_Arg::functor
		)
		struct arg final{
			using config = typename _Arg::config;
			using storage = typename _Arg::storage;
			using type = std::conditional_t<
				std::same_as<_Shift, void>,
				typename detail::builder::branch<storage, _Yield::value, _Arg::reference>::type,
				_Shift
			>;
			
			template<typename _Type>
			using shift = detail::builder::arg<_Arg, std::bool_constant<true>, _Type>;
			using transform = detail::builder::arg<_Arg, std::bool_constant<true>>;
			
		public:
			static constexpr fake::view_c auto key = _Arg::key;
			static constexpr bool functor = _Arg::functor;
			static constexpr bool immediate = _Arg::immediate;
			static constexpr bool yield = _Yield::value;
			static constexpr bool valid = _Arg::valid || yield || functor;
			static constexpr bool invalid = valid == false;
			static constexpr bool element = _Arg::element;
			static constexpr bool reference = _Arg::reference;
			
			static constexpr auto detector = _Arg::detector;
			static constexpr bool specific = _Arg::specific;
			static constexpr bool abstract = _Arg::abstract;
			
		public:
			[[nodiscard]] constexpr arg(_Arg _arg) noexcept: value{std::move(_arg.value)}{}
			[[nodiscard]] constexpr arg(const fake::trait_c<detail::builder::arg> auto &_arg) noexcept
			requires std::same_as<config, typename std::remove_cvref_t<decltype(_arg)>::config>:
				value{std::move(_arg.value)}
			{}
			
		public:
			config value;
		};
		
	}
	
	template<fake::view_c auto _tag, detail::builder::arg... _args> struct builder;
	
	template<typename _Type>
	concept builder_c = fake::trait_auto_c<std::remove_cvref_t<_Type>, fake::patterns::builder>;
	
	namespace detail::builder
	{
		
		template<auto _functor>
		struct apply final{
			static constexpr const auto& value(){
				static const auto value = _functor();
				
				return value;
			}
		};
		
		template<fake::trait_c<detail::builder::arg> _Arg, fake::view_c auto _Key = _Arg::key>
		requires _Arg::specific || _Arg::invalid || fake::detect::as<typename _Arg::type, _Arg::detector>
		struct tie{
		public:
			using type = typename _Arg::type;
			
		public:
			static constexpr bool valid = _Arg::valid;
			static constexpr bool invalid = _Arg::invalid;
			
		private:
			static constexpr bool functor = _Arg::functor;
			static constexpr bool immediate = _Arg::immediate;
			static constexpr bool element = _Arg::element;
			static constexpr bool reference = _Arg::reference;
			
		public:
			static constexpr auto detector = _Arg::detector;
			static constexpr bool specific = _Arg::specific;
			static constexpr bool abstract = _Arg::abstract;
			
		public:
			static constexpr bool settle = tie::functor && _Arg::yield;
			static constexpr bool delay = tie::functor && _Arg::yield == false;
			
		public:
			static constexpr bool stable = tie::immediate && tie::valid && tie::specific ||
				tie::functor && tie::valid && tie::specific && tie::settle;
			
		public:
			constexpr tie(std::convertible_to<type> auto _value) requires immediate && element:
				value{std::move(_value)}
			{}
			constexpr tie(const std::convertible_to<type> auto &_value) requires functor && element && settle:
				value{_value}
			{}
			constexpr tie(detail::builder::functor_c auto _functor) requires functor && element && delay:
				value{_functor}
			{}
			
			constexpr tie(const std::convertible_to<type> auto &_value) requires immediate && reference && valid:
				value{_value}
			{}
			constexpr tie(const auto &_value) requires immediate && reference && invalid:
				value{}
			{}
			constexpr tie(const std::convertible_to<type> auto &_value) requires functor && reference && settle:
				value{_value}
			{}
			constexpr tie(detail::builder::functor_c auto _functor) requires functor && reference && delay:
				value{_functor}
			{}
			
		public:
			[[nodiscard]] constexpr const auto& extract() const requires functor && delay{
				return detail::builder::apply<value.value>::value();
			}
			
			[[nodiscard]] constexpr auto spawn() const requires functor && delay{
				return value.value();
			}
			
		public:
			type value = {};
		};
		
		template<fake::view_c auto _key, typename _Arg>
		[[nodiscard]] inline constexpr auto& select(detail::builder::tie<_Arg, _key> &_e) noexcept{
			return _e;
		}
		
		template<fake::view_c auto _key, typename _Arg>
		[[nodiscard]] inline constexpr auto&& select(detail::builder::tie<_Arg, _key> &&_e) noexcept{
			return std::move(_e);
		}
		
		template<fake::view_c auto _key, typename _Arg>
		[[nodiscard]] inline constexpr auto&& select(const detail::builder::tie<_Arg, _key> &_e) noexcept{
			return _e;
		}
		
		template<fake::view_c auto _tag, fake::view_c auto _key, typename _Sequence, fake::flat_c auto _args>
		struct transform;
		
		template<fake::view_c auto _tag, fake::view_c auto _key, std::size_t... _index, fake::flat_c auto _args>
		struct transform<_tag, _key, std::index_sequence<_index...>, _args> final{
			using type = fake::patterns::builder<
				_tag,
				std::conditional_t<
					std::tuple_element_t<_index, decltype(_args)>::key == _key,
					typename std::tuple_element_t<_index, decltype(_args)>::transform,
					std::tuple_element_t<_index, decltype(_args)>
				>{_args.template value<_index>()}...
			>;
		};
		
		template<
			fake::view_c auto _tag,
			fake::view_c auto _key,
			typename _Sequence,
			fake::flat_c auto _args,
			typename _Shift
		>
		struct shift;
		
		template<
			fake::view_c auto _tag,
			fake::view_c auto _key,
			std::size_t... _index,
			fake::flat_c auto _args,
			typename _Shift
		>
		struct shift<_tag, _key, std::index_sequence<_index...>, _args, _Shift> final{
			using type = fake::patterns::builder<
				_tag,
				std::conditional_t<
					std::tuple_element_t<_index, decltype(_args)>::key == _key,
					typename std::tuple_element_t<_index, decltype(_args)>::template shift<_Shift>,
					std::tuple_element_t<_index, decltype(_args)>
				>{_args.template value<_index>()}...
			>;
		};
		
		// initial assign. 
		template<typename _Builder, fake::view_c auto _key>
		requires std::same_as<_Builder, std::remove_cvref_t<_Builder>>
		struct builder_broker final{
		private:
			using host = _Builder;
			using origin = std::remove_cvref_t<host>;
			using transform = typename detail::builder::transform<
				_Builder::tag,
				_key,
				std::make_index_sequence<origin::size>,
				origin::args
			>::type;
			using tie = std::remove_cvref_t<decltype(detail::builder::select<_key>(std::declval<transform&>()))>;
			using type = std::remove_cvref_t<decltype(tie::value)>;
			
			static constexpr auto detector = tie::detector;
			static constexpr bool specific = tie::specific;
			static constexpr bool abstract = tie::abstract;
			
		public:
			[[nodiscard]] constexpr builder_broker(fake::like<origin> auto &&_builder):
				builder{std::forward<decltype(_builder)>(_builder)}
			{}
			
		public:
			[[nodiscard]] constexpr transform operator()(const type &_e) && requires specific{
				return {std::move(builder), _key, _e};
			}
			
			[[nodiscard]] constexpr transform operator()(type &&_e) && requires specific{
				return {std::move(builder), _key, std::move(_e)};
			}
			
			[[nodiscard]] constexpr transform operator()(std::convertible_to<type> auto &&_e) && requires specific{
				return {std::move(builder), _key, std::forward<decltype(_e)>(_e)};
			}
			
			[[nodiscard]] constexpr auto operator()(fake::detect::as<detector> auto &&_e) && ->
			typename detail::builder::shift<
				_Builder::tag,
				_key,
				std::make_index_sequence<origin::size>,
				origin::args,
				std::remove_cvref_t<decltype(_e)>
			>::type
			requires abstract{
				return {std::move(builder), _key, std::forward<decltype(_e)>(_e)};
			}
			
		private:
			origin builder;
		};
		
		// modify. 
		template<typename _Builder, fake::view_c auto _key>
		struct arg_broker final{
		private:
			using host = _Builder;
			using origin = std::remove_cvref_t<host>;
			using type = std::remove_cvref_t<decltype(detail::builder::select<_key>(std::declval<origin&>()).value)>;
			using hint = std::remove_reference_t<decltype(detail::builder::select<_key>(std::declval<host&>()).value)>;
			
			static constexpr bool constant = std::is_const_v<std::remove_reference_t<host>> || std::is_const_v<hint>;
			
		public:
			[[nodiscard]] constexpr arg_broker(host _builder) noexcept:
				builder{std::forward<decltype(_builder)>(_builder)}
			{}
			
		public:
			constexpr origin operator()(const type &_e) && requires constant{
				return {std::forward<host>(builder), _key, _e};
			}
			
			constexpr origin operator()(type &&_e) && requires constant{
				return {std::forward<host>(builder), _key, std::move(_e)};
			}
			
			constexpr origin operator()(std::convertible_to<type> auto &&_e) && requires constant{
				return {std::forward<host>(builder), _key, std::forward<decltype(_e)>(_e)};
			}
			
			constexpr host operator()(const type &_e) &&{
				detail::builder::select<_key>(builder).value = _e;
				
				return std::forward<host>(builder);
			}
			
			constexpr host operator()(type &&_e) &&{
				detail::builder::select<_key>(builder).value = std::move(_e);
				
				return std::forward<host>(builder);
			}
			
			constexpr host operator()(std::convertible_to<type> auto &&_e) &&{
				detail::builder::select<_key>(builder).value = std::forward<decltype(_e)>(_e);
				
				return std::forward<host>(builder);
			}
			
		private:
			host builder;
		};
		
	}
	
	namespace detail::builder
	{
		
		template<typename _Builder, fake::view_c auto _key>
		using tie_type = std::remove_cvref_t<decltype(detail::builder::select<_key>(std::declval<_Builder&>()))>;
		
		template<typename _Builder, fake::view_c auto _key, typename _Arg>
		concept tie_c = requires(_Arg _arg){detail::builder::tie_type<_Builder, _key>{std::forward<_Arg>(_arg)};};
		
		template<fake::view_c auto _select, fake::view_c auto _rebind>
		[[nodiscard]] inline constexpr auto&& rebind(fake::patterns::builder_c auto &&_builder, auto &&_arg) noexcept{
			if constexpr(_select == _rebind)
				return std::forward<decltype(_arg)>(_arg);
			else
				return detail::builder::select<_select>(std::forward<decltype(_builder)>(_builder));
		}
		
	}
	
	template<fake::view_c auto _tag, detail::builder::arg... _args>
	struct builder final : public detail::builder::tie<decltype(_args)>...{
		static constexpr fake::view_c auto tag = _tag;
		static constexpr std::size_t size = sizeof...(_args);
		static constexpr fake::flat_c auto args = fake::make_flat(_args...);
		
	public:
		[[nodiscard]] constexpr builder() noexcept: detail::builder::tie<decltype(_args)>{_args.value}...{}
		
	private:
		[[nodiscard]] constexpr builder(
			fake::patterns::builder_c auto &&_builder,
			fake::view_c auto _key,
			auto &&_arg
		) noexcept requires detail::builder::tie_c<builder, decltype(_key){}, decltype(_arg)>:
			detail::builder::tie<decltype(_args)>{
				detail::builder::rebind<_args.key, decltype(_key){}>(
					std::forward<decltype(_builder)>(_builder),
					std::forward<decltype(_arg)>(_arg)
				)
			}...
		{}
		
	public:
		static constexpr bool valid = (detail::builder::tie<decltype(_args)>::valid && ...);
		static constexpr bool invalid = (detail::builder::tie<decltype(_args)>::invalid || ...);
		
	public:
		[[nodiscard]] constexpr auto operator[](fake::view_c auto _key) const & noexcept{
			using query = detail::builder::tie_type<builder, decltype(_key){}>;
			
			if constexpr(query::stable)
				return detail::builder::arg_broker<const builder&, decltype(_key){}>{*this};
			else
				return detail::builder::builder_broker<builder, decltype(_key){}>{*this};
		}
		
		[[nodiscard]] constexpr auto operator[](fake::view_c auto _key) & noexcept{
			using query = detail::builder::tie_type<builder, decltype(_key){}>;
			
			if constexpr(query::stable)
				return detail::builder::arg_broker<builder&, decltype(_key){}>{*this};
			else
				return detail::builder::builder_broker<builder, decltype(_key){}>{*this};
		}
		
		[[nodiscard]] constexpr auto operator[](fake::view_c auto _key) const && noexcept{
			using query = detail::builder::tie_type<builder, decltype(_key){}>;
			
			if constexpr(query::stable)
				return detail::builder::arg_broker<builder, decltype(_key){}>{std::move(*this)};
			else
				return detail::builder::builder_broker<builder, decltype(_key){}>{std::move(*this)};
		}
		
		[[nodiscard]] constexpr auto operator[](fake::view_c auto _key) && noexcept{
			using query = detail::builder::tie_type<builder, decltype(_key){}>;
			
			if constexpr(query::stable)
				return detail::builder::arg_broker<builder, decltype(_key){}>{std::move(*this)};
			else
				return detail::builder::builder_broker<builder, decltype(_key){}>{std::move(*this)};
		}
		
	public:
		[[nodiscard]] constexpr const auto& operator<=(fake::view_c auto _key) const noexcept{
			using query = detail::builder::tie_type<builder, decltype(_key){}>;
			using type = decltype((detail::builder::select<decltype(_key){}>(*this).value));
			
			if constexpr(query::delay)
				return detail::builder::select<decltype(_key){}>(*this).extract();
			else
				return detail::builder::select<decltype(_key){}>(*this).value;
		}
		
	public:
		[[nodiscard]] constexpr auto move(fake::view_c auto _key) noexcept
		requires detail::builder::tie_type<builder, decltype(_key){}>::delay{
			using type = decltype((detail::builder::select<decltype(_key){}>(*this).value));
			
			return detail::builder::select<decltype(_key){}>(*this).spawn();
		}
		
		[[nodiscard]] constexpr auto&& move(fake::view_c auto _key) noexcept{
			using type = decltype((detail::builder::select<decltype(_key){}>(*this).value));
			
			return std::forward<type>(detail::builder::select<decltype(_key){}>(*this).value);
		}
		
	public:
		[[nodiscard]] constexpr auto&& forward(fake::view_c auto _key) const & noexcept{
			using query = detail::builder::tie_type<builder, decltype(_key){}>;
			using type = decltype((detail::builder::select<decltype(_key){}>(*this).value));
			
			if constexpr(query::delay)
				return detail::builder::select<decltype(_key){}>(*this).extract();
			else
				return std::forward<type>(detail::builder::select<decltype(_key){}>(*this).value);
		}
		
		[[nodiscard]] constexpr auto forward(fake::view_c auto _key) && noexcept
		requires detail::builder::tie_type<builder, decltype(_key){}>::delay{
			using type = decltype((detail::builder::select<decltype(_key){}>(*this).value));
			
			return detail::builder::select<decltype(_key){}>(*this).spawn();
		}
		
		[[nodiscard]] constexpr auto&& forward(fake::view_c auto _key) && noexcept{
			using type = decltype((detail::builder::select<decltype(_key){}>(*this).value));
			
			return std::forward<type>(detail::builder::select<decltype(_key){}>(*this).value);
		}
		
	private:
		template<typename _Builder, fake::view_c auto _key>
		requires std::same_as<_Builder, std::remove_cvref_t<_Builder>>
		friend struct detail::builder::builder_broker;
		
		template<typename _Builder, fake::view_c auto _key>
		friend struct detail::builder::arg_broker;
	};
	
}

#include "pop.h"

#endif /*__FAKE_PATTERN_H__*/ 
