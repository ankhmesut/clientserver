cmake_minimum_required(VERSION 3.5)

project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(c:/mingw32/include)
link_directories(c:/mingw32/lib)

file(GLOB SOURCES
  connection.h
  ber.h
  tcpserver.h
  tcpserver_p.h
  threadpool.h
  main.cpp
  ber.cpp
  tcpserver.cpp
  tcpserver_p.cpp
  threadpool.cpp
)

if(WIN32)
  add_executable(server ${SOURCES} tcpserver_windows.cpp)
endif()

if(UNIX)
  add_executable(server ${SOURCES} tcpserver_linux.cpp)
endif()

if(WIN32)
  target_link_libraries(server wsock32 ws2_32 gmp gmpxx)
endif()

if(UNIX)
  target_link_libraries(server pthread gmp gmpxx)
endif()


