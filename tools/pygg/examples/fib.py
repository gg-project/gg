#!/usr/bin/env python3
# ARGS: fib 5
# RESULT: 5
# Copied
import pygg

gg = pygg.init() # Set-up the GG runtime


@gg.thunk_fn() # Calls to this fn can be embedded in a dependency graph
def fib(n: int) -> pygg.Output:
    if n < 2:
        return gg.str_value(str(n)) # Create a gg.Value from a string
    else:
        a = gg.thunk(fib, n - 1) # Create a thunk for fib(n-1)
        b = gg.thunk(fib, n - 2) # Create a thunk for fib(n-2)
        return gg.thunk(add_str, a, b) # Build a new thunk dependent on those, returning it


@gg.thunk_fn()
def add_str(a: pygg.Value, b: pygg.Value) -> pygg.Output:
    ai = int(a.as_str()) # Get the string contents of a gg.Value
    bi = int(b.as_str())
    return gg.str_value(str(ai + bi)) # Do the addition, return a new gg.Value


gg.main() # Yield control to the gg runtime
