cmake_minimum_required(VERSION 3.13)

project(pikchr-import-test
  LANGUAGES C)

find_package(pikchr REQUIRED)

add_executable(test test.c)

target_link_libraries(test pikchr::libpikchr)
