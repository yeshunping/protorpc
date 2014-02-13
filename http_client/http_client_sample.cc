// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include <stdio.h>

#include "base/flags.h"
#include "base/logging.h"
#include "base/es.h"
#include "base/string_util.h"
#include "protorpc/file/file.h"
#include "protorpc/http_client/http_client.h"

DEFINE_string(url, "", "");
DEFINE_bool(is_web, true, "true:web, false:wap");
DEFINE_bool(download_jump_url, false, "true: download until not 30x");
DEFINE_string(out_file, "tmp.htm", "");
DEFINE_string(user_agent, "Mozilla/5.0 (Windows NT 6.1; rv:16.0) Gecko/20100101 Firefox/16.0", "");

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, false);

  protorpc::CurlWrapper curl;
  string data;
  long code = 0;
  curl.SetUserAgent(FLAGS_user_agent);
  curl.FetchUrl(FLAGS_url, &data);
  file::File::WriteStringToFile(data, FLAGS_out_file);
  curl.GetResponseCode(&code);
  string jump_url;

  if (FLAGS_download_jump_url) {
    int try_ = 0;
    jump_url = FLAGS_url;
    while (code / 300 == 3 &&
           curl.GetResponseHeader("Location", &jump_url)) {
      curl.FetchUrl(jump_url, &data);
      file::File::WriteStringToFile(data, FLAGS_out_file);
      curl.GetResponseCode(&code);
      if (++try_ == 5) {
        break;
      }
    }
    fprintf(stdout, "code:%ld %s -> %s\n", code, FLAGS_url.c_str(), jump_url.c_str());
  } else {
    if (code / 100 == 3 &&
        curl.GetResponseHeader("Location", &jump_url)) {
      fprintf(stdout, "code:%ld %s -> %s\n", code, FLAGS_url.c_str(), jump_url.c_str());
    } else {
      fprintf(stdout, "code:%ld %s -> %s\n", code, FLAGS_url.c_str(), FLAGS_url.c_str());
    }
  }
  return 0;
}
