/// \file Generic hash map implementation
/// \copyright Randomcode Developers

#pragma once

#include "base/compiler.h"
#include "base/container.h"
#include "base/types.h"
#include "base/vector.h"

template <typename K, typename V> class CHashMap: public IContainer<V, K>
{
  public:
	CHashMap();
	CHashMap(const K* keys, const V* values, ssize count);
	~CHashMap();

  private:
	struct Node
	{
		K key;
		V value;
	};

	static constexpr ssize INITIAL_CAPACITY = 16;

	// each bin can have multiple nodes, and they store the key to differentiate them
	CVector<CVector<Node>> m_nodes;
};
