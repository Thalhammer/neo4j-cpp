#pragma once
#include <stdexcept>

namespace neo4j {
    class exception : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}