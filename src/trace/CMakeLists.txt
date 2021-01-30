string(TOLOWER ${CMAKE_SYSTEM_NAME} LOWER_SYSTEM_NAME)
add_library(syscalltbl ${LOWER_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/syscalltbl.cc)
target_compile_options(syscalltbl PRIVATE  -fno-var-tracking)
target_include_directories(syscalltbl PRIVATE .)

add_library(trace syscall.hh
    invocation.hh
    invocation.cc
    tracer.hh
    tracer.cc)
target_link_libraries(trace syscalltbl)
