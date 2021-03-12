#!/usr/bin/env python3
# ARGS: fib 5
# RESULT: 5
import pygg

from typing import Optional

gg = pygg.init()


@gg.thunk_fn()
def fib(n: int) -> pygg.Output:
    if n < 2:
        return gg.str_value(str(n))
    else:
        a = gg.thunk(fib, n - 1)
        b = gg.thunk(fib, n - 2)
        return gg.thunk(add_str, a, b)


@gg.thunk_fn()
def add_str(a: Optional[pygg.Value], b: Optional[pygg.Value]) -> pygg.Output:
    if a is None:
        return gg.this()
    if b is None:
        return gg.this()
    ai = int(a.as_str())
    bi = int(b.as_str())
    return gg.str_value(str(ai + bi))


gg.main()
