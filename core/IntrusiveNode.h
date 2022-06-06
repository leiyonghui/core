#pragma once

namespace core
{
	template<typename Type, typename Tag>
	struct IntrusiveNode
	{
		IntrusiveNode<Type, Tag>(const IntrusiveNode<Type, Tag>&) = delete;

		IntrusiveNode* _prev;
		IntrusiveNode* _next;

		IntrusiveNode() : _prev(this), _next(this)
		{

		}

		Type* front()
		{
			return static_cast<Type*>(_next);
		}

		Type* back()
		{
			return static_cast<Type*>(_prev);
		}

		void pushFront(IntrusiveNode* node)
		{
			node->_prev = this;
			node->_next = _next;
			_next->_prev = node;
			_next = node;
		}

		void pushBack(IntrusiveNode* node)
		{
			node->_next = this;
			node->_prev = _prev;
			_prev->_next = node;
			_prev = node;
		}

		void popFront()
		{
			_next->leave();
		}

		void popBack()
		{
			_prev->leave();
		}

		void leave()
		{
			_next->_prev = _prev;
			_prev->_next = _next;
			_next = this;
			_prev = this;
		}

		bool empty()
		{
			return _prev == this && _next == this;
		}

		bool linked()
		{
			return _prev != this || _next != this;
		}
	};
}
