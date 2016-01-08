/* lws_config_private.h.in. Private compilation options. */

#ifndef NDEBUG
	#ifndef _DEBUG
		#define _DEBUG
	#endif
#endif

/* Define to 1 to use CyaSSL as a replacement for OpenSSL. 
 * LWS_OPENSSL_SUPPORT needs to be set also for this to work. */
/* #undef USE_CYASSL */

/* Define to 1 if you have the `bzero' function. */
#define LWS_HAVE_BZERO 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define LWS_HAVE_DLFCN_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define LWS_HAVE_FCNTL_H 1

/* Define to 1 if you have the `fork' function. */
#define LWS_HAVE_FORK 1

/* Define to 1 if you have the `getenv’ function. */
#define LWS_HAVE_GETENV 1

/* Define to 1 if you have the <in6addr.h> header file. */
#define LWS_HAVE_IN6ADDR_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define LWS_HAVE_INTTYPES_H 1

/* Define to 1 if you have the `ssl' library (-lssl). */
/* #undef LWS_HAVE_LIBSSL */

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#define LWS_HAVE_MALLOC 1

/* Define to 1 if you have the <memory.h> header file. */
#define LWS_HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define LWS_HAVE_MEMSET 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define LWS_HAVE_NETINET_IN_H 1

/* Define to 1 if your system has a GNU libc compatible `realloc' function,
   and to 0 otherwise. */
#define LWS_HAVE_REALLOC 1

/* Define to 1 if you have the `socket' function. */
#define LWS_HAVE_SOCKET 1

/* Define to 1 if you have the <stdint.h> header file. */
#define LWS_HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define LWS_HAVE_STDLIB_H 1

/* Define to 1 if you have the `strerror' function. */
#define LWS_HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define LWS_HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define LWS_HAVE_STRING_H 1

/* Define to 1 if you have the <sys/prctl.h> header file. */
/* #undef LWS_HAVE_SYS_PRCTL_H */

/* Define to 1 if you have the <sys/socket.h> header file. */
#define LWS_HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define LWS_HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define LWS_HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define LWS_HAVE_UNISTD_H 1

/* Define to 1 if you have the `vfork' function. */
#define LWS_HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef LWS_HAVE_VFORK_H */

/* Define to 1 if `fork' works. */
#define LWS_HAVE_WORKING_FORK 1

/* Define to 1 if `vfork' works. */
#define LWS_HAVE_WORKING_VFORK 1

/* Define to 1 if you have the <zlib.h> header file. */
#define LWS_HAVE_ZLIB_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#undef LT_OBJDIR // We're not using libtool

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

/* Define to rpl_realloc if the replacement function should be used. */
/* #undef realloc */

/* Define to 1 if we have getifaddrs */
#define LWS_HAVE_GETIFADDRS 1

/* Define if the inline keyword doesn't exist. */
/* #undef inline */


