cmake_minimum_required(VERSION 3.10)

project(CWEBSERVER)

option(BUILD_FLAG "build flag" TPOLL)

# 添加C++11标准支持
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -g -O0")

set(BOOST_ROOT "thirdparty/boost/1.79.0_1")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

file(GLOB SOURCES
    "src/*.cc"
    "src/tcpserver/*.cc"
    "src/tcpserver/base/*.cc"
    "src/util/*.cc"
    "src/cweb/*.cc"
    "src/cweb/jsoncpp/*.cc"
    "src/log/*.cc"
)

include_directories(
  src
  src/co_tcpserver
  src/tcpserver
  src/tcpserver/poll
  src/tcpserver/epoll
  src/tcpserver/kqueue
  src/tcpserver/base
  src/util
  src/cweb
  src/cweb/jsoncpp
  src/log
  thirdparty/boost/1.79.0_1/include
)

link_directories(${BOOST_LIBRARYDIR})

if(BUILD_FLAG STREQUAL TKQUEUE)
  add_definitions(-DKQUEUE)
  file(GLOB_RECURSE EXTRA_SOURCES "src/tcpserver/kqueue/*.cc")
  set(SOURCES 
    ${SOURCES}
    ${EXTRA_SOURCES}
  )
  add_executable(CWEBSERVER ${SOURCES})
elseif(BUILD_FLAG STREQUAL TEPOLL)
  add_definitions(-DEPOLL)
  file(GLOB_RECURSE EXTRA_SOURCES "src/tcpserver/epoll/*.cc")
  set(SOURCES 
    ${SOURCES}
    ${EXTRA_SOURCES}
  )
  add_executable(CWEBSERVER ${SOURCES} "src/tcpserver/epoll/*.cc")
elseif(BUILD_FLAG STREQUAL CPOLL)
  add_definitions(-DCOROUTINE)
  file(GLOB_RECURSE EXTRA_SOURCES 
    "src/tcpserver/poll/*.cc"
    "src/co_tcpserver/*.cc"
    "src/co_tcpserver/context_swap.o"
  )
  set(SOURCES 
    ${SOURCES}
    ${EXTRA_SOURCES}
  )
  add_executable(CWEBSERVER ${SOURCES})
elseif(BUILD_FLAG STREQUAL CKQUEUE)
  add_definitions(-DKQUEUE)
  add_definitions(-DCOROUTINE)
  file(GLOB_RECURSE EXTRA_SOURCES 
    "src/tcpserver/kqueue/*.cc"
    "src/co_tcpserver/*.cc"
    "src/co_tcpserver/context_swap.o"
  )
  set(SOURCES 
    ${SOURCES}
    ${EXTRA_SOURCES}
  )
  add_executable(CWEBSERVER ${SOURCES})
elseif(BUILD_FLAG STREQUAL CEPOLL)
  add_definitions(-DEPOLL)
  add_definitions(-DCOROUTINE)
  file(GLOB_RECURSE EXTRA_SOURCES 
    "src/tcpserver/epoll/*.cc"
    "src/co_tcpserver/*.cc"
    "src/co_tcpserver/context_swap.o"
  )
  set(SOURCES 
    ${SOURCES}
    ${EXTRA_SOURCES}
  )
  add_executable(CWEBSERVER ${SOURCES})
else()
  file(GLOB_RECURSE EXTRA_SOURCES "src/tcpserver/poll/*.cc")
  set(SOURCES 
    ${SOURCES}
    ${EXTRA_SOURCES}
  )
  add_executable(CWEBSERVER ${SOURCES})
endif()



