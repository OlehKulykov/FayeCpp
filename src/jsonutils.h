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


#ifndef __FAYECPP_JSON_UTILS_H__
#define __FAYECPP_JSON_UTILS_H__

#include "../fayecpp.h"

#include <jansson.h>

namespace FayeCpp {
	
	class JsonParser
	{
	private:
		json_t * _json;
		static void jsonToMap(json_t * json, VariantMap & map);
		static void jsonToList(json_t * json, VariantList & list);
		
	public:
		bool isMap() const;
		bool isList() const;
		bool toMap(VariantMap & map);
		bool toList(VariantList & list);
		JsonParser(const char * text);
		~JsonParser();
	};
	
	class JsonGenerator
	{
	private:
		char * _string;
		static json_t * jsonObjectFromList(const VariantList & list);
		static json_t * jsonObjectFromValue(const Variant & value);
		static void addMapToJson(const VariantMap & map, json_t * json);
		static char * jsonStringFromMap(const VariantMap & message);
		static char * jsonStringFromList(const VariantList & message);
		
	public:
		const char * string() const;
		JsonGenerator(const VariantMap & map);
		JsonGenerator(const VariantList & list);
		~JsonGenerator();
	};
	
}

#endif /* __FAYECPP_JSON_UTILS_H__ */
