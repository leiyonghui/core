#pragma once
#include "Configs.h"

namespace core
{
#define core_find(container, key, value) (container.find(key) == container.end() ? value : container.find(key)->second)

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
}