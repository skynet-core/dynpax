cmake_minimum_required(VERSION 3.28)

project(
  dynpax
  VERSION 0.1.0
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(LIEF)
include(GNUInstallDirs)

find_package(CLI11 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

file(GLOB SOURCES "app/src/*.cpp")

if(LINUX)
  file(GLOB LINUX_SOURCES "app/src/linux/*.cpp")
  list(APPEND SOURCES ${LINUX_SOURCES})
endif()

add_executable(dynpax)

target_sources(dynpax PRIVATE ${SOURCES})

target_include_directories(dynpax PRIVATE "app/include")

target_link_libraries(dynpax PRIVATE CLI11::CLI11 LIEF::LIEF
                                     fmt::fmt-header-only)
install(
  TARGETS dynpax
  RUNTIME COMPONENT dynpax DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY COMPONENT dynpax DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE COMPONENT dynpax DESTINATION ${CMAKE_INSTALL_LIBDIR})

  install(
    CODE [[
  function(install_library_with_deps LIBRARY)
    get_filename_component(DEST_DIR ${LIBRARY} DIRECTORY)
    file(
      INSTALL
      DESTINATION "${CMAKE_INSTALL_PREFIX}${DEST_DIR}"
      TYPE SHARED_LIBRARY FOLLOW_SYMLINK_CHAIN FILES "${LIBRARY}")
  
    file(
      GET_RUNTIME_DEPENDENCIES
      LIBRARIES
      ${LIBRARY}
      RESOLVED_DEPENDENCIES_VAR
      RESOLVED_DEPS
      UNRESOLVED_DEPENDENCIES_VAR
      UNRESOLVED_DEPS)
  
    foreach(FILE ${RESOLVED_DEPS})
      if(NOT IS_SYMLINK ${FILE})
        install_library_with_deps(${FILE})
      endif()
    endforeach()
  
    foreach(FILE ${UNRESOLVED_DEPS})
      message(STATUS "Unresolved from ${LIBRARY}: ${FILE}")
    endforeach()
  
  endfunction()
  
  file(
    GET_RUNTIME_DEPENDENCIES
    EXECUTABLES
    $<TARGET_FILE:dynpax>
    RESOLVED_DEPENDENCIES_VAR
    RESOLVED_DEPS
    UNRESOLVED_DEPENDENCIES_VAR
    UNRESOLVED_DEPS)
  
  foreach(FILE ${RESOLVED_DEPS})
    install_library_with_deps(${FILE})
  endforeach()
  
  foreach(FILE ${UNRESOLVED_DEPS})
    message(STATUS "Unresolved: ${FILE}")
  endforeach()
  ]])