// SPDX-License-Identifier: BSL-1.0

package traeger

/*
#cgo LDFLAGS: -ltraeger_module
// #include <stdlib.h>
#include <traeger/module/module.h>
*/
import "C"
import (
	"errors"
	"runtime"
)

type Module struct {
	self *C.traeger_module_t
}

func FreeModule(module *Module) {
	C.traeger_module_free(module.self)
}

func wrap_c_module(self *C.traeger_module_t) *Module {
	module := Module{self}
	runtime.SetFinalizer(&module, FreeModule)
	return &module
}

func NewModuleFromPathOrError(address string, configuration *Map) (*Module, error) {
	var result *C.traeger_module_t
	var err *C.traeger_string_t

	if C.traeger_module_new_from_path_or_error(C._GoStringPtr(address), C._GoStringLen(address), configuration.self, &result, &err) {
		return wrap_c_module(result), nil
	} else {
		return nil, errors.New(wrap_c_string(err).String())
	}
}

func (module *Module) Mailbox() *Mailbox {
	return wrap_c_mailbox(C.traeger_module_get_mailbox(module.self))
}

var moduleInit func(init_map *Map) (*BaseActor, error)

func ModuleInit(moduleInitFunc func(init_map *Map) (*BaseActor, error)) {
	moduleInit = moduleInitFunc
}

//export traeger_module_init
func traeger_module_init(
	c_map *C.traeger_const_map_t,
	c_mailbox_interface **C.traeger_mailbox_interface_t,
	c_error *C.traeger_string_t) {
	if moduleInit != nil &&
		c_map != nil &&
		c_mailbox_interface != nil &&
		c_error != nil {
		actor, err := moduleInit(&Map{c_map})
		if actor != nil {
			*c_mailbox_interface = C.traeger_actor_get_mailbox_interface(actor.self)
		} else if err != nil {
			str := err.Error()
			C.traeger_string_set(c_error, C._GoStringPtr(str), C._GoStringLen(str))
		}
	}
}
