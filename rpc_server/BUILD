cc_library(
    name = 'rpc_server',
    srcs = 'rpc_server.cc',
    deps = [':rpc_controller',
            '//base:base',
            '//protorpc/http_server:http_server',
            '//thirdparty/protobuf:protobuf',
            ],
)

cc_library(
    name = 'rpc_channel',
    srcs = 'rpc_channel.cc',
    deps = ['//base:base',
            '//protorpc/rpc_client:rpc_client',
            '//thirdparty/protobuf:protobuf',
            ],
)

cc_library(
    name = 'rpc_controller',
    srcs = 'rpc_controller.cc',
    deps = ['//base:base',
            '//thirdparty/protobuf:protobuf',
            ],
)

cc_binary(
    name = 'rpc_server_sample',
    srcs = 'rpc_server_sample.cc',
    deps = [':rpc_server',
            '//base:base',
            '//thirdparty/event:event',
            ],
)
