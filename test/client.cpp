#include <gtest/gtest.h>
#include <neo4j-cpp/client.h>
#include <string>

using namespace std::string_literals;

TEST(Client, Init) {
    neo4j::client::init();
}

TEST(Client, Info) {
    ASSERT_NE("", neo4j::client::default_client_id());
    ASSERT_NE("", neo4j::client::version());
}