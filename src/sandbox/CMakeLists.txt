add_library(sandbox sandbox.hh sandbox.cc)
target_link_libraries(sandbox PRIVATE trace)
