set(LIEF_GIT_URL "https://github.com/lief-project/LIEF.git")
set(LIEF_VERSION 0.15.1)

include(FetchContent)
FetchContent_Declare(
  LIEF
  GIT_REPOSITORY "${LIEF_GIT_URL}"
  GIT_TAG ${LIEF_VERSION})

if(${CMAKE_VERSION} VERSION_LESS "3.14.0")
  # CMake 3.11 to 3.13 needs more verbose method to make LIEF available
  FetchContent_GetProperties(LIEF)
  if(NOT LIEF_POPULATED)
    FetchContent_Populate(LIEF)
    add_subdirectory(${LIEF_SOURCE_DIR} ${LIEF_BINARY_DIR})
  endif()
else()
  # CMake 3.14+ has single function to make LIEF available (recommended)
  FetchContent_MakeAvailable(LIEF)
endif()