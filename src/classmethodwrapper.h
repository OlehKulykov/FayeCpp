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


#ifndef __FAYECPP_CLASS_METHOD_WRAPPER_H__
#define __FAYECPP_CLASS_METHOD_WRAPPER_H__

#include "../fayecpp.h"

namespace FayeCpp {

template<class T, typename MethodTypeName, class PT>
class __RE_PUBLIC_CLASS_API__ ClassMethodWrapper
{
protected:
    T * _classPointer;
    MethodTypeName _method;

public:
    void invokeWithPointer(PT * object)
    {
        (_classPointer->*_method)(object);
    }

    T * classPointer()
    {
        return _classPointer;
    }

    ClassMethodWrapper(T * classPtr, MethodTypeName method) : _classPointer(classPtr), _method(method) { }
    ~ClassMethodWrapper() { }
};

#ifndef NEW_CLASS_METHOD
#define NEW_CLASS_METHOD(CLASS_NAME,CLASS_PTR,METHOD_NAME) ((ClassMethod*)(new ClassMethodWrapper<CLASS_NAME, void(CLASS_NAME::*)(void*)>(CLASS_PTR,&CLASS_NAME::METHOD_NAME)) )
#endif

}

#endif /* __FAYECPP_CLASS_METHOD_WRAPPER_H__ */
