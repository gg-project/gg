FROM ubuntu:18.04 AS build-stage

RUN apt-get update -qy
#RUN yum groupinstall -y "Development Tools"
RUN apt-get install -y libssl-dev \
                      python3 \
                      libhiredis-dev \
                      protobuf-compiler \
                      libprotobuf-dev \
                      cmake \
                      make \
                      g++ \
                      libcap-dev

RUN mkdir /gg
COPY . /gg
WORKDIR /gg
RUN rm -rf build
RUN mkdir build
WORKDIR /gg/build
RUN cmake .. -DSTATIC_BINS=True
RUN make -j $(nproc)

FROM scratch AS export-stage

COPY --from=build-stage /gg/build/src/frontend/gg-hash /gg-hash-lambda
COPY --from=build-stage /gg/build/src/frontend/gg-collect /gg-collect-lambda
COPY --from=build-stage /gg/build/src/frontend/gg-create-thunk /gg-create-thunk-lambda
COPY --from=build-stage /gg/build/src/frontend/gg-execute /gg-execute-lambda
