/*
 * Copyright (c) 2010-2014 Petri Lehtinen <petri@digip.org>
 *
 * Jansson is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 *
 *
 * This file specifies a part of the site-specific configuration for
 * Jansson, namely those things that affect the public API in
 * jansson.h.
 *
 * The CMake system will generate the jansson_config.h file and
 * copy it to the build and install directories.
 */

#ifndef JANSSON_CONFIG_H
#define JANSSON_CONFIG_H

/* 
 
 All defines in .pri file.
 
 */

#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#endif
