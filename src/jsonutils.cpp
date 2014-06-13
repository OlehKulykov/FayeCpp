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
#include "../fayecpp.h"

#include <jansson.h>

namespace FayeCpp {
	
    static json_t * __JsonUtils__jsonObjectFromList(const VariantList & list);
    static json_t * __JsonUtils__jsonObjectFromValue(const Variant & value);
    static void __JsonUtils__addMapToJson(const VariantMap & map, json_t * json);
    static char * __JsonUtils__jsonStringFromMap(const VariantMap & message);
    static char * __JsonUtils__jsonStringFromList(const VariantList & message);

    static char * __JsonUtils__jsonStringFromMap(const VariantMap & message)
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

    static char * __JsonUtils__jsonStringFromList(const VariantList & message)
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

    static json_t * __JsonUtils__jsonObjectFromList(const VariantList & list)
	{
		json_t * array = json_array();
		if (array)
		{
			VariantList::Iterator i = list.iterator();
			while (i.next()) 
			{
				json_t * object = __JsonUtils__jsonObjectFromValue(i.value());
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
			case Variant::TypeString: return json_string(value.string()); break;
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
	
    static void __JsonUtils__addMapToJson(const VariantMap & map, json_t * json)
	{
		VariantMap::Iterator i = map.iterator();
		while (i.next()) 
		{
			json_t * object = __JsonUtils__jsonObjectFromValue(i.value());
			if (object)
			{
				json_object_set(json, i.key().UTF8String(), object);
				json_decref(object);
			}

		}
	}
	
	char * JsonUtils::toJsonCString(const VariantMap & message)
	{
		return __JsonUtils__jsonStringFromMap(message);
	}
	
	char * JsonUtils::toJsonCString(const VariantList & message)
	{
		return __JsonUtils__jsonStringFromList(message);
	}
}

