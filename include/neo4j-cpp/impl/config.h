#pragma once
#include <neo4j-client.h>
#include "../config.h"
#include "../exception.h"

namespace neo4j {
    void config::copy_from(const config& other)
    {
        struct neo4j_config* old_cfg = cfg;
        cfg = neo4j_config_dup(other.cfg);
        if(cfg == nullptr) {
            cfg = old_cfg;
            throw exception(neo4j_strerror(errno, nullptr, 0));
        }
        if(other.custom_client_id) {
            this->set_client_id(other.get_client_id());
        }
        if(other.password_cb) {
            password_cb = other.password_cb;
            int res = neo4j_config_set_TLS_private_key_password_callback(cfg, [](void* data, char *buf, size_t len) -> ssize_t {
                config* that = static_cast<config*>(data);
                std::string password = "";
                if(that->password_cb) password = that->password_cb();
                len = (password.size() < len)?password.size():len;
                memcpy(buf, password.data(), len);
                return len;
            }, this);
            if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
        }
        if(other.unverified_cb) {
            unverified_cb = other.unverified_cb;
            int res = neo4j_config_set_unverified_host_callback(cfg, [](void* data, const char *host, const char *fingerprint, neo4j_unverified_host_reason_t reason){
                config* that = static_cast<config*>(data);
                if(that->unverified_cb) return that->unverified_cb(host, fingerprint, (int)reason);
                else return NEO4J_HOST_VERIFICATION_REJECT;
            }, this);
            if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
        }
        if(old_cfg != nullptr) {
            if(custom_client_id) free((void*)neo4j_config_get_client_id(old_cfg));
            neo4j_config_free(old_cfg);
        }
    }

    config::config()
        : cfg(neo4j_new_config()), custom_client_id(false)
    {
        if(cfg == nullptr) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    config::config(const config& other)
        : cfg(nullptr)
    {
        this->copy_from(other);
    }

    config& config::operator=(const config& other)
    {
        this->copy_from(other);
        return *this;
    }

    config::~config()
    {
        neo4j_config_free(cfg);
    }

    std::string config::get_client_id() const
    {
        auto ptr = neo4j_config_get_client_id(cfg);
        return ptr == nullptr ? "" : ptr;
    }

    std::string config::get_known_hosts_file() const
    {
        auto ptr = neo4j_config_get_client_id(cfg);
        return ptr == nullptr ? "" : ptr;
    }

    unsigned int config::get_max_pipelined_requests() const
    {
        return neo4j_config_get_max_pipelined_requests(cfg);
    }

    struct neo4j_memory_allocator* config::get_memory_allocator() const
    {
        return neo4j_config_get_memory_allocator(cfg);
    }

    const struct neo4j_plan_table_colors* config::get_plan_table_colorization() const
    {
        return neo4j_config_get_plan_table_colorization(cfg);
    }

    size_t config::get_rcvbuf_size() const
    {
        return neo4j_config_get_rcvbuf_size(cfg);
    }

    bool config::get_render_ascii() const
    {
        return neo4j_config_get_render_ascii(cfg);
    }

    unsigned int config::get_render_inspect_rows() const
    {
        return neo4j_config_get_render_inspect_rows(cfg);
    }

    bool config::get_render_nulls() const
    {
        return neo4j_config_get_render_nulls(cfg);
    }

    bool config::get_render_quoted_strings() const
    {
        return neo4j_config_get_render_quoted_strings(cfg);
    }
    
    bool config::get_render_rowlines() const
    {
        return neo4j_config_get_render_rowlines(cfg);
    }

    bool config::get_render_wrap_markers() const
    {
        return neo4j_config_get_render_wrap_markers(cfg);
    }

    bool config::get_render_wrapped_values() const
    {
        return neo4j_config_get_render_wrapped_values(cfg);
    }

    const struct neo4j_results_table_colors* config::get_results_table_colors() const
    {
        return neo4j_config_get_results_table_colors(cfg);
    }

    size_t config::get_sndbuf_size() const
    {
        return neo4j_config_get_sndbuf_size(cfg);
    }

    size_t config::get_so_rcvbuf_size() const
    {
        return neo4j_config_get_so_rcvbuf_size(cfg);
    }

    size_t config::get_so_sndbuf_size() const
    {
        return neo4j_config_get_so_sndbuf_size(cfg);
    }

    std::string config::get_TLS_ca_dir() const
    {
        return neo4j_config_get_TLS_ca_dir(cfg);
    }

    std::string config::get_TLS_ca_file() const
    {
        return neo4j_config_get_TLS_ca_file(cfg);
    }

    std::string config::get_TLS_ca_private_key() const
    {
        return neo4j_config_get_TLS_private_key(cfg);
    }

    bool config::get_trust_known_hosts() const
    {
        return neo4j_config_get_trust_known_hosts(cfg);
    }

    std::string config::get_username() const
    {
        return neo4j_config_get_username(cfg);
    }

    void config::set_client_id(const std::string& id)
    {
        char* dup = strdup(id.c_str());
        if(dup == nullptr) throw exception("Failed to allocate client_id");
        if(custom_client_id) {
            free((void*)neo4j_config_get_client_id(cfg));
        }
        neo4j_config_set_client_id(cfg, dup);
        custom_client_id = true;
    }

    void config::set_connection_factory(struct neo4j_connection_factory* factory)
    {
        neo4j_config_set_connection_factory(cfg, factory);
    }

    void config::set_known_hosts_file(const std::string& file)
    {
        int res = neo4j_config_set_known_hosts_file(cfg, file.c_str());
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    void config::set_log_provider(struct neo4j_logger_provider* provider)
    {
        neo4j_config_set_logger_provider(cfg, provider);
    }

    void config::set_max_pipelined_requests(unsigned int n)
    {
        neo4j_config_set_max_pipelined_requests(cfg, n);
    }
    
    void config::set_memory_allocator(struct neo4j_memory_allocator* allocator)
    {
        neo4j_config_set_memory_allocator(cfg, allocator);
    }

    void config::set_password(const std::string& password)
    {
        int res = neo4j_config_set_password(cfg, password.c_str());
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }
    
    void config::set_plan_table_colors(const struct neo4j_plan_table_colors* colors)
    {
        // TODO: Copy and handle free
        neo4j_config_set_plan_table_colors(cfg, colors);
    }

    void config::set_rcvbuf_size(size_t size)
    {
        int res = neo4j_config_set_rcvbuf_size(cfg, size);
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    void config::set_render_ascii(bool enable)
    {
        neo4j_config_set_render_ascii(cfg, enable);
    }

    void config::set_render_inspect_rows(unsigned int rows)
    {
        neo4j_config_set_render_inspect_rows(cfg, rows);
    }

    void config::set_render_nulls(bool enable)
    {
        neo4j_config_set_render_nulls(cfg, enable);
    }

    void config::set_render_quoted_strings(bool enable)
    {
        neo4j_config_set_render_quoted_strings(cfg, enable);
    }

    void config::set_render_rowlines(bool enable)
    {
        neo4j_config_set_render_rowlines(cfg, enable);
    }

    void config::set_render_wrap_markers(bool enable)
    {
        neo4j_config_set_render_wrap_markers(cfg, enable);
    }

    void config::set_render_wrapped_values(bool enable)
    {
        neo4j_config_set_render_wrapped_values(cfg, enable);
    }

    void config::set_results_table_colors(const struct neo4j_results_table_colors* colors)
    {
        // TODO: Copy and handle free
        neo4j_config_set_results_table_colors(cfg, colors);
    }

    void config::set_sndbuf_size(size_t size)
    {
        int res = neo4j_config_set_sndbuf_size(cfg, size);
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    void config::set_so_rcvbuf_size(size_t size)
    {
        int res = neo4j_config_set_rcvbuf_size(cfg, size);
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    void config::set_so_sndbuf_size(size_t size)
    {
        int res = neo4j_config_set_sndbuf_size(cfg, size);
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    void config::set_TLS_ca_dir(const std::string& dir)
    {
        int res = neo4j_config_set_TLS_ca_dir(cfg, dir.c_str());
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    void config::set_TLS_ca_file(const std::string& file)
    {
        int res = neo4j_config_set_TLS_ca_file(cfg, file.c_str());
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    void config::set_TLS_private_key(const std::string& path)
    {
        int res = neo4j_config_set_TLS_private_key(cfg, path.c_str());
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    void config::set_TLS_password_callback(std::function<std::string()> fn)
    {
        int res = neo4j_config_set_TLS_private_key_password_callback(cfg, [](void* data, char *buf, size_t len) -> ssize_t {
                config* that = static_cast<config*>(data);
                std::string password = "";
                if(that->password_cb) password = that->password_cb();
                len = (password.size() < len)?password.size():len;
                memcpy(buf, password.data(), len);
                return len;
            }, this);
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
        password_cb = fn;
    }

    void config::set_TLS_password(const std::string& pass)
    {
        set_TLS_password_callback([tmp = pass](){
            return tmp;
        });
    }

    void config::set_trust_known_hosts(bool enable)
    {
        int res = neo4j_config_set_trust_known_hosts(cfg, enable);
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    void config::set_unverified_host_callback(std::function<int(const char*, const char*, int)> fn)
    {
        int res = neo4j_config_set_unverified_host_callback(cfg, [](void* data, const char *host, const char *fingerprint, neo4j_unverified_host_reason_t reason){
            config* that = static_cast<config*>(data);
            if(that->unverified_cb) return that->unverified_cb(host, fingerprint, (int)reason);
            else return NEO4J_HOST_VERIFICATION_REJECT;
        }, this);
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
        unverified_cb = fn;
    }

    void config::set_username(const std::string& username)
    {
        int res = neo4j_config_set_username(cfg, username.c_str());
        if(res != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
    }
}