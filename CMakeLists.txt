cmake_minimum_required(VERSION 3.26)
project(cpu_raytracing C)
set(CMAKE_C_STANDARD 17)

find_package(SDL2 REQUIRED)

# Add option for enabling tests (default: OFF)
option(BUILD_TESTS "Build tests" OFF)

# If tests are enabled, set up testing
if(BUILD_TESTS)
    add_compile_options(-Wall -Wextra -Wpedantic -DDEBUG -mfma -mavx2)
    include(FetchContent)

    # Fetch CMocka
    FetchContent_Declare(
            cmocka
            GIT_REPOSITORY https://git.cryptomilk.org/projects/cmocka.git
            GIT_TAG        cmocka-1.1.7
    )

    set(WITH_STATIC_LIB ON CACHE BOOL "CMocka: Build with static library" FORCE)
    set(WITH_CMOCKERY_SUPPORT OFF CACHE BOOL "CMocka: Install a cmockery header" FORCE)
    set(WITH_EXAMPLES OFF CACHE BOOL "CMocka: Build examples" FORCE)

    FetchContent_MakeAvailable(cmocka)

    # Enable testing
    enable_testing()

    # Add test executable
    add_executable(cpu_raytracing
            bvh/bvh.c
            bvh/aabb.c
            util/camera.c
            key_event_handler.c
            util/datastructures/bitmap.c
            model/polygon.c
            util/fmt.c
            global.c
            model/obj_parser.c
            util/linalg/matrix.c
            util/linalg/matrix.h
            util/linalg/vec4.h
            util/linalg/tests/test_matrix.c
    )
    target_link_libraries(cpu_raytracing SDL2::SDL2 cmocka)

    # Add tests
    add_test(NAME cpu_raytracing COMMAND cpu_raytracing)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -mfma -mavx2)
    add_executable(cpu_raytracing main.c
            bvh/bvh.c
            bvh/aabb.c
            util/camera.c
            key_event_handler.c
            util/datastructures/bitmap.c
            model/polygon.c
            util/fmt.c
            global.c
            model/obj_parser.c
            util/linalg/matrix.c
            util/linalg/matrix.h
            util/linalg/vec4.h
            bvh/bvh_builder.c
            bvh/bvh_builder.h
            util/threading/threadpool.c
            util/threading/spmc_queue.c
            util/threading/thread.c
    )


    target_link_libraries(cpu_raytracing SDL2::SDL2)
endif ()