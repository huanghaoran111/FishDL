#pragma once

#include <FishDL/model/utils/weight_buffer.h>
#include <FishDL/data/all.h>
#include <unordered_map>
#include <string>
#include <stdexcept>

namespace FishDL
{
    template<typename TElem, typename TDevice>
    class LoadBuffer
    {
    public:
        LoadBuffer() = default;

        // 给定张量的名称和张量的类别，尝试获取相应的张量
        // 能获取，则返回相应的张量指针，否则返回nullptr
        template<typename TCategory>
        const auto* TryGet(const std::string& name) const
        {
            using AimType = PrincipalDataType<TCategory, TElem, TDevice>;
            return m_weightBuffer.TryGet<AimType>(name);
        }

        // 将张量保存到LoadBuffer中
        template<typename TData>
        void Set(const std::string& name, const TData& data)
        {
            static_assert(std::is_same_v<typename TData::ElementType, TElem>);
            static_assert(std::is_same_v<typename TData::DeviceType, TDevice>);
            m_weightBuffer.Set(name, data);
        }

        // 清除当前LoadBuffer中的内容
        void Clear()
        {
            m_weightBuffer.Clear();
        }
        
        // 给定张量的名称与类别标签，返回一个bool值来表示该张量是否存在
        template<typename TParamCate>
        bool IsParamExist(const std::string& name) const
        {
            using AimType = PrincipalDataType<TParamCate, TElem, TDevice>;
            return m_weightBuffer.isExist<AimType>(name);
        }
    private:
        WeightBuffer m_weightBuffer;
    };
}