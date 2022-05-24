#pragma once
#include "Configs.h"

namespace core
{
	template<typename T>
	class Queue
	{
	private:
		std::list<T> _queue;
		std::mutex _mutex;

	private:
		Queue(const Queue&) = delete;
		Queue& operator = (const Queue&) = delete;

	public:
		Queue()
		{

		}

		virtual ~Queue()
		{

		}

		void push(const T& item)
		{
			std::unique_lock<std::mutex> locker(_mutex);
			_queue.push_back(item);
		}

		void push(const T& item, bool& notify)
		{
			std::unique_lock<std::mutex> locker(_mutex);
			if (_queue.empty())
			{
				notify = true;
			}
			_queue.push_back(item);
		}

		//items will be modify
		void push(std::list<T>& items, bool& notify)
		{
			std::unique_lock<std::mutex> locker(_mutex);
			if (_queue.empty())
			{
				notify = true;
			}
			_queue.splice(_queue.end(), items);
		}

		bool pop(T& item)
		{
			std::unique_lock<std::mutex> locker(_mutex);
			if (!_queue.empty())
			{
				item = _queue.front();
				_queue.pop_front();
				return true;
			}
			else
			{
				return false;
			}
		}

		bool pop(std::list<T>& items)
		{
			std::unique_lock<std::mutex> locker(_mutex);
			if (!_queue.empty())
			{
				items.splice(items.end(), _queue);
				return true;
			}
			else
			{
				return false;
			}
		}

		bool pop(std::list<T>& items, uint32_t maxn)
		{
			std::unique_lock<std::mutex> locker(_mutex);
			if (!_queue.empty())
			{
				items.splice(items.end(), _queue, _queue.begin(), move_iterator(_queue.begin(), _queue.end(), maxn));
				return true;
			}
			else
			{
				return false;
			}
		}

		template<typename Disposer>
		void cleanup(Disposer disposer)
		{
			for (typename std::list<T>::iterator iter = _queue.begin(); iter != _queue.end(); ++iter)
			{
				disposer(*iter);
			}
			_queue.clear();
		}

	private:
		template<typename Iter>
		Iter move_iterator(Iter begin, Iter end, uint32_t count)
		{
			while (begin != end && count > 0)
			{
				++begin;
				--count;
			}
			return begin;
		}
	};
}