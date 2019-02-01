#pragma once
#include <string>
#include <memory>
#include "connect_flags.h"

namespace neo4j {
    class config;
    class connection;
    class client {
    public:
        static void init();
        static void cleanup();
        static std::string default_client_id();
        static std::string version();

        static std::shared_ptr<connection> connect(const std::string& uri, connect_flags flags = connect_flags::none);
        static std::shared_ptr<connection> connect(const std::string& uri, const config& conf, connect_flags flags = connect_flags::none);
        static std::shared_ptr<connection> connect(const std::string& hostname, uint16_t port, const config& conf, bool insecure = false);
    };
}
#ifndef NEO4JPP_IMPL_FILE
#include "impl/client.h"
#endif