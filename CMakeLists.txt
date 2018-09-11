cmake_minimum_required(VERSION 3.0.1)

project(PracticalCppMetaprogramming)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES Clang)
	# See https://stackoverflow.com/questions/16655820/clang-mac-os-x-c11-linker-issue/16656234#16656234
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-W -Wall -Wextra  -Werror -stdlib=libc++ -D'_LIBCPP_EXTERN_TEMPLATE(...)='")
else()
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-W -Wall -Wextra  -Werror")
endif()


enable_testing()

add_subdirectory("1_Introduction")
add_subdirectory("2_CppMetaprogrammingInPractice")

