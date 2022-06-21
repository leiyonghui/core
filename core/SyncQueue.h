#pragma once
#include "Configs.h"

namespace core
{
	template<class T>
	class SyncQueue
	{
	public:
		SyncQueue(int32 maxSize = -1) : _maxSize(maxSize), _needStop(false)
		{

		}

		void put(const T& value)
		{
			add(value);
		}

		void put(T&& value)
		{
			add(std::move(value));
		}

		void take(T& value)
		{
			std::unique_lock<std::mutex> locker(_mutex);
			_notEmpty.wait(locker, [this] {return _needStop || notEmpty(); });

			if (_needStop)
				return;

			value = _list.pop_front();
			_notFull.notify_one();
		}

		void take(std::list<T>& list, int32 maxnum)
		{
			std::unique_lock<std::mutex> locker(_mutex);
			_notEmpty.wait(locker, [this] {return _needStop || notEmpty(); });

			if (_needStop)
				return;

			list.splice(list.end(), _list, _list.begin(), advance(_list.begin(), _list.end(), maxnum));
			_notFull.notify_one();
		}

		void flush(std::list<T>& list)
		{
			std::lock_guard<std::mutex> locker(_mutex);
			list.splice(list.end(), _list);
		}

		void stop()
		{
			_needStop = true;

			_notEmpty.notify_all();
			_notFull.notify_all();
		}

		bool empty()
		{
			std::lock_guard<std::mutex> locker(_mutex);
			return _list.empty();
		}

		int32 size()
		{
			std::lock_guard<std::mutex> locker(_mutex);
			return _list.size();
		}

		bool full()
		{
			if (_maxSize < 0)
				return false;
			std::lock_guard<std::mutex> locker(_mutex);
			return _list.size() >= _maxSize;
		}

	private:

		template<class F>
		void add(F&& value)
		{
			std::unique_lock<std::mutex> locker(_mutex);
			if (_maxSize > 0)
			{
				_notFull.wait(locker, [this]() { return _needStop || notFull(); });
			}
			if (_needStop)
				return;

			_list.push_back(std::forward<F>(value));
			_notEmpty.notify_one();
		}

		bool notFull()
		{
			if (_maxSize < 0)
				return false;
			auto isFull = _list.size() >= _maxSize;
			/*if (isFull)
				std::cout << "wait full" << std::endl;*/
			return !isFull;
		}

		bool notEmpty()
		{
			auto isEmpty = _list.empty();
			//if (isEmpty)
				//std::cout << "wait not empty" << std::endl;
			return !isEmpty;
		}

		template<typename Iter>
		Iter advance(Iter begin, Iter end, uint32 count)
		{
			while (begin != end && count > 0)
			{
				++begin;
				--count;
			}
			return begin;
		}

	private:
		int32 _maxSize;
		std::atomic_bool _needStop;
		std::list<T> _list;
		std::mutex	_mutex;
		std::condition_variable _notEmpty;
		std::condition_variable _notFull;
	};
}