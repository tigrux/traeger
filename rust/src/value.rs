use crate::*;

// Types

pub type Null = ();
pub const NULL: () = ();

pub type Bool = bool;

pub type Int = i64;

pub type UInt = u64;

pub type Float = f64;

#[derive(Clone, Debug, PartialEq)]
pub enum Variant {
    Null,
    Bool(Bool),
    Int(Int),
    UInt(UInt),
    Float(Float),
    String(String),
    List(List),
    Map(Map),
}

// String

pub struct String {
    pub(crate) ptr: *mut c::traeger_string_t,
}

impl String {
    pub fn as_ptr(&self) -> *const u8 {
        unsafe { c::traeger_string_data(self.ptr) }
    }

    pub fn len(&self) -> usize {
        unsafe { c::traeger_string_size(self.ptr) }
    }

    pub fn as_slice(&self) -> &[u8] {
        unsafe { std::slice::from_raw_parts(self.as_ptr(), self.len()) }
    }

    pub fn as_str(&self) -> &str {
        unsafe { std::str::from_utf8_unchecked(self.as_slice()) }
    }
}

impl Drop for String {
    fn drop(&mut self) {
        unsafe {
            c::traeger_string_free(self.ptr);
        }
    }
}

impl Clone for String {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_string_copy(self.ptr);
            String { ptr }
        }
    }
}

impl std::fmt::Display for String {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        f.write_str(self.as_str())
    }
}

impl std::fmt::Debug for String {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        f.write_str(self.as_str())
    }
}

impl PartialEq for String {
    fn eq(&self, other: &Self) -> bool {
        unsafe { c::traeger_string_equal(self.ptr, other.ptr) }
    }
}

impl From<&str> for String {
    fn from(slice: &str) -> Self {
        unsafe {
            let ptr = c::traeger_string_new(slice.as_ptr(), slice.len());
            String { ptr }
        }
    }
}

impl From<&[u8]> for String {
    fn from(slice: &[u8]) -> Self {
        unsafe {
            let ptr = c::traeger_string_new(slice.as_ptr(), slice.len());
            String { ptr }
        }
    }
}

// List

pub struct List {
    pub(crate) ptr: *mut c::traeger_list_t,
}

impl List {
    pub fn new() -> Self {
        unsafe {
            let ptr = c::traeger_list_new();
            List { ptr }
        }
    }

    pub fn to_string(&self) -> String {
        unsafe {
            let mut ptr: *mut c::traeger_string_t = std::ptr::null_mut();
            c::traeger_list_to_string(self.ptr, &mut ptr);
            String { ptr }
        }
    }

    pub fn find_value(&self, index: i32) -> Option<Value> {
        unsafe {
            let mut ptr: *mut c::traeger_value_t = std::ptr::null_mut();
            if c::traeger_list_find(self.ptr, index, &mut ptr) {
                let value = Value { ptr };
                Some(value)
            } else {
                None
            }
        }
    }

    pub fn get(&self, index: i32) -> Option<Variant> {
        match self.find_value(index) {
            Some(value) => Some(value.to_variant()),
            None => None,
        }
    }

    pub fn is_empty(&self) -> bool {
        unsafe { c::traeger_list_empty(self.ptr) }
    }

    pub fn len(&self) -> usize {
        unsafe { c::traeger_list_size(self.ptr) }
    }

    pub fn resize(&mut self, size: usize) -> usize {
        unsafe { c::traeger_list_resize(self.ptr, size) }
    }

    pub fn iter<'a>(&'a self) -> ListIterator<'a> {
        unsafe {
            let ptr = c::traeger_list_iterator_new(self.ptr);
            ListIterator {
                ptr,
                _marker: std::marker::PhantomData,
            }
        }
    }
}

pub trait Append<V> {
    fn append(&mut self, value: V);
}

impl Append<Null> for List {
    fn append(&mut self, _variant: Null) {
        unsafe {
            c::traeger_list_append_null(self.ptr);
        }
    }
}

impl Append<Bool> for List {
    fn append(&mut self, value: Bool) {
        unsafe {
            c::traeger_list_append_bool(self.ptr, value);
        }
    }
}

impl Append<Int> for List {
    fn append(&mut self, value: Int) {
        unsafe {
            c::traeger_list_append_int(self.ptr, value);
        }
    }
}

impl Append<i32> for List {
    fn append(&mut self, value: i32) {
        self.append(Int::from(value));
    }
}

impl Append<u32> for List {
    fn append(&mut self, value: u32) {
        self.append(Int::from(value));
    }
}

impl Append<UInt> for List {
    fn append(&mut self, value: UInt) {
        unsafe {
            c::traeger_list_append_uint(self.ptr, value);
        }
    }
}

impl Append<Float> for List {
    fn append(&mut self, value: Float) {
        unsafe {
            c::traeger_list_append_float(self.ptr, value);
        }
    }
}

impl Append<&str> for List {
    fn append(&mut self, string: &str) {
        unsafe {
            c::traeger_list_append_string(self.ptr, string.as_ptr(), string.len());
        }
    }
}

impl Append<&List> for List {
    fn append(&mut self, list: &List) {
        unsafe {
            c::traeger_list_append_list(self.ptr, list.ptr);
        }
    }
}

impl Append<&Map> for List {
    fn append(&mut self, map: &Map) {
        unsafe {
            c::traeger_list_append_map(self.ptr, map.ptr);
        }
    }
}

impl Append<Variant> for List {
    fn append(&mut self, variant: Variant) {
        match variant {
            Variant::Null => self.append(NULL),
            Variant::Bool(value) => self.append(value),
            Variant::Int(value) => self.append(value),
            Variant::UInt(value) => self.append(value),
            Variant::Float(value) => self.append(value),
            Variant::String(string) => self.append(string.as_str()),
            Variant::List(list) => self.append(&list),
            Variant::Map(map) => self.append(&map),
        }
    }
}

pub trait SetIndex<V> {
    fn set(&mut self, index: i32, value: V);
}

impl SetIndex<Null> for List {
    fn set(&mut self, index: i32, _value: Null) {
        unsafe {
            c::traeger_list_set_null(self.ptr, index);
        }
    }
}

impl SetIndex<Bool> for List {
    fn set(&mut self, index: i32, value: Bool) {
        unsafe {
            c::traeger_list_set_bool(self.ptr, index, value);
        }
    }
}

impl SetIndex<Int> for List {
    fn set(&mut self, index: i32, value: Int) {
        unsafe {
            c::traeger_list_set_int(self.ptr, index, value);
        }
    }
}

impl SetIndex<i32> for List {
    fn set(&mut self, index: i32, value: i32) {
        self.set(index, Int::from(value));
    }
}

impl SetIndex<u32> for List {
    fn set(&mut self, index: i32, value: u32) {
        self.set(index, Int::from(value));
    }
}

impl SetIndex<UInt> for List {
    fn set(&mut self, index: i32, value: UInt) {
        unsafe {
            c::traeger_list_set_uint(self.ptr, index, value);
        }
    }
}

impl SetIndex<Float> for List {
    fn set(&mut self, index: i32, value: Float) {
        unsafe {
            c::traeger_list_set_float(self.ptr, index, value);
        }
    }
}

impl SetIndex<&str> for List {
    fn set(&mut self, index: i32, string: &str) {
        unsafe {
            c::traeger_list_set_string(self.ptr, index, string.as_ptr(), string.len());
        }
    }
}

impl SetIndex<&List> for List {
    fn set(&mut self, index: i32, list: &List) {
        unsafe {
            c::traeger_list_set_list(self.ptr, index, list.ptr);
        }
    }
}

impl SetIndex<&Map> for List {
    fn set(&mut self, index: i32, map: &Map) {
        unsafe {
            c::traeger_list_set_map(self.ptr, index, map.ptr);
        }
    }
}

impl SetIndex<Variant> for List {
    fn set(&mut self, index: i32, variant: Variant) {
        match variant {
            Variant::Null => self.set(index, NULL),
            Variant::Bool(value) => self.set(index, value),
            Variant::Int(value) => self.set(index, value),
            Variant::UInt(value) => self.set(index, value),
            Variant::Float(value) => self.set(index, value),
            Variant::String(string) => self.set(index, string.as_str()),
            Variant::List(list) => self.set(index, &list),
            Variant::Map(map) => self.set(index, &map),
        }
    }
}

impl Drop for List {
    fn drop(&mut self) {
        unsafe {
            c::traeger_list_free(self.ptr);
        }
    }
}

impl Clone for List {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_list_copy(self.ptr);
            List { ptr }
        }
    }
}

impl std::fmt::Display for List {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        f.write_str(self.to_string().as_str())
    }
}

impl std::fmt::Debug for List {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        f.write_str(self.to_string().as_str())
    }
}

impl PartialEq for List {
    fn eq(&self, other: &Self) -> bool {
        unsafe { c::traeger_list_equal(self.ptr, other.ptr) }
    }
}

// ListIterator

pub struct ListIterator<'a> {
    ptr: *mut c::traeger_list_iterator_t,
    _marker: std::marker::PhantomData<&'a Value>,
}

impl<'a> IntoIterator for &'a List {
    type Item = Value;
    type IntoIter = ListIterator<'a>;

    fn into_iter(self) -> Self::IntoIter {
        return self.iter();
    }
}

impl<'a> Drop for ListIterator<'a> {
    fn drop(&mut self) {
        unsafe {
            c::traeger_list_iterator_free(self.ptr);
        }
    }
}

impl<'a> std::iter::Iterator for ListIterator<'a> {
    type Item = Value;

    fn next(&mut self) -> Option<Self::Item> {
        unsafe {
            let mut ptr: *mut c::traeger_value_t = std::ptr::null_mut();
            if c::traeger_list_iterator_next(self.ptr, &mut ptr) {
                let value = Value { ptr };
                Some(value)
            } else {
                None
            }
        }
    }
}

// Map

pub struct Map {
    pub(crate) ptr: *mut c::traeger_map_t,
}

impl Map {
    pub fn new() -> Self {
        unsafe {
            let ptr = c::traeger_map_new();
            Map { ptr }
        }
    }

    pub fn to_string(&self) -> String {
        unsafe {
            let mut ptr: *mut c::traeger_string_t = std::ptr::null_mut();
            c::traeger_map_to_string(self.ptr, &mut ptr);
            String { ptr }
        }
    }

    pub fn find_value(&self, key: &str) -> Option<Value> {
        unsafe {
            let mut ptr: *mut c::traeger_value_t = std::ptr::null_mut();
            if c::traeger_map_find(self.ptr, key.as_ptr(), key.len(), &mut ptr) {
                let value = Value { ptr };
                Some(value)
            } else {
                None
            }
        }
    }

    pub fn get(&self, key: &str) -> Option<Variant> {
        match self.find_value(key) {
            Some(value) => Some(value.to_variant()),
            None => None,
        }
    }

    pub fn erase(&mut self, key: &str) {
        unsafe {
            c::traeger_map_erase(self.ptr, key.as_ptr(), key.len());
        }
    }

    pub fn contains(&mut self, key: &str) -> bool {
        unsafe { c::traeger_map_contains(self.ptr, key.as_ptr(), key.len()) }
    }

    pub fn is_empty(&self) -> bool {
        unsafe { c::traeger_map_empty(self.ptr) }
    }

    pub fn len(&self) -> usize {
        unsafe { c::traeger_map_size(self.ptr) }
    }

    pub fn iter<'a>(&'a self) -> MapIterator<'a> {
        unsafe {
            let ptr = c::traeger_map_iterator_new(self.ptr);
            MapIterator {
                ptr,
                _marker: std::marker::PhantomData,
            }
        }
    }
}

pub trait SetKey<V> {
    fn set(&mut self, key: &str, value: V);
}

impl SetKey<Null> for Map {
    fn set(&mut self, key: &str, _variant: Null) {
        unsafe {
            c::traeger_map_set_null(self.ptr, key.as_ptr(), key.len());
        }
    }
}

impl SetKey<Bool> for Map {
    fn set(&mut self, key: &str, value: Bool) {
        unsafe {
            c::traeger_map_set_bool(self.ptr, key.as_ptr(), key.len(), value);
        }
    }
}

impl SetKey<Int> for Map {
    fn set(&mut self, key: &str, value: Int) {
        unsafe {
            c::traeger_map_set_int(self.ptr, key.as_ptr(), key.len(), value);
        }
    }
}

impl SetKey<i32> for Map {
    fn set(&mut self, key: &str, value: i32) {
        self.set(key, Int::from(value));
    }
}

impl SetKey<u32> for Map {
    fn set(&mut self, key: &str, value: u32) {
        self.set(key, Int::from(value));
    }
}

impl SetKey<UInt> for Map {
    fn set(&mut self, key: &str, value: UInt) {
        unsafe {
            c::traeger_map_set_uint(self.ptr, key.as_ptr(), key.len(), value);
        }
    }
}

impl SetKey<Float> for Map {
    fn set(&mut self, key: &str, value: Float) {
        unsafe {
            c::traeger_map_set_float(self.ptr, key.as_ptr(), key.len(), value);
        }
    }
}

impl SetKey<&str> for Map {
    fn set(&mut self, key: &str, string: &str) {
        unsafe {
            c::traeger_map_set_string(
                self.ptr,
                key.as_ptr(),
                key.len(),
                string.as_ptr(),
                string.len(),
            );
        }
    }
}

impl SetKey<&List> for Map {
    fn set(&mut self, key: &str, list: &List) {
        unsafe {
            c::traeger_map_set_list(self.ptr, key.as_ptr(), key.len(), list.ptr);
        }
    }
}

impl SetKey<&Map> for Map {
    fn set(&mut self, key: &str, map: &Map) {
        unsafe {
            c::traeger_map_set_map(self.ptr, key.as_ptr(), key.len(), map.ptr);
        }
    }
}

impl SetKey<Variant> for Map {
    fn set(&mut self, key: &str, variant: Variant) {
        match variant {
            Variant::Null => self.set(key, NULL),
            Variant::Bool(value) => self.set(key, value),
            Variant::Int(value) => self.set(key, value),
            Variant::UInt(value) => self.set(key, value),
            Variant::Float(value) => self.set(key, value),
            Variant::String(string) => self.set(key, string.as_str()),
            Variant::List(list) => self.set(key, &list),
            Variant::Map(map) => self.set(key, &map),
        }
    }
}

impl Drop for Map {
    fn drop(&mut self) {
        unsafe {
            c::traeger_map_free(self.ptr);
        }
    }
}

impl Clone for Map {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_map_copy(self.ptr);
            Map { ptr }
        }
    }
}

impl std::fmt::Display for Map {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        f.write_str(self.to_string().as_str())
    }
}

impl std::fmt::Debug for Map {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        f.write_str(self.to_string().as_str())
    }
}

impl PartialEq for Map {
    fn eq(&self, other: &Self) -> bool {
        unsafe { c::traeger_map_equal(self.ptr, other.ptr) }
    }
}

// MapIterator

pub struct MapIterator<'a> {
    ptr: *mut c::traeger_map_iterator_t,
    _marker: std::marker::PhantomData<&'a Value>,
}

impl<'a> IntoIterator for &'a Map {
    type Item = (String, Value);
    type IntoIter = MapIterator<'a>;

    fn into_iter(self) -> Self::IntoIter {
        return self.iter();
    }
}

impl<'a> Drop for MapIterator<'a> {
    fn drop(&mut self) {
        unsafe {
            c::traeger_map_iterator_free(self.ptr);
        }
    }
}

impl<'a> std::iter::Iterator for MapIterator<'a> {
    type Item = (String, Value);

    fn next(&mut self) -> Option<Self::Item> {
        unsafe {
            let mut c_key: *mut c::traeger_string_t = std::ptr::null_mut();
            let mut c_value: *mut c::traeger_value_t = std::ptr::null_mut();
            if c::traeger_map_iterator_next(self.ptr, &mut c_key, &mut c_value) {
                let key = String { ptr: c_key };
                let value = Value { ptr: c_value };
                Some((key, value))
            } else {
                None
            }
        }
    }
}

// Value

pub struct Value {
    pub(crate) ptr: *mut c::traeger_value_t,
}

impl Value {
    pub fn new() -> Self {
        unsafe {
            let ptr = c::traeger_value_new();
            Value { ptr }
        }
    }

    pub fn to_string(&self) -> String {
        unsafe {
            let mut ptr: *mut c::traeger_string_t = std::ptr::null_mut();
            c::traeger_value_to_string(self.ptr, &mut ptr);
            String { ptr }
        }
    }

    pub fn to_variant(&self) -> Variant {
        const NULL: u32 = 0;
        const BOOL: u32 = 1;
        const INT: u32 = 2;
        const UINT: u32 = 3;
        const FLOAT: u32 = 4;
        const STRING: u32 = 5;
        const LIST: u32 = 6;
        const MAP: u32 = 7;
        unsafe {
            match c::traeger_value_get_type(self.ptr) {
                BOOL => {
                    let mut value: Bool = false;
                    c::traeger_value_get_bool(self.ptr, &mut value);
                    Variant::Bool(value)
                }
                INT => {
                    let mut value: Int = 0i64;
                    c::traeger_value_get_int(self.ptr, &mut value);
                    Variant::Int(value)
                }
                UINT => {
                    let mut value: UInt = 0u64;
                    c::traeger_value_get_uint(self.ptr, &mut value);
                    Variant::UInt(value)
                }
                FLOAT => {
                    let mut value: Float = 0f64;
                    c::traeger_value_get_float(self.ptr, &mut value);
                    Variant::Float(value)
                }
                STRING => {
                    let mut ptr: *mut c::traeger_string_t = std::ptr::null_mut();
                    c::traeger_value_get_string(self.ptr, &mut ptr);
                    let string = String { ptr };
                    Variant::String(string)
                }
                LIST => {
                    let mut ptr: *mut c::traeger_list_t = std::ptr::null_mut();
                    c::traeger_value_get_list(self.ptr, &mut ptr);
                    let list = List { ptr };
                    Variant::List(list)
                }
                MAP => {
                    let mut ptr: *mut c::traeger_map_t = std::ptr::null_mut();
                    c::traeger_value_get_map(self.ptr, &mut ptr);
                    let map = Map { ptr };
                    Variant::Map(map)
                }
                NULL | _ => Variant::Null,
            }
        }
    }
}

pub trait Get<V> {
    fn get(&self) -> Option<V>;
}

impl Get<Null> for Value {
    fn get(&self) -> Option<Null> {
        unsafe {
            if c::traeger_value_get_null(self.ptr) {
                Some(NULL)
            } else {
                None
            }
        }
    }
}

impl Get<Bool> for Value {
    fn get(&self) -> Option<Bool> {
        unsafe {
            let mut value: Bool = false;
            if c::traeger_value_get_bool(self.ptr, &mut value) {
                Some(value)
            } else {
                None
            }
        }
    }
}

impl Get<Int> for Value {
    fn get(&self) -> Option<Int> {
        unsafe {
            let mut value: Int = 0;
            if c::traeger_value_get_int(self.ptr, &mut value) {
                Some(value)
            } else {
                None
            }
        }
    }
}

impl Get<UInt> for Value {
    fn get(&self) -> Option<UInt> {
        unsafe {
            let mut value: UInt = 0;
            if c::traeger_value_get_uint(self.ptr, &mut value) {
                Some(value)
            } else {
                None
            }
        }
    }
}

impl Get<Float> for Value {
    fn get(&self) -> Option<Float> {
        unsafe {
            let mut value: Float = 0.0;
            if c::traeger_value_get_float(self.ptr, &mut value) {
                Some(value)
            } else {
                None
            }
        }
    }
}

impl Get<String> for Value {
    fn get(&self) -> Option<String> {
        unsafe {
            let mut ptr: *mut c::traeger_string_t = std::ptr::null_mut();
            if c::traeger_value_get_string(self.ptr, &mut ptr) {
                let string = String { ptr };
                Some(string)
            } else {
                None
            }
        }
    }
}

impl Get<std::string::String> for Value {
    fn get(&self) -> Option<std::string::String> {
        match self.get() as Option<String> {
            Some(string) => Some(std::string::String::from(string.as_str())),
            None => None,
        }
    }
}

impl Get<List> for Value {
    fn get(&self) -> Option<List> {
        unsafe {
            let mut ptr: *mut c::traeger_list_t = std::ptr::null_mut();
            if c::traeger_value_get_list(self.ptr, &mut ptr) {
                let list = List { ptr };
                Some(list)
            } else {
                None
            }
        }
    }
}

impl Get<Map> for Value {
    fn get(&self) -> Option<Map> {
        unsafe {
            let mut ptr: *mut c::traeger_map_t = std::ptr::null_mut();
            if c::traeger_value_get_map(self.ptr, &mut ptr) {
                let map = Map { ptr };
                Some(map)
            } else {
                None
            }
        }
    }
}

impl Get<Variant> for Value {
    fn get(&self) -> Option<Variant> {
        Some(self.to_variant())
    }
}

pub trait Set<V> {
    fn set(&mut self, value: V);
}

impl Set<Null> for Value {
    fn set(&mut self, _value: Null) {
        unsafe {
            c::traeger_value_set_null(self.ptr);
        }
    }
}

impl Set<Bool> for Value {
    fn set(&mut self, value: Bool) {
        unsafe {
            c::traeger_value_set_bool(self.ptr, value);
        }
    }
}

impl Set<Int> for Value {
    fn set(&mut self, value: Int) {
        unsafe {
            c::traeger_value_set_int(self.ptr, value);
        }
    }
}

impl Set<i32> for Value {
    fn set(&mut self, value: i32) {
        self.set(Int::from(value));
    }
}

impl Set<u32> for Value {
    fn set(&mut self, value: u32) {
        self.set(Int::from(value));
    }
}

impl Set<UInt> for Value {
    fn set(&mut self, value: UInt) {
        unsafe {
            c::traeger_value_set_uint(self.ptr, value);
        }
    }
}

impl Set<Float> for Value {
    fn set(&mut self, value: Float) {
        unsafe {
            c::traeger_value_set_float(self.ptr, value);
        }
    }
}

impl Set<&str> for Value {
    fn set(&mut self, string: &str) {
        unsafe {
            c::traeger_value_set_string(self.ptr, string.as_ptr(), string.len());
        }
    }
}

impl Set<&List> for Value {
    fn set(&mut self, list: &List) {
        unsafe {
            c::traeger_value_set_list(self.ptr, list.ptr);
        }
    }
}

impl Set<&Map> for Value {
    fn set(&mut self, map: &Map) {
        unsafe {
            c::traeger_value_set_map(self.ptr, map.ptr);
        }
    }
}

impl Set<Variant> for Value {
    fn set(&mut self, variant: Variant) {
        match variant {
            Variant::Null => self.set(NULL),
            Variant::Bool(value) => self.set(value),
            Variant::Int(value) => self.set(value),
            Variant::UInt(value) => self.set(value),
            Variant::Float(value) => self.set(value),
            Variant::String(string) => self.set(string.as_str()),
            Variant::List(list) => self.set(&list),
            Variant::Map(map) => self.set(&map),
        }
    }
}

impl<T> From<T> for Value
where
    Value: Set<T>,
{
    fn from(variant: T) -> Self {
        let mut value = Value::new();
        value.set(variant);
        value
    }
}

impl Drop for Value {
    fn drop(&mut self) {
        unsafe {
            c::traeger_value_free(self.ptr);
        }
    }
}

impl Clone for Value {
    fn clone(&self) -> Self {
        unsafe {
            let ptr = c::traeger_value_copy(self.ptr);
            Value { ptr }
        }
    }
}

impl std::fmt::Display for Value {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        f.write_str(self.to_string().as_str())
    }
}

impl std::fmt::Debug for Value {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        f.write_str(self.to_string().as_str())
    }
}

impl PartialEq for Value {
    fn eq(&self, other: &Self) -> bool {
        unsafe { c::traeger_value_equal(self.ptr, other.ptr) }
    }
}

#[macro_export]
macro_rules! list {
    ( $($value:expr),* ) => {{
        use $crate::Append;
        let mut l = traeger::value::List::new();
        $(
            l.append($value);
        )*
        l
    }};
}

#[macro_export]
macro_rules! map {
    ( $($key:expr => $value:expr),* ) => {{
        use $crate::SetKey;
        let mut m = traeger::value::Map::new();
        $(
            m.set($key, $value);
        )*
        m
    }};
}

#[macro_export]
macro_rules! unpack {
    ( $container:expr $(, $item:expr => $to:ident)* ) => {{
        use $crate::Get;
        (|| {
            let container = &$container;
            let tuple = (
                $(
                    match container.find_value($item) {
                        None => {
                            return Err(
                                format!(
                                    "invalid item '{}'",
                                    $item))
                        },
                        Some(value) => {
                            match value.get() as Option<$to> {
                                None => {
                                    return Err(
                                        format!(
                                            "invalid conversion of item '{}' to type {}",
                                            $item,
                                            stringify!($to)))
                                },
                                Some(value) => value,
                            }
                        }
                    },
                )*
            );
            Ok(tuple)
        })()
    }};
}
