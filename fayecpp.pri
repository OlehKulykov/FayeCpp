


DEFINES += HAVE_SUITABLE_QT_VERSION=1

QT += websockets

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/builds/qt/
INCLUDEPATH += $$PWD/jansson/src/

DEPENDPATH += $$PWD

## FayeCpp sources
SOURCES += $$PWD/client.cpp \
           $$PWD/message.cpp \
           $$PWD/variant.cpp \
           $$PWD/private/jsonutils.cpp \
           $$PWD/private/REBuffer.cpp \
           $$PWD/private/REMutableString.cpp \
           $$PWD/private/REMutex.cpp \
           $$PWD/private/REString.cpp \
           $$PWD/private/REStringBase.cpp \
           $$PWD/private/REStringList.cpp \
           $$PWD/private/REStringUtilsPrivate.cpp \
           $$PWD/private/REThread.cpp \
           $$PWD/private/REWideString.cpp \
           $$PWD/private/transport.cpp \
           $$PWD/private/websocketqt.cpp

## FayeCpp headers
HEADERS += $$PWD/fayecpp.h \
           $$PWD/private/websocketqt.h

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
#DEFINES += USE_WINDOWS_CRYPTOAPI

}

macx {
DEFINES += __RE_OS_MACOSX__=1

#common defines
DEFINES += HAVE_INTTYPES_H=1
DEFINES += HAVE_STDINT_H=1

DEFINES += HAVE_SYS_TIME_H=1
DEFINES += HAVE_SYS_PARAM_H=1
DEFINES += HAVE_SYS_STAT_H=1
DEFINES += HAVE_SYS_TIME_H=1
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

linux-* {
DEFINES += __RE_OS_LINUX__=1

}



