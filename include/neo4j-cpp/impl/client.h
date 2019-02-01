#pragma once
#include <neo4j-client.h>
#include "../client.h"
#include "../exception.h"
#include "../connection.h"

namespace neo4j {
    void client::init() {
        int res = neo4j_client_init();
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    void client::cleanup() {
        int res = neo4j_client_cleanup();
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    std::string client::default_client_id() {
        return libneo4j_client_id();
    }

    std::string client::version() {
        return libneo4j_client_version();
    }

    std::shared_ptr<connection> client::connect(const std::string& uri, connect_flags flags)
    {
        return std::make_shared<connection>(uri, flags);
    }

    std::shared_ptr<connection> client::connect(const std::string& uri, const config& conf, connect_flags flags)
    {
        return std::make_shared<connection>(uri, conf, flags);
    }

    std::shared_ptr<connection> client::connect(const std::string& hostname, uint16_t port, const config& conf, bool insecure)
    {
        return std::make_shared<connection>(hostname, port, conf, insecure);
    }
}