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
 * @(#)test.c: cunit test interface implementation
 * $Id: test.c,v 1.4 2006/03/18 04:53:21 zhangruiyi Exp $
 */

#include "cunit_int.h"

CUNITEXP char * CUNITAPI cunit_get_name(IN Test test) {
    if (test)
        return test->name;
    else
        return NULL;
}

/*CUNITEXP Test CUNITAPI cunit_new_test(IN const char *name) {
	return cunit_new_test_ex(name, NULL, NULL, NULL);
}

CUNITEXP Test CUNITAPI cunit_new_test_case(IN const char *name, IN TestCall call) {
	return cunit_new_test_ex(name, call, NULL, NULL);
}*/

CUNITEXP Test CUNITAPI cunit_new_test(IN const char *name, IN TestCall call, 
										 TestCall set_up, TestCall tear_down) {
	Test test = malloc(sizeof (struct _cunit_test_t));
	if (test) {
		memset(test, 0, sizeof (struct _cunit_test_t));
		test->runner = (CaseRunner)cunit_run_tests;
		test->counter = (CaseCounter)cunit_count_test;
		test->destroyer = (CaseDestroyer)cunit_delete_test;

		if (name) 
			test->name = strdup(name);
		test->set_up = set_up;
		test->tear_down = tear_down;
		test->call = call;

		test->child = malloc(sizeof (list_t));
		if (!test->child)
			cunit_fatal_error("Memory allocation failed!");

		test->parent = NULL;
		list_init(&test->sibling);
		list_init(test->child);
	}

	return test;
}

CUNITEXP void CUNITAPI cunit_add_test(IN Test suite, IN Test test) {
	test->parent = &suite->sibling;
	list_insert_tail(&test->sibling, suite->child);
}

CUNITEXP void cunit_delete_test(Test test) {
	Test tmp;
	list_t *pos, *n;

	if (test) {
		if (list_empty(test->child)) {
			free(test->child);
			free(test->name);

			free(test);
		}
		else {
			list_iterate_forward(pos, n, test->child) {
				tmp = list_entry(pos, struct _cunit_test_t, sibling);
				if (tmp && tmp->destroyer)
					tmp->destroyer(tmp);
			}
		}
	}
}

CUNITEXT int cunit_count_test(Test test) {
	Test tmp;
	list_t *pos, *n;
	int count = 0;

	if (list_empty(test->child)) 
		return 1;
	
	list_iterate_forward(pos, n, test->child) {
		tmp = list_entry(pos, struct _cunit_test_t, sibling);
		if (tmp && tmp->counter)
			count += tmp->counter(tmp);
	}
	return count;
}

CUNITEXT void cunit_run_tests(Test test, TestResult result) {
	Test tmp;
	list_t *pos, *n;
	if (test && test->runner) {
		/* case do not include case */
		if (list_empty(test->child)) 
			__cunit_run_test(test, result);

		else {
			list_iterate_forward(pos, n, test->child) {
				if (cunit_should_stop(result))
					break;
				/* fetch and retrun test */
				tmp = list_entry(pos, struct _cunit_test_t, sibling);
				if (tmp && tmp->runner)
					tmp->runner(tmp, result);
			}
		}
	}
}

CUNITINT void __cunit_run_test(IN Test test, IN TestResult result) {
    int caught = FALSE;

    /* mark start of test case */
    cunit_start_test(result, test);
    if (test->set_up)
        (test->set_up)();
    /* run test in the TRY CATCH scope */
    CUNIT_TRY {
        if (test && test->call) {
            /* callback test */
            (test->call)();
        }
        else
            cunit_fatal_error("Can not run null case object!");
    } CUNIT_CATCH(e) {
        if (e->type == CUNIT_ASSERTION) {
            cunit_add_failure(result, test, e);
        } else {
            cunit_add_error(result, test, e);
        }
        caught = TRUE;
    } CUNIT_END_CATCH
    if (test->tear_down)
        (test->tear_down)();
    if (!caught) {
        result->success_count++;
    }
    /* mark end of test test */
    cunit_end_test(result, test);
}