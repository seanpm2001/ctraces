/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  CTraces
 *  =======
 *  Copyright 2022 Eduardo Silva <eduardo@calyptia.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <ctraces/ctraces.h>
#include <ctraces/ctr_span.h>

#include <cfl/cfl.h>
#include <cfl/cfl_array.h>
#include <cfl/cfl_kvlist.h>

#include "ctr_tests.h"

void test_span()
{
    int ret;
    struct ctrace *ctx;
    struct ctrace_span *span_root;
    struct ctrace_span *span_child;
    struct cfl_array *array;
    struct cfl_kvlist *kvlist;

    ctx = ctr_create(NULL);

    span_root = ctr_span_create(ctx, "main", NULL);
    TEST_CHECK(span_root != NULL);
    TEST_CHECK(span_root->kind == CTRACE_SPAN_INTERNAL  );

    span_child = ctr_span_create(ctx, "do-work", span_root);
    TEST_CHECK(span_child != NULL);

    /* set span kind */
    ret = ctr_span_kind_set(span_child, CTRACE_SPAN_CONSUMER);
    TEST_CHECK(ret == 0);
    TEST_CHECK(span_child->kind == CTRACE_SPAN_CONSUMER);

    /* parent id check */
    TEST_CHECK(span_child->parent_span_id == span_root->id);

    /* add attributes to span_child */
    ctr_span_set_attribute_string(span_child, "agent", "fluent bit");
    ctr_span_set_attribute_bool(span_child, "bool_t", 1);
    ctr_span_set_attribute_bool(span_child, "bool_f", 0);
    ctr_span_set_attribute_int(span_child, "integer", 123456789);
    ctr_span_set_attribute_double(span_child, "double", 1.5);

    array = cfl_array_create(128);
    TEST_CHECK(array != NULL);
    ctr_span_set_attribute_array(span_child, "array", array);

    kvlist = cfl_kvlist_create();
    TEST_CHECK(kvlist != NULL);
    ctr_span_set_attribute_kvlist(span_child, "kvlist", kvlist);

    ctr_destroy(ctx);
}

TEST_LIST = {
    {"span", test_span},
    { 0 }
};
