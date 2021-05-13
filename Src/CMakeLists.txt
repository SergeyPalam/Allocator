include_directories(${ROOT_DIR}/Inc)
                                   
add_executable(alloc_example.out main.cpp)

target_link_libraries(alloc_example.out)

install(TARGETS alloc_example.out DESTINATION ${ROOT_DIR}/bin)