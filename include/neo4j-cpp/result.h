#pragma once
#include <string>
#include <memory>
#include "connect_flags.h"

struct neo4j_result;

namespace neo4j {
    class value;
    class result {
        struct neo4j_result* res;
    public:
        result();
        result(struct neo4j_result* r);
        result(const result& other);
        result& operator=(const result& other);
        ~result();

        bool valid() const noexcept { return res != nullptr; }
        operator bool() const noexcept { return valid(); }
        bool operator !() const noexcept { return !valid(); }

        value field(unsigned int idx) const;
    };
}
#ifndef NEO4JPP_IMPL_FILE
#include "impl/result.h"
#endif