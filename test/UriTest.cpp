#include <gtest/gtest.h>

#include <cell/core/string.hpp>
#include <cell/core/string_slice.hpp>
#include <cell/http/uri.hpp>

using cell::String, cell::StringSlice, cell::http::Uri;

TEST(UriParsingTests, SimpleUriToDecode) {
  Uri uri;
  uri.set_data_buffer(StringSlice::from_cstr("/index.php"));

  ASSERT_TRUE(uri.get_path_decoded().Compare(uri.get_path_raw()));
  ASSERT_STREQ(uri.get_path_decoded().GetConstCharPtr(), "index.php");
  ASSERT_TRUE(uri.get_queries().IsEmpty());
}
