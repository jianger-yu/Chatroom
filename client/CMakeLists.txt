# client/CMakeLists.txt

file(GLOB_RECURSE CLIENT_SOURCES
    ${PROJECT_SOURCE_DIR}/client/*.cpp
    ${PROJECT_SOURCE_DIR}/socket/*.cpp
)

add_executable(client_app ${CLIENT_SOURCES})

target_include_directories(client_app PRIVATE
    ${PROJECT_SOURCE_DIR}/client
    ${PROJECT_SOURCE_DIR}/socket
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(client_app pthread hiredis sodium curl)
