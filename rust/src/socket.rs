// SPDX-License-Identifier: BSL-1.0

use crate::*;

// Context

pub struct Context {
    ptr: *mut c::traeger_context_t,
}

impl Context {
    pub fn new() -> Self {
        unsafe {
            let ptr = c::traeger_context_new();
            Self { ptr }
        }
    }

    pub fn replier(&self, address: &str) -> std::result::Result<Replier, std::string::String> {
        unsafe {
            let mut c_result: *mut c::traeger_replier_t = std::ptr::null_mut();
            let mut c_error: *mut c::traeger_string_t = std::ptr::null_mut();
            if c::traeger_replier_new(
                self.ptr,
                address.as_ptr(),
                address.len(),
                &mut c_result,
                &mut c_error,
            ) {
                Ok(Replier { ptr: c_result })
            } else {
                let error = String { ptr: c_error };
                Err(error.as_str().to_string())
            }
        }
    }

    pub fn requester(
        &self,
        address: &str,
        format: &Format,
    ) -> std::result::Result<Requester, std::string::String> {
        unsafe {
            let mut c_result: *mut c::traeger_requester_t = std::ptr::null_mut();
            let mut c_error: *mut c::traeger_string_t = std::ptr::null_mut();
            if c::traeger_requester_new(
                self.ptr,
                address.as_ptr(),
                address.len(),
                format.ptr,
                &mut c_result,
                &mut c_error,
            ) {
                Ok(Requester { ptr: c_result })
            } else {
                let error = String { ptr: c_error };
                Err(error.as_str().to_string())
            }
        }
    }

    pub fn publisher(
        &self,
        address: &str,
        format: &Format,
    ) -> std::result::Result<Publisher, std::string::String> {
        unsafe {
            let mut c_result: *mut c::traeger_publisher_t = std::ptr::null_mut();
            let mut c_error: *mut c::traeger_string_t = std::ptr::null_mut();
            if c::traeger_publisher_new(
                self.ptr,
                address.as_ptr(),
                address.len(),
                format.ptr,
                &mut c_result,
                &mut c_error,
            ) {
                Ok(Publisher { ptr: c_result })
            } else {
                let error = String { ptr: c_error };
                Err(error.as_str().to_string())
            }
        }
    }

    pub fn subscriber(
        &self,
        address: &str,
        topics: &[&str],
    ) -> std::result::Result<Subscriber, std::string::String> {
        unsafe {
            let mut c_result: *mut c::traeger_subscriber_t = std::ptr::null_mut();
            let mut c_error: *mut c::traeger_string_t = std::ptr::null_mut();
            let mut list = List::new();
            for topic in topics {
                list.append(*topic);
            }
            if c::traeger_subscriber_new(
                self.ptr,
                address.as_ptr(),
                address.len(),
                list.ptr,
                &mut c_result,
                &mut c_error,
            ) {
                Ok(Subscriber { ptr: c_result })
            } else {
                let error = String { ptr: c_error };
                Err(error.as_str().to_string())
            }
        }
    }
}

impl Clone for Context {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_context_copy(self.ptr);
            Self { ptr }
        }
    }
}

impl Drop for Context {
    fn drop(&mut self) {
        unsafe {
            c::traeger_context_free(self.ptr);
        }
    }
}

// Replier

pub struct Replier {
    ptr: *mut c::traeger_replier_t,
}

impl Replier {
    pub fn reply(&self, scheduler: &Scheduler, mailbox: Mailbox) -> Promise {
        unsafe {
            let ptr = c::traeger_replier_reply(self.ptr, scheduler.ptr, mailbox.ptr);
            Promise { ptr }
        }
    }
}

impl Clone for Replier {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_replier_copy(self.ptr);
            Self { ptr }
        }
    }
}

impl Drop for Replier {
    fn drop(&mut self) {
        unsafe {
            c::traeger_replier_free(self.ptr);
        }
    }
}

// Requester

pub struct Requester {
    ptr: *mut c::traeger_requester_t,
}

impl Requester {
    pub fn mailbox(&self) -> Mailbox {
        unsafe {
            let ptr = c::traeger_requester_get_mailbox(self.ptr);
            Mailbox { ptr }
        }
    }
}

impl Clone for Requester {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_requester_copy(self.ptr);
            Self { ptr }
        }
    }
}

impl Drop for Requester {
    fn drop(&mut self) {
        unsafe {
            c::traeger_requester_free(self.ptr);
        }
    }
}

// Publisher

pub struct Publisher {
    ptr: *mut c::traeger_publisher_t,
}

impl Publisher {
    pub fn publish(&self, scheduler: &Scheduler, topic: &str, variant: Variant) -> Promise {
        let value = Value::from(variant);
        unsafe {
            let ptr = c::traeger_publisher_publish(
                self.ptr,
                scheduler.ptr,
                topic.as_ptr(),
                topic.len(),
                value.ptr,
            );
            Promise { ptr }
        }
    }
}

impl Clone for Publisher {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_publisher_copy(self.ptr);
            Self { ptr }
        }
    }
}

impl Drop for Publisher {
    fn drop(&mut self) {
        unsafe {
            c::traeger_publisher_free(self.ptr);
        }
    }
}

// Subscriber

pub struct Subscriber {
    ptr: *mut c::traeger_subscriber_t,
}

impl Subscriber {
    pub fn listen(
        &self,
        scheduler: &Scheduler,
        listen: impl Fn(&str, Variant) + 'static,
    ) -> Promise {
        let function = Function::new(move |arguments| {
            let (topic, variant) = unpack!(arguments, 0 => String, 1 => Variant)?;
            listen(topic.as_str(), variant);
            Ok(Variant::Null)
        });
        unsafe {
            let ptr = c::traeger_subscriber_listen(self.ptr, scheduler.ptr, function.ptr);
            Promise { ptr }
        }
    }
}

impl Clone for Subscriber {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_subscriber_copy(self.ptr);
            Self { ptr }
        }
    }
}

impl Drop for Subscriber {
    fn drop(&mut self) {
        unsafe {
            c::traeger_subscriber_free(self.ptr);
        }
    }
}
