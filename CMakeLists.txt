cmake_minimum_required (VERSION 2.8.11)

project(nxa2opus LANGUAGES C)

set(SOURCES src/nxa_opus.c src/nxa_streamfile.c src/streamfile.c src/util.c src/main.c)
set(HEADERS src/nxa_opus.h src/nxa_streamfile.h src/streamfile.h src/util.h src/streamtypes.h)

add_executable(nxa2opus ${SOURCES} ${HEADERS})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()