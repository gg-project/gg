file(GLOB here *.py)
file(GLOB lambda lambda_function/*)
file(GLOB common common/*)
add_custom_target(ggfunctions
  COMMAND
  cd ${PROJECT_SOURCE_DIR}/src/remote && python3 ${PROJECT_SOURCE_DIR}/src/remote/create-function.py --delete --gg-execute=${CMAKE_BINARY_DIR}/src/frontend/gg-execute --toolchain-path=${PROJECT_SOURCE_DIR}/toolchain --gg-meow-worker=${CMAKE_BINARY_DIR}/src/frontend/gg-meow-worker
  DEPENDS
    gg-execute
    gg-meow-worker
    ${here}
    ${lambda}
    ${common})
