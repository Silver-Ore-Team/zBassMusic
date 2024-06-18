#pragma once

#include "NH/Commons.h"

#pragma warning(push, 1)
#include <cstdint>
#include <unordered_map>
#pragma warning(pop)

namespace NH
{
    namespace FNV1a
    {
        // FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit
        // str should be a null terminated string literal, value should be left out
        // e.g hash_32_fnv1a_const("example")
        // code license: public domain or equivalent
        // post: https://notes.underscorediscovery.com/constexpr-fnv1a/
        constexpr uint32_t val_32_const = 0x811c9dc5;
        constexpr uint32_t prime_32_const = 0x1000193;
        constexpr uint64_t val_64_const = 0xcbf29ce484222325;
        constexpr uint64_t prime_64_const = 0x100000001b3;

        inline constexpr uint32_t hash_32_fnv1a_const(const char* const str, const uint32_t value = val_32_const) noexcept
        {
            return (str[0] == '\0') ? value : hash_32_fnv1a_const(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
        }

        inline constexpr uint64_t hash_64_fnv1a_const(const char* const str, const uint64_t value = val_64_const) noexcept
        {
            return (str[0] == '\0') ? value : hash_64_fnv1a_const(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
        }
    }

    // HashString is a compile-time 64-bit FNV1a hash of a string literal.
    // It can be used as a key in hash maps to avoid runtime string comparisons.
    class HashString
    {
        uint64_t Id;
        const char* Value;

    public:
        constexpr HashString() noexcept : Id(0), Value(nullptr) {}
        constexpr explicit(false) HashString(const char* str) noexcept: Id(FNV1a::hash_64_fnv1a_const(str)), Value(str) {} // NOLINT(google-explicit-constructor)
        explicit(false) HashString(const String& str) noexcept: Id(FNV1a::hash_64_fnv1a_const(str)), Value(str.ToChar()) {} // NOLINT(google-explicit-constructor)

        [[nodiscard]] constexpr uint64_t GetHash() const { return Id; }

        [[nodiscard]] constexpr const char* GetValue() const { return Value; }

        constexpr explicit(false) operator uint64_t() const noexcept { return Id; } // NOLINT(google-explicit-constructor)
        constexpr explicit(false) operator const char*() const noexcept { return Value; } // NOLINT(google-explicit-constructor)
        explicit(false) operator String() const noexcept { return { Value }; } // NOLINT(google-explicit-constructor)

        constexpr bool operator==(const HashString& other) const noexcept { return Id == other.Id; }
    };

    constexpr HashString operator "" _hs(const char* str, size_t) noexcept
    {
        return { str };
    }
}

template<>
struct std::hash<NH::HashString>
{
    std::size_t operator()(const NH::HashString& k) const
    {
        return std::hash<uint64_t>()(static_cast<uint64_t>(k));
    }
};