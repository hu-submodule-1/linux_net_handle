cmake_minimum_required(VERSION 3.10)

# 定义静态库
add_library(linux_net_handle STATIC net_handle.c)

# 添加头文件搜索路径
target_include_directories(linux_net_handle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
