#pragma once
#include "FastNode.h"
#include "Configs.h"

namespace core
{
	using int32 = int;
	using int64 = long long;
	using uint64 = unsigned long long;
	using Tick = uint64;

	typedef std::function<void()> TimeoutCallback;

	namespace timerwheel {
		class TimerWheel;
	};
	namespace timerset {
		class TimerSet;
		class TimerSetImpl;
	};
	class IScheduler;
	class TimerHander;

	class TimerEvent : CFastNode<TimerEvent*>
	{
		friend class IScheduler;
		friend class TimerHander;
		friend class timerwheel::TimerWheel;
		friend class timerset::TimerSetImpl;

		int64 _id;
		TimerHander* _hander;
		Tick _tick;
		Tick _period;
		int32 _count;
		bool _invalid;
		TimeoutCallback _callback;
	public:
		TimerEvent(int64 id, TimerHander* ptr, Tick tick, Tick period, int32 count, TimeoutCallback&& callback, bool invalid = true) :
			CFastNode<TimerEvent*>(this),
			_id(id),
			_hander(ptr),
			_tick(tick),
			_period(period),
			_count(count),
			_invalid(invalid),
			_callback(std::move(callback))
		{
			//assert(_hander);
		}

		virtual ~TimerEvent();

		void onTimeout()
		{
			if (_callback)
				_callback();
		}

		Tick tick()
		{
			return _tick;
		}
	};

	class TimerSlot
	{
		friend class timerset::TimerSetImpl;
		friend class timerwheel::TimerWheel;

		CFastNode<TimerEvent*> _slot;
	public:
		TimerSlot() : _slot(nullptr)
		{

		}
	};

	class TimerHander
	{
		friend class TimerEvent;
		friend class timerwheel::TimerWheel;
		friend class timerset::TimerSetImpl;

		using Duration = std::chrono::milliseconds;
		using Datetime = std::chrono::system_clock::time_point;

		int64 _nextId;
		std::map<int64, TimerEvent*> _timerMap;
		IScheduler* _scheduler;

		TimerHander(const TimerHander&) = delete;
		TimerHander operator=(const TimerHander&) = delete;
	public:
		TimerHander(IScheduler* scheduler) : _nextId(0), _scheduler(scheduler) {}

		virtual ~TimerHander()
		{
			cancel();
		}

		int64 addTimer(Tick delay, Tick duration, int32 count, TimeoutCallback&& callback);

		int64 addTimer(Duration delay, Duration duration, int32 count, TimeoutCallback&& callback);

		int64 addTimer(Datetime time, Duration duration, int32 count, TimeoutCallback&& callback);

		int64 addTimer(Duration delay, Duration duration, TimeoutCallback&& callback);

		int64 addTimer(Datetime time, Duration duration, TimeoutCallback&& callback);

		bool cancel(int64 id);

		void cancel();

		int64 nextId() {return ++_nextId;}
	};


	class IScheduler//implement timerwheel or heap
	{
	public:
		IScheduler() {}

		virtual ~IScheduler() {}

		virtual Tick tick() = 0;

		virtual void update(Tick now) = 0;

		virtual void addTimer(TimerEvent* event) = 0;

		virtual void delTimer(TimerEvent* event) = 0;
	};
}