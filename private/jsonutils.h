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

namespace FayeCpp {
	
	class JsonUtils
	{		
	public:
        static std::vector<unsigned char> toJsonData(const std::map<std::string, Variant> & message);
        static std::vector<unsigned char> toJsonData(const std::list<Variant> & message);

        static std::string toJsonString(const std::map<std::string, Variant> & message);
        static std::string toJsonString(const std::list<Variant> & message);
	};
	
}

#endif /* __FAYECPP_JSON_UTILS_H__ */
