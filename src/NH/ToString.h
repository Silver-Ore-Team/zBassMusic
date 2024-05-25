#pragma once

#include "Logger.h"

namespace NH
{
    struct HasToString
    {
        [[nodiscard]] virtual String ToString() const = 0;

        explicit(false) operator String() const noexcept { return ToString(); } // NOLINT(google-explicit-constructor)
    };
}
