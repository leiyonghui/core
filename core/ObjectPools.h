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
			core_log_trace("[================ObjectPoolInfo start:================]");
			for (const auto& iter : objectPoolMap)
				iter.second();
			core_log_trace("[================ObjectPoolInfo end.==================]");
		}
	};


	template<class T, class F = void>
	class CObjectPool;

	template<class T>
	class CObjectPool<T, std::enable_if_t<std::is_base_of<CPoolObject, T>::value, void>>
	{
	public:
		using List = std::list<T*>;
		using Deleter = typename std::function<void(T*)>;

		static const int32 INIT_SIZE = 8;

		CObjectPool(std::enable_if_t <std::is_base_of<CPoolObject, T>::value, int> initsize = INIT_SIZE) :_usecount(0), _freecount(0), _initsize(initsize){
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

		int32 getUseCount() const { return _usecount; }

		int32 getFreeCount() const { return _freecount; }

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
		T* popObject()
		{
			T* ptr = nullptr;
			{
				std::lock_guard<std::mutex> lock(_freemutex);
				if (_freeObjects.empty())
				{
					int32 add = 0;
					{
						std::lock_guard<std::mutex> lock2(_recyclemutex);
						if (!_recycleObjects.empty())
						{
							add = int32(_recycleObjects.size());
							_freeObjects.splice(_freeObjects.end(), _recycleObjects);
						}
					}
					for (int32 i = 0; i < _initsize - add; ++i, ++_freecount)
						_freeObjects.push_back(new T());
				}
				ptr = _freeObjects.front();
				_freeObjects.pop_front();
			}
			++_usecount;
			--_freecount;
			return ptr;
		}

		void recycle(T* ptr) {
			assert(ptr && ptr->isUsing());
			ptr->onRecycle();
			ptr->setUsing(false);
			{
				std::lock_guard<std::mutex> lock(_recyclemutex);
				_recycleObjects.push_back(ptr);
			}
			++_freecount;
			--_usecount;
		}

	private:
		static CObjectPool<T>* _instance;

		std::mutex _freemutex;
		std::mutex _recyclemutex;
		List	_freeObjects;
		List	_recycleObjects;
		int32	_usecount;
		std::atomic_int	  _freecount;
		std::atomic_int   _initsize;
	};

	template<class T>
	CObjectPool<T>* CObjectPool<T, std::enable_if_t<std::is_base_of<CPoolObject, T>::value, void>>::_instance = nullptr;



	template<class T>
	class CObjectPool<T, std::enable_if_t<!std::is_base_of_v<CPoolObject, T>, void>>
	{
	public:
		static const int32 MAX_SIZE = 64;

		CObjectPool(int32 maxsize = MAX_SIZE):_maxsize(maxsize), _ptr(static_cast<T*>(::operator new(sizeof(T) * _maxsize))), _objcount(0)//malloc
		{
			for (int32 i = 0; i < _maxsize; ++i)
				_pool.push_back(i);

			CObjectPoolMonitor::monitorPool(typeid(T).name(), []() { CObjectPool<T>::Instance()->printInfo(); });
		}

		CObjectPool(const CObjectPool&) = delete;

		virtual ~CObjectPool()
		{
			::operator delete(_ptr);//free
		}

	public:
		static CObjectPool<T>* Instance()
		{
			if (!_instance)
				_instance = new CObjectPool<T>();
			return _instance;
		}

		static void Instance(CObjectPool<T>* ptr)
		{
			assert(_instance == nullptr);
			_instance = ptr;
		}

	public:

		template<class ...Args>
		std::shared_ptr<T> create(Args&& ...args)
		{
			auto index = popIndex();
			if (index >= 0)
			{
				auto ptr = (void*)(_ptr + index);
				new(ptr) T(std::forward<Args>(args)...);		//调用构造函数,注意要用void*

				++_objcount;
				return std::shared_ptr<T>((T*)ptr, [index, this](T* ptr) { 
					ptr->~T();			//调用析构
					recycle(index);
					--_objcount;
				});
			}
			else
			{
				auto ptr = new T(std::forward<Args>(args)...);

				++_objcount;
				return std::shared_ptr<T>(ptr, [this](T* ptr) {
					delete ptr;
					--_objcount;
				});
			}
		}

		using Deleter = typename std::function<void(T*)>;

		template<class ...Args>
		std::unique_ptr<T, Deleter> createUnique(Args&& ...args)
		{
			auto index = popIndex();
			if (index >= 0)
			{
				auto ptr = (void*)(_ptr + index);
				new(ptr) T(std::forward<Args>(args)...);		//调用构造函数

				++_objcount;
				return std::unique_ptr<T, Deleter>((T*)ptr, [index, this](T* ptr) {
					ptr->~T();	//调用析构
					recycle(index);
					--_objcount;
				});
			}
			else
			{
				auto ptr = new T(std::forward<Args>(args)...);

				++_objcount;
				return std::unique_ptr<T, Deleter>(ptr, [](T* ptr) { 
					delete ptr;
					--_objcount;
				});
			}
		}

		void printInfo()
		{
			core_log_info(typeid(T).name(), "using:", getUseCount(), "obj:", getObjectCount(), "maxsize:", _maxsize);
		}

		int32 getUseCount()
		{
			int32 sz = 0;
			{
				std::unique_lock<std::mutex> locker(_mutex);
				sz = int32(_pool.size());
			}
			return _maxsize - sz;
		}

		int32 getObjectCount() const
		{
			return _objcount;
		}

	protected:
		int32 popIndex()
		{
			std::unique_lock<std::mutex> locker(_mutex);
			if (!_pool.empty())
			{
				auto index = _pool.front();
				_pool.pop_front();
				return index;
			}
			return -1;
		}

		void recycle(int32 index)
		{
			std::unique_lock<std::mutex> locker(_mutex);
			_pool.push_back(index);
		}

	private:
		static CObjectPool<T>* _instance;

		const int32			_maxsize;
		T*					_ptr;
		std::atomic_int		_objcount;
		std::list<int32>	_pool;
		std::mutex			_mutex;
	};
	
	template<class T>
	CObjectPool<T>* CObjectPool<T, std::enable_if_t<!std::is_base_of_v<CPoolObject, T>, void> >::_instance = nullptr;
}