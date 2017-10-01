#ifndef _GRAPH_H
#define _GRAPH_H

#include <vector>
#include <map>

namespace gr {

template<typename T>
class Node {
	T data;
	std::vector<Node<T>*> neighbors;
public:
	Node(T);
	T getData();
	void setData(T);
	void addNeighbors(std::vector<Node<T>*>);
	void addNeighbors(Node<T>*);
	bool removeNeighbor(Node<T>*);
	std::vector<Node<T>*> getNeighbors() const;

};


template<typename Key, typename T>
class Graph {
	std::vector<Key> keys;
	std::map<Key, Node<T>*> nodeMap;
public:
	Node<T>* getKey(int);
	Node<T>* getNode(Key);
	void addNode(Node<T>*, Key);

};

template<typename T>
Node<T>::Node(T data) {
	this->data = data;
};

template<typename T>
T Node<T>::getData() { return this->data; }

template<typename T>
void Node<T>::setData(T data) { this->data = data; }

template<typename T>
void Node<T>::addNeighbors(std::vector<Node<T>*> nodes) {
	for(int i = 0; i < nodes.length(); ++i) {
		this->neighbors.push_back(nodes[i]);
	}
}

template<typename T>
void Node<T>::addNeighbors(Node<T>* node) {
	this->neighbors.push_back(node);
}

template<typename T>
bool Node<T>::removeNeighbor(Node<T>* node) {
	for(int i = 0; i < this->neighbors.length(); ++i) {
		if(node == this->neighbors[i]) {
			this->neighbors.erase(this->neighbors.begin() + i);
			return true;
		}
	}
	return false;
}

template<typename T>
std::vector<Node<T>*> Node<T>::getNeighbors() const {
	return this->neighbors;
}

template<typename Key, typename T>
Node<T>* Graph<Key, T>::getKey(int index) { return this->nodes[index]; }

template<typename Key, typename T>
Node<T>* Graph<Key, T>::getNode(Key key) { return this->nodeMap.at(key); }

template<typename Key, typename T>
void Graph<Key, T>::addNode(Node<T>* node, Key key) {
	this->keys.push_back(key);
	this->nodeMap[key] = node;
}

} // namespace gr

#endif // _GRAPH_H
