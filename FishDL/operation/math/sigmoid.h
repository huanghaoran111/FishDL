#pragma once

#include <FishDL/data/utils/traits.h>
#include <FishDL/operation/utils/operation_frame.h>

namespace FishDL
{
namespace OpTags
{
    struct Sigmoid;
    struct SigmoidGrad;
}


    // Sigmoid 计算模块
    // 如果传入诸如double类型，编译会出现错误
    template<typename TP, std::enable_if_t<IsValidOper<OpTags::Sigmoid, TP>>* = nullptr>
    auto Sigmoid(TP&& p_m)
    {
        using rawM = RemConstRef<TP>;
        using ResType = Operation<OpTags::Sigmoid, OperandContainer<rawM>>;
        return ResType(std::forward<TP>(p_m));
    }


    // Sigmoid grad(梯度) 计算模块
    template<typename TGrad, typename TInput,
             std::enable_if_t<IsValidOper<OpTags::SigmoidGrad, TGrad, TInput>>* = nullptr>
    auto SigmoidGrad(TGrad&& p_grad, TInput&& p_input)
    {
        static_assert(DataCategory<TInput>::DimNum >= DataCategory<TGrad>::DimNum);

        using rawOp1 = RemConstRef<TGrad>;
        using rawOp2 = RemConstRef<TInput>;
        using ResType = Operation<OpTags::SigmoidGrad, OperandContainer<rawOp1, rawOp2>>;
        return ResType(std::forward<TGrad>(p_grad), std::forward<TInput>(p_input));
    }
}