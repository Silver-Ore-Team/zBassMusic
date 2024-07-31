#pragma once

#include "Logger.h"

namespace NH
{
    struct HasToString
    {
        virtual ~HasToString() = default;
        [[nodiscard]] virtual std::string ToString() const = 0;

        explicit(false) operator std::string() const noexcept { return ToString(); } // NOLINT(google-explicit-constructor)
    };
}
