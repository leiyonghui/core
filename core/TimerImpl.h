#pragma once
#include "Timers.h"
#include <memory>

namespace core
{
	template<class T/*, std::enable_if_t<std::is_base_of_v<std::enable_shared_from_this<T>, T>, int> = 0*/>
	class TimerImpl : protected TimerHander
	{
	public:
		using Duration = std::chrono::milliseconds;
		using Datetime = std::chrono::system_clock::time_point;

		using TimeoutCallback = std::function<void(const std::shared_ptr<T>&)>;

		TimerImpl(IScheduler* scheduler): TimerHander(scheduler)
		{

		}

		int64 startTimer(const Duration& delay, const Duration& duration, TimeoutCallback&& callback)
		{
			T* ptr = dynamic_cast<T*>(this);
			auto pointer = ptr->shared_from_this();
			return addTimer(delay, duration, [callback, pointer]() {
				callback(pointer);
			});
		}

		int64 startTimer(const Datetime& time, const Duration& duration, TimeoutCallback&& callback)
		{
			T* ptr = dynamic_cast<T*>(this);
			auto pointer = ptr->shared_from_this();
			return addTimer(time, duration, [callback, pointer]() {
				callback(pointer);
			});
		}
	};
}