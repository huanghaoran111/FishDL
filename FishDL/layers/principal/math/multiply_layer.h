#pragma once

#include <FishDL/layers/utils/policies.h>
#include <FishDL/layers/utils/traits.h>
#include <FishDL/policies/all.h>

namespace FishDL
{
    template<typename TInputs, typename TPolicies>
    class MultiplyLayer
    {
        static_assert(IsPolicyContainer<TPolicies>);
        using CurLayerPolicy = PlainPolicy<TPolicies>;
    
    public:
        static constexpr bool IsFeedbackOutput = PolicySelect<GradPolicy, CurLayerPolicy>::IsFeedbackOutput;
        static constexpr bool IsUpdate = false;

        using InputPortSet = LayerPortSet<struct LeftOperand, struct RightOperand>;
        using OutputPortSet = LayerPortSet<struct LayerOutput>;
        using InputMap = typename std::conditional_t<std::is_same_v<TInputs, NullParameter>,
                                                     >
    };
}
