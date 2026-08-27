if (NOT TARGET usermod_cxx_terminate)
    add_library(usermod_cxx_terminate INTERFACE)

    target_sources(usermod_cxx_terminate INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/cxx_terminate.cpp
    )

    target_link_libraries(usermod INTERFACE usermod_cxx_terminate)
endif()
