cmake_minimum_required(VERSION 3.5)

project(shm_mutex)

add_library(lock SHARED ./lock/lock.c)
target_include_directories(lock PRIVATE "./include")

add_executable(shm_mutex main.c)
target_include_directories(shm_mutex PRIVATE "./include")
target_link_libraries(shm_mutex PRIVATE lock)
target_link_libraries(shm_mutex PRIVATE pthread)
