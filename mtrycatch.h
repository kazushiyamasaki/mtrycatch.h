/*
 * mtrycatch.h -- a header library defining macros for structured exception
 *                handling in C with support for nested TRY-CATCH-FINALLY
 *                blocks
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
 *
 *
 *
 * IMPORTANT:
 * Local variables that must be modified inside a TRY block and also used in
 * CATCH or FINALLY blocks must be declared as 'volatile'.
 * If RETHROW() is used, any variables updated in the CATCH block are also
 * subject to this requirement.
 *
 * Never use functions that terminate programs, threads, replace processes, or
 * perform jumps within TRY, CATCH or FINALLY blocks, as they may prevent
 * FINALLY from executing and break the exception stack cleanup.
 * If you need to exit early from a TRY, CATCH or FINALLY blocks, use 'break'
 * instead.
 *
 * THROW() must be within a TRY block, and RETHROW() must be within a CATCH
 * block.
 *
 *
 * Example usage:
 *
 *    #include "mtrycatch.h"
 *
 *    BEGIN_THROW_CODES    // SUCCESS = 0 is automatically defined
 *        EXCEPTION_OUT_OF_MEMORY
 *    END_THROW_CODES
 *
 *    void func() {
 *        TRY {
 *            // Any code
 *            if (something_failed()) {
 *                THROW(EXCEPTION_OUT_OF_MEMORY);
 *            }
 *        } CATCH(EXCEPTION_OUT_OF_MEMORY) {
 *            // Exception handling
 *        } FINALLY {      // Optional
 *            // Post processing
 *        } END_TRY;       // Required
 *    }
 */

#pragma once

#ifndef MTRYCATCH_H
#define MTRYCATCH_H


#include "mtrycatch_macros.h"


MTRYCATCH_CPP_C_BEGIN


#if !defined (__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
#error "This program requires C99 or higher."
#endif


#include <stdio.h>
#include <setjmp.h>


#define MAX_EX_DEPTH 32


typedef struct {
	jmp_buf stack[MAX_EX_DEPTH];
	int top;
} ex_stack_t;


#ifdef THREAD_LOCAL
	static THREAD_LOCAL ex_stack_t _ex_stack = { .top = 0 };
#else
	#error "THREAD_LOCAL not supported"
#endif


#define _INTERNAL_JUMP_FINALLY_MACRO -1


#define THROW(throw_code) do { \
	if (_ex_stack.top <= 0) { \
		fprintf(stderr, "Exception stack underflow\nFile: %s   Line: %d\n", __FILE__, __LINE__); \
		abort(); \
	} \
	longjmp(_ex_stack.stack[_ex_stack.top - 1], throw_code); \
} while(0)

#define RETHROW() do { \
	_rethrow_ex_code = _ex_code; \
	THROW(_INTERNAL_JUMP_FINALLY_MACRO); \
} while(0)


#define TRY do { \
	if (_ex_stack.top >= MAX_EX_DEPTH) { \
		fprintf(stderr, "Exception stack overflow\nFile: %s   Line: %d\n", __FILE__, __LINE__); \
		abort(); \
	} \
	SHADOW_IGNORE_BLOCK(volatile int _rethrow_ex_code = 0); \
	do { /* block for compatibility with END_TRY */ \
		SHADOW_IGNORE_BLOCK(int _ex_code = setjmp(_ex_stack.stack[_ex_stack.top++])); \
		switch (_ex_code) { \
			default: /* dummy default to suppress warnings */ break; \
			case _INTERNAL_JUMP_FINALLY_MACRO: /* RETHROW */ break; \
			case 0:

#define CATCH(throw_code) break; \
			case throw_code:

#define FINALLY break; } /* switch end */ \
	} while(0); do {   { /* block for compatibility with END_TRY */

#define END_TRY break; } /* end of FINALLY or switch */ \
	} while(0); \
	_ex_stack.top--; \
	if (_rethrow_ex_code != 0) /* RETHROW check */ \
		THROW(_rethrow_ex_code); \
} while(0)


#define BEGIN_THROW_CODES typedef enum { \
	_INTERNAL_JUMP_FINALLY = _INTERNAL_JUMP_FINALLY_MACRO, \
	SUCCESS = 0,

#define END_THROW_CODES } ThrowCode;


MTRYCATCH_CPP_C_END


#endif
