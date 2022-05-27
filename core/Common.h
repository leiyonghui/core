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

	template<class K, class V>
	bool insert(const std::map<K, V>& conatiner, const K& key, const V& value)
	{
		auto iter = conatiner.insert({key, value});
		return iter.second;
	}

	template<class K, class V>
	bool insert(const std::unordered_map<K, V>& conatiner, const K& key, const V& value)
	{
		auto iter = conatiner.insert({ key, value });
		return iter.second;
	}
}