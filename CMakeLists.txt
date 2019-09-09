cmake_minimum_required(VERSION 3.10)

project("Your project name here" CXX)
enable_testing()

set(tests_src_dir "tests")
set(objs_src_dir "src/objs")
set(programs_src_dir "src/main")

# set(warnings "-Wall -Wextra -Werror")
set(cpp_version "c++14")

set(CMAKE_CXX_FLAGS "${warnings} -std=${cpp_version} -O0 -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${objs_src_dir})
file(GLOB_RECURSE objs_src_files "${objs_src_dir}/*.cpp")

file(GLOB_RECURSE tests_src_files "${tests_src_dir}/*.cpp")
foreach(test_src_file ${tests_src_files})
	string(REGEX REPLACE "(^(.*/)*${tests_src_dir}/)|(.cpp)" "" test_binary ${test_src_file})
	add_executable(${test_binary} ${test_src_file} ${objs_src_files})
	add_test(NAME ${test_binary} COMMAND ${test_binary})
endforeach()	


file(GLOB programs_src_files "${programs_src_dir}/*.cpp")
foreach(program_src_file ${programs_src_files})
	string(REGEX REPLACE "(^(.*/)*${programs_src_dir}/)|(.cpp)" "" program_binary ${program_src_file})
	add_executable(${program_binary} ${program_src_file} ${objs_src_files})
endforeach()
