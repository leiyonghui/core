#pragma once
#include "Configs.h"

namespace core
{
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
			core_log_trace("ObjectPoolInfo start:", TimeHelp::TimeToString(TimeHelp::now()));
			for (const auto& iter : objectPoolMap)
				iter.second();
			core_log_trace("ObjectPoolInfo end.");
		}
	};


#define INIT_OBJECT_SIZE 16
	template<class T>
	class CObjectPool
	{
		using List = std::list<T*>;
		using Iterator = typename std::list<T*>::iterator;
		using Deleter = typename std::function<void(T*)>;
	public:
		const static Iterator NullIter;

		CObjectPool() :_useCount(0), _freeCount(0) {
			CObjectPoolMonitor::monitorPool(typeid(T).name(), [this]() { this->printInfo(); });
		}

		virtual~CObjectPool() {

		}

		template<class ...Args>
		std::shared_ptr<T> create(Args&& ...args)
		{
			auto ptr = popObject();
			ptr->setUsing(true);
			ptr->onAwake(std::forward<Args>(args)...);
			return std::shared_ptr<T>(ptr, [this](T* ptr) { this->recycle(ptr); });
		}

		template<class ...Args>
		std::unique_ptr<T, Deleter> createUnique(Args&& ...args)
		{
			auto ptr = popObject();
			ptr->setUsing(true);
			ptr->onAwake(std::forward<Args>(args)...);
			return std::unique_ptr<T, Deleter>(ptr, [this](T* ptr) { this->recycle(ptr); });
		}

		void printInfo()
		{
			core_log_info(typeid(T).name(), "using:", _useCount, "free:", _freeCount);
		}

		static CObjectPool<T>* Instance()
		{
			return _instance;
		}

	private:
		static CObjectPool<T>* _instance;

		List	_freeObjects;
		int32	_useCount;
		int32	_freeCount;
		std::mutex _mutex;

		T* popObject()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			while (_freeObjects.empty())
				assignObjs(INIT_OBJECT_SIZE);
			T* ptr = _freeObjects.front();
			_freeObjects.pop_front();
			++_useCount;
			--_freeCount;
			return ptr;
		}

		void recycle(T* ptr) {
			if (ptr && ptr->isUsing()) {
				std::lock_guard<std::mutex> lock(_mutex);
				ptr->onRecycle();
				ptr->setUsing(false);
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
	const typename CObjectPool<T>::Iterator CObjectPool<T>::NullIter = CObjectPool<T>::Iterator();

	template<class T>
	CObjectPool<T>* CObjectPool<T>::_instance = new CObjectPool<T>();

	class CPoolObject
	{
		bool _using;
	public:
		CPoolObject() :_using(false) {}

		void setUsing(bool use) { _using = use; };

		bool isUsing() const { return _using; }
	};

}