#pragma once
#include "Timers.h"
#include <memory>

namespace core
{
	template<class T>
	class TimerImpl
	{
	public:
		using Duration = std::chrono::milliseconds;
		using Datetime = std::chrono::system_clock::time_point;

		using TimeoutCallback = std::function<void(std::shared_ptr<T>)>;

		TimerImpl(IScheduler* scheduler = nullptr): _handler(std::make_unique<TimerHander>(scheduler))
		{

		}

		virtual~TimerImpl() = default;

		void setScheduler(IScheduler* scheduler)
		{
			_handler = std::make_unique<TimerHander>(scheduler);
		}

		int64 startTimer(int64 id, const Duration& delay, const Duration& duration, TimeoutCallback&& callback)
		{
			//point不能用shared，否则延长生命周期
			T* ptr = dynamic_cast<T*>(this);
			auto weak = ptr->weak_from_this();
			return _handler->addTimer(id, delay, duration, [callback, weak]() {
				if (!weak.expired())
				{
					callback(std::shared_ptr<T>(weak));
				}
			});
		}

		int64 startTimer(int64 id, const Datetime& time, const Duration& duration, TimeoutCallback&& callback)
		{
			T* ptr = dynamic_cast<T*>(this);
			auto weak = ptr->weak_from_this();
			return _handler->addTimer(id, time, duration, [callback, weak]() {
				if (!weak.expired())
				{
					callback(std::shared_ptr<T>(weak));
				}
			});
		}

		int64 startTimer(const Duration& delay, const Duration& duration, TimeoutCallback&& callback)
		{
			return startTimer(0, delay, duration, std::move(callback));
		}

		int64 startTimer(const Datetime& time, const Duration& duration, TimeoutCallback&& callback)
		{
			return startTimer(0, time, duration, std::move(callback));
		}

		bool stopTimer(int64 id)
		{
			return _handler->cancel(id);
		}

		void reset()
		{
			_handler.reset();
		}

	protected:
		std::unique_ptr<TimerHander> _handler;
	};
}