/*
 *   Copyright 2014 Kulykov Oleh
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


#ifndef __FAYECPP_FAYECPP_H__
#define __FAYECPP_FAYECPP_H__

#if !defined(__RE_OS_WINDOWS__)
/* No manualy selected, try to auto select */

#if (defined(WIN32) || defined(_WIN32) || defined(WIN32_LEAN_AND_MEAN) || defined(_WIN64) || defined(WIN64))

#define __RE_OS_WINDOWS__ 1


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN /* Exclude rarely-used stuff from Windows headers */
#endif /* WIN32_LEAN_AND_MEAN */


#endif /* END CHECKING WINDOWS PLATFORM  */
/***********************************************************************************/

#endif


#if defined(__RE_OS_WINDOWS__)
#include <windows.h>

#if defined(CMAKE_BUILD) || defined(__BUILDING_RECORE_DYNAMIC_LIBRARY__)
#	if defined(_MSC_VER)
#		define __RE_PUBLIC_CLASS_API__ __declspec(dllexport)
#       define __RE_EXPORT_IMPLEMENTATION_TEMPLATE__
#	elif defined(__GNUC__)
#		define __RE_PUBLIC_CLASS_API__ __attribute__((dllexport))
#	endif
#else
#	if defined(_MSC_VER)
#		define __RE_PUBLIC_CLASS_API__ __declspec(dllimport)
#       define __RE_EXPORT_IMPLEMENTATION_TEMPLATE__ extern
#	elif defined(__GNUC__)
#		define __RE_PUBLIC_CLASS_API__ __attribute__((dllimport))
#	endif
#endif

#endif /* __RE_OS_WINDOWS__ */

#if __GNUC__ >= 4
#	define __RE_PUBLIC_CLASS_API__ __attribute__ ((visibility("default")))
#endif


#ifndef __RE_PUBLIC_CLASS_API__
#define __RE_PUBLIC_CLASS_API__
#endif


#ifndef __RE_EXPORT_IMPLEMENTATION_TEMPLATE__
#define __RE_EXPORT_IMPLEMENTATION_TEMPLATE__
#endif


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <wchar.h>
#include <iostream>

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
 @brief unsigned identifier integer type.
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
#define INT_MIN (−2147483648)
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


#if !defined(HAVE_SUITABLE_QT_VERSION) && defined(QT_VERSION) && defined(QT_VERSION_CHECK)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
#define HAVE_SUITABLE_QT_VERSION 1
#endif

#endif


namespace FayeCpp {
	
    class Client;
    class Message;
	class VariantList;
	class VariantMap;
	class Variant;

	/*
#if defined(__RE_OS_WINDOWS__) && defined(_MSC_VER)
	__RE_EXPORT_IMPLEMENTATION_TEMPLATE__ template class __RE_PUBLIC_CLASS_API__ std::map<std::string, FayeCpp::Variant>;
#endif
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
		REBOOL isEmpty() const
		{
			return (_object == NULL);
		}
		
		REBOOL isNotEmpty() const
		{
			return (_object != NULL);
		}
		
		REBOOL isSingleOwner() const
		{
			return _referenceCount ? ((*_referenceCount) <= 1) : false;
		}
		
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
		
		PointerType* operator->()
		{
			return _object;
		}
		
		const PointerType* operator->() const
		{
			return _object;
		}
		
		operator PointerType* ()
		{
			return _object;
		}
		
		operator const PointerType* () const
		{
			return _object;
		}
		
		REPtr() :
		_object(NULL),
		_referenceCount(NULL)
		{
			
		}
		
		REPtr(PointerType* object) :
		_object(object),
		_referenceCount(NULL)
		{
			if (_object)
			{
				REInt32 * count = (REInt32 *)malloc(sizeof(REInt32 *));
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
		
		~REPtr()
		{
			this->release();
		}
	};
	
	
	template <typename resultType, typename sourceType>
	static resultType* REPtrCast(sourceType* sourcePointer)
	{
		return static_cast<resultType*>( static_cast<void*>(sourcePointer) );
	}
	
	template <typename resultType, typename sourceType>
	static const resultType* REPtrCast(const sourceType* sourcePointer)
	{
		return static_cast<const resultType*>( static_cast<const void*>(sourcePointer) );
	}
	
	template <typename T>
	class __RE_PUBLIC_CLASS_API__ REList
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
		
		bool isEmpty() const
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
				if (comparator(&next->value, customValue) == Same)
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
				if (comparator(&next->value, &value) == Same)
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
		
		bool isContaines(const T & value) const
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
		
		virtual ~REList()
		{
			this->clear();
			if (this->_head)
			{
				free(this->_head);
			}
		}
	};
	
	template <typename TK, typename TV>
	class __RE_PUBLIC_CLASS_API__ REMap
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
			TK key;
			TV value;
			Node(const TK & newKey, const TV & newValue) : NodeBase(), key(newKey), value(newValue) { }
		};
		
		typedef Node * NodePtr;
		typedef NodePtr (*CreateNodeCallback)(const TK & newKey, const TV & newValue);
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
		static NodePtr newNode(const TK & newKey, const TV & newValue)
		{
			return (new Node(newKey, newValue));
		}
		
		static void deleteNode(NodePtr node)
		{
			delete node;
		}
		
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
			
			Node * node() const
			{
				return _node;
			}
			
			const TK & key() const
			{
				return _node->key;
			}
			
			const TV & value() const
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
		
		bool isEmpty() const
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
		
		bool isContainesKey(const TK & key) const
		{
			return this->findNode(key) ? true : false;
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
		
		bool add(const TK & newKey, const TV & newValue)
		{
			return this->addNewNodeWithKeyValue(newKey, newValue) ? true : false;
		}
		
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
		
		virtual ~REMap()
		{
			this->clear();
			if (this->_head)
			{
				free(this->_head);
			}
		}
	};
	
	class REString;
	
	/// Class of memory buffer.
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
		/// Checks is buffer equal with another buffer.
		REBOOL isEqualToBuffer(const REBuffer & anotherBuffer) const;
		
		/// Returns void pointer to memory buffer or NULL.
		void * buffer() const;
		
		/// Returns size of memory byffer.
		REUInt32 size() const;
		
		/// Resizes memory buffer with new size with copying prev data.
		/// Returns true if memory was resized to 'newSize', or false if new memory not created.
		REBOOL resize(const REUInt32 newSize, REBOOL isCopyPrevData);
		
		/// Cleares memory buffer.
		void clear();
		
		/// Set new size of memory buffer, copy from inputed buffer data with size to destination.
		REBOOL set(const void * buff, const REUInt32 buffSize);
		
		/// Setes memory beffer from another buffer.
		REBuffer & operator=(const REBuffer & anotherBuff);
		
		/// Appends new memory with size
		REBOOL append(const void * buff, const REUInt32 buffSize);
		
		/// Appends with another buffer object.
		REBOOL append(const REBuffer & anotherBuff);
		
		/// Appends with another buffer object.
		REBuffer & operator+=(const REBuffer & anotherBuff);
		
		/// This functionality avaiable only with RENetwork and
		/// in config file must be defined __RE_RECORE_CAN_INITIALIZE_FROM_URL_STRING__
		/// Initializing buffer object from URL String.
		/// Example: http://example.com/index for downloading from web.
		/// Example: file:///Volumes/Data/file.txt for reading from from file. See file url scemes.
		REBOOL initFromURLString(const REString & urlString);
		
		/// Constructs buffer object with content from another buffer object.
		REBuffer(const REBuffer & anotherBuff);
		
		/// Constructs buffer object with content from another buffer with size.
		REBuffer(const void * buff, const REUInt32 buffSize);
		
		/// Constructs buffer object with memory size.
		REBuffer(const REUInt32 buffSize);
		
		/// Constructs empty buffer object.
		REBuffer();
		
		virtual ~REBuffer();
	};
	
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
		REBufferNoCopy(const void * originalBuff, const REUInt32 buffSize, REBufferNoCopy::FreeOriginalBuff freeOriginalBuff = REBuffer::defaultFree);
		
		virtual ~REBufferNoCopy();
	};
	
	typedef enum _reStringType
	{
		REStringTypeUTF8 = 0,
		REStringTypeWide = 1
	} REStringType;
	
	class __RE_PUBLIC_CLASS_API__ REStringBase
	{
	protected:
		REPtr<REBuffer> _p;
		
		void * stringBuffer() const;
		REUInt32 stringBufferSize() const;
	public:
		
		REBOOL isEmpty() const;
		REBOOL isNotEmpty() const;
		void clear();
		
		void setFromUTF8String(const char * utf8String,
							   const REUInt32 utf8StringLength = RENotFound,
							   const REStringType toType = REStringTypeUTF8);
		
		void setFromWideString(const wchar_t * wideString,
							   const REUInt32 wideStringLength = RENotFound,
							   const REStringType toType = REStringTypeUTF8);
		
		bool operator==(const REStringBase & s) const;
		
		REStringBase();
		
		REStringBase(const char * utf8String,
					 const REUInt32 utf8StringLength = RENotFound,
					 const REStringType toType = REStringTypeUTF8);
		
		REStringBase(const wchar_t * wideString,
					 const REUInt32 wideStringLength = RENotFound,
					 const REStringType toType = REStringTypeUTF8);
		
		REStringBase(const REPtr<REBuffer> & stringBuffer);
		
		virtual ~REStringBase();
	};
	
	class REMutableString;
	class REWideString;
	
	class __RE_PUBLIC_CLASS_API__ REString : public REStringBase
	{
	public:
		REMutableString mutableString() const;
		REWideString wideString() const;
		
		const char * UTF8String() const;
		REUInt32 length() const;
		
		REBOOL isContainsNonASCII() const;
		
		REBOOL isContaines(const char * utf8String) const;
		
		REBOOL isContaines(const wchar_t * wideString) const;
		
		REBOOL isDigit() const;
		
		REBOOL isEqual(const REWideString & anotherString) const;
		REBOOL isEqual(const REString & anotherString) const;
		REBOOL isEqual(const REMutableString & anotherString) const;
		REBOOL isEqual(const char * utf8String, const REUInt32 utf8StringLength = RENotFound) const;
		REBOOL isEqual(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound) const;
		
		REString & operator=(const char * utf8String);
		REString & operator=(const wchar_t * wideString);
		
		REString & operator=(const REWideString & anotherString);
		REString & operator=(const REString & anotherString);
		REString & operator=(const REMutableString & anotherString);
		
		bool operator==(const REString & s) const;
		
		operator const char* () { return this->UTF8String(); }
		operator const char* () const { return this->UTF8String(); }
		
		/// Returns path extension.
		REString pathExtension() const;
		
		/// Try find first integer value from the string.
		/// If can't find result will be zero.
		REInt64 integerValue(REBOOL * isOk = NULL) const;
		
		/// Try find first integer value from the string.
		/// If can't find result will be zero.
		REFloat64 floatValue(REBOOL * isOk = NULL) const;
		
		REString();
		REString(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);
		REString(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);
		//		REString(const char * utf8String, const RERange & range);
		//		REString(const wchar_t * wideString, const RERange & range);
		REString(const REWideString & anotherString);
		REString(const REString & anotherString);
		REString(const REMutableString & anotherString);
		
		REString(const REPtr<REBuffer> & utf8StringBuffer);
		
		virtual ~REString();
		
		static REString createWithFormat(const char * format, ...);
	};
	
	class __RE_PUBLIC_CLASS_API__ REStaticString : public REString
	{
	private:
		static void freeNonCopyBuffMem(void * m);
	public:
		REStaticString(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);
		REStaticString(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);
		
		virtual ~REStaticString();
	};
	
	class REMutableString;
	
	class __RE_PUBLIC_CLASS_API__ REWideString : public REStringBase
	{
	public:
		REString string() const;
		REMutableString mutableString() const;
		
		const wchar_t * wideChars() const;
		REUInt32 length() const;
		
		REWideString & operator=(const char * utf8String);
		REWideString & operator=(const wchar_t * wideString);
		
		REWideString & operator=(const REWideString & anotherString);
		REWideString & operator=(const REString & anotherString);
		REWideString & operator=(const REMutableString & anotherString);
		
		operator const wchar_t* () { return this->wideChars(); }
		operator const wchar_t* () const { return this->wideChars(); }
		
		REWideString();
		REWideString(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);
		REWideString(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);
		REWideString(const REWideString & anotherString);
		REWideString(const REString & anotherString);
		REWideString(const REMutableString & anotherString);
		
		REWideString(const REPtr<REBuffer> & wideStringBuffer);
		
		virtual ~REWideString();
	};
	
	class REString;
	class REWideString;
	
	class __RE_PUBLIC_CLASS_API__ REMutableString : public REString
	{
	protected:
		void replaceWithLen(const char * charsStringValue, 
							const char * withCharsStringValue, 
							const REUInt32 firstLen, 
							const REUInt32 secondLen);
	public:
		REMutableString & operator=(const char * utf8String);
		REMutableString & operator=(const wchar_t * wideString);
		
		REMutableString & operator=(const REWideString & anotherString);
		REMutableString & operator=(const REString & anotherString);
		REMutableString & operator=(const REMutableString & anotherString);
		
		/// Conserts string ti it's lower presentation.
		REMutableString & toLower();
		
		/// Conserts string ti it's uper presentation.
		REMutableString & toUpper();
		
		/// Appends another UTF8 string.
		REMutableString & append(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);
		
		/// Appends another wide char string.
		REMutableString & append(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);
		
		/// Appends another UTF8 string using format.
		REMutableString & appendFormat(const char * format, ...);
		
		/// Replaces UTF8 string with another UTF8 string.
		REMutableString & replace(const char * utf8String, const char * withUTF8StringOrNULL = NULL);
		
		/// Replaces needed char with another char.
		REMutableString & replace(const char needChar, const char targetChar);
		
		/// Replaces wide chars string with another wide chars string.
		REMutableString & replace(const wchar_t * wideString, const wchar_t * withWideStringOrNULL = NULL);
		
		REMutableString & appendPathComponent(const char * pComponent);
		
		REMutableString & removeLastPathComponent();
		
		REMutableString & removePathExtension();
		
		REMutableString();
		REMutableString(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);
		REMutableString(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);
		REMutableString(const REWideString & anotherString);
		REMutableString(const REString & anotherString);
		REMutableString(const REMutableString & anotherString);
		
		REMutableString(const REPtr<REBuffer> & utf8StringBuffer);
		
		virtual ~REMutableString();
	};
	
	class __RE_PUBLIC_CLASS_API__ REStringList : public REList<REString>
	{
	public:
		REStringList & operator=(const REStringList & list);
		REStringList(const REStringList & list);
		REStringList();
		virtual ~REStringList();
	};
	
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
		 @param client Faye client object.
		 @param channel Channel name.
		 */
		virtual void onFayeClientSubscribedToChannel(FayeCpp::Client * client, 
													 const FayeCpp::REString & channel) = 0;
		
		
		/**
		 @brief Faye clent unsubscribed from channel.
		 @param client Faye client object.
		 @param channel Target channel name.
		 */
		virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client, 
														 const FayeCpp::REString & channel) = 0;
		
		
		/**
		 @brief Called when faye client received non empty data from server responce using subscribed channel.
		 @param client Faye client object.
		 @param data Received non empty responce data.
		 @param channel Subscribed channel which received message data.
		 */
		virtual void onFayeClientReceivedDataFromChannel(FayeCpp::Client * client, 
														 const FayeCpp::REBuffer & data, 
														 const FayeCpp::REString & channel) = 0;
		
		
		/**
		 @brief Called before message will be sended. You can modify this message if needed.
		 @detailed Can be called from other(work) thread.
		 @param client Faye client object.
		 @param message Message object.
		 */
		virtual void onFayeClientWillSendMessage(FayeCpp::Client * client, 
												 FayeCpp::Message * message) = 0;
		
		
		/**
		 @brief Called before message will be processed by the client object.
		 @detailed Can be called from other(work) thread.
		 @param client Faye client object.
		 @param message Message object.
		 */
		virtual void onFayeClientWillReceiveMessage(FayeCpp::Client * client, 
													FayeCpp::Message * message) = 0;
		
		
		/**
		 @brief Called on faye client or transport error.
		 @param client Faye client object.
		 @param errorString Readable error string.
		 */
		virtual void onFayeErrorString(FayeCpp::Client * client, 
									   const FayeCpp::REString & errorString) = 0;
		
		
		virtual ~Delegate() { }
	};
	
    class Transport;
	
	class __RE_PUBLIC_CLASS_API__ Client
	{
	private:
		Transport * _transport;
		Delegate * _delegate;
		REString _clientId;
		
		REStringList _subscribedChannels;
		REStringList _pendingSubscriptions;
		REStringList _supportedConnectionTypes;
		
		bool _isFayeConnected;
		
		void processMessage(Message * message);
		
		void onTransportConnected(Message * message);
		void onTransportDisconnected(Message * message);
		
		void onClientMessageReceived(Message * message);
		
		void onClientError(Message * message);
		
		void onHandshakeDone(Message * message);
		void handshake();
		
		void onConnectFayeDone(Message * message);
		void connectFaye();
		
		void onSubscriptionDone(Message * message);
		void subscribePendingSubscriptions();
		
		void onUnsubscribingDone(Message * message);
		void onDisconnectFayeDone(Message * message);
		
		bool isPendingChannel(const char * channel) const;
		
		static unsigned long long _messageId;
		static unsigned long long nextMessageId();
		
	public:
		/**
		 @return List of subscribed channels.
		 */
		const REStringList & subscribedChannels() const;
		
		
		/**
		 @return List of supported connection names. Based on server responce(handshake) and on implemented types.
		 */
		const REStringList & supportedTransportNames() const;
		
		
		/**
		 @return Currently connected transport protocol connection type.
		 */
		REString currentTransportName() const;
		
		
		/**
		 @brief Getter for client delegate.
		 @return Pointer to faye client delegate.
		 */
		Delegate * delegate();
		
		
		/**
		 @brief Setter for client delegate.
		 */
		void setDelegate(Delegate * delegate);
		
		
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
		 @brief Start send message to subscribed channel via connected faye.
		 @detailed Clent will NOT inform delegate for this user message.
		 @param message Non empty message for send.
		 @param channel Non empty, subscribed channel.
		 @return True - if connected and parameters non empty and sended, othervice false.
		 */
		bool sendMessageToChannel(const VariantMap & message, const char * channel);
		
		
		/**
		 @brief Check channel is subscribed.
		 @param channel Non empty channel.
		 @return True - if subscribed, otherwice false.
		 */
		bool isSubscribedToChannel(const char * channel) const;
		
		
		/**
		 * @brief Subscribes or storing to pendnig subscriptions required channel.
		 * @param channel Non empty channel.
		 * @return True - if already suscribed, started or stored to peding subscriptions, otherwice false.
		 */
		bool subscribeToChannel(const char * channel);
		
		
		/**
		 @brief Unsubscribe from specific channel.
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
		 * @brief List with implemented/available connection types.
		 * @return Type strings list.
		 */
		static REStringList availableConnectionTypes();
	};
	
	
	
	/**
	 @brief Message class for internal logic communication.
	 */
	class __RE_PUBLIC_CLASS_API__ Message
	{
    public:
        /**
         @brief Type of Faye channel
         */
        typedef enum _channelType
        {
            /**
             @brief Undefined Faye channel
             */
            ChannelTypeNone = 0,
			
			
            /**
             @brief Handshake Faye channel
             */
            ChannelTypeHandshake,
			
			
            /**
             @brief Connect Faye channel
             */
            ChannelTypeConnect,
			
			
            /**
             @brief Disconnect Faye channel
             */
            ChannelTypeDisconnect,
			
			
            /**
             @brief Subscribe Faye channel
             */
            ChannelTypeSubscribe,
			
			
            /**
             @brief Unsubscribe Faye channel
             */
            ChannelTypeUnsubscribe
        }
        /**
         @brief Type of Faye channel
         */
        ChannelType;
		
        /**
         @brief Faye message type.
         */
        typedef enum _messageType
        {
            /**
             @brief Undefined, default type.
             */
            MessageTypeNone = 0,
			
			
            /**
             @brief Faye transport protocol connected to server.
             */
            MessageTypeTransportConnected,
			
			
            /**
             @brief Faye transport protocol disconnected from server.
             */
            MessageTypeTransportDisconnected,
			
			
            /**
             @brief Faye transport protocol error.
             */
            MessageTypeTransportError,
			
			
            /**
             @brief Faye transport protocol received message.
             */
            MessageTypeServerResponce
        }
        /**
         @brief Faye message type.
         */
        MessageType;
		
	private:
		REString _clientId;
		REString _channel;
		REString _subscription;
		REString _errorString;
		REString _version;
		REString _minimumVersion;
		REString _connectionType;
		REStringList _connectionTypes;
		REBuffer _data;
		MessageType _type;
		bool _isSuccessfully;
		
		void fromJsonDataBuff(const char * jsonData, const size_t dataSize);
		
	public:
		/**
		 @brief Getter for message type.
		 @return Enum type.
		 */
        Message::MessageType type() const;
		
		
		/**
		 @brief Getter for client ID.
		 @return Std string with client ID.
		 */
		const REString & clientId() const;
		
		
		/**
		 @brief Getter for channel.
		 @return Std string with channel name.
		 */
		const REString & channel() const;
		
		
		/**
		 @brief Getter for channel type.
		 @return Enum type.
		 */
        Message::ChannelType channelType() const;
		
		
		/**
		 @brief Getter for subscription.
		 @return Std string with subscription.
		 */
		const REString & subscription() const;
		
		
		/**
		 @brief Getter for error string.
		 @return Std string with error.
		 */
		const REString & errorString() const;
		
		
		/**
		 @brief Getter for success flag.
		 @return Boolen value of success.
		 */
		bool isSuccessfully() const;
		
		
		/**
		 @brief Getter for version string.
		 @return Std string with version.
		 */
		const REString & version() const;
		
		
		/**
		 @brief Getter for minimum version string.
		 @return Std string with minimum version.
		 */
		const REString & minimumVersion() const;
		
		
		/**
		 @brief Getter for connection type string.
		 @return Std string with connection type.
		 */
		const REString & connectionType() const;
		
		
		/**
		 @brief Getter for list of connection type strings.
		 @return Std list with connection types strings.
		 */
		const REStringList & connectionTypes() const;
		
		
		/**
		 @brief Getter for message data.
		 @return Buffer with message data.
		 */
		const REBuffer & data() const;
		
		
		/**
		 @brief Setter for message type.
		 @param type The new message type.
		 @return This message address.
		 */
        Message & setType(Message::MessageType type);
		
		
		/**
		 @brief Setter for client ID.
		 @param value - New client ID value.
		 @return This message address.
		 */
		Message & setClientId(const char * value);
		
		
		/**
		 @brief Setter for channel.
		 @param value - New channel value.
		 @return This message address.
		 */
		Message & setChannel(const char * value);
		
		
		/**
		 @brief Setter for channel type.
		 @param value - New channel type value.
		 @return This message address.
		 */
        Message & setChannelType(const Message::ChannelType type);
		
		
		/**
		 @brief Setter for subscription.
		 @param value - New subscription value.
		 @return This message address.
		 */
		Message & setSubscription(const char * value);
		
		
		/**
		 @brief Setter for successful flag.
		 @param value - New successful flag value.
		 @return This message address.
		 */
		Message & setSuccessfully(bool value);
		
		
		/**
		 @brief Setter for error string.
		 @param value - New error string value.
		 @return This message address.
		 */
		Message & setErrorString(const char * value);
		
		
		/**
		 @brief Setter for version string.
		 @param value - New version string value.
		 @return This message address.
		 */
		Message & setVersion(const char * value);
		
		
		/**
		 @brief Add connection type to message.
		 @param value - Connection type string.
		 @return This message address.
		 */
		Message & addConnectionType(const char * connectionType);
		
		
		/**
		 @brief Setter for minimum version string.
		 @param value - New minimum version value.
		 @return This message address.
		 */
		Message & setMinimumVersion(const char * value);
		
		
		/**
		 @brief Setter for connection type string.
		 @param value - New connection type value.
		 @return This message address.
		 */
		Message & setConnectionType(const char * value);
		
		
		/**
		 @brief Check empty message.
		 @return True if empty, otherwice false.
		 */
		bool isEmpty() const;
		
		
		/**
		 @brief Convert message to JSON C string.
		 @return JSON C string or NULL. Should free after using.
		 */
		char * jsonCString() const;	
		
		/**
		 @brief Convert message to JSON string.
		 @return JSON string.
		 */
		REString toJsonString() const;
		
		
		/**
		 @brief Default constructor.
		 */
		Message();
		
		
		/**
		 @brief Constructor.
		 @param jsonString - JSON string with message params.
		 */
		Message(const char * jsonString);
		
		
		/**
		 @brief Constructor.
		 @param jsonData - JSON data with message params.
		 */
		Message(const unsigned char * data, const size_t dataSize);
		
		
		/**
		 @brief Convert channel type string to it's type.
		 @param typeString - Channel type string.
		 @return Channel type enum value.
		 */
        static Message::ChannelType stringToType(const char * typeString);
		
		
		/**
		 @brief Convert channel type to string.
		 @param type - Channel type enum value.
		 @return String with channel type.
		 */
        static const char * typeToString(const Message::ChannelType type);
	};
	
	class __RE_PUBLIC_CLASS_API__ Variant
	{
	public:
		typedef enum _variantType
		{
			TypeNone,
			TypeInteger,
			TypeUnsignedInteger,
			TypeReal,
			TypeBool,
			TypeString,
			TypeMap,
			TypeList
        }
        VariantType;
		
	protected:
		typedef union _variantUnion
		{
			int64_t int64Value;
			uint64_t uint64Value;
			double doubleValue;
			bool boolValue;
			void * pointerValue;
		} VariantUnion;
		
		VariantUnion _u;
		VariantType _t;
		
		void clean();
		
	public:
		VariantType type() const;
		
		int toInt() const;
		
		int64_t toInt64() const;
		
		uint64_t toUInt64() const;
		
		double toDouble() const;
		
		bool toBool() const;
		
		Variant & operator=(int v);
		Variant & operator=(float v);
		Variant & operator=(double v);
		
		Variant & operator=(long long v);
		Variant & operator=(unsigned long long v);
		Variant & operator=(long double v);
		Variant & operator=(bool v);
		Variant & operator=(const REString & s);
		Variant & operator=(const char * s);
		Variant & operator=(const wchar_t * s);
		
		Variant & operator=(const VariantMap & m);
		Variant & operator=(const VariantList & l);
		
		Variant & operator=(const Variant & v);
		
		const REString & string() const;
		
		const VariantMap & map() const;
		
		const VariantList & list() const;
		
		REString & string();
		
		VariantMap & map();
		
		VariantList & list();
		
		Variant();
		
		Variant(int v);
		Variant(float v);
		Variant(double v);
		Variant(long long v);
		Variant(unsigned long long v);
		Variant(long double v);
		Variant(bool v);
		Variant(const char * v);
		Variant(const wchar_t * v);
		Variant(const REString & v);
		Variant(const VariantMap & v);
		Variant(const VariantList & v);
		Variant(const Variant & v);
		
		~Variant();
	};
	
	class __RE_PUBLIC_CLASS_API__ VariantMap : public REMap<REString, Variant>
	{
	public:
		const Variant operator[](const char * key) const;
		const Variant operator[](const wchar_t * key) const;
		const Variant operator[](const REString & key) const;
		Variant & operator[](const char * key);
		Variant & operator[](const wchar_t * key);
		Variant & operator[](const REString & key);
		VariantMap & operator=(const VariantMap & map);
		VariantMap(const VariantMap & map);
		VariantMap();
		virtual ~VariantMap();
	};
	
	class __RE_PUBLIC_CLASS_API__ VariantList : public REList<Variant>
	{
	public:
		VariantList & operator=(const VariantList & list);
		VariantList(const VariantList & list);
		VariantList();
		virtual ~VariantList();
	};
}


#endif /* __FAYECPP_VARIANT_H__ */

