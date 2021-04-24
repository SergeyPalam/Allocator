
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.8.0
)

FetchContent_MakeAvailable(googletest)

include_directories(${ROOT_DIR}/Inc ${GTEST_INCLUDE_DIRS})

add_executable(test_custom_allocator.out test_custom_allocator.cpp)
add_executable(test_hash_table.out test_hash_table.cpp)
add_executable(test_memory_pool.out test_hash_table.cpp)

target_link_libraries(test_custom_allocator.out gtest_main)
target_link_libraries(test_hash_table.out gtest_main)
target_link_libraries(test_memory_pool.out gtest_main)

include(GoogleTest)

gtest_discover_tests(test_custom_allocator.out test_hash_table.out test_memory_pool.out) 