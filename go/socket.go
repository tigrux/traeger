// SPDX-License-Identifier: BSL-1.0

package traeger

import (
	"errors"
	"runtime"
	"runtime/cgo"
	"unsafe"
)

/*
#cgo LDFLAGS: -ltraeger_socket
#include <stdlib.h>
#include <traeger/socket/socket.h>

extern void traeger_go_subscriber_callback(const traeger_string_t *topic,
                                           traeger_value_t *value,
                                           traeger_closure_t closure);

extern void traeger_go_closure_free(traeger_closure_t handle);

static inline traeger_promise_t *
traeger_go_subscriber_listen(const traeger_subscriber_t *self,
                             const traeger_scheduler_t *scheduler,
							 uintptr_t handle)
{
	void *closure = (void*)handle;
	return traeger_subscriber_listen(self, scheduler, traeger_go_subscriber_callback,
	                                 closure, traeger_go_closure_free);
}
*/
import "C"

// Replier

type Replier struct {
	self *C.traeger_replier_t
}

func FreeReplier(replier *Replier) {
	C.traeger_replier_free(replier.self)
}

func wrap_c_replier(self *C.traeger_replier_t) *Replier {
	replier := Replier{self}
	runtime.SetFinalizer(&replier, FreeReplier)
	return &replier
}

func (replier *Replier) Reply(scheduler *Scheduler, mailbox *Mailbox) *Promise {
	c_promise := C.traeger_replier_reply(replier.self, scheduler.self, mailbox.self)
	return wrap_c_promise(c_promise)
}

// Requester

type Requester struct {
	self *C.traeger_requester_t
}

func FreeRequester(requester *Requester) {
	C.traeger_requester_free(requester.self)
}

func wrap_c_requester(self *C.traeger_requester_t) *Requester {
	requester := Requester{self}
	runtime.SetFinalizer(&requester, FreeRequester)
	return &requester
}

func (requester *Requester) Mailbox() *Mailbox {
	return wrap_c_mailbox(C.traeger_requester_get_mailbox(requester.self))
}

// Publisher

type Publisher struct {
	self *C.traeger_publisher_t
}

func FreePublisher(publisher *Publisher) {
	C.traeger_publisher_free(publisher.self)
}

func wrap_c_publisher(self *C.traeger_publisher_t) *Publisher {
	publisher := Publisher{self}
	runtime.SetFinalizer(&publisher, FreePublisher)
	return &publisher
}

func (publisher *Publisher) Publish(scheduler *Scheduler, topic string, payload *Value) *Promise {
	c_promise := C.traeger_publisher_publish(
		publisher.self,
		scheduler.self,
		C._GoStringPtr(topic), C._GoStringLen(topic),
		payload.self)
	return wrap_c_promise(c_promise)
}

// Subscriber

type SubscriberFunc func(topic string, value *Value)

type Subscriber struct {
	self *C.traeger_subscriber_t
}

func FreeSubscriber(subscriber *Subscriber) {
	C.traeger_subscriber_free(subscriber.self)
}

func wrap_c_subscriber(self *C.traeger_subscriber_t) *Subscriber {
	subscriber := Subscriber{self}
	runtime.SetFinalizer(&subscriber, FreeSubscriber)
	return &subscriber
}

//export traeger_go_subscriber_callback
func traeger_go_subscriber_callback(c_topic *C.traeger_const_string_t, c_value *C.traeger_value_t, closure C.traeger_closure_t) {
	handle := cgo.Handle(closure)
	subscriberFunc := handle.Value().(SubscriberFunc)
	topic := &String{c_topic}
	value := wrap_c_value(c_value)
	subscriberFunc(topic.String(), value)
}

func (subscriber *Subscriber) Listen(scheduler *Scheduler, subscriberFunc SubscriberFunc) *Promise {
	handle := cgo.NewHandle(subscriberFunc)
	c_promise := C.traeger_go_subscriber_listen(subscriber.self, scheduler.self, C.uintptr_t(handle))
	return wrap_c_promise(c_promise)
}

// Context

type Context struct {
	self *C.traeger_context_t
}

func FreeContext(context *Context) {
	C.traeger_context_free(context.self)
}

func wrap_c_context(self *C.traeger_context_t) *Context {
	context := Context{self}
	runtime.SetFinalizer(&context, FreeContext)
	return &context
}

func NewContext() *Context {
	return wrap_c_context(C.traeger_context_new())
}

func (context *Context) Replier(address string) (*Replier, error) {
	var result *C.traeger_replier_t
	var err *C.traeger_string_t
	cstr_address := C.CString(address)
	defer C.free(unsafe.Pointer(cstr_address))
	if C.traeger_replier_new(context.self, cstr_address, &result, &err) {
		return wrap_c_replier(result), nil
	} else {
		return nil, errors.New(wrap_c_string(err).String())
	}
}

func (context *Context) Requester(address string, format *Format) (*Requester, error) {
	var result *C.traeger_requester_t
	var err *C.traeger_string_t
	cstr_address := C.CString(address)
	defer C.free(unsafe.Pointer(cstr_address))
	if C.traeger_requester_new(context.self, cstr_address, format.self, &result, &err) {
		return wrap_c_requester(result), nil
	} else {
		return nil, errors.New(wrap_c_string(err).String())
	}
}

func (context *Context) Publisher(address string, format *Format) (*Publisher, error) {
	var result *C.traeger_publisher_t
	var err *C.traeger_string_t
	cstr_address := C.CString(address)
	defer C.free(unsafe.Pointer(cstr_address))
	if C.traeger_publisher_new(context.self, cstr_address, format.self, &result, &err) {
		return wrap_c_publisher(result), nil
	} else {
		return nil, errors.New(wrap_c_string(err).String())
	}
}

func (context *Context) Subscriber(address string, topics []string) (*Subscriber, error) {
	var result *C.traeger_subscriber_t
	var err *C.traeger_string_t

	cstr_address := C.CString(address)
	defer C.free(unsafe.Pointer(cstr_address))

	list := NewList()
	for _, topic := range topics {
		list.AppendString(topic)
	}

	if C.traeger_subscriber_new(context.self, cstr_address, list.self, &result, &err) {
		return wrap_c_subscriber(result), nil
	} else {
		return nil, errors.New(wrap_c_string(err).String())
	}
}
