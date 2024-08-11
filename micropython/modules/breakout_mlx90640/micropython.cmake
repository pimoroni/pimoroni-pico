add_library(usermod_mlx90640 INTERFACE)

target_sources(usermod_mlx90640 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/mlx90640.c
    ${CMAKE_CURRENT_LIST_DIR}/mlx90640.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/mlx90640/mlx90640.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/mlx90640/src/functions/MLX90640_API.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/mlx90640/MLX90640_RP2040_I2C_Driver.cpp
)

target_include_directories(usermod_mlx90640 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/mlx90640/
    ${CMAKE_CURRENT_LIST_DIR}/../../../drivers/mlx90640/src/headers/
)

target_compile_definitions(usermod_mlx90640 INTERFACE
    MODULE_MLX90640_ENABLED=1
)

target_link_libraries(usermod INTERFACE usermod_mlx90640)