#pragma once

#include <atomic>

namespace FishDL
{
    namespace TypeIDAuxImpl
    {
        inline std::size_t GenTypeID()
        {
            static std::atomic<std::size_t> m_couter = 0;
            return m_couter.fetch_add(1);
        }
    }

    // 构造类型与整数值的映射
    template<typename T>
    std::size_t TypeID()
    {
        const static size_t id = TypeIDAuxImpl::GenTypeID();
        return id;
    }
}