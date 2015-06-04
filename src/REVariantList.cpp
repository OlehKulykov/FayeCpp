/*
 *   Copyright (c) 2014 - 2015 Kulykov Oleh <info@resident.name>
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


namespace FayeCpp {
	
	REVariantList & REVariantList::operator+=(int v)
	{
		this->add(v);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(unsigned int v)
	{
		this->add(v);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(float v)
	{
		this->add(v);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(double v)
	{
		this->add(v);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(long long v)
	{
		this->add(v);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(unsigned long long v)
	{
		this->add(v);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(long double v)
	{
		this->add(v);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(bool v)
	{
		this->add(v);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(const REString & s)
	{
		this->add(s);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(const char * s)
	{
		this->add(s);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(const wchar_t * s)
	{
		this->add(s);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(const REVariantMap & m)
	{
		this->add(m);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(const REVariantList & l)
	{
		this->add(l);
		return *this;
	}
	
	REVariantList & REVariantList::operator+=(const REVariant & v)
	{
		this->add(v);
		return *this;
	}
	
    REVariantList & REVariantList::operator=(const REVariantList & list)
	{
		this->clear();
		
		REVariantList::Iterator i = list.iterator();
		while (i.next()) 
		{
			this->add(i.value());
		}
		
		return *this;
	}

	bool REVariantList::isEqualToList(const REVariantList & list) const
	{
		REUInt32 count1 = 0;
		REUInt32 count2 = 0;
		REVariantList::Iterator i1 = this->iterator();
		REVariantList::Iterator i2 = list.iterator();
		bool next1 = i1.next();
		bool next2 = i2.next();
		while (next1 && next2)
		{
			count1++; count2++;
			if (!i1.value().isEqualToVariant(i2.value())) return false;
			next1 = i1.next();
			next2 = i2.next();
		}

		if (next1) { count1++; while (i1.next()) count1++; }
		if (next2) { count2++; while (i2.next()) count2++; }

		return (count1 == count2);
	}

	bool REVariantList::operator==(const REVariantList & list) const
	{
		return this->isEqualToList(list);
	}

	bool REVariantList::operator!=(const REVariantList & list) const
	{
		return !this->isEqualToList(list);
	}

	REVariantList::REVariantList(const REVariantList & list)
	{
		*this = list;
	}
	
	REVariantList::REVariantList()
	{
		
	}
	
	REVariantList::~REVariantList()
	{
		
	}

}

