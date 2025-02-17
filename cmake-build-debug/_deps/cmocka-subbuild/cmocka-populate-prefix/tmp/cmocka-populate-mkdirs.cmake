# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/luisruisinger/CLionProjects/cpu_raytracing/cmake-build-debug/_deps/cmocka-src"
  "/Users/luisruisinger/CLionProjects/cpu_raytracing/cmake-build-debug/_deps/cmocka-build"
  "/Users/luisruisinger/CLionProjects/cpu_raytracing/cmake-build-debug/_deps/cmocka-subbuild/cmocka-populate-prefix"
  "/Users/luisruisinger/CLionProjects/cpu_raytracing/cmake-build-debug/_deps/cmocka-subbuild/cmocka-populate-prefix/tmp"
  "/Users/luisruisinger/CLionProjects/cpu_raytracing/cmake-build-debug/_deps/cmocka-subbuild/cmocka-populate-prefix/src/cmocka-populate-stamp"
  "/Users/luisruisinger/CLionProjects/cpu_raytracing/cmake-build-debug/_deps/cmocka-subbuild/cmocka-populate-prefix/src"
  "/Users/luisruisinger/CLionProjects/cpu_raytracing/cmake-build-debug/_deps/cmocka-subbuild/cmocka-populate-prefix/src/cmocka-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/luisruisinger/CLionProjects/cpu_raytracing/cmake-build-debug/_deps/cmocka-subbuild/cmocka-populate-prefix/src/cmocka-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/luisruisinger/CLionProjects/cpu_raytracing/cmake-build-debug/_deps/cmocka-subbuild/cmocka-populate-prefix/src/cmocka-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
