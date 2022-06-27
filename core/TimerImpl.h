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

		int64 startTimer(const Duration& delay, const Duration& duration, TimeoutCallback&& callback)
		{
			//point������shared�������ӳ���������
			T* ptr = dynamic_cast<T*>(this);
			auto weak = ptr->weak_from_this();
			return _handler->addTimer(delay, duration, [callback, weak]() {
				if (!weak.expired())
				{
					callback(std::shared_ptr<T>(weak));
				}
			});
		}

		int64 startTimer(const Datetime& time, const Duration& duration, TimeoutCallback&& callback)
		{
			T* ptr = dynamic_cast<T*>(this);
			auto weak = ptr->weak_from_this();
			return _handler->addTimer(time, duration, [callback, weak]() {
				if (!weak.expired())
				{
					callback(std::shared_ptr<T>(weak));
				}
			});
		}

		bool stopTimer(int64 id)
		{
			return _handler->cancel(id);
		}

		void reset()
		{
			_handler = std::make_unique<TimerHander>(nullptr);
		}

	protected:
		std::unique_ptr<TimerHander> _handler;
	};
}