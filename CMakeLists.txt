cmake_minimum_required(VERSION 3.5)
project(tiny_worm VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_GENERATOR Ninja)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE HEADER_FILE_LIST include/*.hpp)

if (UNIX OR MINGW) 
    set(WARNING_OPTION -Wall)
    set(DEBUG_OPTION -g)
    set(OPTIMIZE_OPTION -O2)
    set(THREAD_OPTION -lpthread)
    set(CMAKE_THREAD_PREFER_PTHREAD ON)                                                                                                                                                                                                           
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
else()
    message("Platform not supported.")
endif()

add_subdirectory(src)
add_subdirectory(test)

add_executable(${PROJECT_NAME}
    main.cpp
    rc/tiny_worm.res
    ${HEADER_FILE_LIST}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        pthread
)

target_compile_options(${PROJECT_NAME}
    PUBLIC
        ${WARNING_OPTION}
        ${DEBUG_OPTION}
        ${OPTIMIZE_OPTION}
        ${THREAD_OPTION}
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        raw_socket
        cli_option
)

if (UNIX)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
            SUFFIX ".out"
    )
endif()
    