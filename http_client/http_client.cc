// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/http_client/http_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/es.h"
#include "base/flags.h"
#include "base/logging.h"
#include "base/string_util.h"

DEFINE_bool(curl_print_debug_info, false, "");
DEFINE_int32(default_max_page_size, 1024 * 1024 * 2, "");
DEFINE_int32(default_max_head_size, 1024 * 100, "");
DEFINE_string(accept_encoding, "gzip, deflate", "");

namespace util {

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) {
  size_t realsize = size * nmemb;
  CurlWrapper::WritePack* buff = (CurlWrapper::WritePack*) data;

  //  Only accept fix size.
  if (buff->data.size() >= (size_t)buff->max_page_size) {
    VLOG(2) << "response too big, max size:" << buff->max_page_size;
    buff->too_big = true;
    return 0;
  }

  buff->data.append(reinterpret_cast<char*>(ptr), realsize);
  if (buff->data.size() > (size_t)buff->max_page_size) {
    VLOG(2) << "response too big, max size:" << buff->max_page_size;
    buff->data.resize(buff->max_page_size);
    buff->too_big = true;
  }
  return realsize;
}

CurlWrapper::WritePack::WritePack()
    : too_big(false),
      max_page_size(0) {
}

CurlWrapper::CurlWrapper()
    : code_(CURLE_OK),
      header_parsed_(false),
      request_header_(NULL) {
  memset(curl_errbuf_, 0, CURL_ERROR_SIZE);
  body_pack_.max_page_size = FLAGS_default_max_page_size;
  head_pack_.max_page_size = FLAGS_default_max_head_size;

  //  Init curl
  curl_handle_ = curl_easy_init();
  CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_FORBID_REUSE, 1L), CURLE_OK);
  CHECK_EQ(curl_easy_setopt(curl_handle_,
                            CURLOPT_ACCEPT_ENCODING,
                            FLAGS_accept_encoding.c_str()),
                            CURLE_OK);
  CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_ERRORBUFFER, curl_errbuf_),
           CURLE_OK);
  CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION,
                            WriteMemoryCallback),
                             CURLE_OK);
  CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_WRITEHEADER,
                       reinterpret_cast<void *> (&head_pack_)),
                       CURLE_OK);
  CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_FILE,
                       reinterpret_cast<void *> (&body_pack_)),
                       CURLE_OK);
  CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_NOSIGNAL, 1L), CURLE_OK);
  /*
   * If you want to connect to a site who isn't using a certificate that is
   * signed by one of the certs in the CA bundle you have, you can skip the
   * verification of the server's certificate. This makes the connection
   * A LOT LESS SECURE.
   *
   * If you have a CA cert for the server stored someplace else than in the
   * default bundle, then the CURLOPT_CAPATH option might come handy for
   * you.
   */
  curl_easy_setopt(curl_handle_, CURLOPT_SSL_VERIFYPEER, 0L);
  /*
   * If the site you're connecting to uses a different host name that what
   * they have mentioned in their server certificate's commonName (or
   * subjectAltName) fields, libcurl will refuse to connect. You can skip
   * this check, but this will make the connection less secure.
   */
  curl_easy_setopt(curl_handle_, CURLOPT_SSL_VERIFYHOST, 0L);
}

CurlWrapper::~CurlWrapper() {
  if (request_header_) {
    curl_slist_free_all(request_header_);
  }
  curl_easy_cleanup(curl_handle_);
}

void CurlWrapper::SetUserAgent(const string& user_agent) {
  CHECK_EQ(
      curl_easy_setopt(curl_handle_, CURLOPT_USERAGENT, user_agent.c_str()),
      CURLE_OK);
}

void CurlWrapper::SetMaxPageSize(int max_size) {
  body_pack_.max_page_size = max_size;
}

void CurlWrapper::SetMaxHeadSize(int max_size) {
  head_pack_.max_page_size = max_size;
}

void CurlWrapper::SetBindInterface(const std::string& ip) {
  CHECK_EQ(
      curl_easy_setopt(curl_handle_, CURLOPT_INTERFACE, (void*)(ip.c_str())),
      CURLE_OK);
}

bool CurlWrapper::GetResponseCode(long* code) {
  return curl_easy_getinfo(curl_handle_, CURLINFO_RESPONSE_CODE, code)
      == CURLE_OK;
}

double CurlWrapper::GetTotalUsedTime() {
  double used_time;
  curl_easy_getinfo(curl_handle_, CURLINFO_TOTAL_TIME, &used_time);
  return used_time;
}

string CurlWrapper::GetErrorInfo() {
  return string(curl_errbuf_);
}

double CurlWrapper::GetDownloadSize() {
  double down_size;
  curl_easy_getinfo(curl_handle_, CURLINFO_SIZE_DOWNLOAD, &down_size);
  return down_size;
}

double CurlWrapper::GetDownloadSpeed() {
  double down_speed;
  curl_easy_getinfo(curl_handle_, CURLINFO_SPEED_DOWNLOAD, &down_speed);
  return down_speed;
}

bool CurlWrapper::IsBodyTooBig() const {
  return body_pack_.too_big;
}
bool CurlWrapper::IsHeadTooBig() const {
  return head_pack_.too_big;
}

void CurlWrapper::Reset() {
  memset(curl_errbuf_, 0, CURL_ERROR_SIZE);

  header_parsed_ = false;
  res_headers_.clear();

  if (request_header_) {
    curl_slist_free_all(request_header_);
    request_header_ = NULL;
  }

  SetHttpMethod("GET");

  head_pack_.data.clear();
  head_pack_.too_big = false;

  body_pack_.data.clear();
  body_pack_.too_big = false;
}

void CurlWrapper::SetHttpMethod(const string& type) {
  if (type == "GET") {
    CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_CUSTOMREQUEST, "GET"),
             CURLE_OK);
    CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_NOBODY, 0L), CURLE_OK);
  } else if (type == "POST") {
    CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_CUSTOMREQUEST, "POST"),
             CURLE_OK);
    CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_NOBODY, 0L), CURLE_OK);
  } else if (type == "HEAD") {
    CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_CUSTOMREQUEST, "HEAD"),
             CURLE_OK);
    CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_NOBODY, 1L), CURLE_OK);
  } else {
    CHECK(false);
  }
}

void CurlWrapper::SetReadTimeOut(int seconds) {
  CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, seconds), CURLE_OK);
}

void CurlWrapper::SetConnectTimeOut(int seconds) {
  CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_CONNECTTIMEOUT, seconds),
           CURLE_OK);
}

void CurlWrapper::AddHeader(const string& key, const string& value) {
  request_header_ = curl_slist_append(
      request_header_,
      StringPrintf("%s:%s", key.c_str(), value.c_str()).c_str());
}

bool CurlWrapper::GetResponseHeader(const string& key, string* value) {
  if (!header_parsed_) {
    ParseResponseHeader();
  }
  string key_lower = StringToLowerASCII(key);
  for (size_t i = 0; i < res_headers_.size(); ++i) {
    if (res_headers_[i].first == key_lower) {
      value->assign(res_headers_[i].second);
      return true;
    }
  }
  return false;
}

void CurlWrapper::ParseResponseHeader() {
  vector<string> lines;
  SplitString(string(head_pack_.data), '\n', &lines);
  string line;
  // i = 1, skip the "HTTP/1.1 200 OK" line
  for (size_t i = 1; i < lines.size(); ++i) {
    line.clear();
    TrimWhitespace(lines[i], TRIM_ALL, &line);
    if (line.empty()) {
      continue;
    }
    string::size_type index = line.find(':');
    if (index == string::npos) {
      VLOG(1) << "bad header line:" << line;
      continue;
    }
    string key;
    TrimWhitespace(StringToLowerASCII(line.substr(0, index)), TRIM_ALL, &key);
    string value;
    TrimWhitespace(line.substr(index + 1), TRIM_ALL, &value);
    res_headers_.push_back(make_pair(key, value));
  }
}

string CurlWrapper::GetRawResponseHeader() {
  return head_pack_.data;
}

void CurlWrapper::SetPostData(const std::string& data) {
  CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDS, data.c_str()), CURLE_OK);
}

bool CurlWrapper::FetchUrl(const string& url, string* out) {
  if (request_header_) {
    CHECK_EQ(
        curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, request_header_),
        CURLE_OK);
  }
  if (FLAGS_curl_print_debug_info) {
    CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_VERBOSE, 1), CURLE_OK);
  }
  CHECK_EQ(curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str()), CURLE_OK);

  code_ = curl_easy_perform(curl_handle_);
  out->assign(body_pack_.data);
  return code_ == CURLE_OK;
}

void ParseHttpHeader(const string& header, map<string, string>* kvs) {
  vector<string> lines;
  SplitString(string(header), '\n', &lines);
  string line;
  // i = 1, skip the "HTTP/1.1 200 OK" line
  for (size_t i = 1; i < lines.size(); ++i) {
    line.clear();
    TrimWhitespace(lines[i], TRIM_ALL, &line);
    if (line.empty()) {
      continue;
    }
    string::size_type index = line.find(':');
    if (index == string::npos) {
      VLOG(1) << "bad header line:" << line;
      continue;
    }
    string key;
    TrimWhitespace(StringToLowerASCII(line.substr(0, index)), TRIM_ALL, &key);
    string value;
    TrimWhitespace(line.substr(index + 1), TRIM_ALL, &value);
    kvs->insert(make_pair(key, value));
  }
}
}  // namespace util
