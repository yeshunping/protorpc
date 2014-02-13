// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/rpc_server/rpc_server.h"

#include <sstream>
#include <utility>
#include <vector>

#include "base/flags.h"
#include "base/logging.h"
#include "base/stl_util-inl.h"
#include "base/binary_version.h"
#include "base/string_util.h"
#include "protorpc/http_server/http_request.h"
#include "protorpc/http_server/http_response.h"
#include "protorpc/http_server/http_handler.h"
#include "protorpc/rpc_server/rpc_controller.h"
#include "thirdparty/protobuf/include/google/protobuf/message.h"
#include "thirdparty/protobuf/include/google/protobuf/message.h"
#include "thirdparty/protobuf/include/google/protobuf/dynamic_message.h"
#include "thirdparty/protobuf/include/google/protobuf/descriptor.h"
#include "thirdparty/protobuf/include/google/protobuf/compiler/importer.h"
#include "thirdparty/protobuf/include/google/protobuf/stubs/substitute.h"

using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::Reflection;
using google::protobuf::DescriptorPool;
using google::protobuf::FileDescriptorProto;
using google::protobuf::FileDescriptor;
using google::protobuf::MessageFactory;
using google::protobuf::DynamicMessageFactory;
using google::protobuf::compiler::Importer;
using google::protobuf::compiler::SourceTree;
using google::protobuf::compiler::DiskSourceTree;
using google::protobuf::compiler::MultiFileErrorCollector;
using google::protobuf::MethodDescriptor;
using google::protobuf::RpcController;
using google::protobuf::ServiceDescriptor;

namespace protorpc {

static const char kVersionHttpPath[] = "/version";
static const char kFlagsHttpPath[] = "/flags";
const char kRpcServiceHttpPath[] = "/__rpc__";

static const char kMethodNameHeader[] = "Protorpc_Method_Name";

bool RpcServer::HandleVersionRequest(HttpRequest* request,
                                     HttpResponse* response) {
//    response->SetHeader("Content-Type", "text/plain");
  std::string http_body = StringPrintf(
      "<html><head><title>Version Info for %s</title></head>"
      "<body><pre>%s</pre></body></html>",
      google::ProgramInvocationName(), google::VersionString());
  response->AppendBuffer(http_body);
  return true;
}

bool RpcServer::HandleFlagsRequest(HttpRequest* request,
                                   HttpResponse* response) {
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
  return true;
}

const MethodDescriptor* RpcServer::GetRpcMethod(const string& method_name,
                                                google::protobuf::Service* service) {
  const ServiceDescriptor* service_desc = service->GetDescriptor();
  return service_desc->FindMethodByName(method_name);
}

bool RpcServer::GetRpcRequest(HttpRequest* request,
                              google::protobuf::Message* msg) {
  string data = request->GetRequestData();
  // TODO(yesp) : Try to Uncompress according to service | method options.
  msg->ParseFromString(data);
  return true;
}

void RpcServer::RequestComplete(RpcController* controller,
                                RpcData* data) {
  // TODO(yeshunping) : Clean some resources here
  delete data;
}

inline bool ParseFullMethodName(const std::string& method_full_name,
                                std::string* service_full_name,
                                std::string* method_name) {
  std::string::size_type pos = method_full_name.rfind('.');
  if (pos == std::string::npos) {
    // The method full name must have one period at least to separate
    // service name and method name.
    return false;
  }

  *service_full_name = method_full_name.substr(0, pos);
  *method_name = method_full_name.substr(pos + 1);
  return true;
}

bool RpcServer::HandleRpcRequest(HttpRequest* request, HttpResponse* response) {
  //  Get service name for it.
  string full_method_name = request->GetHeader(kMethodNameHeader);
  VLOG(2) << "receive request for rpc:" << full_method_name;
  string service_name;
  string method_name;
  if (!ParseFullMethodName(full_method_name, &service_name, &method_name)) {
    // TODO(yeshunping) : Collect error message.
    return false;
  }

  LOG(INFO) << "server_name:" << service_name;
  base::hash_map<std::string, google::protobuf::Service*>::iterator it = services_.find(service_name);
  if (it == services_.end()) {
    LOG(ERROR) << "No Service with name:" << service_name;
    return false;
  }
  google::protobuf::Service* service = it->second;
  LOG(INFO) << "find service for name:" << service_name;

  //  try to call service handler for this Rpc, generate Rpc Response
  const MethodDescriptor* method = GetRpcMethod(method_name, service);
  if (!method) {
    LOG(ERROR) << "Fail to get method using name:" << method_name;
    return false;
  }

  google::protobuf::Message* request_obj = service->GetRequestPrototype(method).New();
  request_obj->ParseFromString(request->GetRequestData());
  google::protobuf::Message* response_obj = service->GetRequestPrototype(method).New();

  RpcController* controller = new RpcController();
  google::protobuf::Closure* callback = google::protobuf::NewCallback(this, &RpcServer::RequestComplete,
                                                                      controller,
                                                                      new RpcData(request_obj, request_obj));
  VLOG(2) << "call rpc method, request:\n" << request_obj->DebugString();

  // TODO(yeshunping) : Improve logic here
  service->CallMethod(method, controller, request_obj, response_obj, callback);
  string response_data = response_obj->SerializeAsString();
  response->AppendBuffer(response_data);
  response->Send();

  delete controller;
  delete request_obj;
  delete response_obj;

  return true;
}

bool RpcServer::RegisterService(google::protobuf::Service* service) {
  const string service_name = service->GetDescriptor()->full_name();
  base::hash_map<std::string, google::protobuf::Service*>::const_iterator it =
      services_.find(service_name);
  if (it != services_.end()) {
    LOG(ERROR) << "Fail to register service :" << service_name
               << ", service exist";
    return false;
  }
  services_.insert(make_pair(service_name, service));
  return true;
}

void RpcServer::RegisterBuiltinHandlers() {
  {
    base::ResultCallback2<bool, protorpc::HttpRequest*, protorpc::HttpResponse*>* callback =
        base::NewPermanentCallback(this, &RpcServer::HandleVersionRequest);
    DefaultHttpHandler* version_handler = new DefaultHttpHandler(callback);
    HttpServer::RegisterHttpHandler(kVersionHttpPath, version_handler);
  }

  {
    base::ResultCallback2<bool, protorpc::HttpRequest*, protorpc::HttpResponse*>* callback =
        base::NewPermanentCallback(this, &RpcServer::HandleFlagsRequest);
    DefaultHttpHandler* flags_handler = new DefaultHttpHandler(callback);
    HttpServer::RegisterHttpHandler(kFlagsHttpPath, flags_handler);
  }

  {
    base::ResultCallback2<bool, protorpc::HttpRequest*, protorpc::HttpResponse*>* callback =
        base::NewPermanentCallback(this, &RpcServer::HandleRpcRequest);
    DefaultHttpHandler* rpc_handler = new DefaultHttpHandler(callback);
    HttpServer::RegisterHttpHandler(kRpcServiceHttpPath, rpc_handler);
  }
}

void RpcServer::UnregisterServices() {
  base::STLDeleteValues(&services_);
}

}  // namespace poppy
