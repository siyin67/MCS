
cmake_minimum_required(VERSION 3.20)
project(BYD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# 查找 SFML
find_package(SFML 3 COMPONENTS Graphics Audio REQUIRED)

# 收集源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

# 链接 SFML
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-audio
)

# Windows 下自动复制 SFML DLL 到 bin 目录
if(WIN32)
    # 尝试从 SFML 安装目录复制 DLL
    if(EXISTS "${SFML_DIR}/../../../bin/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${SFML_DIR}/../../../bin/"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying SFML DLLs to output directory"
        )
    endif()

    # MinGW 特定设置
    if(MINGW)
        # 静态链接 MinGW 运行时，避免依赖 libgcc/libstdc++/libwinpthread DLL
        target_link_options(${PROJECT_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
        # Windows 子系统（无控制台窗口）
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
        endif()
    endif()
endif()

# 复制 assets 目录到 bin
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
    COMMENT "Copying assets to output directory"
)
