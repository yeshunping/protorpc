cc_binary(
    name = 'http_server_sample',
    srcs = 'http_server_sample.cc',
    deps = [':http_server',
            '//base:base',
            '//thirdparty/event:event',
            ],
)

cc_library(
    name = 'http_type',
    srcs = 'http_type.cc',
    deps = ['//base:base',
            ],
)

cc_library(
    name = 'http_request',
    srcs = 'http_request.cc',
    deps = ['//base:base',
            '//thirdparty/event:event',
            ],
)

cc_library(
    name = 'http_response',
    srcs = 'http_response.cc',
    deps = ['//base:base',
            '//thirdparty/event:event',
            ],
)

cc_library(
    name = 'event_loop',
    srcs = 'event_loop.cc',
    deps = ['//base:base',
            '//thirdparty/event:event',
            ],
)

cc_library(
    name = 'debug_util',
    srcs = 'debug_util.cc',
    deps = ['//base:base',
            '//thirdparty/event:event',
            ],
)


cc_library(
    name = 'http_server',
    srcs = 'http_server.cc',
    deps = [':http_type',
            ':debug_util',
            ':http_request',
            ':http_response',
            ':event_loop',
            '//base:base',
            '//thirdparty/event:event',
            ],
)