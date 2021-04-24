include_directories(${ROOT_DIR}/Inc)
                                   
add_executable(alloc_example.out main.cpp)

install(TARGETS alloc_example.out DESTINATION ${ROOT_DIR}/bin)