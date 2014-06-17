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

namespace FayeCpp {
	
	void Variant::clean()
	{
		if (_u.pointerValue)
		{
			switch (_t)
			{
				case TypeString: delete (REString *)_u.pointerValue; break;
				case TypeMap: delete (VariantMap *)_u.pointerValue; break;
				case TypeList: delete (VariantList *)_u.pointerValue; break;
				default: break;
			}
		}
		memset(&_u, 0, sizeof(VariantUnion));
		_t = TypeNone;
	}

	bool Variant::isNULL() const
	{
		return _u.pointerValue ? false : true;
	}
	
	bool Variant::isNumber() const 
	{
		return (_t == VariantType::TypeInteger) || (_t == VariantType::TypeReal); 
	}
	
	Variant::VariantType Variant::type() const
	{
		return _t;
	}
	
	int Variant::toInt() const
	{
		return (int)this->toInt64();
	}
	
	int64_t Variant::toInt64() const
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
	
	uint64_t Variant::toUInt64() const
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
	
	double Variant::toDouble() const
	{
		switch (_t)
		{
			case TypeInteger: return (double)_u.int64Value; break;
			case TypeUnsignedInteger: return (double)_u.uint64Value; break;
			case TypeReal: return (double)_u.doubleValue; break;
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
		
	bool Variant::toBool() const
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
	
	Variant & Variant::operator=(int v)
	{
		_u.int64Value = v;
		_t = TypeInteger;
		return *this;
	}
	Variant & Variant::operator=(float v)
	{
		_u.doubleValue = v;
		_t = TypeReal;
		return *this;
	}
	Variant & Variant::operator=(double v)
	{
		_u.doubleValue = v;
		_t = TypeReal;
		return *this;
	}
	
	Variant & Variant::operator=(long long v)
	{
		_u.int64Value = v;
		_t = TypeInteger;
		return *this;
	}
	Variant & Variant::operator=(unsigned long long v)
	{
		_u.uint64Value = v;
		_t = TypeUnsignedInteger;
		return *this;
	}
	Variant & Variant::operator=(long double v)
	{
		_u.doubleValue = v;
		_t = TypeReal;
		return *this;
	}
	Variant & Variant::operator=(bool v)
	{
		_u.boolValue = v;
		_t = TypeBool;
		return *this;
	}
	Variant & Variant::operator=(const REString & s)
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
	Variant & Variant::operator=(const char * s)
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
	
	Variant & Variant::operator=(const wchar_t * s)
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
	
	Variant & Variant::operator=(const VariantMap & m)
	{
		this->clean();
		VariantMap * p = new VariantMap(m);
		if (p)
		{
			_u.pointerValue = p;
			_t = TypeMap;
		}
		return *this;
	}
	Variant & Variant::operator=(const VariantList & l)
	{
		this->clean();
		VariantList * p = new VariantList(l);
		if (p)
		{
			_u.pointerValue = p;
			_t = TypeList;
		}
		return *this;
	}
	
	Variant & Variant::operator=(const Variant & v)
	{
		switch (v._t)
		{
			case TypeString: *this = v.toString(); break;
			case TypeMap: *this = v.toMap(); break;
			case TypeList: *this = v.toList(); break;
			default: _t = v._t; _u = v._u; break;
		}
		return *this;
	}

	bool Variant::isString() const
	{
		return _t == VariantType::TypeString; 
	}
	
	const REString & Variant::toString() const
	{
		return *(const REString *)_u.pointerValue;
	}
	
	bool Variant::isMap() const
	{
		return _t == VariantType::TypeMap; 
	}
	
	const VariantMap & Variant::toMap() const
	{
		return *(const VariantMap *)_u.pointerValue;
	}
	
	bool Variant::isList() const
	{
		return _t == VariantType::TypeList; 
	}
	
	const VariantList & Variant::toList() const
	{
		return *((VariantList *)_u.pointerValue);
	}
	
	REString & Variant::toString()
	{
		return *(REString *)_u.pointerValue;
	}
	
	VariantMap & Variant::toMap()
	{
		return *(VariantMap *)_u.pointerValue;
	}
	
	VariantList & Variant::toList()
	{
		return *((VariantList *)_u.pointerValue);
	}
	
	Variant::Variant() :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
	}
	
	Variant::Variant(int v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(float v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(double v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(long long v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(unsigned long long v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(long double v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(bool v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(const char * v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(const wchar_t * v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(const REString & v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(const VariantMap & v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(const VariantList & v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::Variant(const Variant & v) :
		_t(TypeNone)
	{
		memset(&_u, 0, sizeof(VariantUnion));
		*this = v;
	}
	
	Variant::~Variant()
	{
		this->clean();
	}
	
}

