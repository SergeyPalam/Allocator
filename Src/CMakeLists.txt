include_directories(${ROOT_DIR}/Inc)

add_library(memory_pool STATIC memory_pool.cpp)
                                   
add_executable(alloc_example.out main.cpp)

target_link_libraries(alloc_example.out memory_pool)

install(TARGETS memory_pool DESTINATION ${ROOT_DIR}/lib)
install(TARGETS alloc_example.out DESTINATION ${ROOT_DIR}/bin)