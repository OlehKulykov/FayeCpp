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


#ifndef __FAYECPP_VARIANT_H__
#define __FAYECPP_VARIANT_H__

#include "common.h"

namespace FayeCpp
{
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
		} VariantType;
		
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
		Variant & operator=(const std::string & s);
		Variant & operator=(const char * s);
		
		Variant & operator=(const std::map<std::string, Variant> & m);
		Variant & operator=(const std::list<Variant> & l);
		
		Variant & operator=(const Variant & v);
		
		const std::string & string() const;
		
		const std::map<std::string, Variant> & map() const;
		
		const std::list<Variant> & list() const;
		
		std::string & string();
		
		std::map<std::string, Variant> & map();
		
		std::list<Variant> & list();
		
		Variant();
		
		Variant(int v);
		Variant(float v);
		Variant(double v);
		Variant(long long v);
		Variant(unsigned long long v);
		Variant(long double v);
		Variant(bool v);
		Variant(const char * v);
		Variant(const std::string & v);
		Variant(const std::map<std::string, Variant> & v);
		Variant(const std::list<Variant> & v);
		Variant(const Variant & v);
		
		~Variant();
	};
}

#endif /* __FAYECPP_VARIANT_H__ */
