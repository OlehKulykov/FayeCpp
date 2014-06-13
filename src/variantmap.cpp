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

