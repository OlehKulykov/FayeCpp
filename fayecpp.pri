##
##   Copyright (c) 2014 - 2015 Kulykov Oleh <nonamedemail@gmail.com>
##
##   Permission is hereby granted, free of charge, to any person obtaining a copy
##   of this software and associated documentation files (the "Software"), to deal
##   in the Software without restriction, including without limitation the rights
##   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
##   copies of the Software, and to permit persons to whom the Software is
##   furnished to do so, subject to the following conditions:
##
##   The above copyright notice and this permission notice shall be included in
##   all copies or substantial portions of the Software.
##
##   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
##   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
##   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
##   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
##   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
##   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
##   THE SOFTWARE.
##


DEFINES += HAVE_SUITABLE_QT_VERSION=1

QT += websockets

DEFINES += HAVE_QT=1

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/builds/qt/
INCLUDEPATH += $$PWD/jansson/src/


DEPENDPATH += $$PWD


## FayeCpp sources
SOURCES += \
    $$PWD/src/REBuffer.cpp \
    $$PWD/src/REBufferNoCopy.cpp \
    $$PWD/src/RELog.cpp \
    $$PWD/src/REMutableString.cpp \
    $$PWD/src/REStaticString.cpp \
    $$PWD/src/REString.cpp \
    $$PWD/src/REStringBase.cpp \
    $$PWD/src/REStringList.cpp \
    $$PWD/src/REStringUtilsPrivate.cpp \
    $$PWD/src/REVariant.cpp \
    $$PWD/src/REVariantList.cpp \
    $$PWD/src/REVariantMap.cpp \
    $$PWD/src/REWideString.cpp \
    $$PWD/src/FCClient.cpp \
    $$PWD/src/FCError.cpp \
    $$PWD/src/FCJsonUtils.cpp \
    $$PWD/src/FCMessage.cpp \
    $$PWD/src/FCTransport.cpp \
    $$PWD/src/FCWebSocketQt.cpp


## FayeCpp headers
HEADERS += $$PWD/fayecpp.h \
           $$PWD/src/FCWebSocketQt.h


## jansson sources
SOURCES += $$PWD/jansson/src/dump.c \
           $$PWD/jansson/src/error.c \
           $$PWD/jansson/src/hashtable.c \
           $$PWD/jansson/src/hashtable_seed.c \
           $$PWD/jansson/src/load.c \
           $$PWD/jansson/src/memory.c \
           $$PWD/jansson/src/pack_unpack.c \
           $$PWD/jansson/src/strbuffer.c \
           $$PWD/jansson/src/strconv.c \
           $$PWD/jansson/src/utf.c \
           $$PWD/jansson/src/value.c


win32 {
DEFINES += __RE_OS_WINDOWS__=1
DEFINES += WIN32_LEAN_AND_MEAN
DEFINES += _WIN32
#common defines
DEFINES += HAVE_STDINT_H=1
DEFINES += HAVE_STDARG_H=1
DEFINES += HAVE_SYS_STAT_H=1
DEFINES += HAVE_SYS_TYPES_H=1
DEFINES += HAVE_FCNTL_H=1

## jansson defines
DEFINES += JSON_INLINE=__inline

##  manually define jason int
#DEFINES += JANSSON_USING_CMAKE=1
#DEFINES += json_int_t=int64_t
#DEFINES += json_strtoint=strtoll
#DEFINES += JSON_INTEGER_FORMAT=PRIi64

##  automatically define jason int
DEFINES += JSON_INTEGER_IS_LONG_LONG=1

## always define with 1 or 0
DEFINES += JSON_HAVE_LOCALECONV=1
}

macx {
DEFINES += __RE_OS_MACOSX__=1

#common defines
DEFINES += HAVE_INTTYPES_H=1
DEFINES += HAVE_STDINT_H=1
DEFINES += HAVE_STDARG_H=1

DEFINES += HAVE_SYS_TIME_H=1
DEFINES += HAVE_SYS_PARAM_H=1
DEFINES += HAVE_SYS_STAT_H=1
DEFINES += HAVE_SYS_TYPES_H=1

DEFINES += HAVE_PTHREAD_H=1
DEFINES += HAVE_UNISTD_H=1
DEFINES += HAVE_FCNTL_H=1

DEFINES += HAVE_SCHED_H=1
DEFINES += HAVE_FUNCTION_PTHREAD_CANCEL=1
DEFINES += HAVE_FUNCTION_USLEEP=1
DEFINES += HAVE_FUNCTION_PTHREAD_ATTR_SETSCOPE=1
DEFINES += HAVE_FUNCTION_PTHREAD_ATTR_SETSTACKSIZE=1
DEFINES += HAVE_MACH_MACH_TIME_H=1
DEFINES += HAVE_ASSERT_H=1

## jansson defines
DEFINES += JSON_INLINE=inline

##  manually define jason int
DEFINES += JANSSON_USING_CMAKE=1
DEFINES += json_int_t=int64_t
DEFINES += json_strtoint=strtoll
DEFINES += JSON_INTEGER_FORMAT=PRIi64

##  automatically define jason int
#DEFINES += JSON_INTEGER_IS_LONG_LONG=1

## always define with 1 or 0
DEFINES += JSON_HAVE_LOCALECONV=1

}

linux-* {
DEFINES += __RE_OS_LINUX__=1
#common defines
DEFINES += HAVE_INTTYPES_H=1
DEFINES += HAVE_STDINT_H=1
DEFINES += HAVE_STDARG_H=1
DEFINES += HAVE_SIGNAL_H=1

DEFINES += HAVE_SYS_TIME_H=1
DEFINES += HAVE_SYS_PARAM_H=1
DEFINES += HAVE_SYS_STAT_H=1
DEFINES += HAVE_SYS_TYPES_H=1

DEFINES += HAVE_PTHREAD_H=1
DEFINES += HAVE_UNISTD_H=1
DEFINES += HAVE_FCNTL_H=1

DEFINES += HAVE_SCHED_H=1
DEFINES += HAVE_FUNCTION_PTHREAD_CANCEL=1
DEFINES += HAVE_FUNCTION_USLEEP=1
DEFINES += HAVE_FUNCTION_PTHREAD_ATTR_SETSCOPE=1
DEFINES += HAVE_FUNCTION_PTHREAD_ATTR_SETSTACKSIZE=1

## jansson defines
DEFINES += JSON_INLINE=inline

##  manually define jason int
DEFINES += JANSSON_USING_CMAKE=1
DEFINES += json_int_t=int64_t
DEFINES += json_strtoint=strtoll
DEFINES += JSON_INTEGER_FORMAT=PRIi64

##  automatically define jason int
#DEFINES += JSON_INTEGER_IS_LONG_LONG=1

## always define with 1 or 0
DEFINES += JSON_HAVE_LOCALECONV=1
}



