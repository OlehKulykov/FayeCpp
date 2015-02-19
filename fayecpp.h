/*
 *   Copyright (c) 2014 - 2015 Kulykov Oleh <nonamedemail@gmail.com>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#ifndef __FAYECPP_FAYECPP_H__
#define __FAYECPP_FAYECPP_H__

/*
 *   Faye C++ client main and one header file.
 *   All class interfaces added to namespace, preventing include files mess.
 *
 *   Changes on version 0.1.9 (current):
 *   - Objective-C client wrapper speed optimizations(used only NON-ARC mode and CoreFoundation framework).
 *   - Apple localization bundle fix.
 *
 *   Changes on version 0.1.8:
 *   - Added additional error processing with new Error class.
 *
 *   Changes on version 0.1.7:
 *   - Minor libwebsockets fixes.
 *   - Added error processing of received messages.
 *   - Client transport based on Libwebsockets can automatically self destruct on socket error.
 *
 *   Changes on version 0.1.6:
 *   - Added extra(ext) message field included in any Bayeux message.
 *      The contents of ext message field may be arbitrary values that allow extensions
 *      to be negotiated and implemented between server and client implementations.
 *      http://docs.cometd.org/2/reference/bayeux_message_fields.html
 *
 *   Changes on version 0.1.5:
 *   - Added secure socket connection support with SSL data source.
 *   - Client transport become logically detached from the client which adds possibility
 *      to control client logic from delegate methods and from another thread.
 *   - Added thread safety to WebSocket transport based on libwebsockets.
 *   - Added processing of destroying socket context during some inactive time.
 *   - Optimized code of WebSocket transport based on libwebsockets.
 *   - Removed unused classes RETime, REThread and REMutex.
 *   - Redused size of code and library size thanks to previous two punkts.
 *   - Added client library build information.
 *   - Build results(binary library/framework & headers) from continuous integration systems now stores
 *      and available on GitHub release page: https://github.com/OlehKulykov/FayeCpp/releases
 *
 *   Changes on version 0.1.4:
 *   - Possibility to switch client between IPV4 & IPV6 if possible.
 * 
 *   Changes on version 0.1.3:
 *   - Added processing large received binary and text frames in case using 'libwebsockets'.
 *
 *   Changes on version 0.1.2:
 *   - Added autoreconnect to the client while disconnect with unknown error(not by user).
 *   - Added to cocoapods repository.
 *
 *   Version 0.1.1:
 *   - Implemented basic logic.
 */


#define FAYECPP_VERSION_MAJOR 0
#define FAYECPP_VERSION_MINOR 1
#define FAYECPP_VERSION_PATCH 9


#if !defined(HAVE_SUITABLE_QT_VERSION) 

/* Try to check Qt version, should be more or equal than 5.3.0 */
#if defined(QT_VERSION) && defined(QT_VERSION_CHECK)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
#define HAVE_SUITABLE_QT_VERSION 1
#endif

#endif /* End check Qt version */
#endif /* End check exestance of Qt suitable version */


#if !defined(__RE_OS_WINDOWS__) && !defined(__RE_OS_ANDROID__)
/* No manualy selected, try to auto select */

#if (defined(WIN32) || defined(_WIN32) || defined(WIN32_LEAN_AND_MEAN) || defined(_WIN64) || defined(WIN64))

#define __RE_OS_WINDOWS__ 1


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN /* Exclude rarely-used stuff from Windows headers */
#endif /* WIN32_LEAN_AND_MEAN */


#endif /* END CHECKING WINDOWS PLATFORM  */
/***********************************************************************************/


#if defined(ANDROID_NDK) || defined(__ANDROID__) || defined(ANDROID)

#define __RE_OS_ANDROID__ 1

#endif /* END CHECKING ANDROID PLATFORM */
/***********************************************************************************/

#endif


#if defined(TARGET_OS_MAC) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(__APPLE__) 
#ifndef __APPLE__
#define __APPLE__ 1
#endif
#endif


#if defined(__cplusplus) || defined(_cplusplus)
#define __RE_EXTERN__ extern "C"
#else
#define __RE_EXTERN__ extern
#endif


#if defined(__RE_OS_WINDOWS__) && !defined(HAVE_SUITABLE_QT_VERSION)
#include <windows.h>

#if defined(CMAKE_BUILD) || defined(__BUILDING_RECORE_DYNAMIC_LIBRARY__)
#	if defined(_MSC_VER)
#		define __RE_PUBLIC_CLASS_API__ __declspec(dllexport)
#		define __RE_EXPORT__ __RE_EXTERN__ __declspec(dllexport) 
#	elif defined(__GNUC__)
#		define __RE_PUBLIC_CLASS_API__ __attribute__((dllexport))
#		define __RE_EXPORT__ __RE_EXTERN__ __attribute__((dllexport)) 
#	endif
#else
#	if defined(_MSC_VER)
#		define __RE_PUBLIC_CLASS_API__ __declspec(dllimport)
#		define __RE_EXPORT__ __RE_EXTERN__ __declspec(dllimport) 
#	elif defined(__GNUC__)
#		define __RE_PUBLIC_CLASS_API__ __attribute__((dllimport))
#		define __RE_EXPORT__ __RE_EXTERN__ __attribute__((dllimport)) 
#	endif
#endif

#endif /* __RE_OS_WINDOWS__ */


#if defined(__GNUC__) 
#	if __GNUC__ >= 4
#		define __RE_PUBLIC_CLASS_API__ __attribute__ ((visibility("default")))
#	endif
#endif


#ifndef __RE_EXPORT__
#define __RE_EXPORT__ __RE_EXTERN__
#endif


#ifndef __RE_PUBLIC_CLASS_API__
#define __RE_PUBLIC_CLASS_API__
#endif


/* Standart C Library headers */ 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <time.h>
#include <wchar.h>


/* C++ Standard Library header */
#ifndef __RE_OS_ANDROID__
#include <iostream>
#endif


#if defined(__RE_OS_ANDROID__)
#include <setjmp.h>
#include <ctype.h>
#include <cerrno>
#include <cstddef>

#include <sys/types.h>
#include <sys/errno.h>
#endif


#if defined(HAVE_ASSERT_H)
#include <assert.h>
#endif


/**
 @brief 8 bit unsigned byte type.
 @detailed 8 bit unsigned byte with 1 byte size.
 */
typedef uint8_t REUByte;


/**
 @brief 16 bit unsigned integer type.
 @detailed 16 bit unsigned integer with 2 byte size.
 */
typedef uint16_t REUInt16;


/**
 @brief 32 bit unsigned integer type.
 @detailed 32 bit unsigned integer with 4 byte size.
 */
typedef uint32_t REUInt32;


/**
 @brief 64 bit unsigned integer type.
 @detailed 64 bit unsigned integer with 8 byte size.
 */
typedef uint64_t REUInt64;


/**
 @brief 8 bit signed byte type.
 @detailed 8 bit signed byte with 1 byte size.
 */
typedef int8_t REByte;


/**
 @brief 16 bit signed integer type.
 @detailed 16 bit signed integer with 2 byte size.
 */
typedef int16_t REInt16;


/**
 @brief 32 bit signed integer type.
 @detailed 32 bit signed integer with 4 byte size.
 */
typedef int32_t REInt32;


/**
 @brief 64 bit signed integer type.
 @detailed 64 bit signed integer with 8 byte size.
 */
typedef int64_t REInt64;


/**
 @brief Unsigned identifier integer type.
 @detailed unsigned identifier integer type size is same as pointer.
 */
typedef uintptr_t REUIdentifier;


/**
 @brief Boolean type.
 @detailed Boolean type with possible values: 'true' or 'false'
 */
typedef bool REBOOL;



#if defined(SIZEOF_FLOAT_T)
/**
 @brief 32 bit float type.
 @detailed 32 bit float type with 4 byte size.
 */
typedef float_t REFloat32;
#else
/**
 @brief 32 bit float type.
 @detailed 32 bit float type with 4 byte size.
 */
typedef float REFloat32;
#endif


#if defined(SIZEOF_DOUBLE_T)
/**
 @brief 64 bit float type.
 @detailed 64 bit float type with 8 byte size.
 */
typedef double_t REFloat64;
#else
/**
 @brief 64 bit float type.
 @detailed 64 bit float type with 8 byte size.
 */
typedef double REFloat64;
#endif


/**
 @brief Time interval type.
 @detailed Time interval type with 8 byte size. Used for holding seconds with fractional part
 */
typedef REFloat64 RETimeInterval;


/**
 @brief Define for NULL pointer.
 */
#ifndef NULL
#define NULL 0
#endif


/**
 @brief Define for 32 bit unsigned integer maximum value.
 */
#ifndef UINT32_MAX
#define UINT32_MAX (4294967295u)
#endif


/**
 @brief Define for 32 bit unsigned integer maximum value.
 */
#ifndef REUInt32Max
#define REUInt32Max UINT32_MAX
#endif


/**
 @brief Define for 16 bit unsigned integer maximum value.
 */
#ifndef USHRT_MAX
#define USHRT_MAX (65535u)
#endif


/**
 @brief Define for 16 bit unsigned integer maximum value.
 */
#ifndef REUInt16Max
#define REUInt16Max USHRT_MAX
#endif


/**
 @brief Define for default signed integer(int) maximum value.
 */
#ifndef INT_MAX
#define INT_MAX (2147483647)
#endif


/**
 @brief Define for default signed integer(int) minimum value.
 */
#ifndef INT_MIN
#define INT_MIN (-2147483648)
#endif


/**
 @brief Define for indicating that something can't be found or undefined.
 */
#define RENotFound REUInt32Max


/**
 @brief Define for indicating that index can't be found.
 */
#define REIndexNotFound REUInt32Max


#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif


#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

namespace FayeCpp {

#if defined(__APPLE__)
	/**
	 @brief Name of the bundle contained localization for faye client.
	 */
	__RE_EXPORT__ const char * const kFayeCppBundleName;


	/**
	 @brief Localization table name
	 */
	__RE_EXPORT__ const char * const kFayeCppBundleLocalizationTableName;
#endif

	class Client;
	class Responce;
	class REVariantList;
	class REVariantMap;
	class REVariant;


	/**
	 @brief Class template of autopointer.
	 @detailed Holds created pointer and delete when it's need, usually when no referecnces to pointer.
	*/
	template <typename PointerType>
	class __RE_PUBLIC_CLASS_API__ REPtr
	{
	private:
		PointerType* _object;
		REInt32* _referenceCount;
		
		void retain()
		{
			if (_referenceCount)
			{
				*_referenceCount = (*_referenceCount) + 1;
			}
		}
		
		void deleteObject()
		{
			if (_object)
			{
				delete _object;
			}
			_object = NULL;
		}
		
	public:
		/**
		 @brief Check pointer is NULL.
		 @return True if pointer is NULL, othervice false;
		 */
		REBOOL isEmpty() const
		{
			return (_object == NULL);
		}
		

		/**
		 @brief Check pointer is not NULL.
		 @return True if not NULL, othervice false.
		 */
		REBOOL isNotEmpty() const
		{
			return (_object != NULL);
		}
		

		/**
		 @brief Check this class is only one woner of the pointer.
		 @return True is only one owner, othervice false.
		 */
		REBOOL isSingleOwner() const
		{
			return _referenceCount ? ((*_referenceCount) <= 1) : false;
		}
		

		/**
		 @brief Releases holded pointer.
		 @detailed Decrement reference counter and if there is not references delete's pointer.
		 */
		void release()
		{
			if (_referenceCount)
			{
				*_referenceCount = (*_referenceCount) - 1;
				if ((*_referenceCount) <= 0)
				{
					this->deleteObject();
					
					free(_referenceCount);
					_referenceCount = NULL;
				}
			}
			
			_object = NULL;
			_referenceCount = NULL;
		}
		

		/**
		 @brief Assing operator.
		 @detailed Releases prev. pointer reference, hold another pointer and increments it's reference.
		 @param anotherPtr Another autopointer object.
		 @return Address of this autopointer object.
		 */
		REPtr<PointerType> & operator=(const REPtr<PointerType> & anotherPtr)
		{
			if (this != &anotherPtr)
			{
				this->release();
				if (anotherPtr.isNotEmpty())
				{
					_object = anotherPtr._object;
					_referenceCount = anotherPtr._referenceCount;
					this->retain();
				}
			}
			return (*this);
		}

		
		/**
		 @brief Structure dereference operator.
		 @return Address(pointer to pointer) to holded object.
		 */
		PointerType* operator->()
		{
			return _object;
		}
		

		/**
		 @brief Const variant of the structure dereference operator.
		 @return Const address(pointer to pointer) to holded object.
		 */
		const PointerType* operator->() const
		{
			return _object;
		}
		

		/**
		 @brief Indirection operator.
		 @return Returns object pointer.
		 */
		operator PointerType* ()
		{
			return _object;
		}
		

		/**
		 @brief Const version of indirection operator.
		 @return Returns object const pointer.
		 */
		operator const PointerType* () const
		{
			return _object;
		}
		

		/**
		 @brief Default contructor.
		 @detailed Contructs autopointer without any object pointer and reference.
		 */
		REPtr() :
			_object(NULL),
			_referenceCount(NULL)
		{
			
		}
		

		/**
		 @brief Contructor with typed pointer to object.
		 @detailed If pointer is not NULL then holds pointer and create reference to it.
		 @param object Pointer to typed object.
		 */
		REPtr(PointerType* object) :
			_object(object),
			_referenceCount(NULL)
		{
			if (_object)
			{
				REInt32 * count = (REInt32 *)malloc(sizeof(REInt32 *));
#if defined(HAVE_ASSERT_H)
				assert(count);
#endif				
				if (count)
				{
					*count = 1;
					_referenceCount = count;
				}
				else
				{
					_object = NULL;
				}
			}
		}
		

		/**
		 @brief Contructs autopointer with with another autopointer object.
		 @detailed If another autopointer is not empty(holds pointer) than increment reference, othervice created empty autopointer object.
		 @param anotherPtr Another autopointer object.
		 */
		REPtr(const REPtr<PointerType> & anotherPtr) :
			_object(NULL),
			_referenceCount(NULL)
		{
			if (this != &anotherPtr)
			{
				if (anotherPtr.isNotEmpty())
				{
					_object = anotherPtr._object;
					_referenceCount = anotherPtr._referenceCount;
					this->retain();
				}
			}
		}
		

		/**
		 @brief Default destructor.
		 @detailed Releases holded pointer if available.
		*/
		~REPtr()
		{
			this->release();
		}
	};
	

	/**
	 @brief Function template for casting objects using "static_cast".
	 */
	template <typename resultType, typename sourceType>
	static resultType* REPtrCast(sourceType* sourcePointer)
	{
		return static_cast<resultType*>( static_cast<void*>(sourcePointer) );
	}
	

	/**
	 @brief Const version of function template for casting objects using "static_cast".
	 */
	template <typename resultType, typename sourceType>
	static const resultType* REPtrCast(const sourceType* sourcePointer)
	{
		return static_cast<const resultType*>( static_cast<const void*>(sourcePointer) );
	}
	

	/**
	 @brief Class template for list.
	 */
	template <typename T>
	class __RE_PUBLIC_CLASS_API__ REList
	{
	public:
		/**
		 @brief Class of list node.
		 */
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
		/**
		 @brief Class of list node.
		 @detailed Holds value and pointer to prev. & next nodes.
		 */
		class Node : public NodeBase
		{
		public:
			T value;
			Node(const T & newValue) : NodeBase(), value(newValue) { }
		};
		

		/**
		 @brief Type of list values comparation results.
		 */
		typedef enum
		{
			/**
			 @brief Left operand is lest then right.
			 */
			Descending = -1,


			/**
			 @brief Left and right operands are the same.
			 */
			Same = 0,


			/**
			 @brief Left operand is greater then right.
			 */
			Ascending = 1
		}
		/**
		 @brief Type of list values comparation results.
		 */
		ValueCompareResult;
		

		/**
		 @brief Define pointer to list node.
		 */
		typedef Node * NodePtr;


		/**
		 @brief Define node creator callback function with defaul value.
		 @param newValue Const address to new node value.
		 */
		typedef NodePtr (*CreateNodeCallback)(const T & newValue);


		/**
		 @brief Define node destructor callback function with node pointer.
		 @param Pointer to list node.
		 */
		typedef void (*ReleaseNodeCallback)(NodePtr);


		/**
		 @brief Define node values comparator callback function.
		 @param left Pointer to left value.
		 @param right Pointer to right value.
		 */
		typedef ValueCompareResult (*NodeValuesComparator)(const T * left, const T * right);


		/**
		 @brief Define node values comparator callback function.
		 @param left Pointer to left value.
		 @param right Pointer to custom right value.
		 */
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
		/**
		 @brief Default node creator static method callback.
		 @detailed Create node with "new" operator.
		 @param newValue Default node value.
		 @return Pointer to created node with value.
		 */
		static NodePtr newNode(const T & newValue)
		{
			return (new Node(newValue));
		}
		

		/**
		 @brief Default node destructor static method callback.
		 @detailed Release node with "delete" operator.
		 @param node Pointer to node.
		 */
		static void deleteNode(NodePtr node)
		{
			delete node;
		}
		

		/**
		 @brief Node creator static method callback.
		 @detailed Allocate node with "malloc" function.
		 @param newValue Default node value.
		 @return Pointer to allocated node with value.
		 */
		static NodePtr allocateNode(const T & newValue)
		{
			NodePtr node = (NodePtr)malloc(sizeof(Node));
			if (node)
			{
				node->value = newValue;
			}
			return node;
		}
		

		/**
		 @brief Node destructor static method callback.
		 @detailed Release node with "free" function.
		 @param node Pointer to node.
		 */
		static void freeNode(NodePtr node)
		{
			free(node);
		}
		
	public:
		/**
		 @brief Iterator class for list values.
		 */
		class Iterator
		{
		private:
			Node * _head;
			Node * _node;
		public:
			/**
			 * @brief Move to next list node.
			 * @return True if moved next, othervice false.
			 */
			bool next()
			{
				_node = _node ? _node->next : this->_head->next;
				return _node != this->_head;
			}
			

			/**
			 @brief Getter for node pointer.
			 @return Pointer to current list node.
			 */
			Node * node() const
			{
				return _node;
			}


			/**
			 @brief Getter for current node value.
			 @return Const address for current value.
			 */
			const T & value() const
			{
				return _node->value;
			}
			

			/**
			 @brief Constructs iterator with another iterator.
			 @param it Another iterator.
			 */
			Iterator(const Iterator & it) :
				_head(it._head),
				_node(NULL)
			{
				
			}
			

			/**
			 @brief Constructs iterator with pointer to list head node.
			 @detailed Need for internal list implementation.
			 @param listHead Pointer to list head node.
			 */
			Iterator(Node * listHead) :
				_head(listHead),
				_node(NULL)
			{
				
			}
		};
		

		/**
		 @brief Creates new list iterator object.
		 @return New iterator.
		 */
		Iterator iterator() const
		{
			return Iterator(this->_castHead);
		}
		

		/**
		 @brief Check list containes any values.
		 @return True if there is no values, othervice false.
		 */
		bool isEmpty() const
		{
			return (this->_head->next == this->_head);
		}
		

		/**
		 @brief Removes all list values.
		 */
		void clear()
		{
			Node * node = this->_head->next;
			while (node != this->_head)
			{
				node = this->removeNode(node);
			}
		}
		

		/**
		 @brief Locate node with same value described as void pointer.
		 @param customValue Void pointer to searched value.
		 @param comparator Values comparator callback
		 @return Pointer to node contained same value or NULL if there is such values in list.
		 */
		Node * findNode(void * customValue, CustomNodeValueComparator comparator) const
		{
			Node * next = this->_head->next;
			while (next != this->_head)
			{
				if (comparator(&next->value, customValue) == Same)
				{
					return next;
				}
				next = next->next;
			}
			return NULL;
		}
		

		/**
		 @brief Locate node with same value as parameter.
		 @param value The value to be located in list.
		 @param comparator Conparator callback for comparing 2 values.
		 @return Pointer to node contained same value or NULL if there is such values in list.
		 */
		Node * findNode(const T & value, NodeValuesComparator comparator) const
		{
			Node * next = this->_head->next;
			while (next != this->_head)
			{
				if (comparator(&next->value, &value) == Same)
				{
					return next;
				}
				next = next->next;
			}
			return NULL;
		}
		

		/**
		 @brief Locate node with same value as parameter.
		 @param value The value to be located in list. For comparing values used "==" operator.
		 @return Pointer to node contained same value or NULL if there is such values in list.
		 */
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
		

		/**
		 @brief Check is list containes some value.
		 @param value The value to be finded in list.
		 @return True if such value containes in list, othervice false.
		 */
		bool isContaines(const T & value) const
		{
			return this->findNode(value) ? true : false;
		}
		

		/**
		 @brief Removes node from list. Can be removed during iterating.
		 @param node The node pointer.
		 @return Pointer to the next node.
		 */
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
		

		/**
		 @brief Adds value to the list.
		 @param newValue The new value to add.
		 @return True if value was added, othervice false.
		 */
		bool add(const T & newValue)
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
		
		/**
		 @brief Construct list object
		 @param nodeCreator Node creator callback, if no assigned will use default callback(create node with "new" operator).
		 @param nodeReleaser Node destructor callback, if no assigned will use default callback(delete node with "delete" operator).
		 */
		REList(CreateNodeCallback nodeCreator = &newNode,
			   ReleaseNodeCallback nodeReleaser = &deleteNode) :
			_createNode(nodeCreator),
			_releaseNode(nodeReleaser),
			_head(NULL)
		{
			NodeBase * newHead = (NodeBase *)malloc(sizeof(NodeBase));
			if (newHead)
			{
				this->_head = newHead;
				this->_head->next = this->_castHead;
				this->_head->previous = this->_castHead;
			}
		}
		

		/**
		 * @brief Default virtual desctructor.
		 */
		virtual ~REList()
		{
			this->clear();
			if (this->_head)
			{
				free(this->_head);
			}
		}
	};
	

	/**
	 @brief Map class template.
	 */
	template <typename TK, typename TV>
	class __RE_PUBLIC_CLASS_API__ REMap
	{
	public:
		/**
		 @brief Class of map node.
		 */
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
		/**
		 @brief Map node class. Containes key and value object.
		 */
		class Node : public NodeBase
		{
		public:
			TK key;
			TV value;
			Node(const TK & newKey, const TV & newValue) : NodeBase(), key(newKey), value(newValue) { }
		};
		

		/**
		 @brief Node pointer type.
		 */
		typedef Node * NodePtr;


		/**
		 @brief Node creator callback type. With key and value.
		 @param Key value.
		 @param Value.
		 */
		typedef NodePtr (*CreateNodeCallback)(const TK & newKey, const TV & newValue);


		/**
		 @brief Release node callback type.
		 @param Node pointer.
		 */
		typedef void (*ReleaseNodeCallback)(NodePtr);
	private:
		CreateNodeCallback _createNode;
		ReleaseNodeCallback _releaseNode;
		union
		{
			NodeBase * _head;
			Node * _castHead;
		};
	protected:
		Node * addNewNodeWithKeyValue(const TK & newKey, const TV & newValue)
		{
			Node * newNode = _createNode(newKey, newValue);
			if (newNode)
			{
				newNode->previous = _head->previous;
				newNode->previous->next = newNode;
				_head->previous = newNode;
				newNode->next = _castHead;
			}
			return newNode;
		}
	public:
		/**
		 @brief Default node creator callback.
		 @detailed Create node with "new" operator.
		 @param newKey Node key.
		 @param newValue Node value.
		 @return Pointer to created node.
		 */
		static NodePtr newNode(const TK & newKey, const TV & newValue)
		{
			return (new Node(newKey, newValue));
		}
		

		/**
		 @brief Default node release callback.
		 @detailed Release node with "delete" operator.
		 @param node Node pointer.
		 */
		static void deleteNode(NodePtr node)
		{
			delete node;
		}
		

		/**
		 @brief Node allocator callback.
		 @detailde Create node with "malloc" function.
		 @param newKey Node key.
		 @param newValue Node value.
		 @return Pointer to created node.
		 */
		static NodePtr allocateNode(const TK & newKey, const TV & newValue)
		{
			NodePtr node = (NodePtr)malloc(sizeof(Node));
			if (node)
			{
				node->key = newKey;
				node->value = newValue;
			}
			return node;
		}
		

		/**
		 @brief Node release callback.
		 @detailed Release node with "free" function.
		 @param node Node pointer.
		 */
		static void freeNode(NodePtr node)
		{
			free(node);
		}
		
	public:
		/**
		 @brief Map iterator class.
		 */
		class Iterator
		{
		private:
			Node * _head;
			Node * _node;
		public:
			/**
			 @brief Move to next map key-pair.
			 @return True if moved to next key-pari, othervice false.
			 */
			bool next()
			{
				_node = _node ? _node->next : this->_head->next;
				return _node != this->_head;
			}
			

			/**
			 @brief Getter for current node pointer.
			 @return Current node pointer.
			 */
			Node * node() const
			{
				return _node;
			}
			

			/**
			 @brief Getter for current node key object.
			 @return Address of the key.
			 */
			const TK & key() const
			{
				return _node->key;
			}
			

			/**
			 @brief Getter for current node value.
			 @return Address of the value.
			 */
			const TV & value() const
			{
				return _node->value;
			}
			

			/**
			 @brief Constructs iterator with another iterator object.
			 @param it Another iterator object.
			 */
			Iterator(const Iterator & it) :
				_head(it._head),
				_node(NULL)
			{
				
			}
			

			/**
			 @brief Contructs iterator node pointer.
			 @param listHead Pointer to node.
			 */
			Iterator(Node * listHead) :
				_head(listHead),
				_node(NULL)
			{
				
			}
		};
		

		/**
		 @brief Creates new iterator object.
		 @return New iterator.
		 */
		Iterator iterator() const
		{
			return Iterator(this->_castHead);
		}

		/**
		 @brief Check is map has no key-pairs.
		 @return True if there is no key-pairs, othervice false.
		 */
		bool isEmpty() const
		{
			return (this->_head->next == this->_head);
		}
		

		/**
		 @brief Removes all key-pairs from map.
		 */
		void clear()
		{
			Node * node = this->_head->next;
			while (node != this->_head)
			{
				node = this->removeNode(node);
			}
		}
		

		/**
		 @brief Find node by key object.
		 @detailed Comparing key objects using "==" operator.
		 @param key The key object.
		 @return Node pointer or NULL if not found.
		 */
		Node * findNode(const TK & key) const
		{
			Node * next = this->_head->next;
			while (next != this->_head)
			{
				if (next->key == key)
				{
					return next;
				}
				next = next->next;
			}
			return NULL;
		}
		

		/**
		 @brief Check map containes key-pair by key object.
		 @detailed Comparing key objects using "==" operator.
		 @param key The key object.
		 @return True if have key-pair with the key, othervice false.
		 */
		bool isContainesKey(const TK & key) const
		{
			return this->findNode(key) ? true : false;
		}
		

		/**
		 @brief Removes node from map and returns next node pointer.
		 @detailed Can be used during iterating.
		 @param node The node pointer for remove.
		 @return Next node pointer.
		 */
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
		

		/**
		 @brief Set new value for key-pair by key or create new key-pair.
		 @param newKey The key.
		 @param newValue The value.
		 @return True if value by key is updated or successfully created and added new key-pair.
		 */
		bool setKeyValue(const TK & newKey, const TV & newValue)
		{
			Node * node = this->findNode(newKey);
			if (node)
			{
				node->value = newValue;
				return true;
			}
			return this->add(newKey, newValue);
		}
		

		/**
		 @brief Add new key-pair to map without checking existance key-pair with specific key object.
		 @param newKey The key.
		 @param newValue The value.
		 @return True if added new key-pair, othervice false.
		 */
		bool add(const TK & newKey, const TV & newValue)
		{
			return this->addNewNodeWithKeyValue(newKey, newValue) ? true : false;
		}
		

		/**
		 @brief Constructs new map.
		 @param nodeCreator Map node creator callback, if no assigned - using default callback with "new" operator.
		 @param nodeReleaser Map node release callback, if no assigned - using default callback with "new" operator.
		 */
		REMap(CreateNodeCallback nodeCreator = &newNode,
			  ReleaseNodeCallback nodeReleaser = &deleteNode) :
			_createNode(nodeCreator),
			_releaseNode(nodeReleaser),
			_head(NULL)
		{
			NodeBase * newHead = (NodeBase *)malloc(sizeof(NodeBase));
			if (newHead)
			{
				this->_head = newHead;
				this->_head->next = this->_castHead;
				this->_head->previous = this->_castHead;
			}
		}
		

		/**
		 @brief Default map virtual desctructor.
		 */
		virtual ~REMap()
		{
			this->clear();
			if (this->_head)
			{
				free(this->_head);
			}
		}
	};
	
	
	/**
	 @brief Class using for logining text messages.
	 */
	class __RE_PUBLIC_CLASS_API__ RELog
	{
	public:
		/**
		 @brief Log message with arguments.
		 @param logString Format scring, same as for printf
		 */
		static void log(const char * logString, ...);
		
		
		/**
		 @brief Log message with arguments list.
		 @param logString Format scring, same as for printf
		 */
		static void logA(const char * logString, va_list arguments);
	};
	
	class REString;
	
	/**
	 @brief Class of memory buffer.
	 */
	class __RE_PUBLIC_CLASS_API__ REBuffer
	{
	protected:
		void * _buff;
		REUInt32 _size;
		
		static void * defaultMalloc(const REUInt32 size);
		static void defaultFree(void * mem);
		
		virtual void * mallocNewMemory(const REUInt32 size);
		virtual void freeMemory(void * mem);
	public:
		/**
		  @brief Checks is buffer equal with another buffer.
		  @return True if this buffer has the same size as another and content is byte-to-byte equal, othervice false.
		  */
		REBOOL isEqualToBuffer(const REBuffer & anotherBuffer) const;
		

		/**
		  @brief Getter for memory buffer data pointer.
		  @return Void pointer to memory buffer or NULL.
		  */
		void * buffer() const;
		

		/**
		  @brief Getter for buffer size.
		  @return Buffer size in bytes.
		  */
		REUInt32 size() const;
		

		/**
		  @brief Resizes memory buffer with new size.
		  @param newSize New buffer size in bytes.
		  @param isCopyPrevData Flag indicates copying previous data during resize.
		  @return true if memory was resized to 'newSize', or false if new memory not created.
		  */
		REBOOL resize(const REUInt32 newSize, REBOOL isCopyPrevData);
		

		/**
		  @brief Clear memory buffer.
		 */
		void clear();


		/**
		  @brief Set new size of memory buffer, copy from inputed buffer data with size to destination(this).
		  @param buff Target memory buffer to set.
		  @param buffSize Target memory buffer size in bytes. This size will be copyed to destination(this).
		  @return True if new buffer with size created and target memory was copyed.
		  */
		REBOOL set(const void * buff, const REUInt32 buffSize);
		

		/**
		  @brief Assign operator with from another buffer.
		  @detailed New memory buffer will be created and memory from 'anotherBuff' will be copyed.
		  @return Address of this instance.
		  */
		REBuffer & operator=(const REBuffer & anotherBuff);
		

		/**
		  @brief Append this memory buffer with another memory.
		  @detailed New memory buffer will be created, cpyed prev memory and appended with new.
		  @param buff Target memory buffer to append.
		  @param buffSize Target memory buffer size in bytes. This size will be appended to destination(this).
		  @return True if new memory created, and appended, othervice false.
		  */
		REBOOL append(const void * buff, const REUInt32 buffSize);
		

		/**
		  @brief Append this memory buffer with another buffer object.
		  @detailed New memory buffer will be created, cpyed prev memory and appended with new.
		  @param anotherBuff Another buffer object to append.
		  @return True if new memory created, and appended, othervice false.
		  */
		REBOOL append(const REBuffer & anotherBuff);
		

		/**
		  @brief Appends with another buffer object.
		  @detailed New memory buffer will be created, cpyed prev memory and appended with new.
		  @param anotherBuff Another buffer object to append.
		  @return True if new memory created, and appended, othervice false.
		  */
		REBuffer & operator+=(const REBuffer & anotherBuff);
		

		/**
		  @brief Constructs buffer object with content from another buffer object.
		  */
		REBuffer(const REBuffer & anotherBuff);
		

		/**
		  @brief Constructs buffer object with content from another buffer with size.
		  */
		REBuffer(const void * buff, const REUInt32 buffSize);
		

		/**
		  @brief Constructs buffer object with memory size.
		  */
		REBuffer(const REUInt32 buffSize);
		

		/**
		  @brief Constructs empty buffer object.
		  */
		REBuffer();
		

		virtual ~REBuffer();
	};
	

	/**
	 @brief Class of buffer which only hold pointer to buffer without copying.
	 */
	class __RE_PUBLIC_CLASS_API__ REBufferNoCopy : public REBuffer
	{
	public:
		typedef void(*FreeOriginalBuff)(void * mem);
		
	private:
		REBufferNoCopy::FreeOriginalBuff _freeOriginalBuff;
		REBOOL _isNeedToFreeOriginalBuff;
		
	protected:
		virtual void freeMemory(void * mem);
		
	public:
		/**
		 @brief Contructs buffer.
		 @param originalBuff Pointer to some buffer with data.
		 @param buffSize Size of buffer.
		 @param freeOriginalBuff Callback for delete buffer pointer, if not assigned than use default which do nothing.
		 */
		REBufferNoCopy(const void * originalBuff, const REUInt32 buffSize, REBufferNoCopy::FreeOriginalBuff freeOriginalBuff = REBuffer::defaultFree);
		

		/**
		 @brief Default vitual destructor.
		 */
		virtual ~REBufferNoCopy();
	};
	

	/**
	 @brief Type of the string.
	 */
	typedef enum _reStringType
	{
		/**
		 @brief UTF8 type. Using "char" as type.
		 */
		REStringTypeUTF8 = 0,


		/**
		 @brief Wide type. Using "whar_t" as type.
		 */
		REStringTypeWide = 1
	}
	/**
	 @brief Type of the string.
	 */
	REStringType;
	

	/**
	 @brief Base string class. Holds autopointer to string data buffer.
	 @detailed During data assigment strings can be converted between UTF8 and wide char presentation.
	 */
	class __RE_PUBLIC_CLASS_API__ REStringBase
	{
	protected:
		REPtr<REBuffer> _p;
		
		void * stringBuffer() const;
		REUInt32 stringBufferSize() const;
	public:
		/**
		 @brief Check is string empty.
		 @return True if string is empty, othervice false.
		 */
		REBOOL isEmpty() const;


		/**
		 @brief Check is string not empty.
		 @return True if string not empty(has characters), othervice false.
		 */
		REBOOL isNotEmpty() const;


		/**
		 @brief Clears string. Releases string buffer.
		 */
		void clear();
		

		/**
		 @brief Set string data with UTF8 string.
		 @param utf8String The UTF8 string.
		 @param utf8StringLength Length of the UTF8 string, don't include NULL terminated char. This is not count of the UTF8 characters.
		 If not assigned than lenght will be calculated with "strlen" function.
		 @param toType Type of the result string buffer type. If not assigned using UTF8 type by default.
		 */
		void setFromUTF8String(const char * utf8String,
							   const REUInt32 utf8StringLength = RENotFound,
							   const REStringType toType = REStringTypeUTF8);
		

		/**
		 @brief Set string data with wide string.
		 @param wideString The wide string.
		 @param wideStringLength Length of the wide string, don't include NULL terminated char.
		 @param toType Type of the result string buffer type. If not assigned using UTF8 type by default.
		 */
		void setFromWideString(const wchar_t * wideString,
							   const REUInt32 wideStringLength = RENotFound,
							   const REStringType toType = REStringTypeUTF8);
		
		/**
		 @brief Compare byte-to-byte receiver string buffer with another string buffer.
		 @param s Another string.
		 @return True if both string buffers are byte-to-byte equal.
		 */
		bool operator==(const REStringBase & s) const;
		

		/**
		 @brief Construct empty string object.
		 */
		REStringBase();
		

		/**
		 @brief Constructs string object with UTF8 string.
		 @param utf8String The UTF8String.
		 @param utf8StringLength Length of the UTF8 string, don't include NULL terminated char. This is not count of the UTF8 characters.
		 @param toType Type of the result string buffer type. If not assigned using UTF8 type by default
		 */
		REStringBase(const char * utf8String,
					 const REUInt32 utf8StringLength = RENotFound,
					 const REStringType toType = REStringTypeUTF8);
		

		/**
		 @brief Constructs string object with wide characters string.
		 @param wideString The wide string.
		 @param wideStringLength Length of the wide string, don't include NULL terminated char.
		 @param toType Type of the result string buffer type. If not assigned using UTF8 type by default.
		 */
		REStringBase(const wchar_t * wideString,
					 const REUInt32 wideStringLength = RENotFound,
					 const REStringType toType = REStringTypeUTF8);
		

		/**
		 @brief Construct string object with autopointer object to string buffer.
		 @detailed Useful for imutable strings basic assigment, just retain buffer without creating new and copy data.
		 @param stringBuffer Autopinter to string buffer.
		 */
		REStringBase(const REPtr<REBuffer> & stringBuffer);
		

		/**
		 @brief Default string desctructor.
		 */
		virtual ~REStringBase();
	};
	

	class REMutableString;
	class REWideString;
	

	/**
	 @brief Class of the immutable string.
	 */
	class __RE_PUBLIC_CLASS_API__ REString : public REStringBase
	{
	public:
		/**
		 @brief Return mutable string object, string buffer will be copyed.
		 @return New mutable string.
		 */
		REMutableString mutableString() const;


		/**
		 @brief Creates and return wide string object. During creating converting to wide string presentation could be.
		 @return Wide string object.
		 */
		REWideString wideString() const;
		

		/**
		 @brief Pointer to UTF8 string.
		 @return Pointer to string or NULL if string is empty.
		 */
		const char * UTF8String() const;


		/**
		 @brief Length of the string.
		 @return Length of the string, this is not count of the UTF8 characters.
		 */
		REUInt32 length() const;
		

		/**
		 @brief Check is string containes non ASCII symbols.
		 @return True if containes, othervice false.
		 */
		REBOOL isContainsNonASCII() const;
		

		/**
		 @brief Check string containes UTF8 string. Case sensitive.
		 @param utf8String The UTF8 string to search.
		 @return True if containes, othervice false.
		 */
		REBOOL isContaines(const char * utf8String) const;
		

		/**
		 @brief Check string containes wide string. Case sensitive.
		 @detailed BEfore searching wide string will be converted to UTF8 presentation.
		 @param wideString The wide string to search.
		 @return True if containes, othervice false.
		 */
		REBOOL isContaines(const wchar_t * wideString) const;
		

		/**
		 @brief Check is string is digit presentation.
		 @detailed Checked for +/- and dot.
		 @return True if digit, othervice false.
		 */
		REBOOL isDigit() const;
		

		/**
		 @brief Check is string is equal to another string. Case sensitive.
		 @param anotherString Wide string object.
		 @return True is equal, othervice false.
		 */
		REBOOL isEqual(const REWideString & anotherString) const;


		/**
		 @brief Check is string is equal to another string. Case sensitive.
		 @param anotherString String object.
		 @return True is equal, othervice false.
		 */
		REBOOL isEqual(const REString & anotherString) const;


		/**
		 @brief Check is string is equal to another string. Case sensitive.
		 @param anotherString String object.
		 @return True is equal, othervice false.
		 */
		REBOOL isEqual(const REMutableString & anotherString) const;


		/**
		 @brief Check is string is equal to UTF8 string. Case sensitive.
		 @param utf8String The UTF8 string.
		 @param utf8StringLength Length of the UTF8 string, don't include NULL terminated char. This is not count of the UTF8 characters.
		 @return True is equal, othervice false.
		 */
		REBOOL isEqual(const char * utf8String, const REUInt32 utf8StringLength = RENotFound) const;


		/**
		 @brief Check is string is equal to UTF8 string. Case sensitive.
		 @param wideString The wide string.
		 @param wideStringLength Length of the wide string, don't include NULL terminated char.
		 @return True is equal, othervice false.
		 */
		REBOOL isEqual(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound) const;
		

		/**
		 @brief Assign operator with UTF8 string;
		 @param utf8String The UTF8 string.
		 @return Address of this string.
		 */
		REString & operator=(const char * utf8String);


		/**
		 @brief Assign operator with wide string;
		 @param wideString The wide string.
		 @return Address of this string.
		 */
		REString & operator=(const wchar_t * wideString);
		

		/**
		 @brief Assign operator with wide string object;
		 @param anotherString The wide string object.
		 @return Address of this string.
		 */
		REString & operator=(const REWideString & anotherString);


		/**
		 @brief Assign operator with string object;
		 @param anotherString The string object.
		 @return Address of this string.
		 */
		REString & operator=(const REString & anotherString);


		/**
		 @brief Assign operator with mutable string object;
		 @param anotherString The mutable string object.
		 @return Address of this string.
		 */
		REString & operator=(const REMutableString & anotherString);
		

		/**
		 @brief Equal operator.
		 @param s The string object for compare.
		 @return True if equal, othervice false.
		 */
		bool operator==(const REString & s) const;
		

		/**
		 @brief const char * type operator.
		 */
		operator const char* () { return this->UTF8String(); }


		/**
		 @brief const char * type operator.
		 */
		operator const char* () const { return this->UTF8String(); }
		

		/**
		  @brief Path extension.
		  @return String with path extension without dot char.
		 */
		REString pathExtension() const;
		

		/**
		  @brief Try find first integer value from the string.
		  @param isOk Pointer to flag which indicates successfull conversion.
		  @return Integer value or zero.
		 */
		REInt64 integerValue(REBOOL * isOk = NULL) const;
		

		/**
		  @brief Try find first float value from the string.
		  @param isOk Pointer to flag which indicates successfull conversion.
		  @return Float value or zero.
		 */
		REFloat64 floatValue(REBOOL * isOk = NULL) const;
		

		/**
		  @brief Contructs empty string object.
		 */
		REString();


		/**
		  @brief Constructs string object with UTF8 string and/or it's len.
		  @detailed New string buffer will be created.
		  @param utf8String The UTF8 string.
		  @param utf8StringLength Length of the UTF8 string, don't include NULL terminated char. This is not count of the UTF8 characters.
		 */
		REString(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);


		/**
		  @brief Constructs string object with wide string and/or it's len.
		  @detailed New string buffer will be created.
		  @param wideString The wide string.
		  @param wideStringLength Length of the wide string, don't include NULL terminated char.
		 */
		REString(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);


		/**
		  @brief Constructs string object with onother wide string object.
		  @detailed New string buffer will be created and wide string will be converted to UTF8.
		  @param anotherString Wide string object.
		 */
		REString(const REWideString & anotherString);


		/**
		  @brief Constructs string object with onother string object.
		  @detailed String buffer will be retained.
		  @param anotherString The string object.
		 */
		REString(const REString & anotherString);


		/**
		  @brief Constructs string object with mutable string object.
		  @detailed New string buffer will be created.
		  @param anotherString The tring object.
		 */
		REString(const REMutableString & anotherString);
		

		/**
		  @brief Constructs string object with autopointer to UTF8 string buffer.
		  @detailed String buffer will be retained. NULL terminated character should be present at the end.
		  @param utf8StringBuffer The UTF8 string buffer.
		 */
		REString(const REPtr<REBuffer> & utf8StringBuffer);
		

		/**
		  @brief Default virtual destructor.
		 */
		virtual ~REString();
		

		/**
		  @brief Create new formated string.
		  @detailed Max size for the result string is 1024.
		  @param format The format UTF8 string, same as for sprintf functions.
		  @return New string.
		 */
		static REString createWithFormat(const char * format, ...);
	};
	

	/**
	 @brief Immutable string wrapper for holding pointer to UTF8 string without creation new buffer and copying string data.
	 @code
		REStaticString helloString("hello"); // no new string buffer & no copy, just holds pointer to const char * C string.
		// do something with helloString & forgot.
	 @endcode
	 */
	class __RE_PUBLIC_CLASS_API__ REStaticString : public REString
	{
	private:
		static void freeNonCopyBuffMem(void * m);
	public:
		/**
		  @brief Constructs string object with UTF8 string and/or it's len.
		  @detailed No creation new string buffer and no copy.
		  @param utf8String The UTF8 string.
		  @param utf8StringLength Length of the UTF8 string, don't include NULL terminated char. This is not count of the UTF8 characters.
		 */
		REStaticString(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);


		/**
		  @brief Constructs string object with wide string and/or it's len.
		  @detailed In this case - new string buffer will be created.
		  @param wideString The wide string.
		  @param wideStringLength Length of the wide string, don't include NULL terminated char.
		 */
		REStaticString(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);
		
		virtual ~REStaticString();
	};
	
	
	class REMutableString;
	

	/**
	  @brief String class with wide charecters reperesentation.
	 */
	class __RE_PUBLIC_CLASS_API__ REWideString : public REStringBase
	{
	public:
		/**
		  @brief Getter for immutable string with UTF8 string presentation.
		  @return New string object.
		 */
		REString string() const;


		/**
		  @brief Getter for mutable string with UTF8 string presentation.
		  @return New string object.
		 */
		REMutableString mutableString() const;
		

		/**
		  @brief Getter for wide charaters string presentation.
		  @return Pointer to wide chars string.
		 */
		const wchar_t * wideChars() const;


		/**
		  @brief Getter for the number of wide characters.
		  @detailed NULL terminated character not inluded in this number.
		  @return number of wide characters.
		 */
		REUInt32 length() const;
		

		/**
		  @brief Basic assigment operator with UTF8 string.
		  @detailed During assigment all UTF8 chars will be converted to wide.
		  @param utf8String Pointer to UTF8 string.
		  @return Address of this string object.
		 */
		REWideString & operator=(const char * utf8String);


		/**
		  @brief Basic assigment operator with wide string.
		  @detailed No conversion during assigment, chars copyed as is.
		  @param wideString Pointer to wide character string.
		  @return Address of this string object.
		 */
		REWideString & operator=(const wchar_t * wideString);
		

		/**
		  @brief Basic assigment operator with wide string.
		  @detailed No conversion or copying. String buffer will be retained.
		  @param anotherString The wide string object.
		  @return Address of this string object.
		 */
		REWideString & operator=(const REWideString & anotherString);


		/**
		  @brief Basic assigment operator with immutable string.
		  @detailed During assigment all UTF8 chars will be converted to wide and new string buffer will be created.
		  @param anotherString The immutable string object.
		  @return Address of this string object.
		 */
		REWideString & operator=(const REString & anotherString);


		/**
		  @brief Basic assigment operator with mutable string.
		  @detailed During assigment all UTF8 chars will be converted to wide and new string buffer will be created.
		  @param anotherString The mutable string object.
		  @return Address of this string object.
		 */
		REWideString & operator=(const REMutableString & anotherString);
		

		/**
		  @brief Cast operator for wide chars string.
		  @return Pointer to wide chars string.
		 */
		operator const wchar_t* () { return this->wideChars(); }


		/**
		  @brief Cast operator for wide chars string.
		  @return Pointer to wide chars string.
		 */
		operator const wchar_t* () const { return this->wideChars(); }
		

		/**
		  @brief Constructs empty wide string object.
		 */
		REWideString();


		/**
		  @brief Constructs wide string object with UTF8 string.
		  @detailed UTF8 converts to wide and creates new string buffer.
		  @param utf8String The UTF8 string.
		  @param utf8StringLength Length of the UTF8 string, don't include NULL terminated char. This is not count of the UTF8 characters.
		 */
		REWideString(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);


		/**
		  @brief Constructs wide string object with wide string.
		  @detailed No conversion, string copyed to new string buffer as is.
		  @param wideString The wide string.
		  @param wideStringLength Length of the wide string, don't include NULL terminated char.
		 */
		REWideString(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);


		/**
		  @brief Constructs wide string object with onother wide string object.
		  @detailed String buffer will be retained.
		  @param anotherString Wide string object.
		 */
		REWideString(const REWideString & anotherString);


		/**
		  @brief Constructs wide string object with onother immutable UTF8 string object.
		  @detailed Chars converted to wide and new copyed to new string buffer.
		  @param anotherString The immutable UTF8 string object.
		 */
		REWideString(const REString & anotherString);


		/**
		  @brief Constructs wide string object with onother mutable UTF8 string object.
		  @detailed Chars converted to wide and new copyed to new string buffer.
		  @param anotherString The mutable UTF8 string object.
		 */
		REWideString(const REMutableString & anotherString);
		

		/**
		  @brief Constructs wide string object with autopointer to string buffer with wide characters.
		  @detailed NULL terminated char should be included.
		  @param wideStringBuffer autopointer to string buffer with wide characters.
		 */
		REWideString(const REPtr<REBuffer> & wideStringBuffer);
		

		/**
		  @brief Default virtual destructor.
		 */
		virtual ~REWideString();
	};
	
	class REString;
	class REWideString;
	
	
	/**
	 @brief Class of mutable UTF8 string.
	 */
	class __RE_PUBLIC_CLASS_API__ REMutableString : public REString
	{
	protected:
		void replaceWithLen(const char * charsStringValue,
							const char * withCharsStringValue,
							const REUInt32 firstLen,
							const REUInt32 secondLen);
	public:
		/**
		  @brief Basic assignment operator with UTF8 C string.
		  @detailed New string buffer will be created.
		  @param utf8String The UTF8 string.
		  @return Address of this string object.
		 */
		REMutableString & operator=(const char * utf8String);


		/**
		  @brief Basic assignment operator with wide character string.
		  @detailed Wide string will be converted to UTF8 and new string buffer be created.
		  @param wideString The wide string.
		  @return Address of this string object.
		 */
		REMutableString & operator=(const wchar_t * wideString);
		

		/**
		  @brief Basic assignment operator with wide string object.
		  @detailed Wide string will be converted to UTF8 and new string buffer be created.
		  @param anotherString The wide string object.
		  @return Address of this string object.
		 */
		REMutableString & operator=(const REWideString & anotherString);


		/**
		  @brief Basic assignment operator with immutable string object.
		  @detailed New string buffer will be created.
		  @param anotherString The immutable string object.
		  @return Address of this string object.
		 */
		REMutableString & operator=(const REString & anotherString);


		/**
		  @brief Basic assignment operator with mutable string object.
		  @detailed New string buffer will be created.
		  @param anotherString The mutable string object.
		  @return Address of this string object.
		 */
		REMutableString & operator=(const REMutableString & anotherString);
		

		/**
		  @brief Conserts string to it's lower presentation.
		  @return Address of this string object.
		 */
		REMutableString & toLower();
		

		/**
		 @brief Conserts string to it's upper presentation.
		 @return Address of this string object.
		*/
		REMutableString & toUpper();
		

		/**
		  @brief Append this string with another UTF8 C string.
		  @param utf8String The UTF8 string.
		  @param utf8StringLength Length of the UTF8 string, don't include NULL terminated char. This is not count of the UTF8 characters.
		  @return Address of this string object.
		 */
		REMutableString & append(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);
		

		/**
		  @brief Append this string with another wide string.
		  @detailed During appending wide characters will be converted to UTF8 presentation.
		  @param wideString The wide string.
		  @param wideStringLength Length of the wide string, don't include NULL terminated char.
		  @return Address of this string object.
		 */
		REMutableString & append(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);
		

		/**
		  @brief Append this string with another formated UTF8 C string.
		  @param format The format string used for creating appended string.
		  @return Address of this string object.
		 */
		REMutableString & appendFormat(const char * format, ...);
		

		/**
		  @brief Search and replace UTF8 substring with another UTF8 string or with NULL - meant remove substring.
		  @detailed During replacement all subscrings will be replaced.
		  @param utf8String The UTF8 string for searching.
		  @param withUTF8StringOrNULL Replace UTF8 string or NULL. In case of NULL substring will be removed.
		  @return Address of this string object.
		 */
		REMutableString & replace(const char * utf8String, const char * withUTF8StringOrNULL = NULL);

		
		/**
		  @brief Replaces all ocuriences of the char with another char.
		  @param needChar Char for searching.
		  @param targetChar Char for replacing.
		  @return Address of this string object.
		 */
		REMutableString & replace(const char needChar, const char targetChar);
		

		/**
		  @brief Search and replace wide substring with another wide string or with NULL - meant remove substring.
		  @detailed First of all substring & replace string will be converted to it's UTF8 presentation and during replacement all subscrings will be replaced.
		  @param wideString The wide string for searching.
		  @param withWideStringOrNULL Replace wide string or NULL. In case of NULL substring will be removed.
		  @return Address of this string object.
		 */
		REMutableString & replace(const wchar_t * wideString, const wchar_t * withWideStringOrNULL = NULL);
		

		/**
		  @brief Appends this string with path component.
		  @param pComponent Path component for appending. File name or folder name.
		  @return Address of this string object.
		 */
		REMutableString & appendPathComponent(const char * pComponent);
		

		/**
		  @brief Removes last path component. File name or folder name.
		  @return Address of this string object.
		 */
		REMutableString & removeLastPathComponent();
		

		/**
		  @brief Removes path extension from string if last path component is file name(containes dot before slash).
		  @return Address of this string object.
		 */
		REMutableString & removePathExtension();
		

		/**
		  @brief Constructs empty mutable string object.
		 */
		REMutableString();


		/**
		  @brief Constructs mutable string object with UTF8 string.
		  @param utf8String The UTF8 string.
		  @param utf8StringLength Length of the UTF8 string, don't include NULL terminated char. This is not count of the UTF8 characters.
		 */
		REMutableString(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);


		/**
		  @brief Constructs mutable string object with wide characters string.
		  @param wideString The wide string.
		  @param wideStringLength Length of the wide string, don't include NULL terminated char.
		 */
		REMutableString(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);


		/**
		  @brief Constructs mutable string object with wide string object.
		  @param anotherString Wide string object.
		 */
		REMutableString(const REWideString & anotherString);


		/**
		  @brief Constructs mutable string object with immutable string object.
		  @param anotherString Immutable string object.
		 */
		REMutableString(const REString & anotherString);


		/**
		  @brief Constructs mutable string object with mutable string object.
		  @param anotherString Mutable string object.
		 */
		REMutableString(const REMutableString & anotherString);
		

		/**
		  @brief Constructs mutable string object with autopointer to string buffer with UTF8 characters.
		  @detailed NULL terminated char should be included.
		  @param utf8StringBuffer autopointer to string buffer with UTF8 characters.
		 */
		REMutableString(const REPtr<REBuffer> & utf8StringBuffer);
		

		/**
		  @brief Default virtual destructor.
		 */
		virtual ~REMutableString();
	};
	

	/**
	  @brief Subclass of list for holding strings objects.
	 */
	class __RE_PUBLIC_CLASS_API__ REStringList : public REList<REString>
	{
	public:
		/**
		  @brief Basic assignment operator.
		  @detailed Copy all strings to this list.
		  @param list Another string list.
		  @return Address of this list.
		 */
		REStringList & operator=(const REStringList & list);


		/**
		 @brief Append strings list with single string.
		 @param string String for appending.
		 @return Address of this list.
		 */
		REStringList & operator+=(const REString & string);
		
		
		/**
		  @brief Constructs list object with content of another list.
		  @param list
		 */
		REStringList(const REStringList & list);


		/**
		  @brief Constructs empty list object.
		 */
		REStringList();


		/**
		  @brief Default virtual destructor.
		 */
		virtual ~REStringList();
	};
	

	/**
	  @brief Delegate for faye client.
	 */
	class __RE_PUBLIC_CLASS_API__ Delegate
	{
	public:
		/**
		 @brief Client transport protocol connected to server.
		 @param client Faye client object.
		 */
		virtual void onFayeTransportConnected(FayeCpp::Client * client) = 0;
		
		
		/**
		 @brief Client transport protocol disconnected from server.
		 @param client Faye client object.
		 */
		virtual void onFayeTransportDisconnected(FayeCpp::Client * client) = 0;
		
		
		/**
		 @brief Faye client connected to faye server.
		 @param client Faye client object.
		 */
		virtual void onFayeClientConnected(FayeCpp::Client * client) = 0;
		
		
		/**
		 @brief Faye client disconnected to faye server.
		 @param client Faye client object.
		 */
		virtual void onFayeClientDisconnected(FayeCpp::Client * client) = 0;
		
		
		/**
		 @brief Faye clent subscribed to channel.
		 @detailed From "The Bayeux Protocol Specification v1.0" section "Channels"
		 http://docs.cometd.org/reference/bayeux_protocol_elements.html.
		 The channel name consists of an initial "/" followed by an optional sequence of path segments separated by a single slash "/" character. Within a path segment, the character "/" is reserved. 
		 @param client Faye client object.
		 @param channel Channel name.
		 */
		virtual void onFayeClientSubscribedToChannel(FayeCpp::Client * client,
													 const FayeCpp::REString & channel) = 0;
		
		
		/**
		 @brief Faye clent unsubscribed from channel.
		 @detailed From "The Bayeux Protocol Specification v1.0" section "Channels"
		 http://docs.cometd.org/reference/bayeux_protocol_elements.html.
		 The channel name consists of an initial "/" followed by an optional sequence of path segments separated by a single slash "/" character. Within a path segment, the character "/" is reserved. 
		 @param client Faye client object.
		 @param channel Target channel name.
		 */
		virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client,
														 const FayeCpp::REString & channel) = 0;
		
		
		/**
		 @brief Called when faye client received non empty data from server responce using subscribed channel.
		 @detailed From "The Bayeux Protocol Specification v1.0" section "Channels"
		 http://docs.cometd.org/reference/bayeux_protocol_elements.html.
		 The channel name consists of an initial "/" followed by an optional sequence of path segments separated by a single slash "/" character. Within a path segment, the character "/" is reserved. 
		 @param client Faye client object.
		 @param message Received non empty responce message map.
		 @param channel Subscribed channel which received message data.
		 */
		virtual void onFayeClientReceivedMessageFromChannel(FayeCpp::Client * client,
															const FayeCpp::REVariantMap & message,
															const FayeCpp::REString & channel) = 0;
		
		
		/**
		 @brief Called before message will be sended. You can modify this message if needed.
		 @detailed Can be called from other(work) thread.
		 @param client Faye client object.
		 @param message Message map.
		 */
		virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
												 FayeCpp::REVariantMap & message) = 0;
		
		
		/**
		 @brief Called on faye client or transport error.
		 @detailed Use client->lastError() for getting error object.
		 @param client Faye client object.
		 @param errorString Readable error string.
		 */
		virtual void onFayeErrorString(FayeCpp::Client * client,
									   const FayeCpp::REString & errorString) = 0;
		
		
		virtual ~Delegate() { }
	};


	/**
	 @brief SSL data source.
	 */
	class __RE_PUBLIC_CLASS_API__ SSLDataSource
	{
	public:
		/**
		 @brief Get client sertificate file path.
		 @detailed Path to certificate file. Currently supports rsa algorithm & pem encoding format.
		 @code 
		 return REString("/Volumes/Data/faye/client.crt");
		 @endcode
		 @return String with file path or empty string.
		 */
		virtual FayeCpp::REString clientLocalCertificateFilePath() const = 0;
		
		
		/**
		 @brief Get client private key file path.
		 @detailed Path to key file. Currently supports rsa algorithm & pem encoding format.
		 @code 
		 return REString("/Volumes/Data/faye/client.key");
		 @endcode
		 @return String with file path or empty string.
		 */
		virtual FayeCpp::REString clientPrivateKeyFilePath() const = 0;
		
		
		/**
		 @brief Get client private key passphrase. Needs for encrypted client file key.
		 @detailed If client key is encrypted(have '-----BEGIN ENCRYPTED PRIVATE KEY-----'),
		 you should return pass for this key.
		 @return Pass phrase string or empty string.
		 */
		virtual FayeCpp::REString clientPrivateKeyPassPhrase() const = 0;
		
		
		/**
		 @brief Get ca certificate file path.
		 @return String with file path or empty string.
		 */
		virtual FayeCpp::REString clientCACertificateFilePath() const = 0;
		
		
		/**
		 @brief Default virtual destructor.
		 */
		virtual ~SSLDataSource() { }
	};


	class Transport;


	/**
	 @brief Class of variant for storing base types, such as integers, reals, strings, lists and maps.
	 */
	class __RE_PUBLIC_CLASS_API__ REVariant
	{
	public:
		typedef enum _variantType
		{
			/**
			 @brief Variant type is undefined.
			 */
			TypeNone,


			/**
			 @brief Type is signed integer, used 64 bit signed int for storing values.
			 */
			TypeInteger,


			/**
			 @brief Type is unsigned integer, used 64 bit unsigned int for storing values.
			 */
			TypeUnsignedInteger,


			/**
			 @brief Type is real, used double type for storing values.
			 */
			TypeReal,


			/**
			 @brief Type is boolean, used bool type for storing values.
			 */
			TypeBool,


			/**
			 @brief Type is string object.
			 */
			TypeString,


			/**
			 @brief Type is map object.
			 */
			TypeMap,


			/**
			 @brief Type is list object.
			 */
			TypeList
		}
		/**
		 @brief Type of the variant object.
		 */
		VariantType;

	protected:
		typedef union _variantUnion
		{
			int64_t int64Value;
			uint64_t uint64Value;
			double doubleValue;
			bool boolValue;
			void * pointerValue;
		}
		/**
		 @brief Union for storing varian data.
		 */
		VariantUnion;


		/**
		 @brief Union with variant data.
		 */
		VariantUnion _u;


		/**
		 @brief Type of the data.
		 */
		VariantType _t;


		/**
		 @brief Cleans the varint object, releases any alloced value and sets type to TypeNone
		 */
		void clean();

	public:
		/**
		 @brief Checks union pointer for NULL, used for detecting strings, lists and maps.
		 @return True if NULL, othervice false.
		 */
		bool isNULL() const;


		/**
		 @brief Checks variant for number type(integers and reals)
		 @return True if varian is integer or real, othervice false.
		 */
		bool isNumber() const;


		/**
		 @brief Getter for type of the variant.
		 */
		VariantType type() const;


		/**
		 @brief Getter for signed int value.
		 @return Signed int for integers or reals, or 0 for other types.
		 */
		int toInt() const;


		/**
		 @brief Getter for unsigned int value.
		 @return Signed int for unintegers or reals, or 0 for other types.
		 */
		unsigned int toUInt() const;


		/**
		 @brief Getter for signed int 64 bit value.
		 @return 64 bit signed int for integers or reals, or 0 for other types.
		 */
		int64_t toInt64() const;


		/**
		 @brief Getter for unsigned int 64 bit value.
		 @return 64 bit unsigned int for integers or reals, or 0 for other types.
		 */
		uint64_t toUInt64() const;


		/**
		 @brief Getter for double value.
		 @return Double value for integers or reals, or 0 for other types.
		 */
		double toDouble() const;


		/**
		 @brief Getter for boolean value.
		 @return True on type is boolean or any digit value is 1, othervice false.
		 */
		bool toBool() const;


		/**
		 @brief Setter operator for the variant with signed int value.
		 @detailed Type of the variant becomes TypeInteger.
		 @return Address of the variant.
		 */
		REVariant & operator=(int v);


		/**
		 @brief Setter operator for the variant with unsigned int value.
		 @detailed Type of the variant becomes TypeUnsignedInteger.
		 @return Address of the variant.
		 */
		REVariant & operator=(unsigned int v);


		/**
		 @brief Setter operator for the variant with float value.
		 @detailed Type of the variant becomes TypeReal.
		 @return Address of the variant.
		 */
		REVariant & operator=(float v);


		/**
		 @brief Setter operator for the variant with double value.
		 @detailed Type of the variant becomes TypeReal.
		 @return Address of the variant.
		 */
		REVariant & operator=(double v);


		/**
		 @brief Setter operator for the variant with long long value.
		 @detailed Type of the variant becomes TypeInteger.
		 @return Address of the variant.
		 */
		REVariant & operator=(long long v);


		/**
		 @brief Setter operator for the variant with unsigned long long value.
		 @detailed Type of the variant becomes TypeUnsignedInteger.
		 @return Address of the variant.
		 */
		REVariant & operator=(unsigned long long v);


		/**
		 @brief Setter operator for the variant with long double value.
		 @detailed Type of the variant becomes TypeReal and value will be truncated to double value.
		 @return Address of the variant.
		 */
		REVariant & operator=(long double v);


		/**
		 @brief Setter operator for the variant with long double value.
		 @detailed Type of the variant becomes TypeReal.
		 @return Address of the variant.
		 */
		REVariant & operator=(bool v);


		/**
		 @brief Setter operator for the variant string value if not NULL.
		 @detailed Type of the variant becomes TypeString.
		 @return Address of the variant.
		 */
		REVariant & operator=(const REString & s);


		/**
		 @brief Setter operator for the variant C string value if not NULL.
		 @detailed Type of the variant becomes TypeString.
		 @return Address of the variant.
		 */
		REVariant & operator=(const char * s);


		/**
		 @brief Setter operator for the variant wide string value if not NULL.
		 @detailed Type of the variant becomes TypeString.
		 @return Address of the variant.
		 */
		REVariant & operator=(const wchar_t * s);


		/**
		 @brief Setter operator for the variant map value.
		 @detailed Type of the variant becomes TypeMap.
		 @return Address of the variant.
		 */
		REVariant & operator=(const REVariantMap & m);


		/**
		 @brief Setter operator for the variant list value.
		 @detailed Type of the variant becomes TypeList.
		 @return Address of the variant.
		 */
		REVariant & operator=(const REVariantList & l);


		/**
		 @brief Setter operator for the variant value.
		 @detailed Type of the variant becomes same as setted variant.
		 @return Address of the variant.
		 */
		REVariant & operator=(const REVariant & v);


		/**
		 @brief Checks variant is string value.
		 @detailed Checks type for TypeString.
		 @return True if variant is string, othervice false.
		 */
		bool isString() const;


		/**
		 @brief Getter of the string address.
		 @warning Unsequre method. Use this ONLY if variant is string.
		 Check with 'isString()' mathod.
		 @return Const address of the string, or undefined address if not string.
		 */
		const REString & toString() const;


		/**
		 @brief Getter of the string address.
		 @warning Unsequre method. Use this ONLY if variant is string.
		 Check with 'isString()' mathod.
		 @return Address of the string, or undefined address if not string.
		 */
		REString & toString();


		/**
		 @brief Checks variant is map value.
		 @detailed Checks type for TypeMap.
		 @return True if variant is map, othervice false.
		 */
		bool isMap() const;


		/**
		 @brief Getter of the map address.
		 @warning Unsequre method. Use this ONLY if variant is map.
		 Check with 'isMap()' method.
		 @return Const address of the map, or undefined address if not map.
		 */
		const REVariantMap & toMap() const;


		/**
		 @brief Getter of the map address.
		 @warning Unsequre method. Use this ONLY if variant is map.
		 Check with 'isMap()' method.
		 @return Address of the map, or undefined address if not map.
		 */
		REVariantMap & toMap();


		/**
		 @brief Checks variant is list value.
		 @detailed Checks type for TypeList.
		 @return True if variant is list, othervice false.
		 */
		bool isList() const;


		/**
		 @brief Getter of the list address.
		 @warning Unsequre method. Use this ONLY if variant is list.
		 Check with 'isList()' method.
		 @return Const address of the list, or undefined address if not list.
		 */
		const REVariantList & toList() const;


		/**
		 @brief Getter of the list address.
		 @warning Unsequre method. Use this ONLY if variant is list.
		 Check with 'isList()' method.
		 @return Address of the list, or undefined address if not list.
		 */
		REVariantList & toList();


		/**
		 @brief Constructs empty variant object.
		 */
		REVariant();


		/**
		 @brief Constructs variant object with int value.
		 @param v The int value.
		 */
		REVariant(int v);


		/**
		 @brief Constructs variant object with unsigned int value.
		 @param v The unsigned int value.
		 */
		REVariant(unsigned int v);


		/**
		 @brief Constructs variant object with float value.
		 @param v The float value.
		 */
		REVariant(float v);


		/**
		 @brief Constructs variant object with double value.
		 @param v The double value.
		 */
		REVariant(double v);


		/**
		 @brief Constructs variant object with long double value.
		 @param v The long double value.
		 */
		REVariant(long double v);


		/**
		 @brief Constructs variant object with long long value.
		 @param v The long long value.
		 */
		REVariant(long long v);


		/**
		 @brief Constructs variant object with unsigned long long value.
		 @param v The unsigned long long value.
		 */
		REVariant(unsigned long long v);


		/**
		 @brief Constructs variant object with boolean value.
		 @param v The boolean value.
		 */
		REVariant(bool v);


		/**
		 @brief Constructs variant object with C string pointer.
		 @param v The C string pointer.
		 */
		REVariant(const char * v);


		/**
		 @brief Constructs variant object with wide string pointer.
		 @param v The wide string pointer.
		 */
		REVariant(const wchar_t * v);


		/**
		 @brief Constructs variant object with string object.
		 @param v The string object.
		 */
		REVariant(const REString & v);


		/**
		 @brief Constructs variant object with map object.
		 @param v The map object.
		 */
		REVariant(const REVariantMap & v);


		/**
		 @brief Constructs variant object with list object.
		 @param v The list object.
		 */
		REVariant(const REVariantList & v);


		/**
		 @brief Constructs variant object with variant object.
		 @param v The variant object.
		 */
		REVariant(const REVariant & v);


		/**
		 @brief Default virtual destructor.
		 */
		virtual ~REVariant();
	};


	/**
	 @brief Class of the variant map. Keys is string objects and values is variants.
	 */
	class __RE_PUBLIC_CLASS_API__ REVariantMap : public REMap<REString, REVariant>
	{
	public:
		/**
		 @brief Locates pointer of the variant object by C string key with specific variant type of the value.
		 @param key C string of the key.
		 @param type Type of the finded value.
		 @return Pointer of the variant value or NULL if not finded or wrong type.
		 */
		REVariant * findTypedValue(const char * key, const REVariant::VariantType type) const;


		/**
		 @brief Locates pointer of the variant object by wide string key with specific variant type of the value.
		 @param key Wide string of the key.
		 @param type Type of the finded value.
		 @return Pointer of the variant value or NULL if not finded or wrong type.
		 */
		REVariant * findTypedValue(const wchar_t * key, const REVariant::VariantType type) const;


		/**
		 @brief Locates pointer of the variant object by string object key with specific variant type of the value.
		 @param key String object of the key.
		 @param type Type of the finded value.
		 @return Pointer of the variant value or NULL if not finded or wrong type.
		 */
		REVariant * findTypedValue(const REString & key, const REVariant::VariantType type) const;


		/**
		 @brief Getter operator with key.
		 @param key String key.
		 */
		const REVariant operator[](const char * key) const;


		/**
		 @brief Getter operator with key.
		 @param key String key.
		 */
		const REVariant operator[](const wchar_t * key) const;


		/**
		 @brief Getter operator with key.
		 @param key String key.
		 */
		const REVariant operator[](const REString & key) const;


		/**
		 @brief Getter operator with key.
		 @param key String key.
		 */
		REVariant & operator[](const char * key);


		/**
		 @brief Getter operator with key.
		 @param key String key.
		 */
		REVariant & operator[](const wchar_t * key);


		/**
		 @brief Getter operator with key.
		 @param key String key.
		 */
		REVariant & operator[](const REString & key);


		/**
		 @brief Add key/values from another map.
		 @param map Map object.
		 */
		REVariantMap & operator=(const REVariantMap & map);


		/**
		 @brief Contructs map with keys/values from another map.
		 */
		REVariantMap(const REVariantMap & map);


		/**
		 @brief Constructs empty map.
		 */
		REVariantMap();


		/**
		 @brief Default virtual destructor.
		 */
		virtual ~REVariantMap();
	};


	/**
	 @brief Domain for error of the client.
	 @detailed The corresponding value is string object.
	 */
	__RE_EXPORT__ const char * const kErrorDomainClient;


	/**
	 @brief Domain for error of the transport.
	 @detailed The corresponding value is string object.
	 */
	__RE_EXPORT__ const char * const kErrorDomainTransport;


	/**
	 @brief User info key for error localized description.
	 @detailed The corresponding localized description value is string object.
	 */
	__RE_EXPORT__ const char * const kErrorLocalizedDescriptionKey;


	/**
	 @brief User info key for place in the code, file, method, line.
	 @detailed The corresponding value is string object.
	 */
	__RE_EXPORT__ const char * const kErrorPlaceInTheCodeKey;


	/**
	 @brief User info key for url.
	 @detailed The corresponding URL value is string object.
	 */
	__RE_EXPORT__ const char * const kErrorURLKey;


	/**
	 @brief User info key for Bayeux channel.
	 @detailed The corresponding channel value is string object.
	 */
	__RE_EXPORT__ const char * const kErrorChannelKey;


	/**
	 @brief Error object described error reason.
	 */
	class __RE_PUBLIC_CLASS_API__ Error
	{
	private:
		REVariantMap _userInfo;
		REString _domain;
		int _code;

	public:
		/**
		 @brief Error codes used by transport and by the client.
		 @detailed Used int as type and used negative codes out of 16bit range
		 for prevent of duplicates with other potentual system/other libs codes.
		 */
		typedef enum _errorCode
		{
			/**
			 @brief There is no error. Empty error object.
			 */
			None = 0,

			/**
			 @brief Used for tracking unsuccessful object creation, initializations or wrong logic.
			 @detailed User info map contains place in the code where error ocupared.
			 */
			InternalApplicationError = -777000,


			/**
			 @brief Send buffer data extends maximum send size.
			 */
			SendingBufferTooLarge = -777001,


			/**
			 @brief Can't connect to remote host.
			 */
			FailedConnectToHost = -777002,


			/**
			 @brief Handshake error returned from faye server.
			 @detailed Containes in handshake json responce. 
			 In this case used error message provided by server implementation.
			 */
			HandshakeBayeuxError = -777003,


			/**
			 @brief Handshake error: can't find client ID.
			 */
			HandshakeClientIdIsEmpty = -777004,


			/**
			 @brief Handshake error: supported connection types is empty.
			 */
			HandshakeSupportedConnectionTypesIsEmpty = -777005,


			/**
			 @brief Handshake error: implemented transport not found.
			 */
			HandshakeImplementedTransportNotFound = -777006,


			/**
			 @brief Subscription error: can't locate channel.
			 */
			SubscriptionChannelNotFound = -777007,


			/**
			 @brief Subscription error.
			 @detailed Error string can be provided by server implementation or use implemented by the client.
			 */
			SubscriptionError = -777008,


			/**
			 @brief Unsubscription error: can't locate channel.
			 */
			UnsubscriptionChannelNotFound = -777009,


			/**
			 @brief Unsubscription error.
			 @detailed Error string can be provided by server implementation or use implemented by the client.
			 */
			UnsubscriptionError = -777010

		}
		/**
		 @brief Error codes used by transport and by the client.
		 @detailed Used int as type and used negative codes out of 16bit range
		 for prevent of duplicates with other potentual system/other libs codes.
		 */
		ErrorCode;


		/**
		 @brief Check is error exists.
		 @detailed error code should not be None.
		 */
		bool isExists() const;


		/**
		 @brief Cleanup error object.
		 */
		void clear();


		/**
		 @brief Getter for the user info object.
		 @return Address of the user info map. Use strings keys described above.
		 */
		const REVariantMap & userInfo() const;


		/**
		 @brief Domain of the error. Check error from the client of from the transport.
		 @return String with error domain.
		 */
		REString domain() const;


		/**
		 @brief Localized error string described reason.
		 @return String with localized description.
		 */
		REString localizedDescription() const;


		/**
		 @brief Error code.
		 */
		int code() const;


		/**
		 @brief Default copy opertor.
		 @param anotherError The another error object.
		 @return Address of the error object.
		 */
		Error & operator=(const Error & anotherError);


		/**
		 @brief Constructs error with all params.
		 @param domain Error domain.
		 @param code Error code.
		 @param info User info map.
		 */
		Error(const REString & domain, int code, const REVariantMap & info);


		/**
		 @brief Constructs error object with info from another object.
		 @param anotherError Another error object.
		 */
		Error(const Error & anotherError);


		/**
		 @brief Default virtual destructor.
		 */
		virtual ~Error();


		/**
		 @brief Converts error code to localized description or localized format error string.
		 @param code Error code of the error.
		 @return Localized error string or empty string.
		 */
		static REString localizedStringForErrorCode(const ErrorCode code);
	};


	/**
	 @brief Faye clent object.
	 */
	class __RE_PUBLIC_CLASS_API__ Client
	{
	private:
		Transport * _transport;
		Delegate * _delegate;
		SSLDataSource * _sslDataSource;
		Error * _lastError;
		REString _url;
		REString _host;
		REString _path;
		REString _clientId;
		REVariant _extValue;

		REStringList _subscribedChannels;
		REStringList _pendingSubscriptions;
		REStringList _supportedConnectionTypes;

		int _port;
		
		bool _isUseSSL;
		bool _isFayeConnected;
		bool _isDisconnecting;
		bool _isUsingIPV6;

		void processMessage(Responce * responce);
		
		void onTransportConnected();
		void onTransportDisconnected();
		void onTransportWillSelfDestruct();
		
		void onClientResponceMessageReceived(const REVariantMap & message);
		void onClientResponceMessagesListReceived(const REVariantList & messagesList);
		void onClientResponceReceived(Responce * responce);
		
		void onReceivedMessageOnChannel(const REVariantMap & message, const REString & channel);
		
		void onClientError(Responce * responce);
		
		void onHandshakeDone(const REVariantMap & message);
		void handshake();
		
		void onConnectFayeDone(const REVariantMap & message);
		void connectFaye();
		
		void onSubscriptionDone(const REVariantMap & message);
		void subscribePendingSubscriptions();
		
		void onUnsubscribingDone(const REVariantMap & message);
		void onDisconnectFayeDone(const REVariantMap & message);
		
		bool isPendingChannel(const char * channel) const;

		static unsigned int nextMessageId();
		static void parseURL(Client * client);
		
	public:
		/**
		 @brief Last occurred error object address.
		 @detailed This error updates before informing delegate about error.
		 */
		Error * lastError() const;


		/**
		 @brief Constant getter for an ext message field which MAY be included in any Bayeux message.
		 @detailed By default this value is empty(type() is REVariant::TypeNone)
		 and will not included to messages.
		 @return Constant address of the ext message value.
		 */
		const REVariant & extValue() const;


		/**
		 @brief Getter for an ext message field which MAY be included in any Bayeux message.
		 @detailed By default this value is empty(type() is REVariant::TypeNone)
		 and will not included to messages.
		 @return Address of the ext message value.
		 */
		REVariant & extValue();


		/**
		 @brief An ext message field MAY be included in any Bayeux message.
		 @detailed The contents of ext message field may be arbitrary values 
		 that allow extensions to be negotiated and implemented between server and client implementations.
		 If this value is not empty(type not REVariant::TypeNone), than will added to each message.
		 http://docs.cometd.org/2/reference/bayeux_message_fields.html
		 By default this value is empty(type() is REVariant::TypeNone) and will not included to messages.
		 For clean up this value just setExtValue(REVariant());
		 @param value Variant value of the ext field.
		 */
		void setExtValue(const REVariant & value);


		/**
		 @brief Check client should use IPV6.
		 @detailed Default value is false, no matter client supports it or not.
		 To check posibility of using IPV6 by the client use static method 'Client::isSupportsIPV6()'.
		 @return True if client supports & should use IPV6, othervice false.
		 */
		bool isUsingIPV6() const;
		
		
		/**
		 @brief Set client should use IPV6.
		 @detailed During setting checks is client can use it. 
		 Set this value before connecting.
		 @param isUse Flag for using IPV6.
		 @return True if you want to use and client supports it, othervice false.
		 */
		bool setUsingIPV6(bool isUse);
		
		
		/**
		 @detailed From "The Bayeux Protocol Specification v1.0" section "Channels"
		 http://docs.cometd.org/reference/bayeux_protocol_elements.html.
		 The channel name consists of an initial "/" followed by an optional sequence of path segments separated by a single slash "/" character. Within a path segment, the character "/" is reserved. 
		 @return List of subscribed channels.
		 */
		const REStringList & subscribedChannels() const;
		
		
		/**
		 @return List of supported connection names. Based on server responce(handshake) and on implemented types.
		 */
		const REStringList & supportedTransportNames() const;
		
		
		/**
		 @brief Getter for client delegate.
		 @return Pointer to faye client delegate.
		 */
		Delegate * delegate() const;
		
		
		/**
		 @brief Setter for client delegate.
		 */
		void setDelegate(Delegate * delegate);
		
		
		/**
		 @brief Getter for client SSL data source.
		 @return Pointer to faye client SSL data source.
		 */
		SSLDataSource * sslDataSource() const;
		
		
		/**
		 @brief Setter for client SSL data source.
		 */
		void setSSLDataSource(SSLDataSource * dataSource);
		
		
		/**
		 @brief Getter for faye client id. Returned from the server on handshe step.
		 */
		const REString & clientId() const;
		
		
		/**
		 @brief Getter for faye url.
		 */
		const REString & url() const;
		
		
		/**
		 @brief Setter for faye url.
		 @detailed If faye using with Qt: "http" scheme will be replaced with "ws" and "https" - with "wss".
		 @param url Faye server url string.
		 */
		void setUrl(const char * url);
		
		
		/**
		 @brief Returns string with URL host.
		 */
		const REString & host() const;
		
		
		/**
		 @brief URL path string. Should start with '/'
		 */
		const REString & path() const;
		
		
		/**
		 @brief URL port.
		 */
		int port() const;
		
		
		/**
		 @brief Is secure connection. Detects from URL string.
		 */
		bool isUseSSL() const;
		
		
		/**
		 @brief Start connection sequence. On the first step connecting transport protocol to server(inform delegate).
		 @brief Next - connecting faye(also, inform delegate).
		 @return True if sequence initialized, otherwice false.
		 */
		bool connect();
		
		
		/**
		 @brief Check connection of the transport protocol.
		 @return True - connected, otherwice false.
		 */
		bool isTransportConnected() const;
		
		
		/**
		 @brief Check faye client connection.
		 @return True - connected, otherwice false.
		 */
		bool isFayeConnected() const;
		
		
		/**
		 @brief Disconnects from faye server. Transport protocol could be still opened/connected.
		 */
		void disconnect();
		

		/**
		 @brief Check disconnecting phase in progress.
		 @detailed First of all disconnect client and than transport.
		 @return True if faye client and transport are disconnected.
		 */
		bool isDisconnecting() const;
		
		
		/**
		 @brief This is unsequre method which sends text to the transport.
		 @detailed No checking transport for connection to the server.
		 @param text The UTF8 text, should be JSON formated and conforms Bayeux messaging protocol format.
		 @param textLenght Length of the text without NULL terminated character. If textLenght is RENotFound - length will be calculated via strlen() function.
		 @return True if text and transport exists and text sended to transport, otherwice false.
		 */
		bool sendText(const char * text, const REUInt32 textLenght = RENotFound);
		
		
		/**
		 @brief Start send message to subscribed channel via connected faye.
		 @detailed Clent will NOT inform delegate for this user message.
		 From "The Bayeux Protocol Specification v1.0" section "Channels"
		 http://docs.cometd.org/reference/bayeux_protocol_elements.html.
		 The channel name consists of an initial "/" followed by an optional sequence of path segments separated by a single slash "/" character. Within a path segment, the character "/" is reserved. 
		 @param message Non empty message for send.
		 @param channel Non empty, subscribed channel.
		 @return True - if connected and parameters non empty and sended, othervice false.
		 */
		bool sendMessageToChannel(const REVariantMap & message, const char * channel);
		
		
		/**
		 @brief Check channel is subscribed.
		 @detailed From "The Bayeux Protocol Specification v1.0" section "Channels"
		 http://docs.cometd.org/reference/bayeux_protocol_elements.html.
		 The channel name consists of an initial "/" followed by an optional sequence of path segments separated by a single slash "/" character. Within a path segment, the character "/" is reserved. 
		 @param channel Non empty channel.
		 @return True - if subscribed, otherwice false.
		 */
		bool isSubscribedToChannel(const char * channel) const;
		
		
		/**
		  @brief Subscribes or storing to pendnig subscriptions required channel.
		 @detailed From "The Bayeux Protocol Specification v1.0" section "Channels"
		 http://docs.cometd.org/reference/bayeux_protocol_elements.html.
		 The channel name consists of an initial "/" followed by an optional sequence of path segments separated by a single slash "/" character. Within a path segment, the character "/" is reserved. 
		  @param channel Non empty channel.
		  @return True - if already suscribed, started or stored to peding subscriptions, otherwice false.
		 */
		bool subscribeToChannel(const char * channel);
		
		
		/**
		 @brief Unsubscribe from specific channel.
		 @detailed From "The Bayeux Protocol Specification v1.0" section "Channels"
		 http://docs.cometd.org/reference/bayeux_protocol_elements.html.
		 The channel name consists of an initial "/" followed by an optional sequence of path segments separated by a single slash "/" character. Within a path segment, the character "/" is reserved. 
		 @param channel Non empty subscribed channel.
		 @return True on unsubscription started, otherwice false or channel is empty or not subscribed.
		 */
		bool unsubscribeFromChannel(const char * channel);
		
		
		/**
		 @brief Unsubscribe from all subscribed channels. Also cleanup pending subscriptions.
		 */
		void unsubscribeAllChannels();
		
		
		Client();
		virtual ~Client();
		
		
		/**
		  @brief List with implemented/available connection types.
		  @return Type strings list.
		 */
		static REStringList availableConnectionTypes();
		
		
		/**
		 @brief Check is can client use IPV6.
		 @detailed Depends on dependecies build options.
		 @return True if client can use, otherwice false.
		 */
		static bool isSupportsIPV6();
		
		
		/**
		 @brief Check is can client use sequre SSL connection.
		 @detailed Depends on dependecies build options.
		 @return True if client can use, otherwice false.
		 */
		static bool isSupportsSSLConnection();
		
		
		/**
		 @brief Client library information string. Containes build info if available.
		 @return Pointer to the C string with client library information.
		 */
		static const char * info();
	};
	
	
	/**
	 @brief Class of the list containes variants objects.
	 */
	class __RE_PUBLIC_CLASS_API__ REVariantList : public REList<REVariant>
	{
	public:
		/**
		 @brief Add variant to the list with int value.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(int v);
		
		
		/**
		 @brief Add variant to the list with unsigned int value.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(unsigned int v);
		
		
		/**
		 @brief Add variant to the list with float value.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(float v);
		
		
		/**
		 @brief Add variant to the list with double value.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(double v);
		
		
		/**
		 @brief Add variant to the list with long long value.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(long long v);
		
		
		/**
		 @brief Add variant to the list with unsigned long long value.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(unsigned long long v);
		
		
		/**
		 @brief Add variant to the list with long double value.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(long double v);
		
		
		/**
		 @brief Add variant to the list with boolean value.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(bool v);
		
		
		/**
		 @brief Add variant to the list with string object.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(const REString & s);
		
		
		/**
		 @brief Add variant to the list with C string.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(const char * s);
		
		
		/**
		 @brief Add variant to the list with wide string.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(const wchar_t * s);
		
		
		/**
		 @brief Add variant to the list with map object.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(const REVariantMap & m);
		
		
		/**
		 @brief Add variant to the list with list object.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(const REVariantList & l);
		
		
		/**
		 @brief Add variant to the list with varing object.
		 @return Address of this list object.
		 */
		REVariantList & operator+=(const REVariant & v);
		
		
		/**
		 @brief Set variant objects from another list.
		 @return Address of this list object.
		 */
		REVariantList & operator=(const REVariantList & list);
		
		
		/**
		 @brief Constructs list with objects from another list.
		 */
		REVariantList(const REVariantList & list);
		
		
		/**
		 @brief Constructs empty list.
		 */
		REVariantList();
		
		
		/**
		 @brief Default virtual destructor.
		 */
		virtual ~REVariantList();
	};

	
	/**
	 @brief Message class for internal logic communication.
	 */
	class __RE_PUBLIC_CLASS_API__ Responce
	{
	public:
		/**
		 @brief Faye message type.
		 */
		typedef enum _responceType
		{
			/**
			 @brief Undefined, default type.
			 */
			ResponceNone = 0,
			
			
			/**
			 @brief Faye transport protocol connected to server.
			 */
			ResponceTransportConnected,
			
			
			/**
			 @brief Faye transport protocol disconnected from server.
			 */
			ResponceTransportDisconnected,
			
			
			/**
			 @brief Faye transport protocol error.
			 */
			ResponceTransportError,
			
			
			/**
			 @brief Faye transport protocol received message.
			 */
			ResponceMessage,


			/**
			 @brief Faye transport will self destruct
			 */
			ResponceTransportWillSelfDestruct
		}
		/**
		 @brief Faye message type.
		 */
		ResponceType;
		
	private:
		REVariantList * _messageList;
		REVariantMap * _messageMap;
		REBuffer * _messageBuffer;
		Error * _error;
		ResponceType _type;
		
	public:
		/**
		 @brief Get message error string pointer.
		 @return String pointer or NULL.
		 */
		Error * error() const;
		
		
		/**
		 @brief Get message list pointer.
		 @return Variant list pointer or NULL.
		 */
		REVariantList * messageList() const;
		
		
		/**
		 @brief Get message map pointer.
		 @return Variant map pointer or NULL.
		 */
		REVariantMap * messageMap() const;
		
		
		/**
		 @brief Get message buffer.
		 @return Buffer pointer or NULL.
		 */
		REBuffer * messageBuffer() const;
		
		
		/**
		 @brief Get type of responce.
		 @return Responce type.
		 */
		Responce::ResponceType type() const;
		
		
		/**
		 @brief Setter for message responce type.
		 @param type Type of responce.
		 @return Address of this message object.
		 */
		Responce & setType(Responce::ResponceType type);

		
		/**
		 @brief Setter for error object.
		 @param value Error object or NULL.
		 @return Address of this message object.
		 */
		Responce & setError(const Error & error);

		
		/**
		 @brief Setter for message text.
		 @param text C string with message text.
		 @return Address of this message object.
		 */
		Responce & setMessageText(const char * text);
		
		
		/**
		 @brief Setter for message data.
		 @param data Pointer to message data.
		 @param dataSize Size of message data.
		 @return Address of this message object.
		 */
		Responce & setMessageData(const unsigned char * data, const size_t dataSize);
		
		
		/**
		 @brief Contructs typed responce object.
		 @param type Type of the responce.
		 */
		Responce(const Responce::ResponceType type);
		
		
		/**
		 @brief Default contructor for the message.
		 */
		Responce();
		
		
		/**
		 @brief Default destructor for the message.
		 */
		~Responce();
	};
	
	
	/**
	 @brief Section with deprecated types.
	 */
#if defined(__GNUC__)	
	typedef REVariantList VariantList __attribute__((deprecated));
	typedef REVariantMap VariantMap __attribute__((deprecated));
	typedef REVariant Variant __attribute__((deprecated));
#elif defined(_MSC_VER)
	typedef __declspec(deprecated) REVariantList VariantList;
	typedef __declspec(deprecated) REVariantMap VariantMap;
	typedef __declspec(deprecated) REVariant Variant;
#else
	typedef REVariantList VariantList;
	typedef REVariantMap VariantMap;
	typedef REVariant Variant;
#endif
	
}

#endif /* __FAYECPP_FAYECPP_H__ */

