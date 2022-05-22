#include "Timers.h"

namespace core
{
	namespace timerset
	{
		class TimerSet : public IScheduler
		{
		public:
			TimerSet();

			virtual ~TimerSet();

			virtual Tick tick() override;

			virtual void update(Tick now) override;

			virtual void addTimer(TimerEvent* event) override;

			virtual void delTimer(TimerEvent* event) override;

		private:
			TimerSetImpl* _timer;
		};

		class TimerSetImpl
		{
			friend class TimerSet;
		protected:
			TimerSetImpl() :_curTick(0) {}

			virtual ~TimerSetImpl();

			void _update(Tick now);

			void _addTimer(TimerEvent* event);

			void _delTimer(TimerEvent* event);

			void _onTimeout(TimerEvent* event);

		private:
			Tick _curTick;
			std::map<Tick, TimerSlot*> _queue;
			std::list<TimerEvent*> _invalidEvents;
		};
	}
}
