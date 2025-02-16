set(lib_name_list raw_socket cli_option ini)

if (MINGW)
    find_library(WSOCK32_LIBRARY wsock32)
    find_library(WS2_32_LIBRARY ws2_32)
    find_library(iphlpapi_LIBRARY iphlpapi)
endif()

foreach(lib_name ${lib_name_list})   
    add_library(${lib_name}
        STATIC 
            ${lib_name}.cpp
            ${PROJECT_SOURCE_DIR}/include/${lib_name}.hpp
    )

    target_compile_options(${lib_name}
        PUBLIC 
            ${WARNING_OPTION}
            ${DEBUG_OPTION}
            ${OPTIMIZE_OPTION}
            ${THREAD_OPTION}  
    )

    target_include_directories(${lib_name} 
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include
    )

    if (MINGW)
        if (${lib_name} MATCHES "raw_socket")
            target_link_libraries(${lib_name} 
                PRIVATE
                    wsock32 
                    ws2_32
                    iphlpapi
            )
        endif()
    endif()
endforeach()