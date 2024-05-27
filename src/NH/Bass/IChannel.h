#pragma once

#include <functional>
#include <expected>

namespace NH::Bass
{
    class AudioFile;

    struct IChannel
    {
        enum class ErrorType { INVALID_BUFFER };
        constexpr static const char* ErrorTypeStrings[] = { "INVALID_BUFFER" };

        class Error : public std::runtime_error
        {
            ErrorType Type;
            int32_t Code;
            String Details;
        public:
            Error(ErrorType type, int32_t code, const String& details)
                    : std::runtime_error(String::Format("{0} error: {1}, message: %s", ErrorTypeStrings[static_cast<int>(type)], code, details)),
                      Type(type), Code(code), Details(details) {};
            Error(ErrorType type, int32_t code, const String&& details)
                    : std::runtime_error(String::Format("{0} error: {1}, message: %s", ErrorTypeStrings[static_cast<int>(type)], code, details)),
                      Type(type), Code(code), Details(details) {};
            [[nodiscard]] ErrorType GetType() const { return Type; }
            [[nodiscard]] int32_t GetCode() const { return Code; }
            [[nodiscard]] const char* GetDetails() const { return Details; }
        };

        template<typename T>
        using Result = std::expected<T, Error>;

        virtual ~IChannel() = default;

        virtual Result<void> PlayInstant(const AudioFile& audioFile) = 0;
        virtual void StopInstant() = 0;

        virtual void SetVolume(float volume) = 0;
        virtual void SlideVolume(float targetVolume, uint32_t time) = 0;
        virtual void SlideVolume(float targetVolume, uint32_t time, const std::function<void()>& onFinish) = 0;
        virtual void SetDX8ReverbEffect(float reverbMix, float reverbTime, float inputGain = 0, float highFreqRTRatio = 0.001f) = 0;

        virtual void WhenAudioEnds(const std::function<void()>& onFinish) = 0;
        virtual void BeforeAudioEnds(double aheadSeconds, const std::function<void(double)>& onFinish) = 0;

        [[nodiscard]] virtual double Position() const = 0;
        [[nodiscard]] virtual double Length() const = 0;

        virtual void Acquire() = 0;
        virtual void Release() = 0;
        virtual bool IsAvailable() = 0;
    };
}
