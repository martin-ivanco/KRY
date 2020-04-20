cmake_minimum_required(VERSION 3.0)

project( rsa )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package( GMP REQUIRED )
include_directories( ${GMP_INCLUDES} )

add_executable( rsa "src/main.cpp" "src/argparse.cpp" "src/rsa.cpp" )

target_link_libraries( rsa ${GMP_LIBRARIES} )
