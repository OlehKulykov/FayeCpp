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
