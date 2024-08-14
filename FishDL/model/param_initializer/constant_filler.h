#pragma once

#include <random>
#include <stdexcept>
#include <type_traits>
#include <FishDL/data/all.h>

namespace FishDL
{
namespace ConstantFillerAuxImpl
{
    template<typename TData>
    void Fill(TData& data, const double& val)
    {
        if(!data.AvailableForWrite())
        {
            throw std::runtime_error("Data is sharing weight, cannot fill-in.");
        }

        static_assert(std::is_same_v<typename TData::DeviceType, DeviceTags::CPU>);
        auto mem = LowerAccess(data);
        auto r = mem.MutableRawMemory();

        using ElementType = typename TData::ElementType;
        for(std::size_t i = 0; i < data.Shape().Count(); ++i)
        {
            r[i] = static_cast<ElementType>(val);
        }
    }
}

    // 将张量的内容初始化为某个常量
    class ConstantFiller
    {
    public:
        ConstantFiller(double val = 0)
            : m_value(val)
        {}

        template<typename TData>
        void Fill(TData& data) const
        {
            ConstantFillerAuxImpl::Fill(data, m_value);
        }
    private:
        double m_value;
    };
}   