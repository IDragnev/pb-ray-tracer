add_executable(memory_test
  main.cpp
  memory.cpp
  memoryArena.cpp
  blockedUVArray.cpp
)
target_link_libraries(memory_test memory doctest)
target_compile_options(memory_test
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)