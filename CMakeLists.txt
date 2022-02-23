cmake_minimum_required(VERSION 3.5.1)
project(CPP-IM-SERVER C CXX)
include(./common.cmake)

get_filename_component(hw_proto "./proto/cpp_im_server.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/cpp_im_server.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/cpp_im_server.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/cpp_im_server.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/cpp_im_server.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

find_package (mongoc-1.0 1.7 REQUIRED)

# 添加mongo-cxx-driver动态连接库
include_directories(/usr/local/include/bsoncxx/v_noabi/)
include_directories(/usr/local/include/mongocxx/v_noabi/)
include_directories(/usr/local/include/libbson-1.0)
include_directories(/usr/local/include/libmongoc-1.0)

add_library(hw_grpc_proto
  ${hw_grpc_srcs}
  ${hw_grpc_hdrs}
  ${hw_proto_srcs}
  ${hw_proto_hdrs}
)

add_library(server_logic
  "./logic_server.cpp"
  "./logic_server.h"
  "./db_server.cpp"
  "./db_server.h"
  "./CONST.h"
)

target_link_libraries(hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

target_link_libraries(server_logic
${_REFLECTION}
${_GRPC_GRPCPP}
${_PROTOBUF_LIBPROTOBUF}
bsoncxx
mongocxx
)

add_executable(im-server "main.cpp")
target_link_libraries(im-server
  hw_grpc_proto
  server_logic
  bsoncxx
  mongocxx
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

add_executable(logic-server "start_logic_server.cpp")
target_link_libraries(logic-server
  hw_grpc_proto
  server_logic
  bsoncxx
  mongocxx
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

add_executable(db-server "start_db_server.cpp")
target_link_libraries(db-server
  hw_grpc_proto
  server_logic
  bsoncxx
  mongocxx
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
