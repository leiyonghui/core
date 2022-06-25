#include "TimerSet.h"
#include <assert.h>
#include "Configs.h"

namespace core
{
	namespace timerset
	{
		TimerSet::~TimerSet()
		{
			while (!_invalidEvents.empty())
			{
				delete* _invalidEvents.begin();
				_invalidEvents.pop_front();
			}
		}

		void TimerSet::update(Tick now)
		{
			_curTick = now;
			while (!_queue.empty())
			{
				auto top = _queue.begin();
				if (top->first > _curTick)
				{
					break;
				}
				auto& slot = top->second->_slot;
				if (!slot.empty())
				{
					for (auto iter = slot.begin(); iter != slot.end(); )
					{
						auto event = iter->data();
						assert(event->tick() == top->first);
						iter = iter->next();
						event->leave();
						onTimeout(event);
					}
				}
				assert(slot.empty());
				delete top->second;
				_queue.erase(top);
			}
			while (!_invalidEvents.empty())
			{
				delete* _invalidEvents.begin();
				_invalidEvents.pop_front();
			}
		}

		void TimerSet::addTimer(TimerEvent* event)
		{
			auto tick = event->tick();
			assert(!event->_invalid);
			auto iter = _queue.find(tick);
			if (iter == _queue.end())
			{
				iter = _queue.insert({ tick, new TimerSlot() }).first;
			}
			iter->second->_slot.pushBack(*event);
		}

		void TimerSet::delTimer(TimerEvent* event)
		{
			if (!event->_invalid) 
			{
				event->leave();
				event->_invalid = true;
				_invalidEvents.push_back(event);
			}
		}

		void TimerSet::onTimeout(TimerEvent* event)
		{
			assert(event->_tick <= _curTick);
			try
			{
				if (!event->_invalid)
				{
					event->onTimeout();
					if (!event->_invalid)
					{
						if (event->_period > 0 && (event->_count > 1 || !event->_count))
						{
							if (event->_count)
								event->_count--;
							event->_tick = _curTick + event->_period;
							addTimer(event);
						}
						else
						{
							delete event;
						}
					}
				}
				else
				{
					assert(false);
				}
			}
			catch (std::exception e)
			{
				delete event;
			}
		}
	}
}