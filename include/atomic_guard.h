#ifndef __FAKE_ATOMIC_GUARD_H__
#define __FAKE_ATOMIC_GUARD_H__

/*    This project is licensed under the terms of the    *\
 *      DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE      * 
 *                                                       * 
 *                    version 114.514                    * 
 *                                                       * 
 *   as published by fakeloop, on 2022, as it follows:   * 
 *                                                       * 
\*       0. You just DO WHAT THE FUCK YOU WANT TO.       */

#include <atomic>

namespace fake::atomic
{
	
	struct guard{
	private:
		struct mutex_type{
			bool write;
			unsigned int read;
		};
		
	public:
		inline void write_lock() noexcept{
			mutex_type origin{};
			mutex_type desire{};
			
			do{
				origin.write = false;
				desire = {true, origin.read};
			}while(
				!mutex.compare_exchange_weak(
					origin,
					desire,
					std::memory_order_release,
					std::memory_order_relaxed
				)
			);
			
			while(mutex.load(std::memory_order_acquire).read);
		}
		
		inline void write_unlock() noexcept{
			mutex.store({}, std::memory_order_release);
		}
		
		inline void read_lock() noexcept{
			mutex_type origin{};
			mutex_type desire{};
			
			do{
				origin.write = false;
				desire = {false, origin.read + 1};
			}while(
				!mutex.compare_exchange_weak(
					origin,
					desire,
					std::memory_order_release,
					std::memory_order_relaxed
				)
			);
		}
		
		inline void read_unlock() noexcept{
			mutex_type origin{};
			mutex_type desire{};
			
			do{
				desire = {origin.write, origin.read - 1};
			}while(
				!mutex.compare_exchange_weak(
					origin,
					desire,
					std::memory_order_release,
					std::memory_order_relaxed
				)
			);
		}
		
		static_assert(
			std::atomic<mutex_type>::is_always_lock_free,
			"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
			"\nerror<fake::atomic::guard>:\n"
			"\t'std::atomic<mutex_type>' should be always lock free.\n"
			"\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
		);
		
	private:
		std::atomic<mutex_type> mutex = mutex_type{false, 0};
	};
	
	struct write
	{
		write(guard &_ref) noexcept: ref(_ref){ref.write_lock();}
		~write() noexcept{ref.write_unlock();}
		
	private:
		guard& ref;
	};
	
	struct read
	{
		read(guard &_ref) noexcept: ref(_ref){ref.read_lock();}
		~read() noexcept{ref.read_unlock();}
		
	private:
		guard& ref;
	};
	
	struct ultimatum{
		using value_type = std::atomic_uint32_t::value_type;
		enum class signal : std::size_t{nop, trigger, overflow};
		
	public:
		inline constexpr ultimatum(value_type _value) noexcept: condition(_value){}
		
		inline void produce() noexcept{
			condition.fetch_add(1, std::memory_order_release);
		}
		
		inline signal consume() noexcept{
			value_type origin = condition.load(std::memory_order_acquire);
			value_type desire = 0;
			
			do{
				if(origin == 0)
					return signal::overflow;
				
				desire = origin - 1;
			}while(
				!condition.compare_exchange_weak(
					origin,
					desire,
					std::memory_order_release,
					std::memory_order_relaxed
				)
			);
			
			return desire ? signal::nop : signal::trigger;
		}
		
	private:
		std::atomic<value_type> condition;
	};
	
}

#endif /*__FAKE_ATOMIC_GUARD_H__*/ 
