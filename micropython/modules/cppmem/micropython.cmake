add_library(usermod_cppmem INTERFACE)

target_sources(usermod_cppmem INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/cppmem.c
    ${CMAKE_CURRENT_LIST_DIR}/cppmem.cpp
)

target_include_directories(usermod_cppmem INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(usermod_cppmem INTERFACE
    MODULE_CPPMEM_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_cppmem)

target_compile_options(usermod INTERFACE "-DPICO_CXX_DISABLE_ALLOCATION_OVERRIDES")

set_source_files_properties(
    ${CMAKE_CURRENT_LIST_DIR}/cppmem.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-discarded-qualifiers"
)