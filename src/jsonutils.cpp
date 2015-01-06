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


#include "jsonutils.h"

namespace FayeCpp {

	bool JsonParser::isMap() const
	{
		return _json ? json_is_object(_json) : false;
	}
	
	bool JsonParser::isList() const
	{
		return _json ? json_is_array(_json) : false;
	}
	
	bool JsonParser::toMap(REVariantMap & map)
	{
		if (this->isMap())
		{
			JsonParser::jsonToMap(_json, map);
			return true;
		}
		return false;
	}
	
	void JsonParser::jsonToMap(json_t * json, REVariantMap & map)
	{
		void * iter = json_object_iter(json);
		while (iter)
		{
			const char * k = json_object_iter_key(iter);
			json_t * o = json_object_iter_value(iter);
			if (k && o) 
			{
				switch (json_typeof(o)) 
				{
					case JSON_OBJECT: 
					{
						REVariantMap m;
						JsonParser::jsonToMap(o, m);
						map[k] = m;
					}
						break;
						
					case JSON_ARRAY:
					{
						REVariantList l;
						JsonParser::jsonToList(o, l);
						map[k] = l;
					}
						break;
						
					case JSON_STRING: map[k] = REString(json_string_value(o)); break;
					case JSON_INTEGER: map[k] = REVariant((long long)json_integer_value(o)); break;
					case JSON_REAL: map[k] = REVariant((double)json_real_value(o)); break;
					case JSON_TRUE: map[k] = REVariant(true); break;
					case JSON_FALSE: map[k] = REVariant(false); break;
					case JSON_NULL: map[k] = REVariant(); break;
					default: break;
				}
			}
			iter = json_object_iter_next(json, iter);
		}
	}
	
	void JsonParser::jsonToList(json_t * json, REVariantList & list)
	{
		const size_t count = json_array_size(json);
		for (size_t i = 0; i < count; i++) 
		{
			json_t * o = json_array_get(json, i);
			if (o) 
			{
				switch (json_typeof(o)) 
				{
					case JSON_OBJECT: 
					{
						REVariantMap m;
						JsonParser::jsonToMap(o, m);
						list.add(m);
					}
						break;
						
					case JSON_ARRAY:
					{
						REVariantList l;
						JsonParser::jsonToList(o, l);
						list.add(l);
					}
						break;
						
					case JSON_STRING: list.add(REString(json_string_value(o))); break;
					case JSON_INTEGER: list.add(REVariant((long long)json_integer_value(o))); break;
					case JSON_REAL: list.add(REVariant((double)json_real_value(o))); break;
					case JSON_TRUE: list.add(REVariant(true)); break;
					case JSON_FALSE: list.add(REVariant(false)); break;
					case JSON_NULL: list.add(REVariant()); break;
					default: break;
				}
			}
		}
	}
	
	bool JsonParser::toList(REVariantList & list)
	{
		if (this->isList())
		{
			JsonParser::jsonToList(_json, list);
			return true;
		}
		return false;
	}
	
	JsonParser::JsonParser(const char * text) : 
		_json(NULL)
	{
		const size_t len = text ? strlen(text) : 0;
		if (len > 0)
		{
			json_error_t error;
			memset(&error, 0, sizeof(json_error_t));
			// except that the string doesn’t need to be null-terminated
			json_t * json = json_loadb(text, len, 0, &error);
			if (json)
			{
				if (json_is_object(json) || json_is_array(json))
				{
					_json = json;
				}
				else
				{
					json_decref(json);
				}
			}
		}
	}
	
	JsonParser::~JsonParser()
	{
		if (_json) json_decref(_json);
	}
	
	
	
	
	char * JsonGenerator::jsonStringFromMap(const REVariantMap & message)
    {
        char * jsonString = NULL;
        json_t * json = json_object();
        if (json)
        {
			JsonGenerator::addMapToJson(message, json);
            jsonString = json_dumps(json, JSON_COMPACT);
            json_decref(json);
        }
        return jsonString;
    }
	
    char * JsonGenerator::jsonStringFromList(const REVariantList & message)
    {
        char * jsonString = NULL;
        json_t * json = JsonGenerator::jsonObjectFromList(message);
        if (json)
        {
            jsonString = json_dumps(json, JSON_COMPACT);
            json_decref(json);
        }
        return jsonString;
    }
	
    json_t * JsonGenerator::jsonObjectFromList(const REVariantList & list)
	{
		json_t * array = json_array();
		if (array)
		{
			REVariantList::Iterator i = list.iterator();
			while (i.next()) 
			{
				json_t * object = JsonGenerator::jsonObjectFromValue(i.value());
				if (object)
				{
					json_array_append(array, object);
					json_decref(object);
				}
			}
		}
		return array;
	}
	
    json_t * JsonGenerator::jsonObjectFromValue(const REVariant & value)
	{
		switch (value.type())
		{
			case REVariant::TypeInteger: return json_integer(value.toInt64()); break;
			case REVariant::TypeUnsignedInteger: return json_integer(value.toUInt64()); break;
			case REVariant::TypeReal: return json_real(value.toDouble()); break;
			case REVariant::TypeBool: return value.toBool() ? json_true() : json_false(); break;
			case REVariant::TypeString: return json_string(value.toString()); break;
			case REVariant::TypeMap:
			{
				json_t * subMapJson = json_object();
				if (subMapJson)
				{
					JsonGenerator::addMapToJson(value.toMap(), subMapJson);
					return subMapJson;
				}
			}
				break;
            case REVariant::TypeList: return JsonGenerator::jsonObjectFromList(value.toList()); break;
			default: break;
		}
		return json_null();
	}
	
    void JsonGenerator::addMapToJson(const REVariantMap & map, json_t * json)
	{
		REVariantMap::Iterator i = map.iterator();
		while (i.next()) 
		{
			json_t * object = JsonGenerator::jsonObjectFromValue(i.value());
			if (object)
			{
				json_object_set(json, i.key().UTF8String(), object);
				json_decref(object);
			}
		}
	}
	
	const char * JsonGenerator::string() const
	{
		return _string;
	}
	
	JsonGenerator::JsonGenerator(const REVariantMap & map) : 
		_string(NULL)
	{
		_string = JsonGenerator::jsonStringFromMap(map);
	}
	
	JsonGenerator::JsonGenerator(const REVariantList & list) : 
		_string(NULL)
	{
		_string = JsonGenerator::jsonStringFromList(list);
	}
	
	JsonGenerator::~JsonGenerator()
	{
		if (_string) free(_string);
	}
	
}

