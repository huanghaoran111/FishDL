#pragma once

#include <FishDL/policies/policy_macro_begin.h>
#include <cinttypes>
namespace FishDL
{
    struct DimPolicy
    {
        using MajorClass = DimPolicy;

        struct DimArrayValueCate;

        struct ModifyDimNumValueCate;
        static constexpr std::size_t ModifyDimNum = 1;
    };
    ValuePolicyTemplate(PModifyDimNumIs, DimPolicy, ModifyDimNum);
    

    template<std::size_t uDims>
    struct PDimArrayIs : virtual public DimPolicy
    {
        using MinorClass = DimPolicy::DimArrayValueCate;
        static constexpr std::array<std::size_t, sizeof...(uDims)> DimArray{uDims...};
    };
}
#include <FishDL/policies/policy_macro_end.h>