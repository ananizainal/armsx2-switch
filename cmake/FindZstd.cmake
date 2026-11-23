# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# - Try to find Facebook zstd library
# This will define
# Zstd_FOUND
# Zstd_INCLUDE_DIR
# Zstd_LIBRARY
#

find_path(Zstd_INCLUDE_DIR NAMES zstd.h)

if(Zstd_INCLUDE_DIR)
  file(STRINGS "${Zstd_INCLUDE_DIR}/zstd.h" _ver_major_line REGEX "^#define ZSTD_VERSION_MAJOR  *[0-9]+")
  string(REGEX MATCH "[0-9]+" Zstd_VERSION_MAJOR "${_ver_major_line}")
  file(STRINGS "${Zstd_INCLUDE_DIR}/zstd.h" _ver_minor_line REGEX "^#define ZSTD_VERSION_MINOR  *[0-9]+")
  string(REGEX MATCH "[0-9]+" Zstd_VERSION_MINOR "${_ver_minor_line}")
  file(STRINGS "${Zstd_INCLUDE_DIR}/zstd.h" _ver_release_line REGEX "^#define ZSTD_VERSION_RELEASE  *[0-9]+")
  string(REGEX MATCH "[0-9]+" Zstd_VERSION_PATCH "${_ver_release_line}")
  set(Zstd_VERSION "${Zstd_VERSION_MAJOR}.${Zstd_VERSION_MINOR}.${Zstd_VERSION_PATCH}")
endif()

find_library(Zstd_LIBRARY_DEBUG NAMES zstdd zstd_staticd)
find_library(Zstd_LIBRARY_RELEASE NAMES zstd zstd_static)

include(SelectLibraryConfigurations)
SELECT_LIBRARY_CONFIGURATIONS(Zstd)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    Zstd DEFAULT_MSG
    Zstd_LIBRARY Zstd_INCLUDE_DIR
    VERSION_VAR Zstd_VERSION
)

mark_as_advanced(Zstd_INCLUDE_DIR Zstd_LIBRARY)

if(Zstd_FOUND AND NOT (TARGET Zstd::Zstd))
  add_library (Zstd::Zstd UNKNOWN IMPORTED)
  set_target_properties(Zstd::Zstd
    PROPERTIES
    IMPORTED_LOCATION ${Zstd_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${Zstd_INCLUDE_DIR})
endif()
