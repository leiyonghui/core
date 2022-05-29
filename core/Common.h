#pragma once
#include "Configs.h"

namespace core
{
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
}