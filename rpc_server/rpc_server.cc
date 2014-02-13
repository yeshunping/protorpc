// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/rpc_server/rpc_server.h"

#include <sstream>
#include <utility>
#include <vector>

#include "base/flags.h"
#include "base/logging.h"
#include "base/binary_version.h"
#include "base/string_util.h"
#include "protorpc/http_server/http_response.h"

namespace protorpc {

static const char kProtoRpcHttpPath[] = "/protorpc";
static const char kVersionHttpPath[] = "/version";
static const char kFlagsHttpPath[] = "/flags";

const char kRpcFormHttpPath[] = "/rpc/form";
const char kRpcServiceHttpPath[] = "/rpc";

bool RpcServer::HandleVersionRequest(HttpRequest* request, HttpResponse* response) {
//    response->SetHeader("Content-Type", "text/plain");
    std::string http_body = StringPrintf("<html><head><title>Version Info for %s</title></head>"
                                         "<body><pre>%s</pre></body></html>",
                                         binary_path_.c_str(),
                                         google::VersionString());
    response->AppendBuffer(http_body);
    return response->Send();
}

bool RpcServer::HandleFlagsRequest(HttpRequest* request, HttpResponse* response) {
    std::vector<google::CommandLineFlagInfo> flaginfo;
    GetAllFlags(&flaginfo);

    //  response->SetHeader("Content-Type", "text/html");
    std::ostringstream os;

    os << "<html><head><title>FlagsInfo page</title></head><body>\n";
    std::vector<google::CommandLineFlagInfo>::iterator iter = flaginfo.begin();
    while (iter != flaginfo.end()) {
        const std::string& filename = iter->filename;
        os << "<h4>" << filename << "</h4>";
        os << "<table border=1>\n";
        os << "<tr><td><b>name</b></td>\n";
        os << "<td><b>type</b></td>\n";
        os << "<td><b>current value</b></td>\n";
        os << "<td><b>default value</b></td>\n";
        os << "<td><b>description</b></td>\n";

        while ((iter != flaginfo.end()) && (filename == iter->filename)) {
            if (!iter->is_default) {
                os << "<tr style=" << "color:red" << ">";
            } else {
                os << "<tr>\n";
            }
            os << "<td>" << iter->name << "</td>\n";
            os << "<td>" << iter->type << "</td>\n";
            os << "<td>" << iter->current_value << "</td>\n";
            os << "<td>" << iter->default_value << "</td>\n";
            os << "<td>" << iter->description << "</td>\n";
            os << "</tr>\n";
            ++iter;
        }
        os << "</table><br />\n";
    }
    os << "</body></html>\n";
    string response_str(os.str());
    response->AppendBuffer(response_str);
    return response->Send();
}

void RpcServer::RegisterService(google::protobuf::Service* service) {
    // TODO(yeshunping) :
}

void RpcServer::RegisterBuiltinHandlers() {
    // TODO(yeshunping) : Register rpc handler
    HttpServer::SetHttpHandler(kVersionHttpPath,
                               base::NewPermanentCallback(this, &RpcServer::HandleVersionRequest));

    HttpServer::SetHttpHandler(kFlagsHttpPath,
                               base::NewPermanentCallback(this, &RpcServer::HandleFlagsRequest));

}

void RpcServer::UnregisterServices() {
    // TODO(yeshunping) :
}

}  // namespace poppy
