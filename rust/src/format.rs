// SPDX-License-Identifier: BSL-1.0

use crate::*;

// Format

pub struct Format {
    pub(crate) ptr: *const c::traeger_format_t,
}
impl Format {
    pub fn by_name(name: &str) -> Option<Self> {
        unsafe {
            let ptr = c::traeger_format_get_by_name(name.as_ptr(), name.len());
            if ptr.is_null() {
                None
            } else {
                Some(Format { ptr })
            }
        }
    }

    pub fn name(&self) -> &str {
        unsafe {
            let c_name = c::traeger_format_get_name(self.ptr);
            let data = c::traeger_string_data(c_name);
            let len = c::traeger_string_size(c_name);
            std::str::from_utf8_unchecked(std::slice::from_raw_parts(data, len))
        }
    }

    pub fn encode(&self, variant: Variant) -> std::result::Result<Vec<u8>, std::string::String> {
        let value = Value::from(variant);
        match self.encode_value(&value) {
            Ok(string) => {
                let data = string.as_ptr();
                let len = string.len();
                unsafe { Ok(std::slice::from_raw_parts(data, len).to_vec()) }
            }
            Err(error) => Err(error.as_str().to_string()),
        }
    }

    pub fn encode_value(&self, value: &Value) -> std::result::Result<String, String> {
        unsafe {
            let mut c_string: *mut c::traeger_string_t = std::ptr::null_mut();
            let mut c_error: *mut c::traeger_string_t = std::ptr::null_mut();
            if c::traeger_format_encode_value(self.ptr, value.ptr, &mut c_string, &mut c_error) {
                Ok(String { ptr: c_string })
            } else {
                Err(String { ptr: c_error })
            }
        }
    }

    pub fn decode(&self, slice: &[u8]) -> std::result::Result<Variant, std::string::String> {
        let string = String::from(slice);
        match self.decode_string(&string) {
            Ok(value) => Ok(value.to_variant()),
            Err(error) => Err(error.as_str().to_string()),
        }
    }

    pub fn decode_string(&self, string: &String) -> std::result::Result<Value, String> {
        unsafe {
            let mut c_value: *mut c::traeger_value_t = std::ptr::null_mut();
            let mut c_error: *mut c::traeger_string_t = std::ptr::null_mut();
            if c::traeger_format_decode_string(self.ptr, string.ptr, &mut c_value, &mut c_error) {
                Ok(Value { ptr: c_value })
            } else {
                Err(String { ptr: c_error })
            }
        }
    }
}
