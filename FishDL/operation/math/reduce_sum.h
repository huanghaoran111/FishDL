#pragma once


#include <FishDL/data/all.h>
#include <FishDL/operation/utils/all.h>
#include <FishDL/policies/all.h>
#include <FishDL/utils/cont_funcs/value_sequential.h>
#include <numeric>

namespace FishDL
{
namespace OpTags
{
    struct ReduceSum;
}

    template<typename TPolicy = PolicyContainer<>, typename TP,
             std::enable_if_t<IsValidOper<OpTags::ReduceSum, TP>>* = nullptr>
    auto ReduceSum(TP&& oper)
    {
        constexpr bool HasDimArray = HasNonTrivialPolicy<TPolicy, DimPolicy, DimPolicy::DimArrayValueCate>;
        constexpr bool HasModDimNum = HasNonTrivialPolicy<TPolicy, DimPolicy, DimPolicy::ModifyDimNumValueCate>;

        static_assert((int)HasDimArray + (int)HasModDimNum <= 1, "Only one of DimArray or ResDimValue could be set");

        // using TDimBits = typename CompileTimeSwitch<std::integer_sequence<bool, HasDimArray, HasModDimNum>,
        //                                             std::tuple<OperRed>>
        return 1;
    }
}