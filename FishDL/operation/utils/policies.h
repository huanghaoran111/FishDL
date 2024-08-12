#pragma once

#include <FishDL/policies/policy_macro_begin.h>
#include <cinttypes>
namespace FishDL
{
    struct DimPolicies
    {
        using MajorClass = DimPolicies;

        struct ModifyDimNumValueCate;
        static constexpr std::size_t ModifyDimNum = 1;
    };
    ValuePolicyTemplate(PModifyDimNumIs, DimPolicies, ModifyDimNum);
    
}