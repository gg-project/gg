add_library(thunk
    thunk.hh
    function.cc
    thunk.cc
    thunk_writer.cc
    thunk_writer.hh
    thunk_reader.cc
    thunk_reader.hh
    placeholder.cc
    placeholder.hh
    manifest.cc
    manifest.hh
    ggutils.cc
    ggutils.hh
    graph.cc
    graph.hh
    factory.cc
    factory.hh)
target_link_libraries(thunk PUBLIC util sandbox protobufs ${CRYPTOPP_LIBRARIES})
target_include_directories(thunk PUBLIC protobufs_hdrs ${CRYPTOPP_INCLUDE_DIR})
