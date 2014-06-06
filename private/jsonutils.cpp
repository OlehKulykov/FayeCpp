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
#include "../variant.h"

#include <jansson.h>

namespace FayeCpp {
	
    static json_t * __JsonUtils__jsonObjectFromList(const std::list<Variant> & list);
    static json_t * __JsonUtils__jsonObjectFromValue(const Variant & value);
    static void __JsonUtils__addMapToJson(const std::map<std::string, Variant> & map, json_t * json);
    static char * __JsonUtils__jsonStringFromMap(const std::map<std::string, Variant> & message);
    static char * __JsonUtils__jsonStringFromList(const std::list<Variant> & message);

    static char * __JsonUtils__jsonStringFromMap(const std::map<std::string, Variant> & message)
    {
        char * jsonString = NULL;
        json_t * json = json_object();
        if (json)
        {
            __JsonUtils__addMapToJson(message, json);
            jsonString = json_dumps(json, 0);
            json_decref(json);
        }
        return jsonString;
    }

    static char * __JsonUtils__jsonStringFromList(const std::list<Variant> & message)
    {
        char * jsonString = NULL;
        json_t * json = __JsonUtils__jsonObjectFromList(message);
        if (json)
        {
            jsonString = json_dumps(json, 0);
            json_decref(json);
        }
        return jsonString;
    }

    static json_t * __JsonUtils__jsonObjectFromList(const std::list<Variant> & list)
	{
		json_t * array = json_array();
		if (array)
		{
			for (std::list<Variant>::const_iterator i = list.begin(); i != list.end(); ++i)
			{
                json_t * object = __JsonUtils__jsonObjectFromValue(*i);
				if (object)
				{
					json_array_append(array, object);
					json_decref(object);
				}
			}
			return array;
		}
		return NULL;
	}
	
    static json_t * __JsonUtils__jsonObjectFromValue(const Variant & value)
	{
		switch (value.type())
		{
			case Variant::TypeInteger: return json_integer(value.toInt64()); break;
			case Variant::TypeUnsignedInteger: return json_integer(value.toUInt64()); break;
			case Variant::TypeReal: return json_real(value.toDouble()); break;
			case Variant::TypeBool: return value.toBool() ? json_true() : json_false(); break;
			case Variant::TypeString: return json_string(value.string().c_str()); break;
			case Variant::TypeMap:
			{
				json_t * subMapJson = json_object();
				if (subMapJson)
				{
                    __JsonUtils__addMapToJson(value.map(), subMapJson);
					return subMapJson;
				}
			}
				break;
            case Variant::TypeList:	return __JsonUtils__jsonObjectFromList(value.list()); break;
			default:
				break;
		}
		return NULL;
	}
	
    static void __JsonUtils__addMapToJson(const std::map<std::string, Variant> & map, json_t * json)
	{
		for (std::map<std::string, Variant>::const_iterator i = map.begin(); i != map.end(); ++i)
		{
            json_t * object = __JsonUtils__jsonObjectFromValue(i->second);
			if (object)
			{
				json_object_set(json, i->first.c_str(), object);
				json_decref(object);
			}
		}
	}
	
    std::vector<unsigned char> JsonUtils::toJsonData(const std::map<std::string, Variant> & message)
    {
        char * jsonString = __JsonUtils__jsonStringFromMap(message);
        if (jsonString)
        {
            std::vector<unsigned char> data(jsonString, jsonString + strlen(jsonString) + 1);
            free(jsonString);
            return data;
        }
        return std::vector<unsigned char>();
    }
	
    std::vector<unsigned char> JsonUtils::toJsonData(const std::list<Variant> & message)
	{
        char * jsonString = __JsonUtils__jsonStringFromList(message);
        if (jsonString)
		{
			std::vector<unsigned char> data(jsonString, jsonString + strlen(jsonString) + 1);
			free(jsonString);
			return data;
		}
		return std::vector<unsigned char>();
	}

    std::string JsonUtils::toJsonString(const std::map<std::string, Variant> & message)
    {
        char * jsonString = __JsonUtils__jsonStringFromMap(message);
        if (jsonString)
        {
            std::string strng(jsonString);
            free(jsonString);
            return strng;
        }
        return std::string();
    }

    std::string JsonUtils::toJsonString(const std::list<Variant> & message)
    {
        char * jsonString = __JsonUtils__jsonStringFromList(message);
        if (jsonString)
        {
            std::string strng(jsonString);
            free(jsonString);
            return strng;
        }
        return std::string();
    }
}

