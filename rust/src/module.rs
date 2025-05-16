// SPDX-License-Identifier: BSL-1.0

use crate::*;

pub fn module_init<T>(
    function: impl Fn(Map) -> std::result::Result<Actor<T>, std::string::String>,
    c_map: *const std::ffi::c_void,
    c_result: *mut std::ffi::c_void,
    c_error: *mut std::ffi::c_void,
) {
    if c_map.is_null() || c_result.is_null() || c_error.is_null() {
        return;
    }

    unsafe {
        let map = Map {
            ptr: c::traeger_map_copy(c_map as *const c::traeger_map_t),
        };
        match function(map) {
            Ok(actor) => {
                *(c_result as *mut *mut c::traeger_mailbox_interface_t) =
                    c::traeger_actor_get_mailbox_interface(actor.ptr);
            }
            Err(error) => {
                c::traeger_string_set(
                    c_error as *mut c::traeger_string_t,
                    error.as_ptr(),
                    error.len(),
                );
            }
        }
    }
}

#[macro_export]
macro_rules! module_init {
    ( $function:expr ) => {
        #[no_mangle]
        pub extern "C" fn traeger_module_init(
            c_map: *const std::ffi::c_void,
            c_result: *mut std::ffi::c_void,
            c_error: *mut std::ffi::c_void,
        ) {
            $crate::module::module_init($function, c_map, c_result, c_error);
        }
    };
}

pub struct Module {
    ptr: *mut c::traeger_module_t,
}

impl Module {
    pub fn from_path(
        path: &str,
        configuration: &Map,
    ) -> std::result::Result<Module, std::string::String> {
        unsafe {
            let mut c_result: *mut c::traeger_module_t = std::ptr::null_mut();
            let mut c_error: *mut c::traeger_string_t = std::ptr::null_mut();
            if c::traeger_module_new_from_path_or_error(
                path.as_ptr(),
                path.len(),
                configuration.ptr,
                &mut c_result,
                &mut c_error,
            ) {
                Ok(Module { ptr: c_result })
            } else {
                let error = String { ptr: c_error };
                Err(error.as_str().to_string())
            }
        }
    }

    pub fn mailbox(&self) -> Mailbox {
        unsafe {
            let ptr = c::traeger_module_get_mailbox(self.ptr);
            Mailbox { ptr }
        }
    }
}

impl Clone for Module {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_module_copy(self.ptr);
            Self { ptr }
        }
    }
}

impl Drop for Module {
    fn drop(&mut self) {
        unsafe {
            c::traeger_module_free(self.ptr);
        }
    }
}
