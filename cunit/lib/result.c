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
 * @(#)case.c: cunit test result implementation
 * $Id: result.c,v 1.2 2006/03/11 12:47:05 zhangruiyi Exp $
 */

#include "cunit_int.h"
#include <stdio.h>

CUNITEXT int cunit_should_stop(TestResult result) {
    return result->stop;
}

/*
 * result contructor.
 */
CUNITEXT TestResult cunit_new_result() {
    TestResult result = malloc(sizeof (struct _cunit_result_t));

    if (result) {
        memset(result, 0, sizeof (struct _cunit_result_t));

        result->success_count = 0;
        result->runs = 0;

        /* initialize Failure vector */
        result->failure_count = 0;
		list_init(&result->failures);

        /* initialize Error vector */
        result->error_count = 0;
		list_init(&result->errors);

        /* initialize TestListener vector */
        result->listener_count = 0;
		list_init(&result->listeners);

        result->stop = FALSE;
    } else
        cunit_fatal_error("Memory allocation failure!");
    return result;
}

CUNITEXT void cunit_register_listener(IN TestResult result, IN TestListener listener) {
    if (result) {
		list_init(&listener->linker);
		list_insert_tail(&listener->linker, &result->listeners);
		result->listener_count++;
	}
}

CUNITEXT void cunit_add_failure(IN TestResult result, IN Test test, IN Throwable failure) {
	list_t *pos, *n;
	TestListener listener;
	
    if (result) {
		list_init(&failure->linker);
		list_insert_tail(&failure->linker, &result->failures);
		result->failure_count++;

		list_iterate_forward(pos, n, &result->listeners) {
			listener = list_entry(pos, struct _cunit_listener_t, linker);
			if (listener && listener->add_failure)
				listener->add_failure(listener, test, failure);
		}
	}
}

CUNITEXT void cunit_add_error(IN TestResult result, IN Test test, IN Throwable error) {
	list_t *pos, *n;
	TestListener listener;

    if (result) {
		list_init(&error->linker);
		list_insert_tail(&error->linker, &result->errors);
		result->error_count++;

		list_iterate_forward(pos, n, &result->listeners) {
			listener = list_entry(pos, struct _cunit_listener_t, linker);

			if (listener && listener->add_error)
				listener->add_error(listener, test, error);
		}
	}
}

CUNITEXT void cunit_start_test(IN TestResult result, IN Test test) {
	list_t *pos, *n;
	TestListener listener;
    int count = 0;
    
    if (test)
        count = test->counter(test);
    result->runs += count;

	list_iterate_forward(pos, n, &result->listeners) {
		listener = list_entry(pos, struct _cunit_listener_t, linker);
		if (listener && listener->start_test)
			listener->start_test(listener, test);
	}
}

CUNITEXT void cunit_end_test(IN TestResult result, IN Test test) {
	list_t *pos, *n;
	TestListener listener;

	list_iterate_forward(pos, n, &result->listeners) {
		listener = list_entry(pos, struct _cunit_listener_t, linker);
		if (listener && listener->end_test)
			listener->end_test(listener, test);
	}
}

/*
 * result destructor.
 */
CUNITEXT void cunit_delete_result(IN TestResult result) {
	list_t *pos, *n;
	Throwable error, failure; 

    if (result) {
        if (!list_empty(&result->errors)) {
			list_iterate_forward(pos, n, &result->errors) {
				error = list_entry(pos, struct _cunit_throwable_t, linker);
				cunit_delete_throwable(error);
			}
			list_init(&result->errors);
        }

        if (!list_empty(&result->failures)) {
			list_iterate_forward(pos, n, &result->failures) {
				failure = list_entry(pos, struct _cunit_throwable_t, linker);
				cunit_delete_throwable(failure);
			}
			list_init(&result->failures);
		}

        if (!list_empty(&result->listeners)) {
            /*
             * Do not destroy listener here.
             * Listeners should be maintained by runner implementation
             * as it may be permanent.
             */
			list_init(&result->listeners);
		}
        free(result);
	}
}
