cmake_minimum_required(VERSION 3.13)

project(pikchr
  VERSION 1.0
  DESCRIPTION "Pikchr (pronounced \\\"picture\\\") is a PIC-like markup language for diagrams in technical documentation"
  LANGUAGES C)

include(CMakeDependentOption)
include(CheckCSourceCompiles)

option(PIKCHR_BUILD_LEMON "Build lemon generator" ON)
option(PIKCHR_BUILD_LIBPIKCHR "Build pikchr library" ON)
option(PIKCHR_BUILD_PIKCHR "Build pikchr shell" ON)

option(PIKCHR_MAKE_INSTALL "Enable install rules" ON)
cmake_dependent_option(PIKCHR_MAKE_PACKAGE "Enable package rules" ON
  "PIKCHR_MAKE_INSTALL" OFF)

try_compile(PIKCHR_MATH_INBUILT "${CMAKE_CURRENT_BINARY_DIR}"
  "${CMAKE_CURRENT_LIST_DIR}/pikchr.c"
  COMPILE_DEFINITIONS -DPIKCHR_SHELL)
mark_as_advanced(PIKCHR_MATH_INBUILT)
if(NOT PIKCHR_MATH_INBUILT)
  set(PIKCHR_MATH_LIBRARY "m" CACHE STRING "The math library")
  try_compile(PIKCHR_MATH_EXTERNAL "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/pikchr.c"
    COMPILE_DEFINITIONS -DPIKCHR_SHELL
    LINK_LIBRARIES ${PIKCHR_MATH_LIBRARY})
  if(NOT PIKCHR_MATH_EXTERNAL)
    message(FATAL_ERROR "Set the PIKCHR_MATH_LIBRARY variable to math library location (by default is m)")
  else()
    mark_as_advanced(PIKCHR_MATH_EXTERNAL PIKCHR_MATH_LIBRARY)
  endif()
endif()

configure_file(pikchr.h.in
  "${CMAKE_CURRENT_LIST_DIR}/pikchr.h"
  COPYONLY)

if(PIKCHR_BUILD_LEMON)
  add_executable(lemon
    lemon.c)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_LIST_DIR}/pikchr.c"
    COMMAND lemon pikchr.y
    DEPENDS lempar.c pikchr.y
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
endif()

if(PIKCHR_BUILD_LIBPIKCHR)
  add_library(libpikchr
    pikchr.h
    pikchr.c)

  set_target_properties(libpikchr
    PROPERTIES
      OUTPUT_NAME "pikchr")

  target_include_directories(libpikchr
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

  if(PIKCHR_MATH_EXTERNAL)
    target_link_libraries(libpikchr
      PUBLIC
        ${PIKCHR_MATH_LIBRARY})
  endif()
endif()

if(PIKCHR_BUILD_PIKCHR)
  add_executable(pikchr
    pikchr.c)

  target_compile_definitions(pikchr
    PRIVATE
      PIKCHR_SHELL)

  if(PIKCHR_MATH_EXTERNAL)
    target_link_libraries(pikchr
      PRIVATE
        ${PIKCHR_MATH_LIBRARY})
  endif()
endif()

if(PIKCHR_MAKE_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)
  if(PIKCHR_BUILD_LIBPIKCHR)
    install(FILES pikchr.h
      DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
    target_include_directories(libpikchr
      PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  endif()
  foreach(TARGET libpikchr pikchr)
    if(TARGET ${TARGET})
      install(TARGETS ${TARGET} EXPORT ${PROJECT_NAME}
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
    endif()
  endforeach()
  if(PIKCHR_BUILD_LIBPIKCHR OR PIKCHR_BUILD_PIKCHR)
    install(EXPORT ${PROJECT_NAME}
      NAMESPACE "${PROJECT_NAME}::"
      FILE "${PROJECT_NAME}-targets.cmake"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
    file(GENERATE
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
      CONTENT "include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}-targets.cmake\")\n")
    write_basic_package_version_file("${PROJECT_NAME}-config-version.cmake"
      VERSION "${PROJECT_VERSION}"
      COMPATIBILITY AnyNewerVersion)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
    export(EXPORT ${PROJECT_NAME}
      NAMESPACE "${PROJECT_NAME}::"
      FILE "${PROJECT_NAME}-targets.cmake")
  endif()
endif()

if(PIKCHR_MAKE_PACKAGE)
  set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
  set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
  include(CPack)
endif()

