# server/CMakeLists.txt

file(GLOB_RECURSE SERVER_SOURCES
    ${PROJECT_SOURCE_DIR}/server/*.cpp
    ${PROJECT_SOURCE_DIR}/socket/*.cpp
)

add_executable(server_app ${SERVER_SOURCES})

target_include_directories(server_app PRIVATE
    ${PROJECT_SOURCE_DIR}/server
    ${PROJECT_SOURCE_DIR}/socket
    ${PROJECT_SOURCE_DIR}
)

# 关键：链接这些库
target_link_libraries(server_app pthread hiredis sodium curl glog)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -g -O1")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=leak -g -O1")
