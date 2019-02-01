#pragma once
#include <neo4j-client.h>
#include "../exception.h"
#include "../connection.h"
#include "../config.h"

namespace neo4j {
    connection::connection(const std::string& uri, connect_flags flags)
    {
        uint_fast32_t nflags = 0;
        if((flags & connect_flags::insecure) != connect_flags::none) nflags |= NEO4J_INSECURE;
        if((flags & connect_flags::no_uri_credentials) != connect_flags::none) nflags |= NEO4J_NO_URI_CREDENTIALS;
        if((flags & connect_flags::no_uri_password) != connect_flags::none) nflags |= NEO4J_NO_URI_PASSWORD;
        con = neo4j_connect(uri.c_str(), NULL, nflags);
        if(con == nullptr) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    connection::connection(const std::string& uri, const config& conf, connect_flags flags)
        : cfg(std::make_unique<config>(conf))
    {
        uint_fast32_t nflags = 0;
        if((flags & connect_flags::insecure) != connect_flags::none) nflags |= NEO4J_INSECURE;
        if((flags & connect_flags::no_uri_credentials) != connect_flags::none) nflags |= NEO4J_NO_URI_CREDENTIALS;
        if((flags & connect_flags::no_uri_password) != connect_flags::none) nflags |= NEO4J_NO_URI_PASSWORD;
        con = neo4j_connect(uri.c_str(), cfg->cfg, nflags);
        if(con == nullptr) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    connection::connection(const std::string& hostname, uint16_t port, const config& conf, bool insecure)
        : cfg(std::make_unique<config>(conf))
    {
        con = neo4j_tcp_connect(hostname.c_str(), port, cfg->cfg, insecure ? NEO4J_INSECURE : NEO4J_CONNECT_DEFAULT);
        if(con == nullptr) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    connection::~connection()
    {
        int res = neo4j_close(con);
        (void)res;
        // TODO: How to handle error ?
    }

    std::string connection::get_hostname() const
    {
        return neo4j_connection_hostname(con);
    }

    uint16_t connection::get_port() const
    {
        return neo4j_connection_port(con);
    }

    std::string connection::get_username() const
    {
        auto ptr = neo4j_connection_username(con);
        if(ptr == nullptr) return "";
        return ptr;
    }

    bool connection::is_secure() const
    {
        return neo4j_connection_is_secure(con);
    }

    bool connection::is_credentials_expired() const
    {
        return neo4j_credentials_expired(con);
    }

    std::string connection::get_server_id() const
    {
        auto ptr = neo4j_server_id(con);
        if(ptr == nullptr) return "";
        return ptr;
    }

    void connection::reset()
    {
        int res = neo4j_reset(con);
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    std::shared_ptr<result_stream> connection::send(const std::string& query)
    {
        return std::make_shared<result_stream>(this->shared_from_this(), false, query);
    }

    std::shared_ptr<result_stream> connection::run(const std::string& query)
    {
        return std::make_shared<result_stream>(this->shared_from_this(), true, query);
    }
}