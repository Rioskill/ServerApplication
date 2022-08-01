cmake_minimum_required(VERSION 3.16)
project(ServerApplication)

set(CMAKE_CXX_STANDARD 17)

set(server_src_path src/Server)
set(client_src_path src/Client)
set(worker_src_path src/Worker)

set(server_src
    ${server_src_path}/server.cpp
    ${server_src_path}/asyncInputStream.cpp
    ${server_src_path}/asyncOutputStream.cpp
    ${server_src_path}/acceptor.cpp
    ${server_src_path}/eventLoop.cpp
    ${server_src_path}/namedPipe.cpp
    ${server_src_path}/worker.cpp
)

set(client_src
    ${client_src_path}/client.cpp
)

set(worker_src
    ${worker_src_path}/main.cpp
)

add_executable(server ${server_src})
add_executable(client ${client_src})
add_executable(worker ${worker_src})

target_include_directories(server PRIVATE ${server_src_path})
target_include_directories(client PRIVATE ${client_src_path})