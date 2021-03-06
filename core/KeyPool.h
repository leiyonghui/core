#pragma once
#include "Configs.h"

namespace core
{
	template<uint32 Bytes = 16, std::enable_if_t<Bytes <= 20, int32> = 0>
	class KeyPool
	{
		KeyPool(const KeyPool&) = delete;
		KeyPool& operator = (const KeyPool&) = delete;

	public:
		static const uint32_t key_maxnum = (1 << Bytes);//0x10000;//十六进制，65536
		static const uint32_t key_suffix = (1 << Bytes) - 1; //0xffff;//65535
		static const uint32_t key_prefix_min = (1 << Bytes);//0x10000;
		static const uint32_t key_prefix_inc = (1 << Bytes);//0x10000;
		static const uint32_t key_prefix_max = (~((1 << Bytes) - 1));//0xffff0000;//4294901760

		static uint32_t index(uint32_t key)
		{
			return key & key_suffix;//去掉高16位
		}

		KeyPool()
		{
			for (uint32_t i = 0; i < key_maxnum; ++i)
			{
				_keys.push_back(key_prefix_min + i);
			}
		}


		uint32_t pop()
		{
			std::unique_lock<std::mutex> lock(_mutex);
			if (_keys.empty())
				return 0;
			uint32_t key = _keys.front();
			_keys.pop_front();
			return key;
		}

		void push(uint32_t key)
		{
			auto k = remake(key);
			std::unique_lock<std::mutex> lock(_mutex);
			_keys.push_back(k);
		}

	private:
		uint32_t remake(uint32_t key)
		{
			if (key < key_prefix_max)
				return key + key_prefix_inc;
			else
				return index(key) + key_prefix_min;
		}
	private:
		std::mutex _mutex;
		std::list<uint32_t> _keys;
	};
}