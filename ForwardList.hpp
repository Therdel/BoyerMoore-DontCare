#pragma once

template<typename T>
class ForwardList {
public:
	class Node;

	ForwardList();

	auto empty() -> bool { return head() == nullptr; }
	auto head() -> Node* { return _head; }
	auto append(Node* node) -> void;

private:
	Node* _head;
	auto _find_tail() -> Node*;
};

template<typename T>
class ForwardList<T>::Node {
public:
	Node(T value);
	auto next() -> Node* { return _next; }
	auto value() -> const T& { return _value; }
private:
	friend class ForwardList<T>;
	T _value;
	Node* _next;
};

template<typename T>
ForwardList<T>::ForwardList()
: _head(nullptr) {
}

template<typename T>
auto ForwardList<T>::append(Node* node)->void {
	Node* tail = _find_tail();
	if (tail) {
		tail->_next = node;
	} else {
		_head = node;
	}
}

template<typename T>
auto ForwardList<T>::_find_tail() -> Node* {
	if (_head == nullptr) { return nullptr; }
	Node* current = _head;
	while (current->next() != nullptr) {
		current = current->next();
	}
	return current;
}

template<typename T>
ForwardList<T>::Node::Node(T value)
: _value(std::move(value))
, _next(nullptr) {
}