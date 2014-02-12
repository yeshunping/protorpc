// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/http_server/http_type.h"

#include <string>

#include "base/string_util.h"

namespace protorpc {

static const struct TableEntry {
  const char *extension;
  const char *content_type;
} kContentTypes[] = {
    { "txt", "text/plain" },
    { "c", "text/plain" },
    { "h", "text/plain" },
    { "html", "text/html" },
    { "htm", "text/htm" },
    { "css", "text/css" },
    { "gif", "image/gif" },
    { "jpg", "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "png", "image/png" },
    { "pdf", "application/pdf" },
    { "ps", "application/postsript" },
    { NULL, NULL },
};

string GuessContentType(const string& path) {
  const char *last_period, *extension;
  const struct TableEntry *ent;
  last_period = strrchr(path.c_str(), '.');
  if (!last_period || strchr(last_period, '/'))
    goto not_found;
  /* no exension */
  extension = last_period + 1;
  for (ent = &kContentTypes[0]; ent->extension; ++ent) {
    if (!StrCaseCmp(ent->extension, extension))
      return ent->content_type;
  }

  not_found:
  return "application/misc";
}
}  // namespace protorpc
