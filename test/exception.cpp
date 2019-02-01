#include <gtest/gtest.h>
#include <neo4j-cpp/exception.h>
#include <string>

using namespace std::string_literals;

TEST(Exception, Create) {
    neo4j::exception e("Hello");
    ASSERT_EQ(e.what(), "Hello"s);
}