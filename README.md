# gg [![Build Status](https://travis-ci.org/StanfordSNR/gg.svg?branch=master)](https://travis-ci.org/StanfordSNR/gg)

[![xkcd.com/303/](https://s3-us-west-2.amazonaws.com/stanfordsnr/gg-xkcd.jpg)](https://xkcd.com/303/)


## Research Paper

"[From Laptop to Lambda: Outsourcing Everyday Jobs to Thousands of Transient Functional Containers](https://www.usenix.org/system/files/atc19-fouladi.pdf)," in 2019 USENIX Annual Technical Conference (USENIX ATC '19).

## Demo

Building FFmpeg with `gg` on AWS Lambda

[![asciicast](https://asciinema.org/a/257545.svg)](https://asciinema.org/a/257545)

## Tech Video

`gg` was presented at 2019 USENIX Annual Technical Conference (USENIX ATC '19). The talk was given on July 11, 2019.

[![Alt text](https://img.youtube.com/vi/VVWVN6Czji4/0.jpg)](https://www.youtube.com/watch?v=VVWVN6Czji4)


## Build directions

### Install Dependencies

You'll need the following: OpenSSL, ncurses, libcap, protobuf, and hiredis.

On Ubuntu: `apt-get install libssl-dev libcap-dev libcap-dev libncurses5-dev
libhiredis-dev protobuf-compiler libprotobuf-dev`.

### Build Process

`mkdir build && cd build && cmake .. && make -j $(nproc)`

### Installation

To install the gg command line tool suite, run `make install` in the build
directory.

To install the pygg python interface to gg, add `tools/pygg` to your
`PYTHONPATH`.

### Tests

Run `make check` in the `tools/pygg` directory. You'll need to install `pygg`
and `gg` first.

## Usage

### Environment Variables

To use `gg`, the following environment variables *must* be set:

- `GG_MODELPATH` => *absolute path* to `<gg-source-dir>/src/models/wrappers`.
- `GG_STORAGE_URI` =>
  - **S3**: `s3://<bucket-name>/?region=<bucket-region>`
  - **Redis**: `redis://<username>:<password>@<host>[:<port>]`
- `GG_LAMBDA_ROLE` => the role that will be assigned to the executed Lambda.
functions. Must have *AmazonS3FullAccess* and *AWSLambdaBasicExecutionRole*
permissions.
- `AWS_ACCESS_KEY_ID`, `AWS_SECRET_ACCESS_KEY` => your AWS access key
- `AWS_REGION` => your AWS region, where the functions are installed
- `GG_LEAVE_CRASHED_THUNKS` => If set to "1", do not clean up the temporary
  execution directories of crashed thunks.

### Uploading the runtime to AWS Lambda

To execute computations on AWS lambda, GG's runtime must be installed to your
lambda function.
gg ships with a pre-built lambda runtime. You can upload it to your lambda
function by setting the above environmental variables, and running `make
ggfunctions` in the build directory.

### Building the Lambda runtime yourself

If you'd like to build the Lambda runtime yourself, we recommend using Docker
to cross-build. Run `DOCKER_BUILDKIT=1 docker build --output tools/lambda_bins
.` from the root of the repository.

### pygg Example

The easiest way to use gg is through pygg. Pygg allows one to represent gg
thunks as (decorated) python functions. Here is an example of a fibonacci
function:

```python
#!/usr/bin/env python3
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
```

How do we use this script?

  1. Enter the `tools/pygg/examples` directory.
  2. `./fib.py init fib 5` (creates a thunk file, `out`)
  3. `gg-force --jobs 1 --engine local ./out && cat ./out`
  4. (for remote execution): `gg-force --jobs 1 --engine lambda ./out && cat ./out`

You can find a description of the pygg library in `tools/pygg/README.md`.

### Multi-Node GG

To run gg on multiple cooperating computers, using the following
configuration:

#### Worker Nodes

Each worker should have `gg-execute` and `gg-execute-server` installed. It may
be easier to build static version. The execution server is run like
`GG_DIR=/tmp/gg gg-execute-server HOST PORT`.

   * `HOST` should be the worker's host IP
   * `PORT` should be a port it can bind to
   * `GG_DIR` can be changed to anywhere you want GG's cache on this machine

#### Leader Node

The leader node needs `gg-force` installed to run thunks. It also needs pygg,
`gg-init`, `gg-hash`, `gg-create-thunk`, and `gg-collect`, if you want to
build thunks using a pygg script.
```
gg-force \
 --jobs JOBS --engine=remote=HOST:PORT \
 --jobs JOBS --engine=remote=HOST:PORT \
 ...
 --jobs JOBS --engine=remote=HOST:PORT \
 THUNK_FILE
```

where each `(JOBS, HOST, PORT)` tuple indicates how many jobs can be
concurrently run by the worker at `HOST:PORT`.

See the section above for a description of how one might create a THUNK_FILE
using pygg.
