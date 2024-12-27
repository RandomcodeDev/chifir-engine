// Linked list

#pragma once

#include "compiler.h"
#include "types.h"

template <typename T> class CLinkedList;

template <typename T> struct LinkedNode_t
{
	T data;

	LinkedNode_t<T>* GetPrev()
	{
		return prev;
	}

	LinkedNode_t<T>* GetNext()
	{
		return next;
	}

	bool IsHead()
	{
		return prev == nullptr;
	}

	bool IsTail()
	{
		return next == nullptr;
	}

  protected:
	friend class CLinkedList<T>;

	LinkedNode_t<T>* prev;
	LinkedNode_t<T>* next;
};

// A doubly linked list,
template <typename T> class CLinkedList
{
  public:
	typedef LinkedNode_t<T> Node_t;

	CLinkedList() : m_head(nullptr), m_tail(nullptr), m_nodeCount(0)
	{
	}
	~CLinkedList()
	{
		for (Node_t* current = m_head; current->next != nullptr && current->next != m_tail; current = current->next)
		{
			if (current->prev)
			{
				current->prev->next = nullptr;
			}
			current->prev = nullptr;
		}
	}

	bool InsertBefore(Node_t* where, Node_t* node)
	{
		if (!node || where == node)
		{
			return false;
		}

		if (where == m_head)
		{
			Prepend(node);
			return true;
		}

		node->prev = where->prev;
		if (where->prev)
		{
			where->prev->next = node;
		}
		where->prev = node;
		node->next = where;

		m_nodeCount++;
		return true;
	}

	bool InsertAfter(Node_t* where, Node_t* node)
	{
		if (!node || where == node)
		{
			return false;
		}

		if (where == m_tail)
		{
			Append(node);
			return true;
		}

		node->next = where->next;
		if (where->next)
		{
			where->next->prev = node;
		}
		where->next = node;
		node->prev = where;

		m_nodeCount++;
		return true;
	}

	void Append(Node_t* node)
	{
		if (!node)
		{
			return;
		}

		if (!m_head && !m_tail)
		{
			Initialize(node);
		}
		else if (m_head == m_tail)
		{
			m_tail->next = node;
			m_tail = node;
			m_tail->prev = m_head;
			m_tail->next = nullptr;
		}
		else
		{
			m_tail->next = node;
			node->prev = m_tail;
			m_tail = node;
			m_tail->next = nullptr;
		}

		m_nodeCount++;
	}

	void Prepend(Node_t* node)
	{
		if (!node)
		{
			return;
		}

		if (!m_head && !m_tail)
		{
			Initialize(node);
		}
		else if (m_head == m_tail)
		{
			m_tail->prev = node;
			m_head = node;
			m_head->next = m_tail;
			m_head->prev = nullptr;
		}
		else
		{
			m_head->prev = node;
			node->next = m_head;
			m_head = node;
			m_head->prev = nullptr;
		}

		m_nodeCount++;
	}

	void Remove(Node_t* node)
	{
		if (!node)
		{
			return;
		}

		if (node == m_head)
		{
			m_head = m_head->next;
		}

		if (node == m_tail)
		{
			m_tail = m_tail->prev;
		}

		if (node->prev)
		{
			node->prev->next = node->next;
		}
		if (node->next)
		{
			node->next->prev = node->prev;
		}

		node->prev = nullptr;
		node->next = nullptr;

		m_nodeCount--;
	}

	Node_t* GetHead() const
	{
		return m_head;
	}

	Node_t* GetTail() const
	{
		return m_tail;
	}

	ssize Size() const
	{
		return m_nodeCount;
	}

	bool IsEmpty() const
	{
		return Size() == 0;
	}

	Node_t* Find(s32 (*Check)(T* cur, void* data), void* data, bool reverse = false) const
	{
		Node_t* cur = reverse ? m_tail : m_head;
		while (cur)
		{
			if (Check(&cur->data, data) == 0)
			{
				return cur;
			}
			cur = reverse ? cur->GetPrev() : cur->GetNext();
		}
		return nullptr;
	}

  private:
	Node_t* m_head;
	Node_t* m_tail;

	ssize m_nodeCount;

	void Initialize(Node_t* node)
	{
		m_head = node;
		m_tail = node;
		m_head->prev = nullptr;
		m_tail->next = nullptr;
	}
};
