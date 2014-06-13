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
	
	bool JsonParser::toMap(VariantMap & map)
	{
		if (this->isMap())
		{
			JsonParser::jsonToMap(_json, map);
			return true;
		}
		return false;
	}
	
	void JsonParser::jsonToMap(json_t * json, VariantMap & map)
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
						VariantMap m;
						JsonParser::jsonToMap(o, m);
						map[k] = m;
					}
						break;
						
					case JSON_ARRAY:
					{
						VariantList l;
						JsonParser::jsonToList(o, l);
						map[k] = l;
					}
						break;
						
					case JSON_STRING: map[k] = REString(json_string_value(o)); break;
					case JSON_INTEGER: map[k] = Variant((long long)json_integer_value(o)); break;
					case JSON_REAL: map[k] = Variant((double)json_real_value(o)); break;
					case JSON_TRUE: map[k] = Variant(true); break;
					case JSON_FALSE: map[k] = Variant(false); break;
					case JSON_NULL: map[k] = Variant(); break;
					default: break;
				}
			}
			iter = json_object_iter_next(json, iter);
		}
	}
	
	void JsonParser::jsonToList(json_t * json, VariantList & list)
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
						VariantMap m;
						JsonParser::jsonToMap(o, m);
						list.add(m);
					}
						break;
						
					case JSON_ARRAY:
					{
						VariantList l;
						JsonParser::jsonToList(o, l);
						list.add(l);
					}
						break;
						
					case JSON_STRING: list.add(REString(json_string_value(o))); break;
					case JSON_INTEGER: list.add(Variant((long long)json_integer_value(o))); break;
					case JSON_REAL: list.add(Variant((double)json_real_value(o))); break;
					case JSON_TRUE: list.add(Variant(true)); break;
					case JSON_FALSE: list.add(Variant(false)); break;
					case JSON_NULL: list.add(Variant()); break;
					default: break;
				}
			}
		}
	}
	
	bool JsonParser::toList(VariantList & list)
	{
		if (this->isList())
		{
			JsonParser::jsonToList(_json, list);
			return true;
		}
		return false;
	}
	
	JsonParser::JsonParser(const char * text) : _json(NULL)
	{
		const size_t len = text ? strlen(text) : 0;
		if (len > 0)
		{
			json_error_t error;
			memset(&error, 0, sizeof(json_error_t));
			//		except that the string doesn’t need to be null-terminated
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
	
	
	
	
	char * JsonGenerator::jsonStringFromMap(const VariantMap & message)
    {
        char * jsonString = NULL;
        json_t * json = json_object();
        if (json)
        {
			JsonGenerator::addMapToJson(message, json);
            jsonString = json_dumps(json, 0);
            json_decref(json);
        }
        return jsonString;
    }
	
    char * JsonGenerator::jsonStringFromList(const VariantList & message)
    {
        char * jsonString = NULL;
        json_t * json = JsonGenerator::jsonObjectFromList(message);
        if (json)
        {
            jsonString = json_dumps(json, 0);
            json_decref(json);
        }
        return jsonString;
    }
	
    json_t * JsonGenerator::jsonObjectFromList(const VariantList & list)
	{
		json_t * array = json_array();
		if (array)
		{
			VariantList::Iterator i = list.iterator();
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
	
    json_t * JsonGenerator::jsonObjectFromValue(const Variant & value)
	{
		switch (value.type())
		{
			case Variant::TypeInteger: return json_integer(value.toInt64()); break;
			case Variant::TypeUnsignedInteger: return json_integer(value.toUInt64()); break;
			case Variant::TypeReal: return json_real(value.toDouble()); break;
			case Variant::TypeBool: return value.toBool() ? json_true() : json_false(); break;
			case Variant::TypeString: return json_string(value.toString()); break;
			case Variant::TypeMap:
			{
				json_t * subMapJson = json_object();
				if (subMapJson)
				{
					JsonGenerator::addMapToJson(value.toMap(), subMapJson);
					return subMapJson;
				}
			}
				break;
            case Variant::TypeList:	return JsonGenerator::jsonObjectFromList(value.toList()); break;
			default:
				break;
		}
		return NULL;
	}
	
    void JsonGenerator::addMapToJson(const VariantMap & map, json_t * json)
	{
		VariantMap::Iterator i = map.iterator();
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
	
	JsonGenerator::JsonGenerator(const VariantMap & map) : _string(NULL)
	{
		_string = JsonGenerator::jsonStringFromMap(map);
	}
	
	JsonGenerator::JsonGenerator(const VariantList & list) : _string(NULL)
	{
		_string = JsonGenerator::jsonStringFromList(list);
	}
	
	JsonGenerator::~JsonGenerator()
	{
		if (_string) free(_string);
	}
	
}

