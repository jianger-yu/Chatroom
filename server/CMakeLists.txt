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
target_link_libraries(server_app pthread hiredis sodium curl)
