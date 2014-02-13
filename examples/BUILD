proto_library(
    name = 'echo_service_proto',
    srcs = 'echo_service.proto',
    deps = [
        '//protorpc/proto:protorpc_proto',
    ]
)

cc_binary(
    name = 'echoclient',
    srcs = [
        'echo_client.cc'
    ],
    deps = [
        ':echo_service_proto',
        '//protorpc:protorpc',
    ]
)

cc_binary(
    name = 'echoserver',
    srcs = [
        'echo_server.cc'
    ],
    deps = [
        ':echo_service_proto',
        '//protorpc:protorpc',
    ]
)
