#pragma once
namespace core
{
	template<class T>
	class CFastNode
	{
		CFastNode(const CFastNode&) = delete;
		CFastNode operator=(const CFastNode&) = delete;

	protected:
		T _data;

		CFastNode* _prev;
		CFastNode* _next;

	public:
		explicit CFastNode(const T& data) :_data(data), _prev(this), _next(this) {

		}

		~CFastNode() {
			if (linked())
			{
				leave();
			}
		}

		T& data() {
			return _data;
		}

		CFastNode& front() {
			return *_next;
		}

		CFastNode& back() {
			return *_prev;
		}

		CFastNode* begin() {
			return _next;
		}

		CFastNode* next() {
			return _next;
		}

		CFastNode* prev() {
			return _prev;
		}

		CFastNode* end() {
			return this;
		}

		bool empty() {
			return _next == this && _prev == this;
		}

		bool linked() {
			return _next != this || _prev != this;
		}

		void leave() {
			_next->_prev = _prev;
			_prev->_next = _next;
			_prev = this;
			_next = this;
		}

		void pushFront(CFastNode& node) {
			node._prev = this;
			node._next = _next;
			_next->_prev = &node;
			_next = &node;
		}

		void pushBack(CFastNode& node) {
			node._prev = _prev;
			node._next = this;
			_prev->_next = &node;
			_prev = &node;
		}

		void popFront() {
			if (!empty()) {
				front().leave();
			}
		}

		void popBack() {
			if (!empty()) {
				back().leave();
			}
		}

		void swap(CFastNode& node) {
			auto tmpprev = node._prev;
			auto tmpnext = node._next;
			_prev->_next = &node;
			_next->_prev = &node;
			node._prev = _prev;
			node._next = _next;

			tmpprev->_next = this;
			tmpnext->_prev = this;
			_prev = tmpprev;
			_next = tmpnext;
		}

	};
}