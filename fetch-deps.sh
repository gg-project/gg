#!/bin/sh -ex
[[ -a third_party/openssl ]] || ./contrib/get-openssl.sh
[[ -a third_party/hiredis ]] || ./contrib/get-hiredis.sh
[[ -a third_party/protobuf-3.12.0 ]] || ./contrib/get-protobuf.sh
[[ -a third_party/glibc ]] || ./contrib/get-glibc.sh