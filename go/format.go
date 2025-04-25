// SPDX-License-Identifier: BSL-1.0

package traeger

import "errors"

/*
#cgo LDFLAGS: -ltraeger_format
#include <stdlib.h>
#include <traeger/value/value.h>
#include <traeger/format/format.h>
*/
import "C"

type Format struct {
	self *C.traeger_format_t
}

func NewFormatByName(name string) *Format {
	self := C.traeger_format_get_by_name(C._GoStringPtr(name), C._GoStringLen(name))
	if self != nil {
		return &Format{self}
	}
	return nil
}

func (format *Format) Name() string {
	return C.GoString(C.traeger_format_get_name(format.self))
}

func (format *Format) EncodeValue(value *Value) (*String, error) {
	var result *C.traeger_string_t
	var err *C.traeger_string_t
	if C.traeger_format_encode_value(format.self, value.self, &result, &err) {
		return wrap_c_string(result), nil
	} else {
		return nil, errors.New(wrap_c_string(err).String())
	}
}

func (format *Format) DecodeString(content *String) (*Value, error) {
	var result *C.traeger_value_t
	var err *C.traeger_string_t
	if C.traeger_format_decode_string(format.self, content.self, &result, &err) {
		return wrap_c_value(result), nil
	} else {
		return nil, errors.New(wrap_c_string(err).String())
	}
}
