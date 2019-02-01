#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>

struct neo4j_result;
struct neo4j_value;

namespace neo4j {
    enum class value_type {
        type_null,
        type_bool,
        type_int,
        type_float,
        type_string,
        type_bytes,
        type_list,
        type_map,
        type_node,
        type_relationship,
        type_path,
        type_identity,
        type_unknown
    };

    inline std::string to_string(value_type v) {
        switch(v) {
            case value_type::type_null: return "null";
            case value_type::type_bool: return "bool";
            case value_type::type_int: return "int";
            case value_type::type_float: return "float";
            case value_type::type_string: return "string";
            case value_type::type_bytes: return "bytes";
            case value_type::type_list: return "list";
            case value_type::type_map: return "map";
            case value_type::type_node: return "node";
            case value_type::type_relationship: return "relationship";
            case value_type::type_path: return "path";
            case value_type::type_identity: return "identity";
            default:
            case value_type::type_unknown: return "unknown";
        }
    }

    class value {
        class data_base;
        class string_data;
        class bytes_data;
        class list_data;
        class map_data;
        struct neo4j_result* result;
        std::unique_ptr<struct neo4j_value> val;
        std::unique_ptr<data_base> data;
    public:
        value();
        value(bool b);
        value(long long i);
        value(double v);
        value(std::string str, bool asbytes=false);
        value(std::vector<uint8_t> bytes);
        value(std::vector<value> list);
        value(std::map<std::string, value> map);

        value(struct neo4j_result* parent, struct neo4j_value val);
        value(const value&);
        value& operator=(const value&);
        ~value();

        struct neo4j_value get_value() const;

        value_type get_type() const noexcept;

        bool is_null() const noexcept { return get_type() == value_type::type_null; }
        bool is_bool() const noexcept { return get_type() == value_type::type_bool; }
        bool is_int() const noexcept { return get_type() == value_type::type_int; }
        bool is_float() const noexcept { return get_type() == value_type::type_float; }
        bool is_string() const noexcept { return get_type() == value_type::type_string; }
        bool is_bytes() const noexcept { return get_type() == value_type::type_bytes; }
        bool is_list() const noexcept { return get_type() == value_type::type_list; }
        bool is_map() const noexcept { return get_type() == value_type::type_map; }
        bool is_node() const noexcept { return get_type() == value_type::type_node; }
        bool is_relationship() const noexcept { return get_type() == value_type::type_relationship; }
        bool is_path() const noexcept { return get_type() == value_type::type_path; }
        bool is_identity() const noexcept { return get_type() == value_type::type_identity; }

        bool to_bool() const;
        long long to_int() const;
        double to_float() const;
        std::string to_string() const;
        std::vector<uint8_t> to_bytes() const;
        long long to_identity() const;


        unsigned int list_size() const;
        value list_entry(unsigned int idx) const;
        
        std::map<std::string, value> to_map() const;
        std::set<std::string> map_keys() const;
        value map_entry(const std::string& key) const;

        long long node_id() const;
        std::set<std::string> node_labels() const;
        std::map<std::string, value> node_properties() const;
        
        long long relationship_id() const;
        long long relationship_start_node_id() const;
        long long relationship_end_node_id() const;
        std::string relationship_type() const;
        std::map<std::string, value> relationship_properties() const;

        unsigned int path_length() const;
        value path_node(unsigned int hops) const;
        value path_relationship(unsigned int hops, bool& forward) const;
        value path_relationship(unsigned int hops) const;

        std::string dump() const;
    };
}
#ifndef NEO4JPP_IMPL_FILE
#include "impl/value.h"
#endif