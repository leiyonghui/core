#pragma once

namespace core
{
	template<typename T>
	class CSingleton
	{
	public:
		inline static T* Instance()
		{
			if (_instance == nullptr)
			{
				_instance = new T;
			}
			return _instance;
		}

		inline static T* Instance(T* instance)
		{
			if (_instance) 
				delete _instance;
			_instance = instance;
			return _instance;
		}

		template<class ...Args>
		inline static T* Instance(Args&&...args)
		{
			if (_instance)
				delete _instance;
			_instance = new T(std::forward<Args>(args)...);
			return _instance;
		}

	private:
		static T* _instance;
	};

	template<typename T>
	T* CSingleton<T>::_instance = nullptr;
}