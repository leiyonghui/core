#pragma once
#include "Configs.h"

namespace core
{
	class CPoolObject
	{
	public:
		CPoolObject() :_using(false) {}
		virtual ~CPoolObject() = default;

		void setUsing(bool use) { _using = use; };

		bool isUsing() const { return _using; }

		virtual void onRecycle() = 0;
	private:
		bool _using;
	};

	class CObjectPoolMonitor
	{
		using ObjectMap = std::map<std::string, std::function<void()>>;
	public:
		static ObjectMap& getObjectPool()
		{
			static ObjectMap ObjectPoolMap;//以免Monitor还未创建
			return ObjectPoolMap;
		}

		static void monitorPool(std::string name, std::function<void()>&& func)
		{
			auto& objectPoolMap = getObjectPool();
			objectPoolMap[name] = std::move(func);
		}

		static void delMonitorPool(const std::string& name)//?
		{
			auto& objectPoolMap = getObjectPool();
			objectPoolMap.erase(name);
		}

		static void showInfo()
		{
			auto& objectPoolMap = getObjectPool();
			core_log_trace("ObjectPoolInfo start:");
			for (const auto& iter : objectPoolMap)
				iter.second();
			core_log_trace("ObjectPoolInfo end.");
		}
	};


	const int32 INIT_SIZE = 8;

	template<class T>
	class CObjectPool
	{
		using List = std::list<T*>;
		using Deleter = typename std::function<void(T*)>;
	public:

		CObjectPool(std::enable_if_t <std::is_base_of<CPoolObject, T>::value, int> initSize = INIT_SIZE) :_useCount(0), _freeCount(0), _initSize(initSize){
			CObjectPoolMonitor::monitorPool(typeid(T).name(), []() { CObjectPool<T>::Instance()->printInfo(); });
		}

		virtual~CObjectPool() {
			for (auto ptr : _freeObjects)
				delete ptr;
		}

		template<class ...Args>
		std::shared_ptr<T> create(Args&& ...args)
		{
			auto ptr = popObject();
			ptr->setUsing(true);
			ptr->onAwake(std::forward<Args>(args)...);
			return std::shared_ptr<T>(ptr, [](T* ptr) { CObjectPool<T>::Instance()->recycle(ptr); });
		}

		template<class ...Args>
		std::unique_ptr<T, Deleter> createUnique(Args&& ...args)
		{
			auto ptr = popObject();
			ptr->setUsing(true);
			ptr->onAwake(std::forward<Args>(args)...);
			return std::unique_ptr<T, Deleter>(ptr, [](T* ptr) { CObjectPool<T>::Instance()->recycle(ptr); });
		}

		void printInfo()
		{
			core_log_info(typeid(T).name(), "using:", getUseCount(), "free:", getFreeCount());
		}

		int32 getUseCount() const { return _useCount; }

		int32 getFreeCount() const { return _freeCount; }

		static CObjectPool<T>* Instance()
		{
			if (!_instance)
				_instance = new CObjectPool<T>();
			return _instance;
		}

		static void Instance(CObjectPool<T>* ptr)
		{
			if (_instance)
				delete _instance;
			_instance = ptr;
		}

	private:
		static CObjectPool<T>* _instance;

		std::mutex _mutexFree;
		std::mutex _mutexRecycle;
		List	_freeObjects;
		List	_recycleObjects;
		int32	_useCount;
		std::atomic_int	  _freeCount;
		std::atomic_int   _initSize;

		T* popObject()
		{
			T* ptr = nullptr;
			{
				std::lock_guard<std::mutex> lock(_mutexFree);
				if (_freeObjects.empty())
				{
					int32 add = 0;
					{
						std::lock_guard<std::mutex> lock2(_mutexRecycle);
						if (!_recycleObjects.empty())
						{
							add = int32(_recycleObjects.size());
							_freeObjects.splice(_freeObjects.end(), _recycleObjects);
						}
					}
					for (int32 i = 0; i < _initSize - add; ++i, ++_freeCount)
						_freeObjects.push_back(new T());
				}
				ptr = _freeObjects.front();
				_freeObjects.pop_front();
			}
			++_useCount;
			--_freeCount;
			return ptr;
		}

		void recycle(T* ptr) {
			assert(ptr && ptr->isUsing());
			ptr->onRecycle();
			ptr->setUsing(false);
			{
				std::lock_guard<std::mutex> lock(_mutexRecycle);
				_recycleObjects.push_back(ptr);
			}
			++_freeCount;
			--_useCount;
		}
	};

	template<class T>
	CObjectPool<T>* CObjectPool<T>::_instance = nullptr;

}