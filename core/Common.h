#pragma once
#include "Configs.h"

namespace core
{
	template<typename T>
	struct CEmpty
	{
		static const T Empty;
	};

	template<typename T>
	const T CEmpty<T>::Empty = T();

	class CFinalize
	{
	public:
		using Func = std::function<void()>;

		CFinalize(const Func& func): _func(func){}
		CFinalize(Func&& func) : _func(std::move(func)){}

		CFinalize& operator =(const Func& func) {
			_func = func;
			return *this;
		}

		CFinalize& operator =(Func&& func) {
			_func = std::move(func);
			return *this;
		}

		~CFinalize() {
			if (_func) 
				_func();
		}

	private:
		Func _func;
	};

	template<class Key, class Value, class Pr>
	Value find(const std::map<Key, Value, Pr>& container, const Key& key, const Value& vaule) /*-> decltype(vaule)*/
	{
		auto iter = container.find(key);
		if (iter == container.end())
		{
			return vaule;
		}
		return iter->second;
	}

	template<class Key, class Value, class Pr>
	Value find(const std::unordered_map<Key, Value, Pr>& container, const Key& key, const Value& vaule) /*-> decltype(vaule)*/
	{
		auto iter = container.find(key);
		if (iter == container.end())
		{
			return vaule;
		}
		return iter->second;
	}

	template<class K, class V>
	bool insert(std::map<K, V>& conatiner, const K& key, const V& value)
	{
		auto iter = conatiner.insert({key, value});
		return iter.second;
	}

	template<class K, class V>
	bool insert(std::unordered_map<K, V>& conatiner, const K& key, const V& value)
	{
		auto iter = conatiner.insert({ key, value });
		return iter.second;
	}

	template<class K, class Pr>
	bool insert(std::set<K, Pr>& conatiner, const K& key)
	{
		auto iter = conatiner.insert(key);
		return iter.second;
	}

	template<class K, class V>
	bool remove(std::map<K, V>& conatiner, const K &key)
	{
		return conatiner.erase(key) > 0;
	}

	template<class K, class V>
	bool remove(std::unordered_map<K, V>& conatiner, const K& key)
	{
		return conatiner.erase(key) > 0;
	}

	template<class K, class Pr>
	bool remove(std::set<K, Pr>& conatiner, const K& key)
	{
		return conatiner.erase(key) > 0;
	}

	template<class Key, class Value, class Pr>
	bool exist(const std::map<Key, Value, Pr>& container, const Key& key) /*-> decltype(vaule)*/
	{
		auto iter = container.find(key);
		if (iter == container.end())
		{
			return false;
		}
		return true;
	}

	template<class Key, class Value, class Pr>
	bool exist(const std::unordered_map<Key, Value, Pr>& container, const Key& key) /*-> decltype(vaule)*/
	{
		auto iter = container.find(key);
		if (iter == container.end())
		{
			return false;
		}
		return true;
	}

	template<class Key, class Pr>
	bool exist(const std::set<Key, Pr>& container, const Key& key)
	{
		return container.count(key);
	}
}