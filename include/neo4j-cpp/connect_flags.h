#pragma once
#include <cstdint>

namespace neo4j {
    enum class connect_flags : uint32_t {
        none = 0,
        insecure = 1,
        no_uri_credentials = 2,
        no_uri_password = 4
    };

    inline connect_flags operator |(connect_flags lhs, connect_flags rhs)  
    {
        return static_cast<connect_flags> (
            static_cast<std::underlying_type<connect_flags>::type>(lhs) |
            static_cast<std::underlying_type<connect_flags>::type>(rhs)
        );
    }
    inline connect_flags operator &(connect_flags lhs, connect_flags rhs)  
    {
        return static_cast<connect_flags> (
            static_cast<std::underlying_type<connect_flags>::type>(lhs) &
            static_cast<std::underlying_type<connect_flags>::type>(rhs)
        );
    }

    inline connect_flags operator ^(connect_flags lhs, connect_flags rhs)  
    {
        return static_cast<connect_flags> (
            static_cast<std::underlying_type<connect_flags>::type>(lhs) ^
            static_cast<std::underlying_type<connect_flags>::type>(rhs)
        );
    }

    inline connect_flags operator ~(connect_flags rhs)  
    {
        return static_cast<connect_flags> (
            ~static_cast<std::underlying_type<connect_flags>::type>(rhs)
        );
    }

    inline connect_flags& operator |=(connect_flags &lhs, connect_flags rhs)  
    {
        lhs = static_cast<connect_flags> (
            static_cast<std::underlying_type<connect_flags>::type>(lhs) |
            static_cast<std::underlying_type<connect_flags>::type>(rhs)           
        );

        return lhs;
    }

    inline connect_flags& operator &=(connect_flags &lhs, connect_flags rhs)  
    {
        lhs = static_cast<connect_flags> (
            static_cast<std::underlying_type<connect_flags>::type>(lhs) &
            static_cast<std::underlying_type<connect_flags>::type>(rhs)           
        );

        return lhs;
    }

    inline connect_flags& operator ^=(connect_flags &lhs, connect_flags rhs)  
    {
        lhs = static_cast<connect_flags>(
            static_cast<std::underlying_type<connect_flags>::type>(lhs) ^
            static_cast<std::underlying_type<connect_flags>::type>(rhs)           
        );

        return lhs;
    }
}