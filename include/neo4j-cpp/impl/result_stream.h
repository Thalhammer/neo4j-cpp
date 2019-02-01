#pragma once
#include <neo4j-client.h>
#include "../result_stream.h"
#include "../connection.h"
#include "../exception.h"
#include "../result.h"

namespace neo4j {
    result_stream::result_stream(std::shared_ptr<connection> c, bool results, const std::string& q)
        : con(c), query(q)
    {
        if(results) result = neo4j_run(con->con, query.c_str(), neo4j_null);
        else result = neo4j_send(con->con, query.c_str(), neo4j_null);
        if(result == nullptr) throw exception(neo4j_strerror(errno, nullptr, 0));
    }

    result_stream::~result_stream()
    {
        int res = neo4j_close_results(result);
        (void)res;
    }

    int result_stream::check_failure() const
    {
        return neo4j_check_failure(result);
    }

    std::string result_stream::error_code() const
    {
        auto ptr = neo4j_error_code(result);
        if(ptr == nullptr) return "";
        return ptr;
    }

    std::string result_stream::error_message() const
    {
        auto ptr = neo4j_error_message(result);
        if(ptr == nullptr) return "";
        return ptr;
    }

    struct failure_details result_stream::failure_details() const
    {
        auto ptr = neo4j_failure_details(result);
        if(ptr == nullptr) throw exception("No error occurred");
        struct failure_details res;
        res.code = ptr->code;
        res.column = ptr->column;
        if(ptr->context != nullptr) res.context = ptr->context;
        res.context_offset = ptr->context_offset;
        res.description = ptr->description;
        res.line = ptr->line;
        res.message = ptr->message;
        res.offset = ptr->offset;
        return res;
    }

    unsigned int result_stream::nfields() const
    {
        return neo4j_nfields(result);
    }

    std::string result_stream::fieldname(unsigned int index) const
    {
        return neo4j_fieldname(result, index);
    }

    statement_type result_stream::type() const
    {
        int res = neo4j_statement_type(result);
        if(res < 0) throw exception(neo4j_strerror(errno, nullptr, 0));
        switch(res) {
            case NEO4J_READ_ONLY_STATEMENT: return statement_type::read_only;
            case NEO4J_WRITE_ONLY_STATEMENT: return statement_type::write_only;
            case NEO4J_READ_WRITE_STATEMENT: return statement_type::read_write;
            case NEO4J_SCHEMA_UPDATE_STATEMENT: return statement_type::schema_update;
            case NEO4J_CONTROL_STATEMENT: return statement_type::control;
            default: throw exception("Unknown statement type");
        }
    }

    neo4j::result result_stream::fetch_next()
    {
        errno = 0;
        auto ptr = neo4j_fetch_next(result);
        if(ptr == nullptr) {
            if(errno != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
            else return neo4j::result();
        }
        return neo4j::result(ptr);
    }

    neo4j::result result_stream::peek(unsigned int depth)
    {
        errno = 0;
        auto ptr = neo4j_peek(result, depth);
        if(ptr == nullptr) {
            if(errno != 0) throw exception(neo4j_strerror(errno, nullptr, 0));
            else return neo4j::result();
        }
        return neo4j::result(ptr);
    }
}