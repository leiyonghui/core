#pragma once
#include "Configs.h"
#include "NonCopyable.h"
#include "Queue.h"

namespace core
{
	template<class T>
	class MsgQueue : CNoncopyable
	{
	public:
		void pushBack(T packet)
		{
			_queue.push(packet);
		}

		void bindDispatcher(const std::function<void(const T&)> &func)
		{
			_dispatcher = func;
		}

		void poll(bool& busy)
		{
			_polling.splice(_polling.end(), _querying);
			_queue.pop(_polling);
			while (!_polling.empty())
			{
				busy = true;
				auto obj = _polling.front();
				_polling.pop_front();
				dispatch(obj);
			}
		}

		bool pollOne(const int64 timeout, const boost::function<bool(const T &packet)> &condition)
		{
			for (auto now = std::chrono::steady_clock::now(), end = std::max(now + timeout, now); now <= end; now = std::chrono::steady_clock::now())
			{
				_queue.pop(_querying);
				if (_querying.empty())
				{
					std::this_thread::sleep_for(4ms);
					continue;
				}
				for (auto iter = _querying.begin(); iter != _querying.end(); ++iter)
				{
					if (condition(*iter))
					{
						auto obj = *iter;
						_polling.splice(_polling.end(), _querying, _querying.begin(), iter);
						_querying.erase(iter);
						dispatch(obj);
						return true;
					}
				}
				_polling.splice(_polling.end(), _querying);
			}
		}


	private:
		void dispatch(const T& packet)
		{
			try {
				_dispatcher(packet);
			}
			catch (std::exception &ex) {
				CORE_LOG_ERROR("dispatch exception", ex.what(), object->type());
			}			
		}

	private:
		Queue<T> _queue;
		std::list<T> _querying;
		std::list<T> _polling;
		std::function<void(const T&)> _dispatcher;
	};
}