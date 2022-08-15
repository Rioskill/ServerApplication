cmake_minimum_required(VERSION 3.16)
project(ServerApplication)

set(CMAKE_CXX_STANDARD 20)

set(server_src_path src/Server)
set(client_src_path src/Client)
set(worker_src_path src/Worker)

set(server_src
    ${server_src_path}/main.cpp
    ${server_src_path}/streams/asyncInputStream.cpp
    ${server_src_path}/streams/asyncOutputStream.cpp
    ${server_src_path}/acceptor.cpp
    ${server_src_path}/connectionProcessor.cpp
    ${server_src_path}/eventLoop.cpp
    ${server_src_path}/namedPipe.cpp
    ${server_src_path}/worker.cpp
    ${server_src_path}/room.cpp
    ${server_src_path}/managers/workerManager.cpp
    ${server_src_path}/managers/roomManager.cpp
    ${server_src_path}/http/HttpRequest.cpp
    ${server_src_path}/http/HttpResponse.cpp
)

# set(client_src
#     ${client_src_path}/client.cpp
# )

# set(worker_src
#     ${worker_src_path}/main.cpp
# )

FILE(COPY ${client_src_path}/pages DESTINATION .)

add_executable(server ${server_src})
# add_executable(client ${client_src})
# add_executable(worker ${worker_src})

target_include_directories(server PRIVATE 
                            ${server_src_path}
                            ${server_src_path}/streams
                            ${server_src_path}/managers
                            ${server_src_path}/http
                            ${server_src_path}/routing
)

# target_include_directories(client PRIVATE ${client_src_path})