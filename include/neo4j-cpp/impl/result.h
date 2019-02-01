#pragma once
#include <neo4j-client.h>
#include "../result.h"
#include "../value.h"

namespace neo4j {
    result::result()
        : res(nullptr)
    {}

    result::result(struct neo4j_result* r)
        : res(neo4j_retain(r))
    {}

    result::result(const result& other)
        : res(other.valid() ? neo4j_retain(other.res) : nullptr)
    {}

    result& result::operator=(const result& other)
    {
        if(valid()) neo4j_release(res);
        res = other.valid() ? neo4j_retain(other.res) : nullptr;
        return *this;
    }

    result::~result()
    {
        if(valid()) neo4j_release(res);
    }

    value result::field(unsigned int idx) const
    {
        return value(this->res, neo4j_result_field(res, idx));
    }
}