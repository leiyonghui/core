#pragma once
#include "Configs.h"

namespace core
{
	template<class Key, class Value>
	Value* find(const std::map<Key, Value*>& container, const Key& key, const Value* vaule)
	{
		auto iter = container.find(key);
		if (iter == container.end())
		{
			return vaule;
		}
		return iter->second;
	}

	template<class Key, class Value>
	Value find(const std::map<Key, Value>& container, const Key& key, const Value& vaule)
	{
		auto iter = container.find(key);
		if (iter == container.end())
		{
			return vaule;
		}
		return iter->second;
	}


}