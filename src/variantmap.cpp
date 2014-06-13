/*
 *   Copyright (c) 2014 Kulykov Oleh <nonamedemail@gmail.com>
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


#include "../fayecpp.h"

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#if defined(HAVE_ASSERT_H)
#include <assert.h>
#endif

namespace FayeCpp {
	
	Variant * VariantMap::findTypedValue(const char * key, const Variant::VariantType type) const
	{
		return this->findTypedValue(REStaticString(key), type);
	}
	
	Variant * VariantMap::findTypedValue(const wchar_t * key, const Variant::VariantType type) const
	{
		return this->findTypedValue(REStaticString(key), type);
	}
	
	Variant * VariantMap::findTypedValue(const REString & key, const Variant::VariantType type) const
	{
		Node * node = this->findNode(key);
		if (node)
		{
			if (node->value.type() == type) 
			{
				return &node->value;
			}
		}
		return NULL;
	}
	
	const Variant VariantMap::operator[](const char * key) const
	{
		return (*this)[REStaticString(key)];
	}
	
	const Variant VariantMap::operator[](const wchar_t * key) const
	{
		return (*this)[REStaticString(key)];
	}
	
	const Variant VariantMap::operator[](const REString & key) const
	{
		Node * node = this->findNode(key);
		return node ? node->value : Variant();
	}
	
    Variant & VariantMap::operator[](const char * key)
	{
		return (*this)[REString(key)];
	}
	
	Variant & VariantMap::operator[](const wchar_t * key)
	{
		return (*this)[REString(key)];
	}
	
	Variant & VariantMap::operator[](const REString & key)
	{
		Node * node = this->findNode(key);
		if (node) return node->value;
		
		node = this->addNewNodeWithKeyValue(key, Variant());
		
#if defined(HAVE_ASSERT_H)		
		assert(node);
#endif		
		
		return node->value;
	}
	
	VariantMap & VariantMap::operator=(const VariantMap & map)
	{
		this->clear();
		VariantMap::Iterator i = map.iterator();
		while (i.next()) 
		{
			this->add(i.key(), i.value());
		}
		return *this;
	}
	
	VariantMap::VariantMap(const VariantMap & map)
	{
		*this = map;
	}
	
	VariantMap::VariantMap()
	{
		
	}
	
	VariantMap::~VariantMap()
	{
		
	}

}

