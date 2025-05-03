// SPDX-License-Identifier: BSL-1.0

package traeger

import (
	"fmt"
	"reflect"
	"runtime"
	"unsafe"
)

/*
#cgo LDFLAGS: -ltraeger_value
#include <traeger/value/value.h>
*/
import "C"

// String

type String struct {
	self *C.traeger_string_t
}

func FreeString(str *String) {
	C.traeger_string_free(str.self)
}

func wrap_c_string(self *C.traeger_string_t) *String {
	str := String{self}
	runtime.SetFinalizer(&str, FreeString)
	return &str
}

func FromString(str string) *String {
	return wrap_c_string(
		C.traeger_string_new(
			C._GoStringPtr(str),
			C._GoStringLen(str)))
}

func FromBytes(bytes []byte) *String {
	return wrap_c_string(
		C.traeger_string_new(
			*(**C.char)(unsafe.Pointer(&bytes)),
			C.size_t(len(bytes))))
}

func (str *String) Copy() *String {
	return wrap_c_string(C.traeger_string_copy(str.self))
}

func (str *String) String() string {
	return C.GoStringN(
		C.traeger_string_data(str.self),
		C.int(C.traeger_string_size(str.self)))
}

func (str *String) Bytes() []byte {
	return C.GoBytes(
		unsafe.Pointer(C.traeger_string_data(str.self)),
		C.int(C.traeger_string_size(str.self)))
}

func (str *String) Equal(other *String) bool {
	return bool(C.traeger_string_equal(str.self, other.self))
}

// List

type List struct {
	self *C.traeger_list_t
}

func FreeList(list *List) {
	C.traeger_list_free(list.self)
}

func wrap_c_list(self *C.traeger_list_t) *List {
	list := List{self}
	runtime.SetFinalizer(&list, FreeList)
	return &list
}

func NewList() *List {
	list := wrap_c_list(C.traeger_list_new())
	return list
}

func (list *List) Copy() *List {
	return wrap_c_list(C.traeger_list_copy(list.self))
}

func MakeList(variants []any) *List {
	list := NewList()
	for _, variant := range variants {
		list.Append(variant)
	}
	return list
}

func (list *List) Print() {
	C.traeger_list_print(list.self)
}

func (list *List) String() string {
	var s *C.traeger_string_t
	C.traeger_list_to_string(list.self, &s)
	return wrap_c_string(s).String()
}

func (list *List) Equal(other *List) bool {
	return bool(C.traeger_list_equal(list.self, other.self))
}

func (list *List) Append(variant any) bool {
	switch variant := variant.(type) {
	case bool:
		list.AppendBool(variant)
		return true
	case int:
		list.AppendInt(int64(variant))
		return true
	case int32:
		list.AppendInt(int64(variant))
		return true
	case int64:
		list.AppendInt(variant)
		return true
	case uint:
		list.AppendUInt(uint64(variant))
		return true
	case uint32:
		list.AppendUInt(uint64(variant))
		return true
	case uint64:
		list.AppendUInt(variant)
		return true
	case float32:
		list.AppendFloat(float64(variant))
		return true
	case float64:
		list.AppendFloat(variant)
		return true
	case string:
		list.AppendString(variant)
		return true
	case *List:
		list.AppendList(variant)
		return true
	case *Map:
		list.AppendMap(variant)
		return true
	case *Value:
		list.AppendValue(variant)
		return true
	default:
		list.AppendNull(nil)
		return false
	}
}

func (list *List) AppendNull(variant any) {
	C.traeger_list_append_null(list.self)
}

func (list *List) AppendBool(variant bool) {
	C.traeger_list_append_bool(list.self, C.traeger_bool_t(variant))
}

func (list *List) AppendInt(variant int64) {
	C.traeger_list_append_int(list.self, C.traeger_int_t(variant))
}

func (list *List) AppendUInt(variant uint64) {
	C.traeger_list_append_uint(list.self, C.traeger_uint_t(variant))
}

func (list *List) AppendFloat(variant float64) {
	C.traeger_list_append_float(list.self, C.traeger_float_t(variant))
}

func (list *List) AppendString(variant string) {
	C.traeger_list_append_string(list.self, C._GoStringPtr(variant), C._GoStringLen(variant))
}

func (list *List) AppendList(variant *List) {
	C.traeger_list_append_list(list.self, variant.self)
}

func (list *List) AppendMap(variant *Map) {
	C.traeger_list_append_map(list.self, variant.self)
}

func (list *List) SetNull(index int, variant any) {
	C.traeger_list_set_null(list.self, C.int(index))
}

func (list *List) SetBool(index int, variant bool) {
	C.traeger_list_set_bool(list.self, C.int(index), C.traeger_bool_t(variant))
}

func (list *List) SetInt(index int, variant int64) {
	C.traeger_list_set_int(list.self, C.int(index), C.traeger_int_t(variant))
}

func (list *List) SetUInt(index int, variant uint64) {
	C.traeger_list_set_uint(list.self, C.int(index), C.traeger_uint_t(variant))
}

func (list *List) SetFloat(index int, variant float64) {
	C.traeger_list_set_float(list.self, C.int(index), C.traeger_float_t(variant))
}

func (list *List) SetString(index int, variant string) {
	C.traeger_list_set_string(list.self, C.int(index), C._GoStringPtr(variant), C._GoStringLen(variant))
}

func (list *List) SetList(index int, variant List) {
	C.traeger_list_set_list(list.self, C.int(index), variant.self)
}

func (list *List) SetMap(index int, variant Map) {
	C.traeger_list_set_map(list.self, C.int(index), variant.self)
}

func (list *List) AppendValue(variant *Value) {
	C.traeger_list_append_value(list.self, variant.self)
}

func (list *List) Find(index int) (*Value, bool) {
	var value *C.traeger_value_t
	if C.traeger_list_find(list.self, C.int(index), &value) {
		return wrap_c_value(value), true
	}
	return nil, false
}

func (list *List) Unpack(variants ...any) (bool, error) {
	if len(variants) != list.Size() {
		return false, fmt.Errorf("expected %d arguments but %d were given", len(variants), list.Size())
	}
	for index, variant := range variants {
		value, _ := list.Find(index)
		switch variant := variant.(type) {
		case *bool:
			if val, ok := value.GetBool(); ok {
				*variant = val
			}
		case *int:
			if val, ok := value.GetInt(); ok {
				*variant = int(val)
			}
		case *int32:
			if val, ok := value.GetInt(); ok {
				*variant = int32(val)
			}
		case *int64:
			if val, ok := value.GetInt(); ok {
				*variant = val
			}
		case *uint:
			if val, ok := value.GetUInt(); ok {
				*variant = uint(val)
			}
		case *uint32:
			if val, ok := value.GetUInt(); ok {
				*variant = uint32(val)
			}
		case *uint64:
			if val, ok := value.GetUInt(); ok {
				*variant = val
			}
		case *float32:
			if val, ok := value.GetFloat(); ok {
				*variant = float32(val)
			}
		case *float64:
			if val, ok := value.GetFloat(); ok {
				*variant = val
			}
		case *string:
			if val, ok := value.GetString(); ok {
				*variant = val.String()
			}
		case **String:
			if val, ok := value.GetString(); ok {
				*variant = val
			}
		case **List:
			if val, ok := value.GetList(); ok {
				*variant = val
			}
		case **Map:
			if val, ok := value.GetMap(); ok {
				*variant = val
			}
		case **Value:
			*variant = value
		default:
			return false, fmt.Errorf(
				"invalid cast in argument %d from type %s to %s",
				index,
				value.GetTypeName(),
				reflect.TypeOf(variant))
		}

	}
	return true, nil
}

func (list *List) Empty() bool {
	return bool(C.traeger_list_empty(list.self))
}

func (list *List) Size() int {
	return int(C.traeger_list_size(list.self))
}

func (list *List) Resize(new_size int) int {
	return int(C.traeger_list_resize(list.self, C.size_t(new_size)))
}

func (list *List) Values() func(func(value *Value) bool) {
	return func(yield func(value *Value) bool) {
		iterator := wrap_c_list_iterator(C.traeger_list_iterator_new(list.self))
		var value *C.traeger_value_t
		for C.traeger_list_iterator_next(iterator.self, &value) {
			if !yield(wrap_c_value(value)) {
				return
			}
		}
	}
}

// ListIterator

type ListIterator struct {
	self *C.traeger_list_iterator_t
}

func FreeListIterator(it *ListIterator) {
	C.traeger_list_iterator_free(it.self)
}

func wrap_c_list_iterator(self *C.traeger_list_iterator_t) *ListIterator {
	iterator := ListIterator{self}
	runtime.SetFinalizer(&iterator, FreeListIterator)
	return &iterator
}

func (list *List) Iterator() *ListIterator {
	return wrap_c_list_iterator(C.traeger_list_iterator_new(list.self))
}

func (it *ListIterator) HasNext() bool {
	return bool(C.traeger_list_iterator_has_next(it.self))
}

func (it *ListIterator) Next() *Value {
	var value *C.traeger_value_t
	if C.traeger_list_iterator_next(it.self, &value) {
		return wrap_c_value(value)
	}
	return nil
}

// Map

type Map struct {
	self *C.traeger_map_t
}

func FreeMap(mapping *Map) {
	C.traeger_map_free(mapping.self)
}

func wrap_c_map(self *C.traeger_map_t) *Map {
	mapping := Map{self}
	runtime.SetFinalizer(&mapping, FreeMap)
	return &mapping
}

func NewMap() *Map {
	mapping := wrap_c_map(C.traeger_map_new())
	return mapping
}

func (mapping *Map) Copy() *Map {
	return wrap_c_map(C.traeger_map_copy(mapping.self))
}

func MakeMap(variants map[string]any) *Map {
	mapping := NewMap()
	for key, variant := range variants {
		mapping.Set(key, variant)
	}
	return mapping
}

func (mapping *Map) Print() {
	C.traeger_map_print(mapping.self)
}

func (mapping *Map) String() string {
	var str *C.traeger_string_t
	C.traeger_map_to_string(mapping.self, &str)
	return wrap_c_string(str).String()
}

func (mapping *Map) Equal(other *Map) bool {
	return bool(C.traeger_map_equal(mapping.self, other.self))
}

func (mapping *Map) Set(key string, variant any) bool {
	switch variant := variant.(type) {
	case bool:
		mapping.SetBool(key, variant)
		return true
	case int:
		mapping.SetInt(key, int64(variant))
		return true
	case int32:
		mapping.SetInt(key, int64(variant))
		return true
	case int64:
		mapping.SetInt(key, variant)
		return true
	case uint:
		mapping.SetUInt(key, uint64(variant))
		return true
	case uint32:
		mapping.SetUInt(key, uint64(variant))
		return true
	case uint64:
		mapping.SetUInt(key, variant)
		return true
	case float32:
		mapping.SetFloat(key, float64(variant))
		return true
	case float64:
		mapping.SetFloat(key, variant)
		return true
	case string:
		mapping.SetString(key, variant)
		return true
	case *List:
		mapping.SetList(key, variant)
		return true
	case *Map:
		mapping.SetMap(key, variant)
		return true
	case *Value:
		mapping.SetValue(key, variant)
		return true
	default:
		mapping.SetNull(key, nil)
		return false
	}
}

func (mapping *Map) SetNull(key string, variant any) {
	C.traeger_map_set_null(mapping.self, C._GoStringPtr(key), C._GoStringLen(key))
}

func (mapping *Map) SetBool(key string, variant bool) {
	C.traeger_map_set_bool(mapping.self, C._GoStringPtr(key), C._GoStringLen(key), C.traeger_bool_t(variant))
}

func (mapping *Map) SetInt(key string, variant int64) {
	C.traeger_map_set_int(mapping.self, C._GoStringPtr(key), C._GoStringLen(key), C.traeger_int_t(variant))
}

func (mapping *Map) SetUInt(key string, variant uint64) {
	C.traeger_map_set_uint(mapping.self, C._GoStringPtr(key), C._GoStringLen(key), C.traeger_uint_t(variant))
}

func (mapping *Map) SetFloat(key string, variant float64) {
	C.traeger_map_set_float(mapping.self, C._GoStringPtr(key), C._GoStringLen(key), C.traeger_float_t(variant))
}

func (mapping *Map) SetString(key string, variant string) {
	C.traeger_map_set_string(mapping.self, C._GoStringPtr(key), C._GoStringLen(key), C._GoStringPtr(variant), C._GoStringLen(variant))
}

func (mapping *Map) SetList(key string, variant *List) {
	C.traeger_map_set_list(mapping.self, C._GoStringPtr(key), C._GoStringLen(key), variant.self)
}

func (mapping *Map) SetMap(key string, variant *Map) {
	C.traeger_map_set_map(mapping.self, C._GoStringPtr(key), C._GoStringLen(key), variant.self)
}

func (mapping *Map) SetValue(key string, variant *Value) {
	C.traeger_map_set_value(mapping.self, C._GoStringPtr(key), C._GoStringLen(key), variant.self)
}

func (mapping *Map) Find(key string) (*Value, bool) {
	var value *C.traeger_value_t
	if C.traeger_map_find(mapping.self, C._GoStringPtr(key), C._GoStringLen(key), &value) {
		return wrap_c_value(value), true
	}
	return nil, false
}

func (mapping *Map) Empty() bool {
	return bool(C.traeger_map_empty(mapping.self))
}

func (mapping *Map) Size() int {
	return int(C.traeger_map_size(mapping.self))
}

func (mapping *Map) Keys() func(func(key string) bool) {
	return func(yield func(key string) bool) {
		iterator := wrap_c_map_iterator(C.traeger_map_iterator_new(mapping.self))
		var key *C.traeger_string_t
		for C.traeger_map_iterator_next(iterator.self, &key, nil) {
			if !yield(wrap_c_string(key).String()) {
				return
			}
		}
	}
}

func (mapping *Map) Values() func(func(value *Value) bool) {
	return func(yield func(value *Value) bool) {
		iterator := wrap_c_map_iterator(C.traeger_map_iterator_new(mapping.self))
		var value *C.traeger_value_t
		for C.traeger_map_iterator_next(iterator.self, nil, &value) {
			if !yield(wrap_c_value(value)) {
				return
			}
		}
	}
}

func (mapping *Map) Items() func(func(key string, value *Value) bool) {
	return func(yield func(key string, value *Value) bool) {
		iterator := wrap_c_map_iterator(C.traeger_map_iterator_new(mapping.self))
		var key *C.traeger_string_t
		var value *C.traeger_value_t
		for C.traeger_map_iterator_next(iterator.self, &key, &value) {
			if !yield(wrap_c_string(key).String(), wrap_c_value(value)) {
				return
			}
		}
	}
}

// MapIterator

type MapIterator struct {
	self *C.traeger_map_iterator_t
}

func FreeMapIterator(iterator *MapIterator) {
	C.traeger_map_iterator_free(iterator.self)
}

func wrap_c_map_iterator(self *C.traeger_map_iterator_t) *MapIterator {
	iterator := MapIterator{self}
	runtime.SetFinalizer(&iterator, FreeMapIterator)
	return &iterator
}

func (mapping *Map) Iterator() *MapIterator {
	return wrap_c_map_iterator(C.traeger_map_iterator_new(mapping.self))
}

func (iterator *MapIterator) HasNext() bool {
	return bool(C.traeger_map_iterator_has_next(iterator.self))
}

func (iterator *MapIterator) Next() (string, *Value) {
	var key *C.traeger_string_t
	var value *C.traeger_value_t
	if C.traeger_map_iterator_next(iterator.self, &key, &value) {
		return wrap_c_string(key).String(), wrap_c_value(value)
	}
	return "", nil
}

// Value

type Value struct {
	self *C.traeger_value_t
}

type ValueType struct {
	self C.traeger_value_type_t
}

const (
	TypeNull   = C.TRAEGER_VALUE_TYPE_NULL
	TypeBool   = C.TRAEGER_VALUE_TYPE_BOOL
	TypeInt    = C.TRAEGER_VALUE_TYPE_INT
	TypeUInt   = C.TRAEGER_VALUE_TYPE_UINT
	TypeFloat  = C.TRAEGER_VALUE_TYPE_FLOAT
	TypeString = C.TRAEGER_VALUE_TYPE_STRING
	TypeList   = C.TRAEGER_VALUE_TYPE_LIST
	TypeMap    = C.TRAEGER_VALUE_TYPE_MAP
)

func FreeValue(value *Value) {
	C.traeger_value_free(value.self)
}

func wrap_c_value(self *C.traeger_value_t) *Value {
	value := Value{self}
	runtime.SetFinalizer(&value, FreeValue)
	return &value
}

func NewValue() *Value {
	value := wrap_c_value(C.traeger_value_new())
	return value
}

func (value *Value) Copy() *Value {
	return wrap_c_value(C.traeger_value_copy(value.self))
}

func MakeValue(variant any) *Value {
	value := NewValue()
	value.Set(variant)
	return value
}

func (value *Value) Print() {
	C.traeger_value_print(value.self)
}

func (value *Value) String() string {
	var s *C.traeger_string_t
	C.traeger_value_to_string(value.self, &s)
	return wrap_c_string(s).String()
}

func (value *Value) Equal(other *Value) bool {
	return bool(C.traeger_value_equal(value.self, other.self))
}

func (value *Value) Set(variant any) bool {
	switch variant := variant.(type) {
	case bool:
		value.SetBool(variant)
		return true
	case int:
		value.SetInt(int64(variant))
		return true
	case int32:
		value.SetInt(int64(variant))
		return true
	case int64:
		value.SetInt(variant)
		return true
	case uint:
		value.SetUInt(uint64(variant))
		return true
	case uint32:
		value.SetUInt(uint64(variant))
		return true
	case uint64:
		value.SetUInt(variant)
		return true
	case float32:
		value.SetFloat(float64(variant))
		return true
	case float64:
		value.SetFloat(variant)
		return true
	case string:
		value.SetString(variant)
		return true
	case *List:
		value.SetList(variant)
		return true
	case *Map:
		value.SetMap(variant)
		return true
	case *Value:
		value.SetValue(variant)
		return true
	default:
		value.SetNull(nil)
		return false
	}
}

func (value *Value) SetNull(any) {
	C.traeger_value_set_null(value.self)
}

func (value *Value) SetBool(variant bool) {
	C.traeger_value_set_bool(value.self, C.traeger_bool_t(variant))
}

func (value *Value) SetInt(variant int64) {
	C.traeger_value_set_int(value.self, C.traeger_int_t(variant))
}

func (value *Value) SetUInt(variant uint64) {
	C.traeger_value_set_uint(value.self, C.traeger_uint_t(variant))
}

func (value *Value) SetFloat(variant float64) {
	C.traeger_value_set_float(value.self, C.traeger_float_t(variant))
}

func (value *Value) SetString(variant string) {
	C.traeger_value_set_string(value.self, C._GoStringPtr(variant), C._GoStringLen(variant))
}

func (value *Value) SetList(variant *List) {
	C.traeger_value_set_list(value.self, variant.self)
}

func (value *Value) SetMap(variant *Map) {
	C.traeger_value_set_map(value.self, variant.self)
}

func (value *Value) SetValue(variant *Value) {
	C.traeger_value_set_value(value.self, variant.self)
}

func (value *Value) GetType() ValueType {
	return ValueType{C.traeger_value_get_type(value.self)}
}

func (value *Value) GetTypeName() string {
	return C.GoString(C.traeger_value_get_type_name(value.self))
}

func (valueType ValueType) GetName() string {
	return C.GoString(C.traeger_value_type_get_name(valueType.self))
}

func (value *Value) GetNull() (any, bool) {
	if C.traeger_value_get_null(value.self) {
		return nil, true
	}
	return nil, false
}

func (value *Value) GetBool() (bool, bool) {
	var variant C.traeger_bool_t
	if C.traeger_value_get_bool(value.self, &variant) {
		return bool(variant), true
	}
	return false, false
}

func (value *Value) GetInt() (int64, bool) {
	var variant C.traeger_int_t
	if C.traeger_value_get_int(value.self, &variant) {
		return int64(variant), true
	}
	return 0, false
}

func (value *Value) GetUInt() (uint64, bool) {
	var variant C.traeger_uint_t
	if C.traeger_value_get_uint(value.self, &variant) {
		return uint64(variant), true
	}
	return 0, false
}

func (value *Value) GetFloat() (float64, bool) {
	var variant C.traeger_float_t
	if C.traeger_value_get_float(value.self, &variant) {
		return float64(variant), true
	}
	return 0.0, false
}

func (value *Value) GetString() (*String, bool) {
	var variant *C.traeger_string_t
	if C.traeger_value_get_string(value.self, &variant) {
		return wrap_c_string(variant), true
	}
	return nil, false
}

func (value *Value) GetList() (*List, bool) {
	var variant *C.traeger_list_t
	if C.traeger_value_get_list(value.self, &variant) {
		return wrap_c_list(variant), true
	}
	return nil, false
}

func (value *Value) GetMap() (*Map, bool) {
	var variant *C.traeger_map_t
	if C.traeger_value_get_map(value.self, &variant) {
		return wrap_c_map(variant), true
	}
	return nil, false
}
