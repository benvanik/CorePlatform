/*
 *  CPByteOrder.h
 *  CorePlatform
 *
 *  Created by Ben Vanik on 5/07/11.
 *  Copyright 2011 Ben Vanik. All rights reserved.
 *
 */

#include <CorePlatform/CPPlatform.h>
#include <CorePlatform/CPConfig.h>
#include <CorePlatform/CPTypes.h>
#include <CorePlatform/CPAnnotations.h>
#include <CorePlatform/CPCodeUtilities.h>

#if CP_COMPILER(MSVC)
#define cpswap16                        _byteswap_ushort
#define cpswap32                        _byteswap_ulong
#define cpswap64                        _byteswap_uint64
#else
#define cpswap16                        bswap_16
#define cpswap32                        bswap_32
#define cpswap64                        bswap_64
#endif

#if CP_CPU(BIGENDIAN)
#define cpswap16be(p)                   (p)
#define cpswap32be(p)                   (p)
#define cpswap64be(p)                   (p)
#define cpswap16le(p)                   cpswap16(p)
#define cpswap32le(p)                   cpswap32(p)
#define cpswap64le(p)                   cpswap64(p)
#else
#define cpswap16be(p)                   cpswap16(p)
#define cpswap32be(p)                   cpswap32(p)
#define cpswap64be(p)                   cpswap64(p)
#define cpswap16le(p)                   (p)
#define cpswap32le(p)                   (p)
#define cpswap64le(p)                   (p)
#endif

#define cpgetint8be(p)                  (  (int8)(*(p)))
#define cpgetuint8be(p)                 ( (uint8)(*(p)))
#define cpgetint16be(p)                 ( (int16)cpswap16be(*(uint16*)(p)))
#define cpgetuint16be(p)                ((uint16)cpswap16be(*(uint16*)(p)))
#define cpgetint32be(p)                 ( (int32)cpswap32be(*(uint32*)(p)))
#define cpgetuint32be(p)                ((uint32)cpswap32be(*(uint32*)(p)))
#define cpgetint64be(p)                 ( (int64)cpswap64be(*(uint64*)(p)))
#define cpgetuint64be(p)                ((uint64)cpswap64be(*(uint64*)(p)))
#define cpgetint8le(p)                  (  (int8)(*(p)))
#define cpgetuint8le(p)                 ( (uint8)(*(p)))
#define cpgetint16le(p)                 ( (int16)cpswap16le(*(uint16*)(p)))
#define cpgetuint16le(p)                ((uint16)cpswap16le(*(uint16*)(p)))
#define cpgetint32le(p)                 ( (int32)cpswap32le(*(uint32*)(p)))
#define cpgetuint32le(p)                ((uint32)cpswap32le(*(uint32*)(p)))
#define cpgetint64le(p)                 ( (int64)cpswap64le(*(uint64*)(p)))
#define cpgetuint64le(p)                ((uint64)cpswap64le(*(uint64*)(p)))

