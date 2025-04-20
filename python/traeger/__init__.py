# SPDX-License-Identifier: BSL-1.0

import inspect
from .traeger import *


class Actor(StatelessActor):
    @staticmethod
    def __wrap_method(state, method):
        if not callable(method):
            raise TypeError("argument must be a function")
        sig = inspect.signature(method)
        parameters = list(sig.parameters.values())
        n_parameters = len(parameters)
        if n_parameters == 0:
            raise TypeError("function must take at least one parameter")
        parameters.pop(0)
        n_parameters -= 1
        return_annotation = sig.return_annotation
        del sig

        def convert(annotation, value):
            if annotation != None and annotation != inspect._empty:
                return annotation(value)
            return value

        def function(arguments: List):
            try:
                n_arguments = len(arguments)
                if len(parameters) != len(arguments):
                    raise TypeError(
                        f"function takes {n_parameters} arguments but {n_arguments} were given"
                    )
                args = [
                    convert(param.annotation, value.get())
                    for param, value in zip(parameters, arguments)
                ]
                result = method(state, *args)
                return Result.from_value(convert(return_annotation, result))
            except Exception as e:
                return Result.from_error(str(e))

        return function

    def __init__(self, state):
        super().__init__()
        self.__state = state

    def define_reader(self, name, method):
        super().define_reader(name, self.__wrap_method(self.__state, method))

    def define_writer(self, name, method):
        super().define_writer(name, self.__wrap_method(self.__state, method))


def make_actor(clazz, *args, **kwargs):
    return Actor(clazz(*args, **kwargs))
