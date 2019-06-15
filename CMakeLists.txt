cmake_minimum_required (VERSION 3.1.0)
project (linuxDebug)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(COMPILE_FLAGS PUBLIC -g -Wall -Wextra -std=c++1y )

add_subdirectory(
   external/linenoise-ng
 )

add_custom_command(OUTPUT libelf++.a 
                          libdwarf++.a
                   COMMAND make 
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/libelfin
)

add_executable(linbug
  src/breakpoint.cpp
  src/debugger.cpp
  src/main.cpp
  src/ptrace.cpp
  src/registers.cpp
  includes/breakpoint.hpp
  includes/debugger.hpp
  includes/ptrace.hpp
  includes/regDescriptor.hpp
  includes/registers.hpp
  )

target_include_directories(linbug PUBLIC includes
                           external/libelfin/dwarf
                           external/libelfin/elf
                           external/linenoise-ng/include)

target_link_libraries(linbug
  linenoise
  ${CMAKE_SOURCE_DIR}/external/libelfin/dwarf/libdwarf++.a
  ${CMAKE_SOURCE_DIR}/external/libelfin/elf/libelf++.a
  #boost_system boost_thread boost_log boost_log_setup
  )

target_compile_options(linbug ${COMPILE_FLAGS})

add_executable(program
  testProgram/program.cpp
  )

target_compile_options(program ${COMPILE_FLAGS})
