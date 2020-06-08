cmake_minimum_required(VERSION 3.5)

project(client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(c:/mingw32/include)
link_directories(c:/mingw32/lib)

add_executable(client main.cpp)

if(WIN32)
  target_link_libraries(client wsock32 ws2_32 gmp gmpxx)
endif()

if(UNIX)
  target_link_libraries(client gmp gmpxx)
endif()
