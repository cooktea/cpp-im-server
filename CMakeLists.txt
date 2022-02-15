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
${_PROTOBUF_LIBPROTOBUF})

foreach(_target main)
  add_executable(${_target} "${_target}.cpp")
  target_link_libraries(${_target}
    hw_grpc_proto
    server_logic
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()