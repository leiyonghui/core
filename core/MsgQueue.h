#pragma once
#include "Configs.h"
#include "Queue.h"

namespace core
{
	template<class T>
	class MsgQueue : CNoncopyable
	{
	public:
		MsgQueue():_dispatcher(nullptr){}

		void pushBack(const T& packet)
		{
			_queue.push(packet);
		}

		void pushBack(T&& packet)
		{
			_queue.push(std::forward(packet));
		}

		void bindDispatcher(const std::function<void(const T&)> &func)
		{
			_dispatcher = func;
		}

		void bindDispatcher(std::function<void(const T&)>&& func)
		{
			_dispatcher = std::move(func);
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

		bool pollOne(const int64 timeout, const std::function<bool(const T &packet)> &condition)
		{
			for (auto now = int64(TimeHelp::clock().count()), end = int64(TimeHelp::clock().count()) + 1000 * timeout; now <= end; now = TimeHelp::clock().count())
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
			return false;
		}


	private:
		void dispatch(const T& packet)
		{
			try {
				_dispatcher(packet);
			}
			catch (std::exception &ex) {
				core_log_error("dispatch exception", ex.what());
			}			
		}

	private:
		Queue<T> _queue;
		std::list<T> _querying;
		std::list<T> _polling;
		std::function<void(const T&)> _dispatcher;
	};
}