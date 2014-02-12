cc_library(
    name = 'http_client',
    srcs = 'http_client.cc',
    deps = ['//base:base',
            '//thirdparty/curl:curl',
            ]
)

cc_binary(
    name = 'http_client_sample',
    srcs = 'http_client_sample.cc',
    deps = ['//base:base',
            ':http_client',
            '//protorpc/file:file',
            ]
)