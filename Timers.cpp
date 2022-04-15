#include "Timers.h"
#include "TimeHelp.h"

namespace core
{
	using namespace std;
	using namespace chrono;

	TimerEvent::~TimerEvent()
	{
		if (_hander)
			assert(_hander->_timerMap.erase(_id));
	}

	int64 TimerHander::addTimer(Tick delay, Tick duration, int32 count, TimeoutCallback&& callback)
	{
		int64 id = nextId();
		TimerEvent* event = new TimerEvent(id, this, delay, duration, count, std::forward<TimeoutCallback>(callback));
		event->_invalid = false;
		_scheduler->addTimer(event);
		_timerMap[id] = event;
		return id;
	}

	int64 TimerHander::addTimer(Duration delay, Duration duration, int32 count, TimeoutCallback&& callback)
	{
		return addTimer(TimeHelp::clock_ms().count() + delay.count(), duration.count(), count, std::forward<TimeoutCallback>(callback));
	}

	int64 TimerHander::addTimer(Datetime datetime, Duration duration, int32 count, TimeoutCallback&& callback)
	{
		return addTimer(chrono::duration_cast<Duration>(datetime - chrono::system_clock::now()), duration, count, std::forward<TimeoutCallback>(callback));
	}

	int64 TimerHander::addTimer(Duration delay, Duration duration, TimeoutCallback&& callback)
	{
		return addTimer(delay, duration, 1, std::forward<TimeoutCallback>(callback));
	}

	int64 TimerHander::addTimer(Datetime time, Duration duration, TimeoutCallback&& callback)
	{
		return addTimer(time, duration, 1, std::forward<TimeoutCallback>(callback));
	}

	bool TimerHander::cancel(int64 id)
	{
		auto iter = _timerMap.find(id);
		if (iter == _timerMap.end())
		{
			return false;
		}
		_scheduler->delTimer(iter->second);
		iter->second->_hander = nullptr;
		_timerMap.erase(iter);
		return true;
	}

	void TimerHander::cancel()
	{
		auto iter = _timerMap.begin();
		while (iter != _timerMap.end())
		{
			iter->second->_hander = nullptr;
			_scheduler->delTimer(iter->second);
		}
		_timerMap.clear();
	}

}