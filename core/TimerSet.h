#include "Timers.h"

namespace core
{
	namespace timerset
	{
		class TimerSet : public IScheduler
		{
		public:
			TimerSet() :_curTick(0) {}
			virtual ~TimerSet();

			Tick tick() { return _curTick; }
			void addTimer(TimerEvent* event) override;
			void delTimer(TimerEvent* event) override;
			void update(Tick now) override;

		protected:
			void onTimeout(TimerEvent* event);

		private:
			Tick _curTick;
			std::map<Tick, TimerSlot*> _queue;
			std::list<TimerEvent*> _invalidEvents;
		};
	}
}
