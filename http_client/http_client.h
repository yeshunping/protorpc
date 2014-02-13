// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_CLIENT_HTTP_CLIENT_H_
#define PROTORPC_HTTP_CLIENT_HTTP_CLIENT_H_

#include <string>
#include <vector>
#include <utility>
#include <map>

#include "base/basictypes.h"
#include "thirdparty/curl/curl.h"

namespace protorpc {
class CurlWrapper {
 public:
  struct WritePack {
    WritePack();

    std::string data;
    bool too_big;
    int32_t max_page_size;
  };

  CurlWrapper();
  ~CurlWrapper();

  //  Call Reset and set option functions before calling FetchUrl.
  //  After calling FetchUrl, you can cal GetResponseCode, GetErrorInfo
  //  to get status information
  void Reset();

  void SetReadTimeOut(int seconds);
  void SetConnectTimeOut(int seconds);
  void SetBindInterface(const std::string& ip);
  void AddHeader(const std::string& key, const std::string& value);
  void SetUserAgent(const std::string& user_agent);
  void SetHttpMethod(const std::string& method);
  void SetMaxPageSize(int max_size);
  void SetMaxHeadSize(int max_size);
  void SetPostData(const std::string& data);

  bool FetchUrl(const std::string& url,
                std::string* out);
  bool GetResponseCode(long* code);
  CURLcode code() const {
    return code_;
  }
  std::string GetErrorInfo();
  double GetTotalUsedTime();
  double GetDownloadSize();
  double GetDownloadSpeed();
  //  Key must be lower case
  bool GetResponseHeader(const std::string& key, std::string* value);
  std::string GetRawResponseHeader();
  bool IsBodyTooBig() const;
  bool IsHeadTooBig() const;
 private:
  void ParseResponseHeader();

  CURL *curl_handle_;
  CURLcode code_;
  char curl_errbuf_[CURL_ERROR_SIZE];
  WritePack head_pack_;
  WritePack body_pack_;
  bool header_parsed_;
  curl_slist* request_header_;
  //  Since header is not too many, here we use vector instead of map
  std::vector<std::pair<std::string, std::string> > res_headers_;
  DISALLOW_COPY_AND_ASSIGN(CurlWrapper);
};

void ParseHttpHeader(const std::string& header,
                     std::map<std::string, std::string>* kvs);
}  // namespace util

#endif  // PROTORPC_HTTP_CLIENT_HTTP_CLIENT_H_
