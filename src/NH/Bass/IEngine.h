#pragma once

#include "NH/Bass/IChannel.h"

namespace NH::Bass
{
    class EventManager;
    class MusicManager;
    class CommandQueue;

    struct IEngine
    {
        virtual ~IEngine() = default;

        /**
         * Get a free channel and mark it as busy
         * @return shared_ptr to a free channel (or null if all channels are busy)
         */
        virtual std::shared_ptr<IChannel> AcquireFreeChannel() = 0;

        /**
         * Release a channel and mark it as free
         * @param channel
         */
        virtual void ReleaseChannel(const std::shared_ptr<IChannel>& channel) = 0;

        virtual EventManager& GetEM() = 0;
        virtual MusicManager& GetMusicManager() = 0;
        virtual CommandQueue& GetCommandQueue() = 0;
    };
}
