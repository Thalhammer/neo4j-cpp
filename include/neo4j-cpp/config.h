#pragma once
#include <string>
#include <functional>

struct neo4j_config;
struct neo4j_memory_allocator;
struct neo4j_plan_table_colors;
struct neo4j_results_table_colors;
struct neo4j_connection_factory;
struct neo4j_logger_provider;

namespace neo4j {
    class config {
        struct neo4j_config* cfg;
        bool custom_client_id;
        std::function<std::string()> password_cb;
        std::function<int(const char*, const char*, int)> unverified_cb;

        void copy_from(const config& other);

        friend class connection;
    public:
        config();
        config(const config& other);
        config& operator=(const config& other);
        ~config();

        std::string get_client_id() const;
        std::string get_known_hosts_file() const;
        unsigned int get_max_pipelined_requests() const;
        struct neo4j_memory_allocator* get_memory_allocator() const;
        const struct neo4j_plan_table_colors* get_plan_table_colorization() const;
        size_t get_rcvbuf_size() const;
        bool get_render_ascii() const;
        unsigned int get_render_inspect_rows() const;
        bool get_render_nulls() const;
        bool get_render_quoted_strings() const;
        bool get_render_rowlines() const;
        bool get_render_wrap_markers() const;
        bool get_render_wrapped_values() const;
        const struct neo4j_results_table_colors* get_results_table_colors() const;
        size_t get_sndbuf_size() const;
        size_t get_so_rcvbuf_size() const;
        size_t get_so_sndbuf_size() const;
        std::string get_TLS_ca_dir() const;
        std::string get_TLS_ca_file() const;
        std::string get_TLS_ca_private_key() const;
        bool get_trust_known_hosts() const;
        std::string get_username() const;

        void set_client_id(const std::string& id);
        void set_connection_factory(struct neo4j_connection_factory* factory);
        void set_known_hosts_file(const std::string& file);
        void set_log_provider(struct neo4j_logger_provider* provider);
        void set_max_pipelined_requests(unsigned int n);
        void set_memory_allocator(struct neo4j_memory_allocator* allocator);
        void set_password(const std::string& password);
        void set_plan_table_colors(const struct neo4j_plan_table_colors* colors);
        void set_rcvbuf_size(size_t size);
        void set_render_ascii(bool enable);
        void set_render_inspect_rows(unsigned int rows);
        void set_render_nulls(bool enable);
        void set_render_quoted_strings(bool enable);
        void set_render_rowlines(bool enable);
        void set_render_wrap_markers(bool enable);
        void set_render_wrapped_values(bool enable);
        void set_results_table_colors(const struct neo4j_results_table_colors* colors);
        void set_sndbuf_size(size_t size);
        void set_so_rcvbuf_size(size_t size);
        void set_so_sndbuf_size(size_t size);
        void set_TLS_ca_dir(const std::string& dir);
        void set_TLS_ca_file(const std::string& file);
        void set_TLS_private_key(const std::string& path);
        void set_TLS_password_callback(std::function<std::string()> fn);
        void set_TLS_password(const std::string& pass);
        void set_trust_known_hosts(bool enable);
        void set_unverified_host_callback(std::function<int(const char*, const char*, int)> fn);
        void set_username(const std::string& username);
    };
}
#ifndef NEO4JPP_IMPL_FILE
#include "impl/config.h"
#endif