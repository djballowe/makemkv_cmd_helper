# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

project(makemkv_cmd_helper)

add_executable(makemkv_helper
    src/main.cpp
    src/build_rip_command.cpp
    src/exec_rip.cpp
    src/parse_output.cpp
)

target_include_directories(makemkv_helper PUBLIC include)
