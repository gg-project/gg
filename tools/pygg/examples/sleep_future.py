#!/usr/bin/env python3
# ARGS: sleep_many 2 4 6
# RESULT: 2
# Copied
import pygg
import functools as ft
import time
from typing import List, Optional

gg = pygg.init()


@gg.thunk_fn()
def sleep_many(t0: int, t1: int, t2: int) -> pygg.Output:
    sleeps = [gg.thunk(sleep, t0), gg.thunk(sleep, t1), gg.thunk(sleep, t1)]
    return ft.reduce(lambda x, y: gg.thunk(first, x, y), sleeps)


@gg.thunk_fn()
def sleep(t: int) -> pygg.Value:
    time.sleep(t)
    return gg.str_value(str(t))


@gg.thunk_fn()
def first(a: Optional[pygg.Value], b: Optional[pygg.Value]) -> pygg.Output:
    if a is not None:
        return a
    if b is not None:
        return b
    return gg.this()


gg.main()
