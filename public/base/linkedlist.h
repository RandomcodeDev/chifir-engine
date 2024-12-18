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

	CLinkedList() DEFAULT;
	~CLinkedList() DEFAULT;

	void InsertBefore(Node_t* where, Node_t* node)
	{
		if (!node)
		{
			return;
		}

		node->prev = where->prev;
		if (where->prev)
		{
			where->prev->next = node;
		}
		where->prev = node;
		node->next = where;
	}

	void InsertAfter(Node_t* where, Node_t* node)
	{
		if (!node)
		{
			return;
		}

		node->next = where->next;
		if (where->next)
		{
			where->next->prev = node;
		}
		where->next = node;
		node->prev = where;
	}

	void Append(Node_t* node)
	{
		if (!node)
		{
			return;
		}

		if (!m_head && !m_tail)
		{
			m_head = node;
			m_tail = node;
			m_head->prev = nullptr;
			m_tail->next = nullptr;
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
			m_head = node;
			m_tail = node;
			m_head->prev = nullptr;
			m_tail->next = nullptr;
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
		if (m_nodeCount == 1)
		{
			if (Check(&m_head->data, data) == 0)
			{
				return m_head;
			}
		}
		else if (reverse)
		{
			if (m_tail)
			{
				for (Node_t* cur = m_tail; cur; cur = cur->GetPrev())
				{
					if (Check(&cur->data, data) == 0)
					{
						return cur;
					}
				}
			}
		}
		else
		{
			if (m_head)
			{
				for (Node_t* cur = m_head; cur; cur = cur->GetNext())
				{
					if (Check(&cur->data, data) == 0)
					{
						return cur;
					}
				}
			}
		}

		return nullptr;
	}

  private:
	Node_t* m_head;
	Node_t* m_tail;

	ssize m_nodeCount;
};
