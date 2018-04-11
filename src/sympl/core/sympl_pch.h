/**********************************************************
 * Author:  GameSencha, LLC
 * The MIT License (MIT)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 **********************************************************/
#ifndef __SYMPL_PCH_H__
#define __SYMPL_PCH_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <uchar.h>
#endif

#ifndef _WIN32
#include <sys/time.h>
#define w_gettimeofday gettimeofday

#include <dirent.h>
#include <getopt.h>
#include <unistd.h>
#else

#include "../thirdparty/direntutil.h"
#include "../thirdparty/getoptutil.h"
#include "../thirdparty/timeutil.h"
#include "../thirdparty/unistdutil.h"

#endif

// Types
typedef char char8;
typedef unsigned char uchar8;
typedef unsigned long ulong64;

#ifndef bool
typedef unsigned short bool;
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef NULL
#define NULL 0
#endif

#endif
