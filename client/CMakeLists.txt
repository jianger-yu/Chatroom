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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -g -O1")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=leak -g -O1")

