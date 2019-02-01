#pragma once
#include <string>
#include <memory>
#include "connect_flags.h"

struct neo4j_connection;

namespace neo4j {
    class config;
    class result_stream;
    class connection : public std::enable_shared_from_this<connection> {
        struct neo4j_connection* con;
        std::unique_ptr<config> cfg;
        friend class result_stream;
    public:
        connection(const std::string& uri, connect_flags flags = connect_flags::none);
        connection(const std::string& uri, const config& conf, connect_flags flags = connect_flags::none);
        connection(const std::string& hostname, uint16_t port, const config& conf, bool insecure = false);
        ~connection();

        connection(const connection&) = delete;
        connection& operator=(const connection&) = delete;

        std::string get_hostname() const;
        uint16_t get_port() const;
        std::string get_username() const;
        bool is_secure() const;
        bool is_credentials_expired() const;
        std::string get_server_id() const;

        void reset();

        std::shared_ptr<result_stream> send(const std::string& query);
        std::shared_ptr<result_stream> run(const std::string& query);
    };
}
#ifndef NEO4JPP_IMPL_FILE
#include "impl/connection.h"
#endif