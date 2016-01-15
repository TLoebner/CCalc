/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2003
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@hzcnc.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)cuint_int.h: C language unit test library internal header
 * $Id: cunit_int.h,v 1.3 2006/03/18 04:53:21 zhangruiyi Exp $
 */

#ifndef __CUNIT_INTERN_H_INCLUDE__
#define __CUNIT_INTERN_H_INCLUDE__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cunit.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef _WIN32
#include <process.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_TIMEB_H
#include <sys/timeb.h>
#endif

#include "list.h"

/*
 *  header file of throw.c : cunit_throw.h
 */
/*=========================================================================
 * Macro definitions
 *=======================================================================*/
#define CUNIT_MAXIMUM_ROOTS     50

/*=========================================================================
 * Data structure definitions
 *=======================================================================*/
typedef union _cunit_cell_t {
    cell cell;
    cell *pointer;
} cunit_cell_t;

typedef struct _cunit_throwable_t {
    int type;
    char *message;
	int line;
	char *file;
	list_t linker;
} cunit_throwable_t;

/*=========================================================================
 * Function definitions
 *=======================================================================*/
CUNITEXT ThrowableScope _throwable_scope;

CUNITEXT Throwable cunit_new_throwable(CUNIT_SOURCE_LINE_DECL,
                                       IN int type, IN const char *message);

/*
 *  header file of output.c : cunit_output.h
 */
/*=========================================================================
 * Function definitions
 *=======================================================================*/
CUNITEXT char *cunit_format_message(IN char *message, IN int size, IN const char *format, ...);
CUNITEXT char *cunit_format_messagev(IN char *message, IN int size,
                                     IN const char *format, IN va_list pvar);
CUNITEXT void cunit_output_message(IN const char *format, ...);
CUNITEXT void cunit_output_messagev(IN const char *oformat, IN va_list pvar);


/*
 *  header file of runner.c : cunit_runner.h
 */
/*=========================================================================
 * Macro definitions
 *=======================================================================*/
#define CUNIT_FATAL_ERROR           127
#define CUNIT_UNCAUGHT_EXCEPTION    128
/*
 * This is a non-nesting use of setjmp/longjmp used solely for the purpose
 * of exiting the test runner in a clean way. By separating it out from the
 * exception mechanism above, we don't need to worry about whether or not
 * we are throwing an exception or exiting the runner in an CUNIT_CATCH block.
 */
#define CUNIT_START                                                 \
    {                                                               \
        jmp_buf __env__;                                            \
        cunit_set_runner_scope(&(__env__));                         \
        if (setjmp(__env__) == 0) {

#define CUNIT_EXIT(__code__)                                        \
        cunit_set_exit_code(__code__);                              \
        longjmp(*((jmp_buf*)cunit_get_runner_scope()),1)

#define CUNIT_FINISH(__code__)                                      \
        } else {                                                    \
            int __code__ = cunit_get_exit_code();

#define CUNIT_END_FINISH                                            \
        }                                                           \
    }

/*=========================================================================
 * Data structure definitions
 *=======================================================================*/
typedef TestResult (CUNITCBK *TestMain)(TestRunner, Test);
typedef struct _cunit_runner_t {
    TestMain main;
    //TestResult result;
} cunit_runner_t;

extern TestRunner _default_runner;
/*=========================================================================
 * Function definitions
 *=======================================================================*/
CUNITEXT void cunit_set_runner_scope(IN void *env);
CUNITEXT void *cunit_get_runner_scope();
CUNITEXT void cunit_set_exit_code(IN int code);
CUNITEXT int cunit_get_exit_code();

/*
 *  header file of listener.c : cunit_listener.h
 */
/*=========================================================================
 * Data structure definitions
 *=======================================================================*/
typedef struct _cunit_listener_t    *TestListener;

/* error or failure collector */
typedef void (CUNITCBK *ResultCollector)(TestListener, Test, Throwable);
typedef void (CUNITCBK *TestStart)(TestListener, Test);
typedef void (CUNITCBK *TestEnd)(TestListener, Test);

typedef struct _cunit_listener_t {
	/* error occurred. */
	ResultCollector add_error;
	/* failure occurred. */
 	ResultCollector add_failure;  
	/* test started flag */
	TestStart start_test;
	/* test ended flag */
 	TestEnd end_test; 
	/* list */
	list_t linker;
} cunit_listener_t;

/*
 *  header file of result.c : cunit_result.h
 */
/*=========================================================================
 * Data structure definitions
 *=======================================================================*/
typedef struct _cunit_result_t {
    /* failure collector */
    list_t failures;
    int failure_count;

    /* error collector */
	list_t errors;
    int error_count;

    /* registered listeners */
	list_t listeners;
	int listener_count;

    /* run tests */
    int runs;
    /* succeeded test count */
    int success_count;
    /* run seconds */
    double run_seconds;
    /* milli-seconds precision */
    unsigned short run_millitm;

    /* runner stopped */
    int stop;
} cunit_result_t;

/*=========================================================================
 * Function definitions
 *=======================================================================*/
CUNITEXT TestResult cunit_new_result();
CUNITEXT int cunit_should_stop(TestResult result);
CUNITEXT void cunit_add_failure(IN TestResult result, IN Test test, IN Throwable failure);
CUNITEXT void cunit_add_error(IN TestResult result, IN Test test, IN Throwable error);
CUNITEXT void cunit_delete_result(IN TestResult result);
CUNITEXT void cunit_start_test(IN TestResult result, IN Test test);
CUNITEXT void cunit_end_test(IN TestResult result, IN Test test);
CUNITEXT void cunit_register_listener(IN TestResult result, IN TestListener listener);

/*
 *  header file of test.c : cunit_test.h
 */
/*=========================================================================
 * Data structure definitions
 *=======================================================================*/
typedef int (*CaseCounter)(IN Test);
typedef void (*CaseRunner)(IN Test, IN TestResult);
typedef void (*CaseDestroyer)(IN Test);

typedef struct _cunit_test_t {
    /* test name */
    char *name;

    /* test counter */
    CaseCounter counter;
    /* test runner */
    CaseRunner runner;
    /* test destroyer */
    CaseDestroyer destroyer;

	/* case caller */
    TestCall call;
    /* bare test setUp */
    TestCall set_up;
    /* bare test tearDown */
    TestCall tear_down;

	/* pointer to parent */
	list_t *parent;
	/* pointer to sibling */
	list_t sibling;
	/* pointer to child */
	list_t *child;

} cunit_test_t;

/*=========================================================================
 * Function definitions
 *=======================================================================*/
CUNITEXP Test CUNITAPI cunit_new_test(IN const char *name, IN TestCall call, TestCall set_up, TestCall tear_down);
CUNITEXP void CUNITAPI cunit_add_test(IN Test suite, IN Test test);
CUNITEXP void cunit_delete_test(Test test);
CUNITEXT int cunit_count_test(Test test);
CUNITEXT void cunit_run_tests(Test test, TestResult result);
CUNITINT void __cunit_run_test(IN Test caze, IN TestResult result);

#endif /* __CUNIT_INTERN_H_INCLUDE__ */
