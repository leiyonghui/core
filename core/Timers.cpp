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

	TimerHander::~TimerHander()
	{
		cancel();
	}

	int64 TimerHander::addTimer(Tick delay, Tick duration, int32 times, TimeoutCallback&& callback)
	{
		if (_scheduler == nullptr)
			return 0;
		int64 id = nextId();
		TimerEvent* event = new TimerEvent(id, this, delay, duration, times, std::forward<TimeoutCallback>(callback));
		event->_invalid = false;
		_scheduler->addTimer(event);
		_timerMap[id] = event;
		return id;
	}

	int64 TimerHander::addTimer(const Duration& delay, const Duration& duration, int32 times, TimeoutCallback&& callback)
	{
		return addTimer(TimeHelp::clock().count() + delay.count(), duration.count(), times, std::forward<TimeoutCallback>(callback));
	}

	int64 TimerHander::addTimer(const Datetime& datetime, const Duration& duration, int32 times, TimeoutCallback&& callback)
	{
		return addTimer(chrono::duration_cast<Duration>(datetime - TimeHelp::now()), duration, times, std::forward<TimeoutCallback>(callback));
	}

	int64 TimerHander::addTimer(const Duration& delay, const Duration& duration, TimeoutCallback&& callback)
	{
		return addTimer(delay, duration, 1, std::forward<TimeoutCallback>(callback));
	}

	int64 TimerHander::addTimer(const Datetime& time, const Duration& duration, TimeoutCallback&& callback)
	{
		return addTimer(time, duration, 1, std::forward<TimeoutCallback>(callback));
	}

	bool TimerHander::cancel(int64 id)
	{
		if (_scheduler == nullptr)
			return true;
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
		if (_scheduler == nullptr)
			return;
		auto iter = _timerMap.begin();
		while (iter != _timerMap.end())
		{
			iter->second->_hander = nullptr;
			_scheduler->delTimer(iter->second);
		}
		_timerMap.clear();
	}

}