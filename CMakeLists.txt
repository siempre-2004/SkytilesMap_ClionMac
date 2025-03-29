cmake_minimum_required(VERSION 3.28)
project(zhehan_hu_render_assignment)

set(CMAKE_CXX_STANDARD 14)

# 设置外部库目录
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/External)

# 查找 raylib（如果安装了 raylib，可以用 find_package）
find_package(raylib REQUIRED)

# 添加头文件目录
include_directories(${EXTERNAL_DIR})
include_directories(${EXTERNAL_DIR}/nlohmann)
include_directories(${EXTERNAL_DIR}/raylib/include)
include_directories(Source)

# 定义源文件
set(SOURCES
        Source/app.cpp
        Source/particle.cpp
        Source/sprite.cpp
        Source/tile.cpp
        Source/tilemap_loader.cpp
)

set(HEADERS
        Source/app.h
        Source/particle.h
        Source/sprite.h
        Source/tile.h
        Source/tilemap_loader.h
        External/nlohmann/json.hpp
)

# 添加可执行文件
add_executable(zhehan_hu_render_assignment ${SOURCES} ${HEADERS})

# 链接 Raylib
target_link_libraries(zhehan_hu_render_assignment raylib)

# 复制资源文件到构建目录
file(COPY Source/sky.png Source/tilemap.json Source/tilesheet_packed.png DESTINATION ${CMAKE_BINARY_DIR})

# 复制资源文件到可执行文件的运行目录
file(COPY ${CMAKE_SOURCE_DIR}/Source/sky.png
        ${CMAKE_SOURCE_DIR}/Source/tilesheet_packed.png
        ${CMAKE_SOURCE_DIR}/Source/tilemap.json
        ${CMAKE_SOURCE_DIR}/Source/BackgroundOne.png
        ${CMAKE_SOURCE_DIR}/Source/BackgroundThree.png
        DESTINATION ${CMAKE_BINARY_DIR})
