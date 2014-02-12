proto_library(
    name = 'rpc_option_proto',
    srcs = 'rpc_option.proto'
)

proto_library(
    name = 'rpc_meta_info_proto',
    srcs = 'rpc_meta_info.proto',
    deps = ':rpc_option_proto'
)

proto_library(
    name = 'rpc_message_proto',
    srcs = 'rpc_message.proto',
    deps = ':rpc_option_proto'
)

proto_library(
    name = 'rpc_error_code_proto',
    srcs = 'rpc_error_code.proto'
)