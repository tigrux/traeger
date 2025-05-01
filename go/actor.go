// SPDX-License-Identifier: BSL-1.0

package traeger

import (
	"errors"
	"runtime"
	"runtime/cgo"
	"time"
)

/*
#cgo LDFLAGS: -ltraeger_actor
#include <traeger/actor/actor.h>

extern void traeger_go_promise_result_callback(const traeger_value_t *value,
                                               traeger_closure_t closure,
                                               traeger_result_t *result);

extern void traeger_go_promise_promise_callback(const traeger_value_t *value,
                                                traeger_closure_t closure,
                                                const traeger_promise_t *promise);

extern void traeger_go_promise_error_callback(const traeger_string_t *error, traeger_closure_t handle);

extern void traeger_go_closure_free(traeger_closure_t handle);

static inline traeger_promise_t *
traeger_go_promise_then_result(const traeger_promise_t *self, uintptr_t handle)
{
    void *closure = (void*)handle;
    return traeger_promise_then_result(self, traeger_go_promise_result_callback,
                                       closure, traeger_go_closure_free);
}

static inline traeger_promise_t *
traeger_go_promise_then_promise(const traeger_promise_t *self, uintptr_t handle)
{
    void *closure = (void*)handle;
    return traeger_promise_then_promise(self, traeger_go_promise_promise_callback,
                                        closure, traeger_go_closure_free);
}

static inline void traeger_go_promise_fail(const traeger_promise_t *self, uintptr_t handle)
{
    void *closure = (void*)handle;
    traeger_promise_fail(self, traeger_go_promise_error_callback,
                         closure, traeger_go_closure_free);
}

extern void traeger_go_work_callback(traeger_closure_t handle);

extern void traeger_go_closure_free(traeger_closure_t handle);

static inline void traeger_go_scheduler_schedule(traeger_scheduler_t *self,
                                                 uintptr_t handle)
{
    void *closure = (void*)handle;
    traeger_scheduler_schedule(self, traeger_go_work_callback,
                               closure, traeger_go_closure_free);
}

static inline void traeger_go_scheduler_schedule_delayed(traeger_scheduler_t *self,
                                                         traeger_float_t delay,
                                                         uintptr_t handle)
{
    void *closure = (void*)handle;
    traeger_scheduler_schedule_delayed(self, delay, traeger_go_work_callback,
                                       closure, traeger_go_closure_free);
}

extern void traeger_go_function_callback(const traeger_list_t *arguments,
                                         traeger_closure_t closure,
                                         traeger_result_t *result);

extern void traeger_go_closure_free(traeger_closure_t handle);

static inline traeger_function_t *
traeger_go_function_new(uintptr_t handle)
{
    void *closure = (void*)handle;
	return traeger_function_new(traeger_go_function_callback,
	                            closure,
								traeger_go_closure_free);
}
*/
import "C"

// Result

type Result struct {
	self *C.traeger_result_t
}

func FreeResult(result *Result) {
	C.traeger_result_free(result.self)
}

func wrap_c_result(self *C.traeger_result_t) *Result {
	result := Result{self}
	runtime.SetFinalizer(&result, FreeResult)
	return &result
}

func NewResult() *Result {
	return wrap_c_result(C.traeger_result_new())
}

func MakeResult(variant any) *Result {
	result := NewResult()
	result.Set(variant)
	return result
}

func (result *Result) GetValueOrError() (*Value, error) {
	var value *C.traeger_value_t
	var err *C.traeger_string_t
	switch C.traeger_result_get_value_or_error(result.self, &value, &err) {
	case C.TRAEGER_RESULT_TYPE_VALUE:
		return wrap_c_value(value), nil
	case C.TRAEGER_RESULT_TYPE_ERROR:
		return nil, errors.New(wrap_c_string(err).String())
	}
	return nil, nil
}

func (result *Result) Set(variant any) bool {
	switch variant := variant.(type) {
	case bool:
		result.SetBool(variant)
		return true
	case int:
		result.SetInt(int64(variant))
		return true
	case int32:
		result.SetInt(int64(variant))
		return true
	case int64:
		result.SetInt(variant)
		return true
	case uint:
		result.SetUInt(uint64(variant))
		return true
	case uint32:
		result.SetUInt(uint64(variant))
		return true
	case uint64:
		result.SetUInt(variant)
		return true
	case float32:
		result.SetFloat(float64(variant))
		return true
	case float64:
		result.SetFloat(variant)
		return true
	case string:
		result.SetString(variant)
		return true
	case *List:
		result.SetList(variant)
		return true
	case *Map:
		result.SetMap(variant)
		return true
	case *Value:
		result.SetValue(variant)
		return true
	case *Result:
		result.SetResult(variant)
		return true
	case error:
		result.SetError(variant)
		return true
	default:
		result.SetNull(nil)
		return false
	}
}

func (result *Result) SetNull(any) {
	C.traeger_result_set_null(result.self, nil)
}

func (result *Result) SetBool(variant bool) {
	C.traeger_result_set_bool(result.self, C.traeger_bool_t(variant))
}

func (result *Result) SetInt(variant int64) {
	C.traeger_result_set_int(result.self, C.traeger_int_t(variant))
}

func (result *Result) SetUInt(variant uint64) {
	C.traeger_result_set_uint(result.self, C.traeger_uint_t(variant))
}

func (result *Result) SetFloat(variant float64) {
	C.traeger_result_set_float(result.self, C.traeger_float_t(variant))
}

func (result *Result) SetString(variant string) {
	C.traeger_result_set_string(result.self, C._GoStringPtr(variant), C._GoStringLen(variant))
}

func (result *Result) SetList(variant *List) {
	C.traeger_result_set_list(result.self, variant.self)
}

func (result *Result) SetMap(variant *Map) {
	C.traeger_result_set_map(result.self, variant.self)
}

func (result *Result) SetValue(variant *Value) {
	C.traeger_result_set_value(result.self, variant.self)
}

func (result *Result) SetResult(variant *Result) {
	C.traeger_result_set_result(result.self, variant.self)
}

func (result *Result) SetError(err error) {
	str := err.Error()
	C.traeger_result_set_error(result.self, C._GoStringPtr(str), C._GoStringLen(str))
}

// Function

type FunctionFunc func(arguments *List) (any, error)

type Function struct {
	self *C.traeger_function_t
}

func FreeFunction(function *Function) {
	C.traeger_function_free(function.self)
}

func wrap_c_function(self *C.traeger_function_t) *Function {
	function := Function{self}
	runtime.SetFinalizer(&function, FreeFunction)
	return &function
}

func NewFunction(functionFunc FunctionFunc) *Function {
	handle := cgo.NewHandle(functionFunc)
	return wrap_c_function(C.traeger_go_function_new(C.uintptr_t(handle)))
}

func (function *Function) Call(arguments *List) (any, error) {
	var c_result *C.traeger_result_t
	C.traeger_function_call(function.self, arguments.self, &c_result)
	result := wrap_c_result(c_result)
	return result.GetValueOrError()
}

// Promise

type PromiseResultFunc func(value *Value) (any, error)

type PromisePromiseFunc func(value *Value) *Promise

type PromiseErrorFunc func(err error)

//export traeger_go_promise_result_callback
func traeger_go_promise_result_callback(c_value *C.traeger_const_value_t, closure C.traeger_closure_t, c_result *C.traeger_result_t) {
	handle := cgo.Handle(closure)
	valueFunc := handle.Value().(PromiseResultFunc)
	value := wrap_c_value(c_value)
	result := &Result{c_result}
	returned, err := valueFunc(value)
	if returned != nil {
		result.Set(returned)
	} else if err != nil {
		result.SetError(err)
	}
}

//export traeger_go_promise_promise_callback
func traeger_go_promise_promise_callback(c_value *C.traeger_const_value_t, closure C.traeger_closure_t, c_promise *C.traeger_const_promise_t) {
	handle := cgo.Handle(closure)
	resultFunc := handle.Value().(PromisePromiseFunc)
	value := wrap_c_value(c_value)
	promise := &Promise{c_promise}
	returned := resultFunc(value)
	promise.SetPromise(returned)
}

//export traeger_go_promise_error_callback
func traeger_go_promise_error_callback(c_error *C.traeger_const_string_t, closure C.traeger_closure_t) {
	handle := cgo.Handle(closure)
	errorFunc := handle.Value().(PromiseErrorFunc)
	err := String{c_error}
	errorFunc(errors.New(err.String()))
}

type Promise struct {
	self *C.traeger_promise_t
}

func FreePromise(promise *Promise) {
	C.traeger_promise_free(promise.self)
}

func wrap_c_promise(self *C.traeger_promise_t) *Promise {
	promise := Promise{self}
	runtime.SetFinalizer(&promise, FreePromise)
	return &promise
}

func NewPromise(scheduler *Scheduler) *Promise {
	return wrap_c_promise(C.traeger_promise_new(scheduler.self))
}

func (promise *Promise) SetResult(result *Result) bool {
	return bool(C.traeger_promise_set_result(promise.self, result.self))
}

func (promise *Promise) GetResult() *Result {
	var result *C.traeger_result_t
	C.traeger_promise_get_result(promise.self, &result)
	return wrap_c_result(result)
}

func (promise *Promise) SetPromise(other *Promise) {
	C.traeger_promise_set_promise(promise.self, other.self)
}

func (promise *Promise) ThenResult(resultFunc PromiseResultFunc) *Promise {
	handle := cgo.NewHandle(resultFunc)
	c_promise := C.traeger_go_promise_then_result(promise.self, C.uintptr_t(handle))
	return wrap_c_promise(c_promise)
}

func (promise *Promise) ThenPromise(resultFunc PromisePromiseFunc) *Promise {
	handle := cgo.NewHandle(resultFunc)
	c_promise := C.traeger_go_promise_then_promise(promise.self, C.uintptr_t(handle))
	return wrap_c_promise(c_promise)
}

func (promise *Promise) Fail(errorFunc PromiseErrorFunc) {
	handle := cgo.NewHandle(errorFunc)
	C.traeger_go_promise_fail(promise.self, C.uintptr_t(handle))
}

// Scheduler

type WorkFunc func()

//export traeger_go_work_callback
func traeger_go_work_callback(closure C.traeger_closure_t) {
	h := cgo.Handle(closure)
	work := h.Value().(WorkFunc)
	work()
}

//export traeger_go_closure_free
func traeger_go_closure_free(closure C.traeger_closure_t) {
	h := cgo.Handle(closure)
	h.Delete()
}

type Scheduler struct {
	self *C.traeger_scheduler_t
}

func FreeScheduler(scheduler *Scheduler) {
	C.traeger_scheduler_free(scheduler.self)
}

func wrap_c_scheduler(self *C.traeger_scheduler_t) *Scheduler {
	scheduler := Scheduler{self}
	runtime.SetFinalizer(&scheduler, FreeScheduler)
	return &scheduler
}

func NewScheduler(threadsCount uint) *Scheduler {
	return wrap_c_scheduler(C.traeger_scheduler_new(C.uint(threadsCount)))
}

func (scheduler *Scheduler) Count() uint {
	return uint(C.traeger_scheduler_count(scheduler.self))
}

func (scheduler *Scheduler) Schedule(work WorkFunc) {
	h := cgo.NewHandle(work)
	C.traeger_go_scheduler_schedule(scheduler.self, C.uintptr_t(h))
}

func (scheduler *Scheduler) ScheduleDelayed(delay time.Duration, work WorkFunc) {
	h := cgo.NewHandle(work)
	C.traeger_go_scheduler_schedule_delayed(scheduler.self, C.traeger_float_t(delay.Seconds()), C.uintptr_t(h))
}

// Mailbox

type Mailbox struct {
	self *C.traeger_mailbox_t
}

func FreeMailbox(mailbox *Mailbox) {
	C.traeger_mailbox_free(mailbox.self)
}

func wrap_c_mailbox(self *C.traeger_mailbox_t) *Mailbox {
	malbox := Mailbox{self}
	runtime.SetFinalizer(&malbox, FreeMailbox)
	return &malbox
}

func (mailbox *Mailbox) Send(sched *Scheduler, name string, arguments ...any) *Promise {
	list := MakeList(arguments)
	c_prom := C.traeger_mailbox_send(mailbox.self,
		sched.self,
		C._GoStringPtr(name), C._GoStringLen(name),
		list.self)
	return wrap_c_promise(c_prom)
}

// Actor

type MethodFunc[State any] func(state *State, values *List) (any, error)

//export traeger_go_function_callback
func traeger_go_function_callback(c_arguments *C.traeger_const_list_t, closure C.traeger_closure_t, c_result *C.traeger_result_t) {
	h := cgo.Handle(closure)
	functionFunc := h.Value().(FunctionFunc)
	arguments := &List{c_arguments}
	result := &Result{c_result}
	returned, err := functionFunc(arguments)
	if returned != nil {
		result.Set(returned)
	} else if err != nil {
		result.SetError(err)
	}
}

type BaseActor struct {
	self *C.traeger_actor_t
}

func (actor *BaseActor) Mailbox() *Mailbox {
	return wrap_c_mailbox(C.traeger_actor_get_mailbox(actor.self))
}

type Actor[State any] struct {
	BaseActor
	state *State
}

func FreeActor[State any](actor *Actor[State]) {
	C.traeger_actor_free(actor.self)
}

func wrap_c_actor[State any](self *C.traeger_actor_t, state *State) *Actor[State] {
	actor := Actor[State]{BaseActor{self}, state}
	runtime.SetFinalizer(&actor, FreeActor[State])
	return &actor
}

func MakeActor[State any](state *State) *Actor[State] {
	return wrap_c_actor(C.traeger_actor_new(), state)
}

func (actor *Actor[State]) DefineReader(name string, methodFunc MethodFunc[State]) {
	function := NewFunction(func(arguments *List) (any, error) {
		return methodFunc(actor.state, arguments)
	})
	C.traeger_actor_define_reader(
		actor.BaseActor.self,
		C._GoStringPtr(name), C._GoStringLen(name),
		function.self)
}

func (actor *Actor[State]) DefineWriter(name string, methodFunc MethodFunc[State]) {
	function := NewFunction(func(arguments *List) (any, error) {
		return methodFunc(actor.state, arguments)
	})
	C.traeger_actor_define_writer(
		actor.BaseActor.self,
		C._GoStringPtr(name), C._GoStringLen(name),
		function.self)
}

// Queue

type Queue struct {
	self *C.traeger_queue_t
}

func FreeQueue(queue *Queue) {
	C.traeger_queue_free(queue.self)
}

func wrap_c_queue(self *C.traeger_queue_t) *Queue {
	queue := Queue{self}
	runtime.SetFinalizer(&queue, FreeQueue)
	return &queue
}

func NewQueue() *Queue {
	return wrap_c_queue(C.traeger_queue_new())
}

func (queue *Queue) Closed() bool {
	return bool(C.traeger_queue_closed(queue.self))
}

func (queue *Queue) Count() uint {
	return uint(C.traeger_queue_count(queue.self))
}

func (queue *Queue) Push(variant any) bool {
	switch variant := variant.(type) {
	case bool:
		return bool(queue.PushBool(variant))
	case int:
		return bool(queue.PushInt(int64(variant)))
	case int32:
		return bool(queue.PushInt(int64(variant)))
	case int64:
		return bool(queue.PushInt(variant))
	case uint:
		return bool(queue.PushUInt(uint64(variant)))
	case uint32:
		return bool(queue.PushUInt(uint64(variant)))
	case uint64:
		return bool(queue.PushUInt(variant))
	case float32:
		return bool(queue.PushFloat(float64(variant)))
	case float64:
		return bool(queue.PushFloat(variant))
	case string:
		return bool(queue.PushString(variant))
	case *List:
		return bool(queue.PushList(variant))
	case *Map:
		return bool(queue.PushMap(variant))
	case *Value:
		return bool(queue.PushValue(variant))
	default:
		return bool(queue.PushNull(nil))
	}
}

func (queue *Queue) PushNull(variant any) bool {
	return bool(C.traeger_queue_push_null(queue.self))
}

func (queue *Queue) PushBool(variant bool) bool {
	return bool(C.traeger_queue_push_bool(queue.self, C.traeger_bool_t(variant)))
}

func (queue *Queue) PushInt(variant int64) bool {
	return bool(C.traeger_queue_push_int(queue.self, C.traeger_int_t(variant)))
}

func (queue *Queue) PushUInt(variant uint64) bool {
	return bool(C.traeger_queue_push_uint(queue.self, C.traeger_uint_t(variant)))
}

func (queue *Queue) PushFloat(variant float64) bool {
	return bool(C.traeger_queue_push_float(queue.self, C.traeger_float_t(variant)))
}

func (queue *Queue) PushString(variant string) bool {
	return bool(C.traeger_queue_push_string(queue.self, C._GoStringPtr(variant), C._GoStringLen(variant)))
}

func (queue *Queue) PushList(variant *List) bool {
	return bool(C.traeger_queue_push_list(queue.self, variant.self))
}

func (queue *Queue) PushMap(variant *Map) bool {
	return bool(C.traeger_queue_push_map(queue.self, variant.self))
}

func (queue *Queue) PushValue(variant *Value) bool {
	return bool(C.traeger_queue_push_value(queue.self, variant.self))
}

func (queue *Queue) PopValue() (*Value, bool) {
	var value *C.traeger_value_t
	if C.traeger_queue_pop_value(queue.self, &value) {
		return wrap_c_value(value), true
	}
	return nil, false
}

func (queue *Queue) PopIntoList(list *List) bool {
	return bool(C.traeger_queue_pop_into_list(queue.self, list.self))
}

func (queue *Queue) Values() func(func(value *Value) bool) {
	return func(yield func(value *Value) bool) {
		var value *C.traeger_value_t
		for C.traeger_queue_pop_value(queue.self, &value) {
			if !yield(wrap_c_value(value)) {
				return
			}
		}
	}
}

func (queue *Queue) Close() {
	C.traeger_queue_close(queue.self)
}
