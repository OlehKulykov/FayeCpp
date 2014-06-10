/*
 *   Copyright 2012 - 2013 Kulykov Oleh
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */


#ifndef __RELIST_H__
#define __RELIST_H__

#include "../fayecpp.h"

#include <assert.h>

namespace FayeCpp {
	
	template <typename T>
	class REList
	{
	public:
		class Node;
	private:
		class NodeBase
		{
		public:
			Node * next;
			Node * previous;
			NodeBase() : next(NULL), previous(NULL) { }
		};
		
	public:
		class Node : public NodeBase
		{
		public:
			T value;
			Node(const T & newValue) : NodeBase(), value(newValue) { }
		};
		
		typedef enum
		{
			Descending = -1,
			Same = 0,
			Ascending = 1
		} ValueCompareResult;
		
		typedef Node * NodePtr;
		typedef NodePtr (*CreateNodeCallback)(const T & newValue);
		typedef void (*ReleaseNodeCallback)(NodePtr);
		typedef ValueCompareResult (*NodeValuesComparator)(const T * left, const T * right);
		typedef ValueCompareResult (*CustomNodeValueComparator)(const T * nodeValue, void * customValue);
	private:
		CreateNodeCallback _createNode;
		ReleaseNodeCallback _releaseNode;
		union
		{
			NodeBase * _head;
			Node * _castHead;
		};
		
	public:
		static NodePtr newNode(const T & newValue)
		{
			return (new Node(newValue));
		}
		
		static void deleteNode(NodePtr node)
		{
			delete node;
		}
		
		static NodePtr allocateNode(const T & newValue)
		{
			NodePtr node = (NodePtr)malloc(sizeof(Node));
			if (node)
			{
				node->value = newValue;
			}
			return node;
		}
		
		static void freeNode(NodePtr node)
		{
			free(node);
		}
	public:
		class Iterator
		{
		private:
			Node * _head;
			Node * _node;
		public:
			bool next()
			{
				_node = _node ? _node->next : this->_head->next;
				return _node != this->_head;
			}
			
			//		bool next() const
			//		{
			//			Node ** n = const_cast<Node **>(&_node);
			//			*n = _node ? _node->next : this->_head->next;
			//			return _node != this->_head;
			//		}
			
			Node * node() const
			{
				return _node;
			}
			
			const T & value() const
			{
				return _node->value;
			}
			
			Iterator(const Iterator & it) :
			_head(it._head),
			_node(NULL)
			{
				
			}
			
			Iterator(Node * listHead) :
			_head(listHead),
			_node(NULL)
			{
				
			}
		};
		
		Iterator iterator() const
		{
			return Iterator(this->_castHead);
		}
		
		REBOOL isEmpty() const
		{
			return (this->_head->next == this->_head);
		}
		
		void clear()
		{
			Node * node = this->_head->next;
			while (node != this->_head)
			{
				node = this->removeNode(node);
			}
		}
		
		Node * findNode(void * customValue, CustomNodeValueComparator comparator) const
		{
			Node * next = this->_head->next;
			while (next != this->_head)
			{
				if (comparator(&next->value, customValue) == REList<T>::Same)
				{
					return next;
				}
				next = next->next;
			}
			return NULL;
		}
		
		Node * findNode(const T & value, NodeValuesComparator comparator) const
		{
			Node * next = this->_head->next;
			while (next != this->_head)
			{
				if (comparator(&next->value, &value) == REList<T>::Same)
				{
					return next;
				}
				next = next->next;
			}
			return NULL;
		}
		
		Node * findNode(const T & value) const
		{
			Node * next = this->_head->next;
			while (next != this->_head)
			{
				if (next->value == value)
				{
					return next;
				}
				next = next->next;
			}
			return NULL;
		}
		
		REBOOL isContaines(const T & value)
		{
			return this->findNode(value) ? true : false;
		}
		
		Node * removeNode(Node * node)
		{
			if (node != this->_head)
			{
				Node * next = node->next;
				node->previous->next = node->next;
				node->next->previous = node->previous;
				_releaseNode(node);
				return next;
			}
			return this->_castHead;
		}
		
		REBOOL add(const T & newValue)
		{
			Node * newNode = _createNode(newValue);
			if (newNode)
			{
				newNode->previous = _head->previous;
				newNode->previous->next = newNode;
				_head->previous = newNode;
				newNode->next = _castHead;
				return true;
			}
			return false;
		}
		
		REList(CreateNodeCallback nodeCreator = &REList<T>::newNode,
			   ReleaseNodeCallback nodeReleaser = &REList<T>::deleteNode) :
		_createNode(nodeCreator),
		_releaseNode(nodeReleaser),
		_head(NULL)
		{
			NodeBase * newHead = (NodeBase *)malloc(sizeof(NodeBase));
			assert(newHead);
			
			if (newHead) 
			{
				this->_head = newHead;
				this->_head->next = this->_castHead;
				this->_head->previous = this->_castHead;
			}
		}
		
		~REList()
		{
			this->clear();
			if (this->_head)
			{
				free(this->_head);
			}
		}
	};
	
}

#endif /* __RELIST_H__ */

