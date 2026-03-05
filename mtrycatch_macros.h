/*
 * mtrycatch_macros.h -- a header file defining macros used in mtrycatch.h
 * version 0.9.0, Mar. 6, 2026
 *
 * License: zlib License
 *
 * Copyright (c) 2026 Kazushi Yamasaki
 *
 * This software is provided ‘as-is’, without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 */

#pragma once

#ifndef MTRYCATCH_MACROS_H
#define MTRYCATCH_MACROS_H


#ifndef MTRYCATCH_CPP_C_BEGIN
	#ifdef __cplusplus  /* C++ */
		#define MTRYCATCH_CPP_C_BEGIN extern "C" {
		#define MTRYCATCH_CPP_C_END }
	#else               /* not C++ */
		#define MTRYCATCH_CPP_C_BEGIN
		#define MTRYCATCH_CPP_C_END
	#endif
#endif


#ifndef THREAD_LOCAL
	/* C++11 or later */
	#if defined(__cplusplus) && (__cplusplus >= 201103L)
		#define THREAD_LOCAL thread_local

	/* GCC or Clang */
	#elif defined(__GNUC__)
		#define THREAD_LOCAL __thread

	/* MSVC */
	#elif defined(_MSC_VER)
		#define THREAD_LOCAL __declspec(thread)

	/* C (other compiler) */
	#elif defined(__STDC_VERSION__) && !defined(__cplusplus)
		/* C23 or later */
		#if __STDC_VERSION__ >= 202311L
			#define THREAD_LOCAL thread_local

		/* C11 or later (support optional standard threads) */
		#elif (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_THREADS__)
			#define THREAD_LOCAL _Thread_local
		#endif
	#endif
#endif  /* THREAD_LOCAL */


/* Be careful, as this macro is not wrapped in a do { ... } while(0) block. */
#ifndef SHADOW_IGNORE_BLOCK
	#if defined(__GNUC__)
		#define SHADOW_IGNORE_BLOCK(code) _Pragma("GCC diagnostic push") \
			_Pragma("GCC diagnostic ignored \"-Wshadow\"") \
				code ; \
			_Pragma("GCC diagnostic pop")
	#elif defined(_MSC_VER)
		#define SHADOW_IGNORE_BLOCK(code) __pragma(warning(push)) \
			__pragma(warning(disable : 4456 4457 4458 4459)) \
				code ; \
			__pragma(warning(pop))
	#else
		#define SHADOW_IGNORE_BLOCK(code) code ;
	#endif
#endif  /* SHADOW_IGNORE_BLOCK */


#endif
