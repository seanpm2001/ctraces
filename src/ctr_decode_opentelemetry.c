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

#include <opentelemetry/opentelemetry_trace_service.pb-c.h>
#include <ctraces/ctraces.h>
#include <cfl/cfl_array.h>

static int ctr_convert_any_value(struct opentelemetry_decode_value *ctr_val,
           opentelemetry_decode_value_type value_type, char *key,
           Opentelemetry__Proto__Common__V1__AnyValue *val);

static int ctr_convert_string_value(struct opentelemetry_decode_value *ctr_val,
                                    opentelemetry_decode_value_type value_type,
                                    char *key, char *val)
{
    int result;

    result = -2;

    switch (value_type) {

        case CTR_OPENTELEMETRY_TYPE_ATTRIBUTE:
            result = ctr_attributes_set_string(ctr_val->ctr_attr, key, val);
            break;

        case CTR_OPENTELEMETRY_TYPE_ARRAY:
            result = cfl_array_append_string(ctr_val->cfl_arr, val);
            break;

        case CTR_OPENTELEMETRY_TYPE_KVLIST:
            result = cfl_kvlist_insert_string(ctr_val->cfl_kvlist, key, val);
            break;

    }

    if (result == -2) {
        printf("ctr_convert_string_value: unknown value type");
    }

    return result;
}

static int ctr_convert_bool_value(struct opentelemetry_decode_value *ctr_val,
                                            opentelemetry_decode_value_type value_type,
                                            char *key, protobuf_c_boolean val)
{
    int result;

    result = -2;

    switch (value_type) {

        case CTR_OPENTELEMETRY_TYPE_ATTRIBUTE:
            result = ctr_attributes_set_bool(ctr_val->ctr_attr, key, val);
            break;

        case CTR_OPENTELEMETRY_TYPE_ARRAY:
            result = cfl_array_append_bool(ctr_val->cfl_arr, val);
            break;

        case CTR_OPENTELEMETRY_TYPE_KVLIST:
            result = cfl_kvlist_insert_bool(ctr_val->cfl_kvlist, key, val);
            break;

    }

    if (result == -2) {
        printf("ctr_convert_bool_value: unknown value type");
    }

    return result;
}

static int ctr_convert_int_value(struct opentelemetry_decode_value *ctr_val,
                                           opentelemetry_decode_value_type value_type,
                                           char *key, int64_t val)
{
    int result;

    result = -2;

    switch (value_type) {

        case CTR_OPENTELEMETRY_TYPE_ATTRIBUTE:
            result = ctr_attributes_set_int64(ctr_val->ctr_attr, key, val);
            break;

        case CTR_OPENTELEMETRY_TYPE_ARRAY:
            result = cfl_array_append_int64(ctr_val->cfl_arr, val);
            break;

        case CTR_OPENTELEMETRY_TYPE_KVLIST:
            result = cfl_kvlist_insert_int64(ctr_val->cfl_kvlist, key, val);
            break;

    }

    if (result == -2) {
        printf("ctr_convert_int_value: unknown value type");
    }

    return result;
}

static int ctr_convert_double_value(struct opentelemetry_decode_value *ctr_val,
                                    opentelemetry_decode_value_type value_type,
                                    char *key, double val)
{
    int result;

    result = -2;

    switch (value_type) {

        case CTR_OPENTELEMETRY_TYPE_ATTRIBUTE:
            result = ctr_attributes_set_double(ctr_val->ctr_attr, key, val);
            break;

        case CTR_OPENTELEMETRY_TYPE_ARRAY:
            result = cfl_array_append_double(ctr_val->cfl_arr, val);
            break;

        case CTR_OPENTELEMETRY_TYPE_KVLIST:
            result = cfl_kvlist_insert_double(ctr_val->cfl_kvlist, key, val);
            break;

    }

    if (result == -2) {
        printf("ctr_convert_double_value: unknown value type");
    }

    return result;
}

static int ctr_convert_array_value(struct opentelemetry_decode_value *ctr_val,
           opentelemetry_decode_value_type value_type,
           char *key, Opentelemetry__Proto__Common__V1__ArrayValue *otel_arr)
{
    int array_index;
    int result;
    struct opentelemetry_decode_value *ctr_arr_val;
    Opentelemetry__Proto__Common__V1__AnyValue *val;

    ctr_arr_val = malloc(sizeof(struct opentelemetry_decode_value));

    ctr_arr_val->cfl_arr = cfl_array_create(otel_arr->n_values);

    result = 0;

    for (array_index = 0;
         array_index < otel_arr->n_values && result == 0;
         array_index++) {
        val = otel_arr->values[array_index];
        result = ctr_convert_any_value(ctr_arr_val, CTR_OPENTELEMETRY_TYPE_ARRAY, NULL, val);
    }

    if (result < 0) {
        free(ctr_arr_val->cfl_arr);
        return result;
    }

    result = -2;

    switch (value_type) {

        case CTR_OPENTELEMETRY_TYPE_ATTRIBUTE:
            result = ctr_attributes_set_array(ctr_val->ctr_attr, key, ctr_arr_val->cfl_arr);
            break;

        case CTR_OPENTELEMETRY_TYPE_ARRAY:
            result = cfl_array_append_array(ctr_val->cfl_arr, ctr_arr_val->cfl_arr);
            break;

        case CTR_OPENTELEMETRY_TYPE_KVLIST:
            result = cfl_kvlist_insert_array(ctr_val->cfl_kvlist, key, ctr_arr_val->cfl_arr);
            break;

    }

    if (result == -2) {
        printf("ctr_convert_array_value: unknown value type");
    }

    return result;
}

static int ctr_convert_kvlist_value(struct opentelemetry_decode_value *ctr_val,
           opentelemetry_decode_value_type value_type,
           char *key, Opentelemetry__Proto__Common__V1__KeyValueList *otel_kvlist)
{
    int kvlist_index;
    int result;
    struct opentelemetry_decode_value *ctr_kvlist_val;
    Opentelemetry__Proto__Common__V1__KeyValue *kv;

    ctr_kvlist_val = malloc(sizeof(struct opentelemetry_decode_value));
    ctr_kvlist_val->cfl_kvlist = cfl_kvlist_create();

    result = 0;

    for (kvlist_index = 0;
         kvlist_index < otel_kvlist->n_values && result ==0;
         kvlist_index++) {

        kv = otel_kvlist->values[kvlist_index];
        result = ctr_convert_any_value(ctr_kvlist_val, CTR_OPENTELEMETRY_TYPE_KVLIST, kv->key, kv->value);
    }

    if (result < 0){
        free(ctr_kvlist_val->cfl_kvlist);
        return result;
    }

    result = -2;

    switch (value_type) {

        case CTR_OPENTELEMETRY_TYPE_ATTRIBUTE:
            result = ctr_attributes_set_kvlist(ctr_val->ctr_attr, key, ctr_kvlist_val->cfl_kvlist);
            break;

        case CTR_OPENTELEMETRY_TYPE_ARRAY:
            result = cfl_array_append_kvlist(ctr_val->cfl_arr, ctr_kvlist_val->cfl_kvlist);
            break;

        case CTR_OPENTELEMETRY_TYPE_KVLIST:
            result = cfl_kvlist_insert_kvlist(ctr_val->cfl_kvlist, key, ctr_kvlist_val->cfl_kvlist);
            break;

    }

    if (result == -2) {
        printf("ctr_convert_kvlist_value: unknown value type");
    }

    return result;
}

static int ctr_convert_bytes_value(struct opentelemetry_decode_value *ctr_val,
                                    opentelemetry_decode_value_type value_type,
                                    char *key, void *buf, size_t len)
{
    int result;

    result = -2;

    switch (value_type) {

        case CTR_OPENTELEMETRY_TYPE_ATTRIBUTE:
            result = -1;
            break;

        case CTR_OPENTELEMETRY_TYPE_ARRAY:
            result = cfl_array_append_bytes(ctr_val->cfl_arr, buf, len);
            break;

        case CTR_OPENTELEMETRY_TYPE_KVLIST:
            result = cfl_kvlist_insert_bytes(ctr_val->cfl_kvlist, key, buf, len);
            break;

    }

    if (result == -2) {
        printf("ctr_convert_bytes_value: unknown value type");
    }

    return result;
}

static int ctr_convert_any_value(struct opentelemetry_decode_value *ctr_val,
           opentelemetry_decode_value_type value_type, char *key,
           Opentelemetry__Proto__Common__V1__AnyValue *val)
{
    int result;

    result = -2;

    switch (val->value_case) {
        case OPENTELEMETRY__PROTO__COMMON__V1__ANY_VALUE__VALUE__NOT_SET:
            result = -1;
            break;

        case OPENTELEMETRY__PROTO__COMMON__V1__ANY_VALUE__VALUE_STRING_VALUE:
            result = ctr_convert_string_value(ctr_val, value_type, key, val->string_value);
            break;

        case OPENTELEMETRY__PROTO__COMMON__V1__ANY_VALUE__VALUE_BOOL_VALUE:
            result = ctr_convert_bool_value(ctr_val, value_type, key, val->bool_value);
            break;

        case OPENTELEMETRY__PROTO__COMMON__V1__ANY_VALUE__VALUE_INT_VALUE:
            result = ctr_convert_int_value(ctr_val, value_type, key, val->int_value);
            break;

        case OPENTELEMETRY__PROTO__COMMON__V1__ANY_VALUE__VALUE_DOUBLE_VALUE:
            result = ctr_convert_double_value(ctr_val, value_type, key, val->double_value);
            break;

        case OPENTELEMETRY__PROTO__COMMON__V1__ANY_VALUE__VALUE_ARRAY_VALUE:
            result = ctr_convert_array_value(ctr_val, value_type, key, val->array_value);
            break;

        case OPENTELEMETRY__PROTO__COMMON__V1__ANY_VALUE__VALUE_KVLIST_VALUE:
            result = ctr_convert_kvlist_value(ctr_val, value_type, key, val->kvlist_value);
            break;

        case OPENTELEMETRY__PROTO__COMMON__V1__ANY_VALUE__VALUE_BYTES_VALUE:
            result = ctr_convert_bytes_value(ctr_val, value_type, key, val->bytes_value.data, val->bytes_value.len);
            break;
        }

        if (result == -2) {
            printf("ctr_convert_any_value: unknown value type");
        }

    return result;
}

static struct ctrace_attributes *convert_otel_attrs(size_t n_attributes,
              Opentelemetry__Proto__Common__V1__KeyValue **otel_attr)
{
    int index_kv;
    int result;
    char *key;
    struct opentelemetry_decode_value *ctr_decoded_attributes;

    Opentelemetry__Proto__Common__V1__KeyValue *kv;
    Opentelemetry__Proto__Common__V1__AnyValue *val;

    ctr_decoded_attributes = malloc(sizeof(struct opentelemetry_decode_value));
    ctr_decoded_attributes->ctr_attr = ctr_attributes_create();

    result = 0;

    for (index_kv = 0; index_kv < n_attributes && result == 0; index_kv++) {
        kv = otel_attr[index_kv];

        key = kv->key;
        val = kv->value;

        result = ctr_convert_any_value(ctr_decoded_attributes,
                                       CTR_OPENTELEMETRY_TYPE_ATTRIBUTE,
                                       key, val);
    }

    if (result < 0) {
        return NULL;
    }

    return ctr_decoded_attributes->ctr_attr;
}

static int ctr_span_set_attributes(struct ctrace_span *span,
           size_t n_attributes,
           Opentelemetry__Proto__Common__V1__KeyValue **attributes)
{
    struct ctrace_attributes *ctr_attributes;

    ctr_attributes = convert_otel_attrs(n_attributes, attributes);

    if (ctr_attributes == NULL) {
        return -1;
    }

    span->attr = ctr_attributes;
    return 0;
}

static int ctr_span_set_events(struct ctrace_span *span,
           size_t n_events,
           Opentelemetry__Proto__Trace__V1__Span__Event **events)
{
    int index_event;
    struct ctrace_span_event *ctr_event;
    struct ctrace_attributes *ctr_attributes;
    Opentelemetry__Proto__Trace__V1__Span__Event *event;

    cfl_list_init(&span->events);

    for (index_event = 0; index_event < n_events; index_event++) {
        event = events[index_event];

        ctr_event = ctr_span_event_add_ts(span, event->name, event->time_unix_nano);

        if (ctr_event == NULL) {
            return -1;
        }

        ctr_attributes = convert_otel_attrs(event->n_attributes, event->attributes);

        if (ctr_attributes == NULL) {
            return -1;
        }

        ctr_event->attr = ctr_attributes;
        ctr_span_event_set_dropped_attributes_count(ctr_event, event->dropped_attributes_count);
    }

    return 0;
}

int ctr_resource_set_data(struct ctrace_resource *resource,
    Opentelemetry__Proto__Resource__V1__Resource *otel_resource)
{
    struct ctrace_attributes *ctr_attributes;

    ctr_attributes = convert_otel_attrs(otel_resource->n_attributes, otel_resource->attributes);

    if (ctr_attributes == NULL) {
        return -1;
    }

    ctr_resource_set_attributes(resource, ctr_attributes);
    ctr_resource_set_dropped_attr_count(resource, otel_resource->dropped_attributes_count);

    return 0;
}

void ctr_scope_span_set_scope(struct ctrace_scope_span *scope_span,
     Opentelemetry__Proto__Common__V1__InstrumentationScope *scope)
{
    struct ctrace_attributes *ctr_attributes;

    ctr_attributes = convert_otel_attrs(scope->n_attributes, scope->attributes);

    if (ctr_attributes == NULL) {
        return;
    }

    ctr_instrumentation_scope_create(scope->name, scope->version,
                                     scope->dropped_attributes_count,
                                     ctr_attributes);

}

void ctr_span_set_links(struct ctrace_span *ctr_span, size_t n_links,
     Opentelemetry__Proto__Trace__V1__Span__Link **links)
{
    int index_link;
    struct ctrace_link *ctr_link;
    struct ctrace_attributes *ctr_attributes;
    Opentelemetry__Proto__Trace__V1__Span__Link *link;

    for (index_link = 0; index_link < n_links; index_link++) {
        link = links[index_link];

        ctr_link = ctr_link_create(ctr_span,
                                   link->trace_id.data, link->trace_id.len,
                                   link->span_id.data, link->span_id.len);

        if (ctr_link == NULL) {
            return;
        }

        ctr_attributes = convert_otel_attrs(link->n_attributes, link->attributes);

        if (ctr_attributes == NULL) {
            return;
        }

        ctr_link->attr = ctr_attributes;
        ctr_link_set_dropped_attr_count(ctr_link, link->dropped_attributes_count);
    }

}

int ctr_decode_opentelemetry_create(struct ctrace **out_ctr,
                                    char *in_buf,
                                    size_t in_size, size_t *offset)
{
    size_t resource_span_index;
    size_t scope_span_index;
    size_t span_index;

    struct ctrace *ctr;
    struct ctrace_opts *opts;
    struct ctrace_span *ctr_span;
    struct ctrace_resource *ctr_resource;
    struct ctrace_resource_span *ctr_resource_span;
    struct ctrace_scope_span *ctr_scope_span;

    Opentelemetry__Proto__Collector__Trace__V1__ExportTraceServiceRequest *service_request;
    Opentelemetry__Proto__Trace__V1__ResourceSpans *otel_resource_span;
    Opentelemetry__Proto__Trace__V1__ScopeSpans *otel_scope_span;
    Opentelemetry__Proto__Trace__V1__Span *otel_span;

    service_request = opentelemetry__proto__collector__trace__v1__export_trace_service_request__unpack(NULL, in_size - *offset, (unsigned char *) &in_buf[*offset]);

    if (service_request == NULL) {
        return -1;
    }

    opts = calloc(1, sizeof(struct ctrace_opts));
    ctr_opts_init(opts);
    ctr = ctr_create(opts);

    for(resource_span_index = 0; resource_span_index < service_request->n_resource_spans; resource_span_index++) {
        otel_resource_span = service_request->resource_spans[resource_span_index];
        if (otel_resource_span == NULL) {
            return -1;
        }

        ctr_resource = ctr_resource_create(ctr);
        ctr_resource_set_data(ctr_resource, otel_resource_span->resource);

        ctr_resource_span = ctr_resource_span_create(ctr);
        ctr_resource_span_set_resource(ctr_resource_span, ctr_resource);
        ctr_resource_span_set_schema_url(ctr_resource_span, otel_resource_span->schema_url);

        for (scope_span_index = 0; scope_span_index < otel_resource_span->n_scope_spans; scope_span_index++) {
            otel_scope_span = otel_resource_span->scope_spans[scope_span_index];
            if (otel_scope_span == NULL) {
                return -1;
            }

            ctr_scope_span = ctr_scope_span_create(ctr_resource_span);
            ctr_scope_span_set_schema_url(ctr_scope_span, otel_scope_span->schema_url);
            ctr_scope_span_set_scope(ctr_scope_span, otel_scope_span->scope);

            for (span_index = 0; span_index < otel_scope_span->n_spans; span_index++) {
                otel_span = otel_scope_span->spans[span_index];
                if (otel_span == NULL) {
                    return -1;
                }

                ctr_span = ctr_span_create(ctr, ctr_scope_span, otel_span->name, NULL);

                // copy data from otel span to ctraces span representation
                ctr_span_set_trace_id(ctr_span, otel_span->trace_id.data, otel_span->trace_id.len);
                ctr_span_set_span_id(ctr_span, otel_span->span_id.data, otel_span->span_id.len);
                ctr_span_set_parent_span_id(ctr_span, otel_span->parent_span_id.data, otel_span->parent_span_id.len);
                ctr_span_kind_set(ctr_span, otel_span->kind);
                ctr_span_start_ts(ctr, ctr_span, otel_span->start_time_unix_nano);
                ctr_span_end_ts(ctr, ctr_span, otel_span->end_time_unix_nano);
                ctr_span_set_status(ctr_span, otel_span->status->code, otel_span->status->message);
                ctr_span_set_attributes(ctr_span, otel_span->n_attributes, otel_span->attributes);
                ctr_span_set_events(ctr_span, otel_span->n_events, otel_span->events);
                ctr_span_set_dropped_attributes_count(ctr_span, otel_span->dropped_attributes_count);
                ctr_span_set_dropped_events_count(ctr_span, otel_span->dropped_events_count);
                ctr_span_set_links(ctr_span, otel_span->n_links, otel_span->links);
            }
        }
    }

    *out_ctr = ctr;

    return 0;
}
