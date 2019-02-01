#pragma once
#include <string>
#include <memory>
#include "connect_flags.h"

struct neo4j_result_stream;

namespace neo4j {
    class connection;
    class result;
    struct failure_details {
        std::string code;
        std::string message;
        std::string description;
        unsigned int line;
        unsigned int column;
        unsigned int offset;
        std::string context;
        unsigned int context_offset;
    };
    enum class statement_type {
        read_only,
        write_only,
        read_write,
        schema_update,
        control
    };
    class result_stream : public std::enable_shared_from_this<result_stream> {
        std::shared_ptr<connection> con;
        struct neo4j_result_stream* result;
        std::string query;
    public:
        result_stream(std::shared_ptr<connection> con, bool results, const std::string& query);
        ~result_stream();

        result_stream(const result_stream&) = delete;
        result_stream& operator=(const result_stream&) = delete;

        int check_failure() const;
        std::string error_code() const;
        std::string error_message() const;
        struct failure_details failure_details() const;

        unsigned int nfields() const;
        std::string fieldname(unsigned int index) const;

        statement_type type() const;
        // update_counts() const;
        // statement_plan() const;
        neo4j::result fetch_next();
        neo4j::result peek(unsigned int depth = 1);
    };
}
#ifndef NEO4JPP_IMPL_FILE
#include "impl/result_stream.h"
#endif