#pragma once

#include <type_traits>

namespace FishDL
{
namespace LayerInterfaceImpl
{
    template <typename L, typename TInitializer, typename TBuffer>
    std::true_type InitTest(decltype(&L::template Init<TInitializer, TBuffer>));

    template <typename L, typename TInitializer, typename TBuffer>
    std::false_type InitTest(...);

    template <typename L, typename TGradCollector>
    std::true_type GradCollectTest(decltype(&L::template GradCollect<TGradCollector>));

    template <typename L, typename TGradCollector>
    std::false_type GradCollectTest(...);

    template <typename L, typename TSave>
    std::true_type SaveWeightsTest(decltype(&L::template SaveWeights<TSave>));

    template <typename L, typename TSave>
    std::false_type SaveWeightsTest(...);

    template <typename L>
    std::true_type NeutralInvariantTest(decltype(&L::NeutralInvariant));

    template <typename L>
    std::false_type NeutralInvariantTest(...);
}
    // 正向传播
    template<typename TLayer, typename TIn>
    auto LayerFeedForward(TLayer& layer, TIn&& p_in)
    {
        return layer.FeedForward(std::forward<TIn>(p_in));
    }

    // 反向传播
    template<typename TLayer, typename TGrad>
    auto LayerFeedBackward(TLayer& layer, TGrad&& p_grad)
    {
        return layer.FeedBackward(std::forward<TGrad>(p_grad));
    }

    // 参数初始化与加载
    template<typename TLayer, typename TInitializer, typename TBuffer>
    void LayerInit(TLayer& layer, TInitializer& initializer, TBuffer& loadBuffer)
    {
        if constexpr(decltype(LayerInterfaceImpl::InitTest<TLayer, TInitializer, TBuffer>(nullptr))::value)
            layer.template Init<TInitializer, TBuffer>(initializer, loadBuffer);
    }

    // 收集参数梯度
    template<typename TLayer, typename TGradCollector>
    void LayerGradCollect(const TLayer& layer, TGradCollector& gc)
    {
        if constexpr(decltype(LayerInterfaceImpl::GradCollectTest<TLayer, TGradCollector>(nullptr))::value)
            layer.GradCollect(gc);
    }

    // 保存参数
    template<typename TLayer, typename TSave>
    void LayerSaveWeights(const TLayer& layer, TSave& saver)
    {
        if constexpr(decltype(LayerInterfaceImpl::SaveWeightsTest<TLayer, TSave>(nullptr))::value)
            layer.SaveWeights(saver);
    }

    // 中性检测
    template<typename TLayer>
    void LayerNeutralInvariant(TLayer& layer)
    {
        if constexpr(decltype(LayerInterfaceImpl::NeutralInvariantTest<TLayer>(nullptr))::value)
            layer.NeutralInvariant();
    }
}