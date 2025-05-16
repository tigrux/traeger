// SPDX-License-Identifier: BSL-1.0

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_string_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_list_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_list_iterator_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_map_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_map_iterator_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_value_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[link(name = "traeger_value")]
extern "C" {

    // String

    pub fn traeger_string_new(
        c_string_data: *const u8,
        c_string_size: usize,
    ) -> *mut traeger_string_t;

    pub fn traeger_string_copy(c_self: *const traeger_string_t) -> *mut traeger_string_t;

    pub fn traeger_string_free(c_self: *mut traeger_string_t);

    pub fn traeger_string_set(c_self: *mut traeger_string_t, c_data: *const u8, c_size: usize);

    pub fn traeger_string_data(c_self: *const traeger_string_t) -> *const u8;

    pub fn traeger_string_size(c_self: *const traeger_string_t) -> usize;

    pub fn traeger_string_equal(
        c_self: *const traeger_string_t,
        c_other: *const traeger_string_t,
    ) -> bool;

    // List

    pub fn traeger_list_new() -> *mut traeger_list_t;

    pub fn traeger_list_copy(c_self: *const traeger_list_t) -> *mut traeger_list_t;

    pub fn traeger_list_free(c_self: *mut traeger_list_t);

    pub fn traeger_list_to_string(c_self: *const traeger_list_t, c_str: *mut *mut traeger_string_t);

    pub fn traeger_list_equal(
        c_self: *const traeger_list_t,
        c_other: *const traeger_list_t,
    ) -> bool;

    pub fn traeger_list_append_null(c_self: *mut traeger_list_t);

    pub fn traeger_list_append_bool(c_self: *mut traeger_list_t, value: bool);

    pub fn traeger_list_append_int(c_self: *mut traeger_list_t, value: i64);

    pub fn traeger_list_append_uint(c_self: *mut traeger_list_t, value: u64);

    pub fn traeger_list_append_float(c_self: *mut traeger_list_t, value: f64);

    pub fn traeger_list_append_string(
        c_self: *mut traeger_list_t,
        c_string_data: *const u8,
        c_string_size: usize,
    );

    pub fn traeger_list_append_list(c_self: *mut traeger_list_t, c_list: *const traeger_list_t);

    pub fn traeger_list_append_map(c_self: *mut traeger_list_t, c_map: *const traeger_map_t);

    pub fn traeger_list_set_null(c_self: *mut traeger_list_t, index: i32);

    pub fn traeger_list_set_bool(c_self: *mut traeger_list_t, index: i32, value: bool);

    pub fn traeger_list_set_int(c_self: *mut traeger_list_t, index: i32, value: i64);

    pub fn traeger_list_set_uint(c_self: *mut traeger_list_t, index: i32, value: u64);

    pub fn traeger_list_set_float(c_self: *mut traeger_list_t, index: i32, value: f64);

    pub fn traeger_list_set_string(
        c_self: *mut traeger_list_t,
        index: i32,
        c_string_data: *const u8,
        c_string_size: usize,
    );

    pub fn traeger_list_set_list(
        c_self: *mut traeger_list_t,
        index: i32,
        c_list: *const traeger_list_t,
    );

    pub fn traeger_list_set_map(
        c_self: *mut traeger_list_t,
        index: i32,
        c_map: *const traeger_map_t,
    );

    pub fn traeger_list_find(
        c_self: *const traeger_list_t,
        index: i32,
        value: *mut *mut traeger_value_t,
    ) -> bool;

    pub fn traeger_list_empty(c_self: *const traeger_list_t) -> bool;

    pub fn traeger_list_size(c_self: *const traeger_list_t) -> usize;

    pub fn traeger_list_resize(c_self: *mut traeger_list_t, size: usize) -> usize;

    // ListIterator

    pub fn traeger_list_iterator_new(c_self: *const traeger_list_t)
        -> *mut traeger_list_iterator_t;

    pub fn traeger_list_iterator_next(
        c_self: *mut traeger_list_iterator_t,
        c_value: *mut *mut traeger_value_t,
    ) -> bool;

    pub fn traeger_list_iterator_free(c_self: *mut traeger_list_iterator_t);

    // Map

    pub fn traeger_map_new() -> *mut traeger_map_t;

    pub fn traeger_map_copy(c_self: *const traeger_map_t) -> *mut traeger_map_t;

    pub fn traeger_map_free(c_self: *mut traeger_map_t);

    pub fn traeger_map_equal(c_self: *const traeger_map_t, c_other: *const traeger_map_t) -> bool;

    pub fn traeger_map_to_string(
        c_self: *const traeger_map_t,
        c_string: *mut *mut traeger_string_t,
    );

    pub fn traeger_map_set_null(
        c_self: *mut traeger_map_t,
        c_key_data: *const u8,
        c_key_size: usize,
    );

    pub fn traeger_map_set_bool(
        c_self: *mut traeger_map_t,
        c_key_data: *const u8,
        c_key_size: usize,
        value: bool,
    );

    pub fn traeger_map_set_int(
        c_self: *mut traeger_map_t,
        c_key_data: *const u8,
        c_key_size: usize,
        value: i64,
    );

    pub fn traeger_map_set_uint(
        c_self: *mut traeger_map_t,
        c_key_data: *const u8,
        c_key_size: usize,
        value: u64,
    );

    pub fn traeger_map_set_float(
        c_self: *mut traeger_map_t,
        c_key_data: *const u8,
        c_key_size: usize,
        value: f64,
    );

    pub fn traeger_map_set_string(
        c_self: *mut traeger_map_t,
        c_key_data: *const u8,
        c_key_size: usize,
        c_string_data: *const u8,
        c_string_size: usize,
    );

    pub fn traeger_map_set_list(
        c_self: *mut traeger_map_t,
        c_key_data: *const u8,
        c_key_size: usize,
        c_list: *const traeger_list_t,
    );

    pub fn traeger_map_set_map(
        c_self: *mut traeger_map_t,
        c_key_data: *const u8,
        c_key_size: usize,
        c_map: *const traeger_map_t,
    );

    pub fn traeger_map_find(
        c_self: *const traeger_map_t,
        c_key_data: *const u8,
        c_key_size: usize,
        c_value: *mut *mut traeger_value_t,
    ) -> bool;

    pub fn traeger_map_erase(c_self: *mut traeger_map_t, c_key_data: *const u8, c_key_size: usize);

    pub fn traeger_map_contains(
        c_self: *const traeger_map_t,
        c_key_data: *const u8,
        c_key_size: usize,
    ) -> bool;

    pub fn traeger_map_empty(c_self: *const traeger_map_t) -> bool;

    pub fn traeger_map_size(c_self: *const traeger_map_t) -> usize;

    // MaptIterator

    pub fn traeger_map_iterator_new(c_self: *const traeger_map_t) -> *mut traeger_map_iterator_t;

    pub fn traeger_map_iterator_next(
        c_self: *mut traeger_map_iterator_t,
        c_string: *mut *mut traeger_string_t,
        c_value: *mut *mut traeger_value_t,
    ) -> bool;

    pub fn traeger_map_iterator_free(c_self: *mut traeger_map_iterator_t);

    // Value

    pub fn traeger_value_new() -> *mut traeger_value_t;

    pub fn traeger_value_copy(c_self: *const traeger_value_t) -> *mut traeger_value_t;

    pub fn traeger_value_free(c_self: *mut traeger_value_t);

    pub fn traeger_value_equal(
        c_self: *const traeger_value_t,
        c_other: *const traeger_value_t,
    ) -> bool;

    pub fn traeger_value_to_string(
        c_self: *const traeger_value_t,
        c_str: *mut *mut traeger_string_t,
    );

    pub fn traeger_value_get_type(c_self: *const traeger_value_t) -> u32;

    pub fn traeger_value_get_null(c_self: *const traeger_value_t) -> bool;

    pub fn traeger_value_get_bool(c_self: *const traeger_value_t, value: *mut bool) -> bool;

    pub fn traeger_value_get_int(c_self: *const traeger_value_t, value: *mut i64) -> bool;

    pub fn traeger_value_get_uint(c_self: *const traeger_value_t, value: *mut u64) -> bool;

    pub fn traeger_value_get_float(c_self: *const traeger_value_t, value: *mut f64) -> bool;

    pub fn traeger_value_get_string(
        c_self: *const traeger_value_t,
        string: *mut *mut traeger_string_t,
    ) -> bool;

    pub fn traeger_value_get_list(
        c_self: *const traeger_value_t,
        list: *mut *mut traeger_list_t,
    ) -> bool;

    pub fn traeger_value_get_map(
        c_self: *const traeger_value_t,
        map: *mut *mut traeger_map_t,
    ) -> bool;

    pub fn traeger_value_set_null(c_self: *mut traeger_value_t);

    pub fn traeger_value_set_bool(c_self: *mut traeger_value_t, value: bool);

    pub fn traeger_value_set_int(c_self: *mut traeger_value_t, value: i64);

    pub fn traeger_value_set_uint(c_self: *mut traeger_value_t, value: u64);

    pub fn traeger_value_set_float(c_self: *mut traeger_value_t, value: f64);

    pub fn traeger_value_set_string(
        c_self: *mut traeger_value_t,
        c_string_data: *const u8,
        c_string_size: usize,
    );

    pub fn traeger_value_set_list(c_self: *mut traeger_value_t, c_list: *const traeger_list_t);

    pub fn traeger_value_set_map(c_self: *mut traeger_value_t, c_map: *const traeger_map_t);
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_result_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_function_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_scheduler_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_promise_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_actor_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_mailbox_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_mailbox_interface_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_queue_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[link(name = "traeger_actor")]
extern "C" {

    // Result

    pub fn traeger_result_new() -> *mut traeger_result_t;

    pub fn traeger_result_copy(c_self: *const traeger_result_t) -> *mut traeger_result_t;

    pub fn traeger_result_free(c_self: *mut traeger_result_t);

    pub fn traeger_result_set_null(c_self: *mut traeger_result_t);

    pub fn traeger_result_set_bool(c_self: *mut traeger_result_t, value: bool);

    pub fn traeger_result_set_int(c_self: *mut traeger_result_t, value: i64);

    pub fn traeger_result_set_uint(c_self: *mut traeger_result_t, value: u64);

    pub fn traeger_result_set_float(c_self: *mut traeger_result_t, value: f64);

    pub fn traeger_result_set_string(
        c_self: *mut traeger_result_t,
        c_string_data: *const u8,
        c_string_size: usize,
    );

    pub fn traeger_result_set_list(c_self: *mut traeger_result_t, c_list: *const traeger_list_t);

    pub fn traeger_result_set_map(c_self: *mut traeger_result_t, c_map: *const traeger_map_t);

    pub fn traeger_result_set_error(
        c_self: *mut traeger_result_t,
        c_error_data: *const u8,
        c_error_size: usize,
    );

    pub fn traeger_result_get_value_or_error(
        c_self: *const traeger_result_t,
        c_value: *mut *mut traeger_value_t,
        c_error: *mut *mut traeger_string_t,
    ) -> u32;

    // Function

    pub fn traeger_function_new(
        callback: Option<
            extern "C" fn(*const traeger_list_t, *const std::ffi::c_void, *mut traeger_result_t),
        >,
        data: *const std::ffi::c_void,
        data_free: Option<extern "C" fn(*mut std::ffi::c_void)>,
    ) -> *mut traeger_function_t;

    pub fn traeger_function_copy(c_self: *const traeger_function_t) -> *mut traeger_function_t;

    pub fn traeger_function_free(c_self: *mut traeger_function_t);

    pub fn traeger_function_call(
        c_self: *const traeger_function_t,
        c_arguments: *const traeger_list_t,
        c_result: *mut *mut traeger_result_t,
    ) -> u32;

    // Scheduler

    pub fn traeger_scheduler_new(threads_count: u32) -> *mut traeger_scheduler_t;

    pub fn traeger_scheduler_copy(c_self: *const traeger_scheduler_t) -> *mut traeger_scheduler_t;

    pub fn traeger_scheduler_free(c_self: *mut traeger_scheduler_t);

    pub fn traeger_scheduler_count(c_self: *const traeger_scheduler_t) -> usize;

    pub fn traeger_scheduler_schedule(
        c_self: *const traeger_scheduler_t,
        callback: Option<extern "C" fn(*const std::ffi::c_void)>,
        data: *const std::ffi::c_void,
        data_free: Option<extern "C" fn(*mut std::ffi::c_void)>,
    );

    pub fn traeger_scheduler_schedule_delayed(
        c_self: *const traeger_scheduler_t,
        delay: f64,
        callback: Option<extern "C" fn(*const std::ffi::c_void)>,
        data: *const std::ffi::c_void,
        data_free: Option<extern "C" fn(*mut std::ffi::c_void)>,
    );

    // Promise

    pub fn traeger_promise_new(c_scheduler: *const traeger_scheduler_t) -> *mut traeger_promise_t;

    pub fn traeger_promise_copy(c_self: *const traeger_promise_t) -> *mut traeger_promise_t;

    pub fn traeger_promise_free(c_self: *mut traeger_promise_t);

    pub fn traeger_promise_set_promise(
        c_self: *mut traeger_promise_t,
        c_promise: *const traeger_promise_t,
    );

    pub fn traeger_promise_get_result(
        c_self: *const traeger_promise_t,
        c_result: *mut *mut traeger_result_t,
    );

    pub fn traeger_promise_set_result(
        c_self: *const traeger_promise_t,
        c_result: *const traeger_result_t,
    ) -> bool;

    pub fn traeger_promise_set_result_from_promise(
        c_self: *const traeger_promise_t,
        c_promise: *const traeger_promise_t,
    );

    pub fn traeger_promise_then_result(
        c_self: *const traeger_promise_t,
        callback: Option<
            extern "C" fn(*mut traeger_value_t, *const std::ffi::c_void, *mut traeger_result_t),
        >,
        data: *const std::ffi::c_void,
        data_free: Option<extern "C" fn(*mut std::ffi::c_void)>,
    ) -> *mut traeger_promise_t;

    pub fn traeger_promise_then_promise(
        c_self: *const traeger_promise_t,
        callback: Option<
            extern "C" fn(*mut traeger_value_t, *const std::ffi::c_void, *mut traeger_promise_t),
        >,
        data: *const std::ffi::c_void,
        data_free: Option<extern "C" fn(*mut std::ffi::c_void)>,
    ) -> *mut traeger_promise_t;

    pub fn traeger_promise_fail(
        c_self: *const traeger_promise_t,
        callback: Option<extern "C" fn(*const traeger_string_t, *const std::ffi::c_void)>,
        data: *const std::ffi::c_void,
        data_free: Option<extern "C" fn(*mut std::ffi::c_void)>,
    );

    // Actor

    pub fn traeger_actor_new() -> *mut traeger_actor_t;

    pub fn traeger_actor_free(c_self: *mut traeger_actor_t);

    pub fn traeger_actor_get_mailbox(c_self: *const traeger_actor_t) -> *mut traeger_mailbox_t;

    pub fn traeger_actor_get_mailbox_interface(
        c_self: *const traeger_actor_t,
    ) -> *mut traeger_mailbox_interface_t;

    pub fn traeger_actor_define_reader(
        c_self: *const traeger_actor_t,
        c_name_data: *const u8,
        c_name_size: usize,
        c_function: *const traeger_function_t,
    );

    pub fn traeger_actor_define_writer(
        c_self: *const traeger_actor_t,
        c_name_data: *const u8,
        c_name_size: usize,
        c_function: *const traeger_function_t,
    );

    // Mailbox

    pub fn traeger_mailbox_copy(c_self: *const traeger_mailbox_t) -> *mut traeger_mailbox_t;

    pub fn traeger_mailbox_free(c_self: *mut traeger_mailbox_t);

    pub fn traeger_mailbox_send(
        c_self: *const traeger_mailbox_t,
        scheduler: *const traeger_scheduler_t,
        c_name_data: *const u8,
        c_name_size: usize,
        c_arguments: *const traeger_list_t,
    ) -> *mut traeger_promise_t;

    // Queue

    pub fn traeger_queue_new() -> *mut traeger_queue_t;

    pub fn traeger_queue_copy(c_self: *const traeger_queue_t) -> *mut traeger_queue_t;

    pub fn traeger_queue_free(c_self: *mut traeger_queue_t);

    pub fn traeger_queue_closed(c_self: *const traeger_queue_t) -> bool;

    pub fn traeger_queue_count(c_self: *const traeger_queue_t) -> usize;

    pub fn traeger_queue_close(c_self: *const traeger_queue_t);

    pub fn traeger_queue_push_null(c_self: *const traeger_queue_t) -> bool;

    pub fn traeger_queue_push_bool(c_self: *const traeger_queue_t, value: bool) -> bool;

    pub fn traeger_queue_push_int(c_self: *const traeger_queue_t, value: i64) -> bool;

    pub fn traeger_queue_push_uint(c_self: *const traeger_queue_t, value: u64) -> bool;

    pub fn traeger_queue_push_float(c_self: *const traeger_queue_t, value: f64) -> bool;

    pub fn traeger_queue_push_string(
        c_self: *const traeger_queue_t,
        c_string_data: *const u8,
        c_string_size: usize,
    );

    pub fn traeger_queue_push_list(c_self: *const traeger_queue_t, c_list: *const traeger_list_t);

    pub fn traeger_queue_push_map(c_self: *const traeger_queue_t, c_map: *const traeger_map_t);

    pub fn traeger_queue_push_value(
        c_self: *const traeger_queue_t,
        c_value: *const traeger_value_t,
    ) -> bool;

    pub fn traeger_queue_pop_value(
        c_self: *const traeger_queue_t,
        c_value: *mut *mut traeger_value_t,
    ) -> bool;

    pub fn traeger_queue_pop_into_list(
        c_self: *const traeger_queue_t,
        c_list: *mut traeger_list_t,
    ) -> bool;
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_format_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[link(name = "traeger_format")]
extern "C" {

    // Format

    pub fn traeger_format_get_by_name(
        c_name_data: *const u8,
        c_name_size: usize,
    ) -> *const traeger_format_t;

    pub fn traeger_format_get_name(c_self: *const traeger_format_t) -> *const traeger_string_t;

    pub fn traeger_format_encode_value(
        c_self: *const traeger_format_t,
        c_value: *const traeger_value_t,
        c_result: *mut *mut traeger_string_t,
        c_error: *mut *mut traeger_string_t,
    ) -> bool;

    pub fn traeger_format_decode_string(
        c_self: *const traeger_format_t,
        string: *const traeger_string_t,
        c_result: *mut *mut traeger_value_t,
        c_error: *mut *mut traeger_string_t,
    ) -> bool;
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_context_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_replier_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_requester_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_publisher_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_subscriber_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[link(name = "traeger_socket")]
extern "C" {

    // Context

    pub fn traeger_context_new() -> *mut traeger_context_t;

    pub fn traeger_context_copy(c_self: *const traeger_context_t) -> *mut traeger_context_t;

    pub fn traeger_context_free(c_self: *mut traeger_context_t);

    // Replier

    pub fn traeger_replier_new(
        c_self: *const traeger_context_t,
        c_address_data: *const u8,
        c_address_size: usize,
        c_result: *mut *mut traeger_replier_t,
        c_error: *mut *mut traeger_string_t,
    ) -> bool;

    pub fn traeger_replier_copy(c_self: *const traeger_replier_t) -> *mut traeger_replier_t;

    pub fn traeger_replier_free(c_self: *mut traeger_replier_t);

    pub fn traeger_replier_reply(
        c_self: *const traeger_replier_t,
        c_scheduler: *const traeger_scheduler_t,
        c_mailbox: *const traeger_mailbox_t,
    ) -> *mut traeger_promise_t;

    // Requester

    pub fn traeger_requester_new(
        c_self: *const traeger_context_t,
        c_address_data: *const u8,
        c_address_size: usize,
        c_format: *const traeger_format_t,
        c_result: *mut *mut traeger_requester_t,
        c_error: *mut *mut traeger_string_t,
    ) -> bool;

    pub fn traeger_requester_copy(c_self: *const traeger_requester_t) -> *mut traeger_requester_t;

    pub fn traeger_requester_free(c_self: *mut traeger_requester_t);

    pub fn traeger_requester_get_mailbox(
        c_self: *const traeger_requester_t,
    ) -> *mut traeger_mailbox_t;

    // Publisher

    pub fn traeger_publisher_new(
        c_self: *const traeger_context_t,
        c_address_data: *const u8,
        c_address_size: usize,
        c_format: *const traeger_format_t,
        c_result: *mut *mut traeger_publisher_t,
        c_error: *mut *mut traeger_string_t,
    ) -> bool;

    pub fn traeger_publisher_copy(c_self: *const traeger_publisher_t) -> *mut traeger_publisher_t;

    pub fn traeger_publisher_free(c_self: *mut traeger_publisher_t);

    pub fn traeger_publisher_publish(
        c_self: *const traeger_publisher_t,
        c_scheduler: *const traeger_scheduler_t,
        c_topic_data: *const u8,
        c_topic_size: usize,
        c_value: *const traeger_value_t,
    ) -> *mut traeger_promise_t;

    // Subscriber

    pub fn traeger_subscriber_new(
        c_self: *const traeger_context_t,
        c_address_data: *const u8,
        c_address_size: usize,
        c_topics: *const traeger_list_t,
        c_result: *mut *mut traeger_subscriber_t,
        c_error: *mut *mut traeger_string_t,
    ) -> bool;

    pub fn traeger_subscriber_copy(
        c_self: *const traeger_subscriber_t,
    ) -> *mut traeger_subscriber_t;

    pub fn traeger_subscriber_free(c_self: *mut traeger_subscriber_t);

    pub fn traeger_subscriber_listen(
        c_self: *const traeger_subscriber_t,
        c_scheduler: *const traeger_scheduler_t,
        c_function: *const traeger_function_t,
    ) -> *mut traeger_promise_t;
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct traeger_module_t {
    _opaque: (),
    _marker: std::marker::PhantomData<(*mut u8, std::marker::PhantomPinned)>,
}

#[link(name = "traeger_module")]
extern "C" {

    // Module

    pub fn traeger_module_new_from_path_or_error(
        c_path_data: *const u8,
        c_path_size: usize,
        c_configuration: *const traeger_map_t,
        c_result: *mut *mut traeger_module_t,
        c_error: *mut *mut traeger_string_t,
    ) -> bool;

    pub fn traeger_module_copy(c_self: *const traeger_module_t) -> *mut traeger_module_t;

    pub fn traeger_module_free(c_self: *mut traeger_module_t);

    pub fn traeger_module_get_mailbox(c_self: *const traeger_module_t) -> *mut traeger_mailbox_t;
}
