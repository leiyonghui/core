#pragma once
#include "Timers.h"
#include <memory>

namespace core
{
	template<class T>
	class TimerImpl : protected TimerHander
	{
	public:
		using Duration = std::chrono::milliseconds;
		using Datetime = std::chrono::system_clock::time_point;

		using TimeoutCallback = std::function<void(std::shared_ptr<T>)>;

		TimerImpl(IScheduler* scheduler): TimerHander(scheduler)
		{

		}

		int64 startTimer(const Duration& delay, const Duration& duration, TimeoutCallback&& callback)
		{
			//point不能在外面，否则延长生命周期
			return addTimer(delay, duration, [callback, this]() {
				T* ptr = dynamic_cast<T*>(this);
				auto weak = ptr->weak_from_this();
				if (!weak.expired())
				{
					callback(std::shared_ptr<T>(weak));
				}
			});
		}

		int64 startTimer(const Datetime& time, const Duration& duration, TimeoutCallback&& callback)
		{
			return addTimer(time, duration, [callback, this]() {
				T* ptr = dynamic_cast<T*>(this);
				auto weak = ptr->weak_from_this();
				if (!weak.expired())
				{
					callback(std::shared_ptr<T>(weak));
				}
			});
		}
	};
}