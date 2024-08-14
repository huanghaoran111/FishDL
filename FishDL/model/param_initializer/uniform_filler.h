#pragma once

#include <random>
#include <stdexcept>
#include <type_traits>
#include <FishDL/model/param_initializer/utils/fill_with_spec_dist.h>
#include <FishDL/model/param_initializer/utils/policies.h>
#include <FishDL/policies/all.h>

namespace FishDL
{
    // 使用均匀分布初始化张量参数
    template<typename TPolicyCont = PolicyContainer<>>
    class UniformFiller
    {
        using TRandomEngine = typename PolicySelect<InitPolicy, TPolicyCont>::RandEngine;
    public:
        UniformFiller(double min, double max,
                     unsigned seed = std::random_device{}())
            : m_engine(seed)
            , m_min(min)
            , m_max(max)
        {
            if(min >= max)
            {
                throw std::runtime_error("min is larger or equal to max");
            }
        }

        template<typename TData>
        void Fill(TData& data)
        {
            using ElementType = typename TData::ElementType;
            using DistType = std::conditional_t<std::is_integral<ElementType>::value,
                                                std::uniform_int_distribution<ElementType>,
                                                std::uniform_real_distribution<ElementType>>;
            DistType dist(static_cast<ElementType>(m_min), static_cast<ElementType>(m_max));
            InitializerAuxImpl::FillWithDist(data, dist, m_engine);
        }
    
    private:
        TRandomEngine m_engine;
        double m_min;
        double m_max;
    };
}