// SPDX-License-Identifier: BSL-1.0

use crate::*;
// Result

pub struct Result {
    ptr: *mut c::traeger_result_t,
}

fn set_result(
    c_result: *mut c::traeger_result_t,
    result: std::result::Result<Variant, std::string::String>,
) {
    match result {
        Ok(variant) => match variant {
            Variant::Null => unsafe {
                c::traeger_result_set_null(c_result);
            },
            Variant::Bool(value) => unsafe {
                c::traeger_result_set_bool(c_result, value);
            },
            Variant::Int(value) => unsafe {
                c::traeger_result_set_int(c_result, value);
            },
            Variant::UInt(value) => unsafe {
                c::traeger_result_set_uint(c_result, value);
            },
            Variant::Float(value) => unsafe {
                c::traeger_result_set_float(c_result, value);
            },
            Variant::String(string) => unsafe {
                c::traeger_result_set_string(c_result, string.as_ptr(), string.len());
            },
            Variant::List(list) => unsafe {
                c::traeger_result_set_list(c_result, list.ptr);
            },
            Variant::Map(map) => unsafe {
                c::traeger_result_set_map(c_result, map.ptr);
            },
        },
        Err(error) => unsafe {
            c::traeger_result_set_error(c_result, error.as_ptr(), error.len());
        },
    }
}

impl Result {
    pub fn new() -> Self {
        unsafe {
            let ptr = c::traeger_result_new();
            Self { ptr }
        }
    }

    pub fn set(&mut self, result: std::result::Result<Variant, std::string::String>) {
        set_result(self.ptr, result);
    }

    pub fn get(&self) -> Option<std::result::Result<Variant, std::string::String>> {
        match self.get_value() {
            Some(Ok(value)) => Some(Ok(value.to_variant())),
            Some(Err(error)) => Some(Err(error.as_str().to_string())),
            None => None,
        }
    }

    pub fn get_value(&self) -> Option<std::result::Result<Value, String>> {
        const UNDEFINED: u32 = 0;
        const VALUE: u32 = 1;
        const ERROR: u32 = 2;
        unsafe {
            let mut c_value: *mut c::traeger_value_t = std::ptr::null_mut();
            let mut c_error: *mut c::traeger_string_t = std::ptr::null_mut();
            match c::traeger_result_get_value_or_error(self.ptr, &mut c_value, &mut c_error) {
                VALUE => {
                    let value = Value { ptr: c_value };
                    Some(Ok(value))
                }
                ERROR => {
                    let error = String { ptr: c_error };
                    Some(Err(error))
                }
                UNDEFINED | _ => None,
            }
        }
    }
}

impl Drop for Result {
    fn drop(&mut self) {
        unsafe {
            c::traeger_result_free(self.ptr);
        }
    }
}

impl Clone for Result {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_result_copy(self.ptr);
            Self { ptr }
        }
    }
}

// Function

pub struct Function {
    pub(crate) ptr: *mut c::traeger_function_t,
}

type FunctionClosure = Box<dyn FnMut(List) -> std::result::Result<Variant, std::string::String>>;

impl Function {
    pub fn new(
        function: impl FnMut(List) -> std::result::Result<Variant, std::string::String> + 'static,
    ) -> Self {
        unsafe {
            let ptr = c::traeger_function_new(
                Some(Self::callback),
                Self::new_closure(function),
                Some(Self::free_closure),
            );
            Self { ptr }
        }
    }

    pub fn call(&self, arguments: &List) -> Result {
        unsafe {
            let mut ptr: *mut c::traeger_result_t = std::ptr::null_mut();
            c::traeger_function_call(self.ptr, arguments.ptr, &mut ptr);
            Result { ptr }
        }
    }

    fn new_closure(
        callback: impl FnMut(List) -> std::result::Result<Variant, std::string::String> + 'static,
    ) -> *mut std::ffi::c_void {
        let closure = Box::new(callback);
        let closure_ptr = Box::<FunctionClosure>::new(closure);
        Box::into_raw(closure_ptr) as *mut std::ffi::c_void
    }

    extern "C" fn free_closure(data: *mut std::ffi::c_void) {
        let closure = data as *mut FunctionClosure;
        unsafe {
            let _indirect_ptr = Box::from_raw(closure);
        }
    }

    extern "C" fn callback(
        c_list: *const c::traeger_list_t,
        data: *const std::ffi::c_void,
        c_result: *mut c::traeger_result_t,
    ) {
        let closure = data as *mut FunctionClosure;
        unsafe {
            let arguments = List {
                ptr: c::traeger_list_copy(c_list),
            };
            set_result(c_result, (*closure)(arguments));
        }
    }
}

impl Drop for Function {
    fn drop(&mut self) {
        unsafe {
            c::traeger_function_free(self.ptr);
        }
    }
}

impl Clone for Function {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_function_copy(self.ptr);
            Self { ptr }
        }
    }
}

// Scheduler

pub struct Scheduler {
    pub(crate) ptr: *mut c::traeger_scheduler_t,
}

type SchedulerClosure = Box<dyn Fn()>;

pub struct Threads {
    pub count: u32,
}

impl Scheduler {
    pub fn new(threads: Threads) -> Self {
        unsafe {
            let ptr = c::traeger_scheduler_new(threads.count);
            Scheduler { ptr }
        }
    }

    pub fn count(&self) -> usize {
        unsafe { c::traeger_scheduler_count(self.ptr) }
    }

    pub fn schedule(&self, function: impl Fn() + 'static) {
        unsafe {
            c::traeger_scheduler_schedule(
                self.ptr,
                Some(Self::callback),
                Self::new_closure(function),
                Some(Self::free_closure),
            );
        }
    }

    pub fn schedule_delayed(&self, delay: std::time::Duration, function: impl Fn() + 'static) {
        unsafe {
            c::traeger_scheduler_schedule_delayed(
                self.ptr,
                delay.as_secs_f64(),
                Some(Self::callback),
                Self::new_closure(function),
                Some(Self::free_closure),
            );
        }
    }

    fn new_closure(callback: impl Fn() + 'static) -> *mut std::ffi::c_void {
        let closure = Box::new(callback);
        let closure_ptr = Box::<SchedulerClosure>::new(closure);
        Box::into_raw(closure_ptr) as *mut std::ffi::c_void
    }

    extern "C" fn free_closure(data: *mut std::ffi::c_void) {
        let closure = data as *mut SchedulerClosure;
        unsafe {
            let _indirect_ptr = Box::from_raw(closure);
        }
    }

    extern "C" fn callback(data: *const std::ffi::c_void) {
        let closure = data as *const SchedulerClosure;
        unsafe {
            (*closure)();
        }
    }
}

impl Drop for Scheduler {
    fn drop(&mut self) {
        unsafe {
            c::traeger_scheduler_free(self.ptr);
        }
    }
}

impl Clone for Scheduler {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_scheduler_copy(self.ptr);
            Self { ptr }
        }
    }
}

// Promise

pub struct Promise {
    pub(crate) ptr: *mut c::traeger_promise_t,
}

type PromiseResultClosure = Box<dyn Fn(Value) -> std::result::Result<Variant, std::string::String>>;

type PromisePromiseClosure =
    Box<dyn Fn(Value) -> std::result::Result<Promise, std::string::String>>;

type PromiseErrorClosure = Box<dyn Fn(std::string::String)>;

impl Promise {
    pub fn new(scheduler: &Scheduler) -> Self {
        unsafe {
            let ptr = c::traeger_promise_new(scheduler.ptr);
            Self { ptr }
        }
    }

    pub fn set_result(&self, result: &Result) -> bool {
        unsafe { c::traeger_promise_set_result(self.ptr, result.ptr) }
    }

    pub fn get_result(&self) -> Result {
        unsafe {
            let mut ptr: *mut c::traeger_result_t = std::ptr::null_mut();
            c::traeger_promise_get_result(self.ptr, &mut ptr);
            Result { ptr }
        }
    }

    pub fn set(&self, result: std::result::Result<Variant, std::string::String>) -> bool {
        let mut res = Result::new();
        res.set(result);
        self.set_result(&res)
    }

    pub fn set_from_promise(&self, promise: &Promise) {
        unsafe {
            c::traeger_promise_set_result_from_promise(self.ptr, promise.ptr);
        }
    }

    pub fn then_result(
        &self,
        then: impl Fn(Value) -> std::result::Result<Variant, std::string::String> + 'static,
    ) -> Promise {
        unsafe {
            let ptr = c::traeger_promise_then_result(
                self.ptr,
                Some(Self::result_callback),
                Self::result_new_closure(then),
                Some(Self::result_free_closure),
            );
            Promise { ptr }
        }
    }

    extern "C" fn result_callback(
        c_value: *mut c::traeger_value_t,
        data: *const std::ffi::c_void,
        c_result: *mut c::traeger_result_t,
    ) {
        let closure = data as *const PromiseResultClosure;
        unsafe {
            let value = Value { ptr: c_value };
            set_result(c_result, (*closure)(value));
        }
    }

    fn result_new_closure(
        callback: impl Fn(Value) -> std::result::Result<Variant, std::string::String> + 'static,
    ) -> *mut std::ffi::c_void {
        let closure = Box::new(callback);
        let closure_ptr = Box::<PromiseResultClosure>::new(closure);
        Box::into_raw(closure_ptr) as *mut std::ffi::c_void
    }

    extern "C" fn result_free_closure(data: *mut std::ffi::c_void) {
        let closure = data as *mut PromiseResultClosure;
        unsafe {
            let _indirect_ptr = Box::from_raw(closure);
        }
    }

    pub fn then_promise(
        &self,
        then: impl Fn(Value) -> std::result::Result<Promise, std::string::String> + 'static,
    ) -> Promise {
        unsafe {
            let ptr = c::traeger_promise_then_promise(
                self.ptr,
                Some(Self::promise_callback),
                Self::promise_new_closure(then),
                Some(Self::promise_free_closure),
            );
            Promise { ptr }
        }
    }

    fn promise_new_closure(
        callback: impl Fn(Value) -> std::result::Result<Promise, std::string::String> + 'static,
    ) -> *mut std::ffi::c_void {
        let closure = Box::new(callback);
        let closure_ptr = Box::<PromisePromiseClosure>::new(closure);
        Box::into_raw(closure_ptr) as *mut std::ffi::c_void
    }

    extern "C" fn promise_callback(
        c_value: *mut c::traeger_value_t,
        data: *const std::ffi::c_void,
        c_promise: *mut c::traeger_promise_t,
    ) {
        let closure = data as *const PromisePromiseClosure;
        unsafe {
            let value = Value { ptr: c_value };
            match (*closure)(value) {
                Ok(promise) => c::traeger_promise_set_promise(c_promise, promise.ptr),
                Err(error) => {
                    let mut result = Result::new();
                    result.set(Err(error));
                    c::traeger_promise_set_result(c_promise, result.ptr);
                }
            }
        }
    }

    extern "C" fn promise_free_closure(data: *mut std::ffi::c_void) {
        let closure = data as *mut PromisePromiseClosure;
        unsafe {
            let _indirect_ptr = Box::from_raw(closure);
        }
    }

    pub fn fail(&self, fail: impl Fn(std::string::String) + 'static) {
        unsafe {
            c::traeger_promise_fail(
                self.ptr,
                Some(Self::error_callback),
                Self::error_new_closure(fail),
                Some(Self::error_free_closure),
            );
        }
    }

    fn error_new_closure(
        callback: impl Fn(std::string::String) + 'static,
    ) -> *mut std::ffi::c_void {
        let closure = Box::new(callback);
        let closure_ptr = Box::<PromiseErrorClosure>::new(closure);
        Box::into_raw(closure_ptr) as *mut std::ffi::c_void
    }

    extern "C" fn error_free_closure(data: *mut std::ffi::c_void) {
        let closure = data as *mut PromiseErrorClosure;
        unsafe {
            let _indirect_ptr = Box::from_raw(closure);
        }
    }

    extern "C" fn error_callback(
        c_error: *const c::traeger_string_t,
        data: *const std::ffi::c_void,
    ) {
        let closure = data as *const PromiseErrorClosure;
        unsafe {
            let data = c::traeger_string_data(c_error);
            let len = c::traeger_string_size(c_error);
            let str = std::str::from_utf8_unchecked(std::slice::from_raw_parts(data, len));
            let error = std::string::String::from(str);
            (*closure)(error);
        }
    }
}

impl Drop for Promise {
    fn drop(&mut self) {
        unsafe {
            c::traeger_promise_free(self.ptr);
        }
    }
}

impl Clone for Promise {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_promise_copy(self.ptr);
            Self { ptr }
        }
    }
}

// Actor

pub struct Actor<T> {
    pub(crate) ptr: *mut c::traeger_actor_t,
    state: std::rc::Rc<std::cell::RefCell<T>>,
}

impl<T: 'static> Actor<T> {
    pub fn new(state: T) -> Self {
        unsafe {
            let ptr = c::traeger_actor_new();
            let state = std::rc::Rc::new(std::cell::RefCell::new(state));
            Actor { ptr, state }
        }
    }

    pub fn mailbox(&self) -> Mailbox {
        unsafe {
            let ptr = c::traeger_actor_get_mailbox(self.ptr);
            Mailbox { ptr }
        }
    }

    pub fn define_reader(
        self,
        name: &str,
        method: impl Fn(&T, List) -> std::result::Result<Variant, std::string::String> + 'static,
    ) -> Self {
        let state = self.state.clone();
        let function = Function::new(move |arguments| method(&state.borrow() as &T, arguments));
        unsafe {
            c::traeger_actor_define_reader(self.ptr, name.as_ptr(), name.len(), function.ptr);
        }
        self
    }

    pub fn define_writer(
        self,
        name: &str,
        mut method: impl FnMut(&mut T, List) -> std::result::Result<Variant, std::string::String>
            + 'static,
    ) -> Self {
        let state = self.state.clone();
        let function =
            Function::new(move |arguments| method(&mut state.borrow_mut() as &mut T, arguments));
        unsafe {
            c::traeger_actor_define_writer(self.ptr, name.as_ptr(), name.len(), function.ptr);
        }
        self
    }
}

pub struct Mailbox {
    pub(crate) ptr: *mut c::traeger_mailbox_t,
}

impl<T> Drop for Actor<T> {
    fn drop(&mut self) {
        unsafe {
            c::traeger_actor_free(self.ptr);
        }
    }
}

// Mailbox

impl Mailbox {
    pub fn send(&self, scheduler: &Scheduler, name: &str, arguments: &List) -> Promise {
        unsafe {
            let ptr = c::traeger_mailbox_send(
                self.ptr,
                scheduler.ptr,
                name.as_ptr(),
                name.len(),
                arguments.ptr,
            );
            Promise { ptr }
        }
    }
}

impl Drop for Mailbox {
    fn drop(&mut self) {
        unsafe {
            c::traeger_mailbox_free(self.ptr);
        }
    }
}

impl Clone for Mailbox {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_mailbox_copy(self.ptr);
            Self { ptr }
        }
    }
}

// Queue

pub struct Queue {
    ptr: *mut c::traeger_queue_t,
}

impl Queue {
    pub fn new() -> Self {
        unsafe {
            let ptr = c::traeger_queue_new();
            Self { ptr }
        }
    }

    pub fn closed(&self) -> bool {
        unsafe { c::traeger_queue_closed(self.ptr) }
    }

    pub fn count(&self) -> usize {
        unsafe { c::traeger_queue_count(self.ptr) }
    }

    pub fn close(&self) {
        unsafe { c::traeger_queue_close(self.ptr) }
    }

    pub fn pop_value(&self) -> Option<Value> {
        unsafe {
            let mut c_value: *mut c::traeger_value_t = std::ptr::null_mut();
            if c::traeger_queue_pop_value(self.ptr, &mut c_value) {
                let value = Value { ptr: c_value };
                Some(value)
            } else {
                None
            }
        }
    }

    pub fn pop(&self) -> Option<Variant> {
        match self.pop_value() {
            Some(value) => Some(value.to_variant()),
            None => None,
        }
    }

    pub fn pop_into_list(&self, list: &mut List) -> bool {
        unsafe { c::traeger_queue_pop_into_list(self.ptr, list.ptr) }
    }
}

impl Drop for Queue {
    fn drop(&mut self) {
        unsafe {
            c::traeger_queue_free(self.ptr);
        }
    }
}

impl Clone for Queue {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_queue_copy(self.ptr);
            Self { ptr }
        }
    }
}

pub trait Push<V> {
    fn push(&self, value: V);
}

impl Push<Null> for Queue {
    fn push(&self, _value: Null) {
        unsafe {
            c::traeger_queue_push_null(self.ptr);
        }
    }
}

impl Push<Bool> for Queue {
    fn push(&self, value: Bool) {
        unsafe {
            c::traeger_queue_push_bool(self.ptr, value);
        }
    }
}

impl Push<Int> for Queue {
    fn push(&self, value: Int) {
        unsafe {
            c::traeger_queue_push_int(self.ptr, value);
        }
    }
}

impl Push<i32> for Queue {
    fn push(&self, value: i32) {
        unsafe {
            c::traeger_queue_push_int(self.ptr, Int::from(value));
        }
    }
}

impl Push<u32> for Queue {
    fn push(&self, value: u32) {
        unsafe {
            c::traeger_queue_push_int(self.ptr, Int::from(value));
        }
    }
}

impl Push<UInt> for Queue {
    fn push(&self, value: UInt) {
        unsafe {
            c::traeger_queue_push_uint(self.ptr, value);
        }
    }
}

impl Push<Float> for Queue {
    fn push(&self, value: Float) {
        unsafe {
            c::traeger_queue_push_float(self.ptr, value);
        }
    }
}

impl Push<&str> for Queue {
    fn push(&self, string: &str) {
        unsafe {
            c::traeger_queue_push_string(self.ptr, string.as_ptr(), string.len());
        }
    }
}

impl Push<&List> for Queue {
    fn push(&self, list: &List) {
        unsafe {
            c::traeger_queue_push_list(self.ptr, list.ptr);
        }
    }
}

impl Push<&Map> for Queue {
    fn push(&self, map: &Map) {
        unsafe {
            c::traeger_queue_push_map(self.ptr, map.ptr);
        }
    }
}

impl Push<&Value> for Queue {
    fn push(&self, value: &Value) {
        unsafe {
            c::traeger_queue_push_value(self.ptr, value.ptr);
        }
    }
}

impl Push<&Variant> for Queue {
    fn push(&self, variant: &Variant) {
        match variant {
            Variant::Null => self.push(NULL),
            Variant::Bool(value) => self.push(*value),
            Variant::Int(value) => self.push(*value),
            Variant::UInt(value) => self.push(*value),
            Variant::Float(value) => self.push(*value),
            Variant::String(string) => self.push(string.as_str()),
            Variant::List(list) => self.push(list),
            Variant::Map(map) => self.push(map),
        }
    }
}
