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


namespace FayeCpp {
	
    VariantList & VariantList::operator=(const VariantList & list)
	{
		this->clear();
		
		VariantList::Iterator i = list.iterator();
		while (i.next()) 
		{
			this->add(i.value());
		}
		
		return *this;
	}
	
	VariantList::VariantList(const VariantList & list)
	{
		*this = list;
	}
	
	VariantList::VariantList()
	{
		
	}
	
	VariantList::~VariantList()
	{
		
	}

}

