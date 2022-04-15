#pragma once
#include "Timers.h"

namespace core
{
	namespace timerwheel
	{
#define WHEEL_SIZE 256
#define BIT_SIZE 8
#define SLOT_SIZE 8
#define WHEEL_MASK 255
#define SLOT_INDEX(n,i) ((uint64(n) >> (i * 8)) & WHEEL_MASK)

		class TimerWheel : public IScheduler
		{
			Tick _curTick;
			Tick _interval;
			Tick _remainder;
			Tick _lasttime;
			uint64 _expend;
			uint64 _executeCount;
			TimerSlot _slot[SLOT_SIZE][WHEEL_SIZE];
			std::list<TimerEvent*> _invalidEvents;
		public:
			TimerWheel(Tick interval = 1);

			Tick tick() { return _curTick; };

			void addTimer(TimerEvent* event) override;

			void delTimer(TimerEvent* event) override;

			void update(Tick now) override;

			uint64 expend() { return _expend; };

			uint64 executeCount() { return _executeCount; };

		private:
			void _onTimeout(TimerEvent* event);

			void _addTimer(TimerEvent* event);

			void _delTimer(TimerEvent* event);

			void _updateSlot(int32 i);

			void _update();
		};
	}
}
