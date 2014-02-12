cc_library(
    name = 'rpc_server',
    srcs = 'rpc_server.cc',
    deps = ['//base:base',
            '//protorpc/http_server:http_server',
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
