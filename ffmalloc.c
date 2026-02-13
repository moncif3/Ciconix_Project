// On x86_64 allocation alignment is usually 16-byte. However, this is only
// required to support certain SSE instructions. If those are not used then
// alignment can be 8-byte and therefore more efficient. Pointers don't seem
// to ever require 16-byte allignment and so 8-byte alignment will always be 
// used for allocations of 8 bytes or less. This is backed up in practice by
// TCMalloc. To enable 8-byte alignment, define FF_EIGHTBYTEALIGN during
// library compilation
//#define FF_EIGHTBYTEALIGN

/*** Headers ***/
#include "ffmalloc.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN64
#ifdef FF_PROFILE
#include <Psapi.h>
#endif
#else
#include <unistd.h>
#include <sys/mman.h>
#include <limits.h>
#include <errno.h>

#ifndef FFSINGLE_THREADED
#include <sched.h>
#include <pthread.h>
#endif
#ifdef FF_PROFILE
#include <sys/time.h>
#include <sys/resource.h>
#endif

// Caution - defining this symbol allows cross compilation on older Linuxes
// however running on those kernels risks ffmalloc overwriting other
// memory allocations. ffmalloc is only intended for Linux kernel 4.17 or later
#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE    0x100000
#endif
#endif

// MAP_POPULATE is Linux-specific.
#ifndef MAP_POPULATE
#define MAP_POPULATE 0
#endif

typedef unsigned char byte;

/*** Library Constants ***/
