#pragma once

#include <atomic>

namespace FishDL
{
    class InstanceID
    {
    public:
        InstanceID() = delete;
        static std::size_t Get()
        {
            return m_counter.fetch_add(1);
        }
    private:
        inline static std::atomic<std::size_t> m_counter = 0;
    };
}
