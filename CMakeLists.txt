cmake_minimum_required(VERSION 3.12)
project(Tiderip_cpp CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# 定义源文件 *.c
aux_source_directory(Include INCLUDE)
aux_source_directory(VM VM)
aux_source_directory(Parser PARSER)
aux_source_directory(CLI CLI)
aux_source_directory(Object OBJECT)
aux_source_directory(Compiler COMPILER)
# 显示源文件路径
set(SCR_SOUCES_LIST ${INCLUDE} ${VM} ${PARSER} ${CLI} ${OBJECT} ${COMPILER})
message("\r\n-----[SCR_SOUCES_PATH]-----")
foreach (SOURCE_PATH ${SCR_SOUCES_LIST})
    message("--> ${SOURCE_PATH}")
endforeach ()
message("-----[END_MESSAGES]-----\r\n")

# 定义头文件 *.h
file(GLOB_RECURSE SCR_HEADER_PATH "*.hpp" "*.h")
message("-----[SCR_HEADER_PATH]-----")
foreach (HEADER_PATH ${SCR_HEADER_PATH})
    message("--> ${HEADER_PATH}")
endforeach ()
message("-----[END_MESSAGES]-----\r\n")

include_directories(Include VM Parser CLI Object Compiler)

add_executable(Tiderip_cpp main.cpp ${SCR_HEADER_PATH} ${SCR_SOUCES_LIST})