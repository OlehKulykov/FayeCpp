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


#include "../fayecpp.h"

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

namespace FayeCpp {
	
	void REVariant::clean()
	{
		if (_u.pointerValue)
		{
			switch (_t)
			{
				case TypeString: delete (REString *)_u.pointerValue; break;
				case TypeMap: delete (REVariantMap *)_u.pointerValue; break;
				case TypeList: delete (REVariantList *)_u.pointerValue; break;
				default: break;
			}
		}
		memset(&_u, 0, sizeof(VariantUnion));
		_t = TypeNone;
	}

	bool REVariant::isNULL() const
	{
		return _u.pointerValue ? false : true;
	}
	
	bool REVariant::isNumber() const 
	{
		return (_t == TypeInteger) || (_t == TypeReal); 
	}
	
	REVariant::VariantType REVariant::type() const
	{
		return _t;
	}
	
	int REVariant::toInt() const
	{
		return (int)this->toInt64();
	}
	
	unsigned int REVariant::toUInt() const
	{
		return (unsigned int)this->toUInt64();
	}
	
	int64_t REVariant::toInt64() const
	{
		switch (_t)
		{
			case TypeInteger: return _u.int64Value; break;
			case TypeUnsignedInteger: return (int64_t)_u.uint64Value; break;
			case TypeReal: return (int64_t)_u.doubleValue; break;
			case TypeBool: return _u.boolValue ? (int64_t)1 : (int64_t)0; break;
			case TypeString:
			{
				if (this->toString().isDigit())
				{
					REBOOL isOk = false;
					const REInt64 parsedValue = this->toString().integerValue(&isOk);
					if (isOk) return (int64_t)parsedValue;
				}
			}
				break;
			default: break;
		}
		return 0;
	}
	
	uint64_t REVariant::toUInt64() const
	{
		switch (_t)
		{
			case TypeInteger: return (uint64_t)_u.int64Value; break;
			case TypeUnsignedInteger: return _u.uint64Value; break;
			case TypeReal: return (uint64_t)_u.doubleValue; break;
			case TypeBool: return _u.boolValue ? (uint64_t)1 : (uint64_t)0; break;
			case TypeString:
			{
				if (this->toString().isDigit())
				{
					REBOOL isOk = false;
					const REInt64 parsedValue = this->toString().integerValue(&isOk);
					if (isOk) return (uint64_t)parsedValue;
				}
			}
				break;
			default: break;
		}
		return 0;
	}
	
	double REVariant::toDouble() const
	{
		switch (_t)
		{
			case TypeInteger: return (double)_u.int64Value; break;
			case TypeUnsignedInteger: return (double)_u.uint64Value; break;
			case TypeReal: return _u.doubleValue; break;
			case TypeBool: return _u.boolValue ? (double)1 : (double)0; break;
			case TypeString:
			{
				if (this->toString().isDigit())
				{
					REBOOL isOk = false;
					const REFloat64 parsedValue = this->toString().floatValue(&isOk);
					if (isOk) return (double)parsedValue;
				}
			}
				break;
			default: break;
		}
		return 0;
	}
		
	bool REVariant::toBool() const
	{
		switch (_t)
		{
			case TypeInteger: return _u.int64Value == 0 ? false : true; break;
			case TypeUnsignedInteger: return _u.uint64Value == 0 ? false: true; break;
			case TypeReal: return _u.doubleValue == 0 ? false : true; break;
			case TypeBool: return _u.boolValue; break;
			case TypeString:
			{
				REMutableString s = this->toString().mutableString();
				s.toLower();
				if (s.isEqual("true")) return true;
			}
				break;
			default: break;
		}
		return false;
	}
	
	REVariant & REVariant::operator=(int v)
	{
		this->clean();
		_u.int64Value = (int64_t)v;
		_t = TypeInteger;
		return *this;
	}
	
	REVariant & REVariant::operator=(unsigned int v)
	{
		this->clean();
		_u.uint64Value = (uint64_t)v;
		_t = TypeInteger;
		return *this;
	}
	
	REVariant & REVariant::operator=(float v)
	{
		this->clean();
		_u.doubleValue = (double)v;
		_t = TypeReal;
		return *this;
	}
	
	REVariant & REVariant::operator=(double v)
	{
		this->clean();
		_u.doubleValue = v;
		_t = TypeReal;
		return *this;
	}
	
	REVariant & REVariant::operator=(long long v)
	{
		this->clean();
		_u.int64Value = (int64_t)v;
		_t = TypeInteger;
		return *this;
	}
	
	REVariant & REVariant::operator=(unsigned long long v)
	{
		this->clean();
		_u.uint64Value = (uint64_t)v;
		_t = TypeUnsignedInteger;
		return *this;
	}
	
	REVariant & REVariant::operator=(long double v)
	{
		this->clean();
		_u.doubleValue = (double)v;
		_t = TypeReal;
		return *this;
	}
	
	REVariant & REVariant::operator=(bool v)
	{
		this->clean();
		_u.boolValue = v;
		_t = TypeBool;
		return *this;
	}
	
	REVariant & REVariant::operator=(const REString & s)
	{
		this->clean();
		REString * p = new REString(s);
		if (p)
		{
			_u.pointerValue = p;
			_t = TypeString;
		}
		return *this;
	}
	
	REVariant & REVariant::operator=(const char * s)
	{
		this->clean();
		if (s)
		{
			REString * p = new REString(s);
			if (p)
			{
				_u.pointerValue = p;
				_t = TypeString;
			}
		}
		return *this;
	}
	
	REVariant & REVariant::operator=(const wchar_t * s)
	{
		this->clean();
		if (s)
		{
			REString * p = new REString(s);
			if (p)
			{
				_u.pointerValue = p;
				_t = TypeString;
			}
		}
		return *this;
	}
	
	REVariant & REVariant::operator=(const REVariantMap & m)
	{
		this->clean();
		REVariantMap * p = new REVariantMap(m);
		if (p)
		{
			_u.pointerValue = p;
			_t = TypeMap;
		}
		return *this;
	}
	
	REVariant & REVariant::operator=(const REVariantList & l)
	{
		this->clean();
		REVariantList * p = new REVariantList(l);
		if (p)
		{
			_u.pointerValue = p;
			_t = TypeList;
		}
		return *this;
	}
	
	REVariant & REVariant::operator=(const REVariant & v)
	{
		this->clean();
		switch (v._t)
		{
			case TypeString: *this = v.toString(); break;
			case TypeMap: *this = v.toMap(); break;
			case TypeList: *this = v.toList(); break;
			default: _t = v._t; _u = v._u; break;
		}
		return *this;
	}

	bool REVariant::isString() const
	{
		return _t == TypeString; 
	}
	
	const REString & REVariant::toString() const
	{
		return *(const REString *)_u.pointerValue;
	}
	
	bool REVariant::isMap() const
	{
		return _t == TypeMap; 
	}
	
	const REVariantMap & REVariant::toMap() const
	{
		return *(const REVariantMap *)_u.pointerValue;
	}
	
	bool REVariant::isList() const
	{
		return _t == TypeList; 
	}
	
	const REVariantList & REVariant::toList() const
	{
		return *(const REVariantList *)_u.pointerValue;
	}
	
	REString & REVariant::toString()
	{
		return *(REString *)_u.pointerValue;
	}
	
	REVariantMap & REVariant::toMap()
	{
		return *(REVariantMap *)_u.pointerValue;
	}
	
	REVariantList & REVariant::toList()
	{
		return *((REVariantList *)_u.pointerValue);
	}
	
	REVariant::REVariant() :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
	}
	
	REVariant::REVariant(int v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(unsigned int v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(float v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(double v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(long long v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(unsigned long long v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(long double v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(bool v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(const char * v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(const wchar_t * v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(const REString & v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(const REVariantMap & v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(const REVariantList & v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::REVariant(const REVariant & v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	REVariant::~REVariant()
	{
		this->clean();
	}
	
}

