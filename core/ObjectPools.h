#pragma once
#include "Configs.h"

namespace core
{
	class CPoolObject
	{
	public:
		CPoolObject() :_using(false) {}

		void setUsing(bool use) { _using = use; };

		bool isUsing() const { return _using; }

		virtual void onRecycle() = 0; //注意多线程
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

		CObjectPool(std::enable_if_t <std::is_base_of<CPoolObject, T>::value, int> = 0) :_useCount(0), _freeCount(0) {
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
			core_log_info(typeid(T).name(), "using:", _useCount, "free:", _freeCount);
		}

		static CObjectPool<T>* Instance()
		{
			if (!_instance)
				_instance = new CObjectPool<T>();
			return _instance;
		}

		static void Instance(T* ptr)
		{
			if (_instance)
				delete _instance;
			_instance = ptr;
		}

	private:
		static CObjectPool<T>* _instance;

		List	_freeObjects;
		int32	_useCount;
		int32	_freeCount;
		std::mutex _mutex;

		T* popObject()
		{
			T* ptr = nullptr;
			{
				std::lock_guard<std::mutex> lock(_mutex);
				while (_freeObjects.empty())
					assignObjs(INIT_SIZE);
				ptr = _freeObjects.front();
				_freeObjects.pop_front();
				++_useCount;
				--_freeCount;
			}
			return ptr;
		}

		void recycle(T* ptr) {
			assert(ptr && ptr->isUsing());
			ptr->onRecycle();
			ptr->setUsing(false);
			{
				std::lock_guard<std::mutex> lock(_mutex);
				_freeObjects.push_back(ptr);
				++_freeCount;
				--_useCount;
			}
		}

		void assignObjs(int32 amount) {
			for (int32 i = 0; i < amount; ++i, ++_freeCount)
				_freeObjects.push_back(new T());
		}
	};

	template<class T>
	CObjectPool<T>* CObjectPool<T>::_instance = nullptr;

}