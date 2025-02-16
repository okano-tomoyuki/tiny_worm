foreach(lib_name ini)
    add_executable(test_${lib_name}
        test_${lib_name}.cpp
        ${PROJECT_SOURCE_DIR}/include/${lib_name}.hpp
    )
    target_include_directories(test_${lib_name}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include
    )
    target_link_libraries(test_${lib_name}
        ${lib_name}
    )
endforeach()
