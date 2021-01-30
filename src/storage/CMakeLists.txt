add_library(storage
    backend.hh
    backend.cc
    backend_local.hh
    backend_s3.hh
    backend_s3.cc
    backend_redis.hh
    backend_redis.cc
    backend_gs.hh
    backend_gs.cc)
target_link_libraries(storage PRIVATE thunk net)
