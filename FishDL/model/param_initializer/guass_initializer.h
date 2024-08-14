#pragma once 

#include <random>
#include <stdexcept>
#include <type_traits>
#include <FishDL/model/param_initializer/utils/fill_with_spec_dist.h>
#include <FishDL/model/param_initializer/utils/policies.h>
#include <FishDL/policies/all.h>

namespace FishDL
{
    // 使用正态分布初始化张量信息
    template<typename TPolicyCont = PolicyContainer<>>
    class GuassianFiller
    {
        using TRandomEngine = typename PolicySelect<InitPolicy, TPolicyCont>::RandEngine;
    public:
        GuassianFiller(double p_mean, double p_std,
                       unsigned seed = std::random_device{}())
            : m_engine(seed)
            , m_mean(p_mean)
            , m_std(p_std)
        {}

        template<typename TData>
        void Fill(TData& data)
        {
            using ElementType = typename TData::ElementType;
            std::normal_distribution<ElementType> dist(static_cast<ElementType>(m_mean), static_cast<ElementType>(m_std));
            InitializerAuxImpl::FillWithDist(data, dist, m_engine);
        }
    private:
        TRandomEngine m_engine;
        double m_mean;
        double m_std;
    };
}