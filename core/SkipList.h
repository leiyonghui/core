
// Created by lyh on 2021.9.7

#pragma once
#include "Types.h"
#include <map>
#include <vector>
#include <algorithm>
#include <functional>

using namespace core;

static const int32 SKIPLIST_MAXLEVEL = 31;

static const double SKIPLIST_P = 0.25;

template<typename K, typename V, typename Compare>
class CSkipNode
{
public:
	K key;
	V value;
	int32 high;

	struct SkipNodeLevel
	{
		CSkipNode<K, V, Compare>* forward;
		int32 span;
	}*level;

	CSkipNode<K, V, Compare>* backward;

	static CSkipNode<K, V, Compare>* createNode(K k, V v, const int32& level)
	{
		CSkipNode<K, V, Compare>* node = new CSkipNode<K, V, Compare>();

		node->level = new CSkipNode<K, V, Compare>::SkipNodeLevel[level];
		memset(node->level, 0, sizeof(CSkipNode<K, V, Compare>*) * level);

		node->backward = NULL;
		node->key = k;
		node->value = v;
		node->high = level;
		return node;
	}

	CSkipNode<K, V, Compare> *next()
	{
		return level ? level[0].forward : NULL;
	}

	void printString()
	{
		printf("==========\n skipNode: key = %d, value = %d \n node = %d, level = %d, hight = %d \n ", (int32)key, (int32)value, this, this->level, this->high);
		for (int32 i = 0; i < this->high; i++)
		{
			if (level[i].span < 0)
			{
				printf("level: %d . forward = %d, span = %d \n", i, level[i].forward, level[i].span);
			}
		}
		printf("==========\n\n");
	}
};

template<typename K, typename V, typename Compare>
class CSkipList
{
	typedef CSkipNode<K, V, Compare> TSkipNode;
	TSkipNode *header, *tail;
	int32 level;
	int32 length;
	Compare compare;
	int32 findCount;

	std::map<K, TSkipNode*> dictMap;


	void freeNode(TSkipNode* node)
	{
		delete[] node->level;
		delete node;
	}

	int32 randomLevel() {
		int32 level = 1;
		while ((rand() & 0xffff) <= (SKIPLIST_P * 0xffff) && level < SKIPLIST_MAXLEVEL)
		{
			level++;
		}
		return level;
	}

	inline bool equipValue(const V &a, const V &b)
	{
		return !compare(a, b) && !compare(b, a);
	}

public:
	CSkipList<K, V, Compare>():header(TSkipNode::createNode(K(), V(), SKIPLIST_MAXLEVEL)), tail(NULL), level(1), length(0), compare(), findCount(0)
	{
		
	}

	~CSkipList<K, V, Compare>()
	{
		TSkipNode* node = header->level[0].forward, *next;
		freeNode(header);
		while (node)
		{
			next = node->level[0].forward;
			freeNode(node);
			node = next;
		}
	}

	int32 getRank(const K &key)
	{
		const auto& iter = dictMap.find(key);
		if (iter == dictMap.end())
		{
			return 0;
		}
		TSkipNode* update = new TSkipNode[SKIPLIST_MAXLEVEL];
		int32 rank[SKIPLIST_MAXLEVEL];

		const V& value = iter->second->value;

		TSkipNode* node = header;
		for (int32 i = this->level - 1; i >= 0; i--)
		{
			rank[i] = i == this->level - 1 ? 0 : rank[i + 1];
			while (node->level[i].forward && ( compare(node->level[i].forward->value, value) || (equipValue(node->level[i].forward->value, value) && node->level[i].forward->key < key)) )
			{
				rank[i] += node->level[i].span;
				node = node->level[i].forward;
			}
		}
		return rank[0] + 1;
	}

	std::pair<bool, std::pair<K, V>> getRankValue(const int32& rank)
	{
		if (rank > this->length || rank < 1)
		{
			return std::make_pair(false, std::make_pair(K(), V()));
		}


		int32 sum[SKIPLIST_MAXLEVEL];
		memset(sum, 0, sizeof(int32)*SKIPLIST_MAXLEVEL);
		this->findCount = 0;
		TSkipNode* node = this->header;
		for (int32 i = this->level - 1; i >= 0; i--)
		{
			sum[i] = i == this->level - 1 ? 0 : sum[i + 1];
			while (node->level[i].forward && node->level[i].span && sum[i] + node->level[i].span <= rank)
			{
				this->findCount++;
				sum[i] += node->level[i].span;
				node = node->level[i].forward;
			}
			this->findCount++;
		}
		if (node)
		{
			return std::make_pair(true, std::make_pair(node->key, node->value));
		}
		return std::make_pair(false, std::make_pair(K(), V()));
	}

	void insert(K key, V value)
	{
		erase(key);
		TSkipNode* update[SKIPLIST_MAXLEVEL];
		int32 rank[SKIPLIST_MAXLEVEL];
		memset(rank, 0, sizeof(int32)*SKIPLIST_MAXLEVEL);

		this->findCount = 0;

		TSkipNode* node = this->header;
		for (int32 i = this->level - 1; i >= 0; i--)
		{
			rank[i] = i == this->level - 1 ? 0 : rank[i+1];
			while (node->level[i].forward && ( compare(node->level[i].forward->value, value) || (equipValue(node->level[i].forward->value, value) && node->level[i].forward->key < key) ))
			{
				this->findCount++;
				rank[i] += node->level[i].span;
				node = node->level[i].forward;
			}
			this->findCount++;
			update[i] = node;
		}

		int32 level = randomLevel();
		TSkipNode* newNode = TSkipNode::createNode(key, value, level);

		int32 minLevel = level < this->level ? level - 1 : this->level - 1;
		for (int32 i = 0; i <= minLevel; i++)
		{
			newNode->level[i].forward = update[i]->level[i].forward;
			update[i]->level[i].forward = newNode;

			newNode->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
			update[i]->level[i].span = rank[0] - rank[i] + 1;
		}

		if (level > this->level)
		{
			for (int32 i = this->level; i < level; i++)
			{
				newNode->level[i].forward = NULL;
				newNode->level[i].span = this->length - rank[0];
				header->level[i].forward = newNode;
				header->level[i].span = rank[0] + 1;
			}
			this->level = level;
		}
		else if(level < this->level)
		{
			for (int32 i = level; i < this->level; i++)
			{
				update[i]->level[i].span++;
			}
		}

		newNode->backward = update[0];
		if (newNode->level[0].forward == NULL)
		{
			this->tail = newNode;
		}
		else
		{
			newNode->level[0].forward->backward = newNode;
		}
		dictMap[key] = newNode;
		this->length++;
	}

	bool erase(K key)
	{
		const auto& iter = dictMap.find(key);
		if (iter == dictMap.end())
		{
			return 0;
		}
		TSkipNode* update[SKIPLIST_MAXLEVEL];

		const V& value = iter->second->value;

		TSkipNode* node = this->header;
		for (int32 i = this->level - 1; i >= 0; i--)
		{
			while (node->level[i].forward && (compare(node->level[i].forward->value, value) || (equipValue(node->level[i].forward->value, value) && node->level[i].forward->key < key) ))
			{
				node = node->level[i].forward;
			}
			update[i] = node;
		}

		if (update[0]->level[0].forward)
		{
			deleteNode(update[0]->level[0].forward, update);
			dictMap.erase(key);
			return true;
		}
		return false;
	}

	void deleteNode(TSkipNode* node, TSkipNode** update)
	{
		if (node->level[0].forward)
		{
			node->level[0].forward->backward = update[0];
		}
		for (int32 i = 0; i < node->high; i++)
		{
			update[i]->level[i].forward = node->level[i].forward;
			update[i]->level[i].span += (node->level[i].span - 1);
		}
		for (int32 i = node->high; i<this->level; i++)
		{
			update[i]->level[i].span--;
		}
		for (int32 i = this->level - 1; i >= 0; i--)
		{
			if (header->level[i].forward)
			{
				this->level = i + 1;
				break;
			}
		}
		if (update[0]->level[0].forward == this->tail)
		{
			this->tail = update[0] == header ? NULL : update[0];
		}
		this->length--;
	}

	void visit(const std::function<bool(K k, V v, int32 r)> &fun)
	{
		int32 rank = 0;
		for (TSkipNode * node = front(); node != NULL; node = node->next())
		{
			rank++;
			if (!fun(node->key, node->value, rank))
			{
				return;
			}
		}
	}

	TSkipNode* front()
	{
		return header->level[0].forward;
	}

	TSkipNode* back()
	{
		return tail;
	}

	V getValue(K key)
	{
		if (dictMap.find(key) == dictMap.end())
		{
			return V();
		}
		return dictMap[key]->value;
	}

	int32 size()
	{
		return length;
	}

	int32 getLevel()
	{
		return this->level;
	}

	int32 getFindCount()
	{
		return findCount;
	}
};

void testSkipList()
{
	int32 lastValue = 0;
	std::vector<std::pair<int32, int32>> values;
	std::map<int32, int32> maps;
	auto func = [&lastValue, &values, &maps](int32 key, int32 value, int32 rank) -> bool {
		//printf("rank = %d,  key =  %d, value =  %d \n", rank, key, value);
		if (value < lastValue)
		{
			printf("error . rank = %d, key = %d, value = %d, lastValue = %d\n\n",rank, key, value, lastValue);
		}
		lastValue = value;
		return true;
	};
	for (int32 i = 10; i <= 18; i++)
	{
		{
			lastValue = 0;
			CSkipList<int32, int32, std::less<int32>> rank;
			int32 sum = 0;
			int32 num = (1 << i);
			maps.clear();
			for (int32 j = 1; j <= num; j++)
			{
				int32 key = rand() % num + 1;
				int32 value = rand();
				rank.insert(key, value);
				sum += rank.getFindCount();
				maps[key] = value;
			}
			printf("insert = %d, size = %d, level = %d\n", num, rank.size(), rank.getLevel());
			printf("findCount = %d, logN = %d, NLogN = %d\n\n", sum, i, i*(1 << i));
			rank.visit(func);

			int32 rr = 0;
			values.clear();
			for (auto& iter : maps)
			{
				values.push_back(std::make_pair(iter.second, iter.first));
			}
			std::sort(values.begin(), values.end());
			if (int32(maps.size()) != rank.size())
			{
				printf("--- size diff", maps.size(), rank.size());
			}
			for (auto& iter : values)
			{
				rr++;
				auto key = iter.second;
				auto value = iter.first;
				auto pair = rank.getRankValue(rr);
				int32 gRank = rank.getRank(iter.second);
				if (rank.getValue(key) != value)
				{
					printf("error value diff = %d, value =  %d \n", iter.second, iter.first);
				}
				if (gRank != rr)
				{
					printf("1 error null rank diff key = %d, value =  %d, rank1 = %d , rank2 = %d \n", iter.second, iter.first, gRank, rr);
				}
				if (!pair.first)
				{
					printf("2 error null rank diff key = %d, value =  %d \n", iter.second, iter.first);
				}
				if (pair.second.first != iter.second || pair.second.second != iter.first)
				{
					printf("rank diff , rank = %d, k1 =  %d, v1 = %d -- k2 =  %d, v2 = %d\n", rr, iter.second, iter.first, pair.second.first, pair.second.second);
				}
			}
		}
		printf("****************************\n\n");
	}
}