cmake_minimum_required(VERSION 3.28)

project(
  dynpax
  VERSION 0.1.0
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(LIEF)
include(GNUInstallDirs)
include(BundleUtils)

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

  fixup_target_bundle(dynpax)
