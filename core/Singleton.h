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
			T* old = _instance;
			_instance = instance;
			return old;
		}

	private:
		static T* _instance;
	};

	template<typename T>
	T* CSingleton<T>::_instance = NULL;
}