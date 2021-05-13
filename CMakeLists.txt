cmake_minimum_required(VERSION 3.14)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
    project(Allocator VERSION 0.0.$ENV{GITHUB_RUN_NUMBER})
else()
    project(Allocator)
endif()

set(ROOT_DIR ${Allocator_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(Src)
add_subdirectory(Test)

set(CPACK_GENERATOR DEB)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sergey")
set(CPACK_DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_DIRECTORY ${ROOT_DIR}/packages)

include(CPack)
