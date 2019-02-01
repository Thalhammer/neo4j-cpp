#include <gtest/gtest.h>
#include <neo4j-cpp/config.h>
#include <string>

using namespace std::string_literals;

TEST(Config, CreateCopy) {
    neo4j::config cfg;
    auto cfg2 = cfg;
    neo4j::config cfg3(cfg);
}