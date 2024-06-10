cmake_minimum_required(VERSION 3.0)
project(sysrpc)

#设置项目可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#设置项目库文件输出的路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#设置项目编译头文件搜索路径 -I
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

#设置项目库文件搜索路径 -L
link_directories(${PROJECT_SOURCE_DIR}/lib)

#在子目录下寻找makefile文件
#src包含了sysrpc框架的所有代码


add_subdirectory(src)
add_subdirectory(example)