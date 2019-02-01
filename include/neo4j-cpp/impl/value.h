#pragma once
#include <neo4j-client.h>
#include <memory>
#include "../value.h"
#include "../exception.h"

namespace neo4j {
    class value::data_base {
    public:
        virtual ~data_base() {}
        virtual std::unique_ptr<data_base> clone() const = 0;
        virtual std::unique_ptr<struct neo4j_value> get_value() const = 0;
    };

    class value::string_data: public value::data_base {
        std::string data;
        bool as_bytes;
    public:
        string_data(std::string d, bool bytes)
            : data(std::move(d)), as_bytes(bytes)
        {}
        std::unique_ptr<data_base> clone() const override {
            return std::make_unique<string_data>(*this);
        }
        std::unique_ptr<struct neo4j_value> get_value() const override {
            if(as_bytes) return std::make_unique<struct neo4j_value>(neo4j_bytes(data.c_str(), data.size()));
            else return std::make_unique<struct neo4j_value>(neo4j_string(data.c_str()));
        }
    };

    class value::bytes_data: public value::data_base {
        std::vector<uint8_t> data;
    public:
        bytes_data(std::vector<uint8_t> d)
            : data(std::move(d))
        {}
        std::unique_ptr<data_base> clone() const override {
            return std::make_unique<bytes_data>(*this);
        }
        std::unique_ptr<struct neo4j_value> get_value() const override {
            return std::make_unique<struct neo4j_value>(neo4j_bytes(reinterpret_cast<const char*>(data.data()), data.size()));
        }
    };

    class value::list_data: public value::data_base {
        std::vector<value> data;
        std::vector<struct neo4j_value> vals;
    public:
        list_data(std::vector<value> d)
            : data(std::move(d))
        {
            for(auto& val : data) {
                vals.push_back(val.get_value());
            }
        }
        std::unique_ptr<data_base> clone() const override {
            return std::make_unique<list_data>(data);
        }
        std::unique_ptr<struct neo4j_value> get_value() const override {
            return std::make_unique<struct neo4j_value>(neo4j_list(vals.data(), vals.size()));
        }
    };

    class value::map_data: public value::data_base {
        std::map<std::string, value> data;
        std::vector<struct neo4j_map_entry> vals;
    public:
        map_data(std::map<std::string, value> d)
            : data(std::move(d))
        {
            for(auto& val : data) {
                vals.push_back(neo4j_map_entry(val.first.c_str(), val.second.get_value()));
            }
        }
        std::unique_ptr<data_base> clone() const override {
            return std::make_unique<map_data>(data);
        }
        std::unique_ptr<struct neo4j_value> get_value() const override {
            return std::make_unique<struct neo4j_value>(neo4j_map(vals.data(), vals.size()));
        }
    };

    value::value()
    {
        val = std::make_unique<struct neo4j_value>(neo4j_null);
    }

    value::value(bool v)
    {
        val = std::make_unique<struct neo4j_value>(neo4j_bool(v));
    }

    value::value(long long v)
    {
        val = std::make_unique<struct neo4j_value>(neo4j_int(v));
    }

    value::value(double v)
    {
        val = std::make_unique<struct neo4j_value>(neo4j_float(v));
    }
        
    value::value(std::string str, bool asbytes)
    {
        data = std::make_unique<string_data>(std::move(str), asbytes);
        val = data->get_value();
    }

    value::value(std::vector<uint8_t> bytes)
    {
        data = std::make_unique<bytes_data>(std::move(bytes));
        val = data->get_value();
    }

    value::value(std::vector<value> list)
    {
        data = std::make_unique<list_data>(std::move(list));
        val = data->get_value();
    }

    value::value(std::map<std::string, value> map)
    {

    }

    value::value(struct neo4j_result* parent, struct neo4j_value v)
        : result(neo4j_retain(parent)), data(nullptr)
    {
        val = std::make_unique<struct neo4j_value>();
        memcpy(val.get(), &v, sizeof(v));
    }

    value::value(const value& other)
    {
        if(other.result != nullptr) {
            result = neo4j_retain(other.result);
            val = std::make_unique<struct neo4j_value>();
            memcpy(val.get(), other.val.get(), sizeof(struct neo4j_value));
        } else if(other.data) {
            data = other.data->clone();
            val = data->get_value();
        } else {
            val = std::make_unique<struct neo4j_value>();
            memcpy(val.get(), other.val.get(), sizeof(struct neo4j_value));
        }
    }

    value& value::operator=(const value& other)
    {
        if(other.result != nullptr) {
            result = neo4j_retain(other.result);
            val = std::make_unique<struct neo4j_value>();
            memcpy(val.get(), other.val.get(), sizeof(struct neo4j_value));
        } else {
            data = other.data->clone();
            val = data->get_value();
        }
        return *this;
    }

    value::~value()
    {
        if(result) neo4j_release(result);
    }

    struct neo4j_value value::get_value() const {
        return *val;
    }


    value_type value::get_type() const noexcept
    {
        auto type = neo4j_type(*val);
        if(type == NEO4J_BOOL) return value_type::type_bool;
        if(type == NEO4J_BYTES) return value_type::type_bytes;
        if(type == NEO4J_FLOAT) return value_type::type_float;
        if(type == NEO4J_IDENTITY) return value_type::type_identity;
        if(type == NEO4J_INT) return value_type::type_int;
        if(type == NEO4J_LIST) return value_type::type_list;
        if(type == NEO4J_MAP) return value_type::type_map;
        if(type == NEO4J_NODE) return value_type::type_node;
        if(type == NEO4J_NULL) return value_type::type_null;
        if(type == NEO4J_PATH) return value_type::type_path;
        if(type == NEO4J_RELATIONSHIP) return value_type::type_relationship;
        if(type == NEO4J_STRING) return value_type::type_string;
        return value_type::type_unknown;
    }

    bool value::to_bool() const
    {
        if(!is_bool()) throw exception("not a bool");
        return neo4j_bool_value(*val);
    }

    long long value::to_int() const
    {
        if(!is_int()) throw exception("not an int");
        return neo4j_int_value(*val);
    }

    double value::to_float() const
    {
        if(!is_float()) throw exception("not a float");
        return neo4j_float_value(*val);
    }

    std::string value::to_string() const
    {
        if(!is_string()) throw exception("not a string");
        std::string res;
        res.resize(neo4j_string_length(*val) + 1);
        neo4j_string_value(*val, const_cast<char*>(res.data()), res.size());
        res.resize(res.size() -1);
        return res;
    }

    std::vector<uint8_t> value::to_bytes() const
    {
        if(!is_bytes()) throw exception("not bytes");
        std::vector<uint8_t> res;
        res.resize(neo4j_bytes_length(*val));
        memcpy(res.data(), neo4j_bytes_value(*val), res.size());
        return res;
    }

    long long value::to_identity() const
    {
        if(!is_identity()) throw exception("not a identity");
        char buf[25];
        neo4j_tostring(*val, buf, 25);
        return std::stoll(buf);
    }

    unsigned int value::list_size() const
    {
        if(!is_list()) throw exception("not a list");
        return neo4j_list_length(*val);
    }

    value value::list_entry(unsigned int idx) const
    {
        if(!is_list()) throw exception("not a list");
        if(idx >= neo4j_list_length(*val)) throw std::out_of_range("invalid index");
        return value(result, neo4j_list_get(*val, idx));
    }

    std::map<std::string, value> value::to_map() const
    {
        if(!is_map()) throw exception("not a map");
        unsigned int size = neo4j_map_size(*val);
        std::map<std::string, value> res;
        for(unsigned int i = 0; i < size; i++) {
            auto entry = neo4j_map_getentry(*val, i);
            std::string str;
            str.resize(neo4j_string_length(entry->key) + 1);
            neo4j_string_value(entry->key, const_cast<char*>(str.data()), str.size());
            str.resize(str.size() -1);
            res.insert({str, value(result, entry->value)});
        }
        return res;
    }

    std::set<std::string> value::map_keys() const
    {
        if(!is_map()) throw exception("not a map");
        unsigned int size = neo4j_map_size(*val);
        std::set<std::string> res;
        for(unsigned int i = 0; i < size; i++) {
            auto entry = neo4j_map_getentry(*val, i);
            std::string str;
            str.resize(neo4j_string_length(entry->key) + 1);
            neo4j_string_value(entry->key, const_cast<char*>(str.data()), str.size());
            str.resize(str.size() -1);
            res.insert(str);
        }
        return res;
    }

    value value::map_entry(const std::string& key) const
    {
        if(!is_map()) throw exception("not a map");
        struct neo4j_value kval = neo4j_string(key.c_str());
        auto res = neo4j_map_kget(*val, kval);
        return value(result, res);
    }

    long long value::node_id() const
    {
        if(!is_node()) throw exception("not a node");
        char buf[25];
        neo4j_tostring(neo4j_node_identity(*val), buf, 25);
        return std::stoll(buf);
    }

    std::set<std::string> value::node_labels() const
    {
        if(!is_node()) throw exception("not a node");
        std::set<std::string> res;
        auto labels = neo4j_node_labels(*val);
        unsigned int len = neo4j_list_length(labels);
        for(unsigned int i=0; i< len; i++) {
            auto label = neo4j_list_get(labels, i);
            std::string str;
            str.resize(neo4j_string_length(label) + 1);
            neo4j_string_value(label, const_cast<char*>(str.data()), str.size());
            str.resize(str.size() -1);
            res.insert(str);
        }
        return res;
    }

    std::map<std::string, value> value::node_properties() const
    {
        if(!is_node()) throw exception("not a node");
        auto props = neo4j_node_properties(*val);
        unsigned int size = neo4j_map_size(props);
        std::map<std::string, value> res;
        for(unsigned int i = 0; i < size; i++) {
            auto entry = neo4j_map_getentry(props, i);
            std::string str;
            str.resize(neo4j_string_length(entry->key) + 1);
            neo4j_string_value(entry->key, const_cast<char*>(str.data()), str.size());
            str.resize(str.size() -1);
            res.insert({str, value(result, entry->value)});
        }
        return res;
    }

    long long value::relationship_id() const
    {
        if(!is_relationship()) throw exception("not a relationship");
        char buf[25];
        neo4j_tostring(neo4j_relationship_identity(*val), buf, 25);
        return std::stoll(buf);
    }

    long long value::relationship_start_node_id() const
    {
        if(!is_relationship()) throw exception("not a relationship");
        char buf[25];
        auto id = neo4j_relationship_start_node_identity(*val);
        if(neo4j_type(id) == NEO4J_NULL) return 0;
        neo4j_tostring(id, buf, 25);
        return std::stoll(buf);
    }

    long long value::relationship_end_node_id() const
    {
        if(!is_relationship()) throw exception("not a relationship");
        char buf[25];
        auto id = neo4j_relationship_end_node_identity(*val);
        if(neo4j_type(id) == NEO4J_NULL) return 0;
        neo4j_tostring(id, buf, 25);
        return std::stoll(buf);
    }

    std::string value::relationship_type() const
    {
        if(!is_relationship()) throw exception("not a relationship");
        auto type = neo4j_relationship_type(*val);
        std::string str;
        str.resize(neo4j_string_length(type) + 1);
        neo4j_string_value(type, const_cast<char*>(str.data()), str.size());
        str.resize(str.size() -1);
        return str;
    }

    std::map<std::string, value> value::relationship_properties() const
    {
        if(!is_relationship()) throw exception("not a relationship");
        auto props = neo4j_relationship_properties(*val);
        unsigned int size = neo4j_map_size(props);
        std::map<std::string, value> res;
        for(unsigned int i = 0; i < size; i++) {
            auto entry = neo4j_map_getentry(props, i);
            std::string str;
            str.resize(neo4j_string_length(entry->key) + 1);
            neo4j_string_value(entry->key, const_cast<char*>(str.data()), str.size());
            str.resize(str.size() -1);
            res.insert({str, value(result, entry->value)});
        }
        return res;
    }

    unsigned int value::path_length() const
    {
        if(!is_path()) throw exception("not a path");
        return neo4j_path_length(*val);
    }

    value value::path_node(unsigned int hops) const
    {
        if(!is_path()) throw exception("not a path");
        return value(result, neo4j_path_get_node(*val, hops));
    }

    value value::path_relationship(unsigned int hops, bool& forward) const
    {
        if(!is_path()) throw exception("not a path");
        return value(result, neo4j_path_get_relationship(*val, hops, &forward));
    }

    value value::path_relationship(unsigned int hops) const
    {
        bool dummy;
        return path_relationship(hops, dummy);
    }

    std::string value::dump() const
    {
        switch(get_type()) {
            case value_type::type_null: return "null";
            case value_type::type_bool: return "bool(" + std::string(to_bool()?"true":"false");
            case value_type::type_int: return std::to_string(to_int());
            case value_type::type_float: return std::to_string(to_float());
            case value_type::type_string: return "\"" + to_string() + "\"";
            case value_type::type_bytes: return "bytes(len=" + std::to_string(neo4j_bytes_length(*val)) + ")";
            case value_type::type_list: {
                std::string res = "[";
                auto len = list_size();
                if(len != 0) res += "\n";
                for(unsigned int i=0; i<len; i++) {
                    res += list_entry(i).dump();
                    if(i != len - 1) res += ",\n";
                    else res += "\n";
                }
                res+= "]";
                return res;
            }
            case value_type::type_map: {
                std::string res = "{";
                auto map = to_map();
                if(!map.empty()) res += "\n";
                for(auto& e: map) {
                    res += e.first + ": " + e.second.dump() + "\n";
                }
                res += "}";
                return res;
            }
            case value_type::type_node: {
                std::string res = "node " + std::to_string(node_id()) + " ( ";
                for(auto& s: node_labels()) res += ":" + s + " ";
                res += ") {";
                auto props = node_properties();
                if(!props.empty()) res += "\n";
                for(auto& e: props) {
                    res += e.first + ": " + e.second.dump() + "\n";
                }
                res += "}";
                return res;
            }
            case value_type::type_relationship:{
                std::string res = "relationship (" +  std::to_string(relationship_start_node_id());
                res += ")--[" + std::to_string(relationship_id()) + ":" + relationship_type() + "]--(" + std::to_string(relationship_end_node_id()) + ") {";
                auto props = relationship_properties();
                if(!props.empty()) res += "\n";
                for(auto& e: props) {
                    res += e.first + ": " + e.second.dump() + "\n";
                }
                res += "}";
                return res;
            }
            case value_type::type_path: {
                auto len = path_length();
                std::string res = "path(" + std::to_string(len) + ") [\n";
                for(unsigned int i=0; i<len; i++) {
                    auto node = path_node(i);
                    res += node.dump() + "\n";
                    auto rel = path_relationship(i);
                    res += rel.dump() + "\n";
                }
                res += path_node(len).dump() + "\n]";
                return res;
            } 
            case value_type::type_identity: return "identity(" + std::to_string(to_identity()) + ")";
            default:
            case value_type::type_unknown: return "unknown";
        }
    }
}