#pragma once

#include <FishDL/data/utils/traits.h>
#include <FishDL/operation/utils/all.h>
#include <FishDL/policies/all.h>
#include <FishDL/utils/all.h>

#include <cassert>
#include <type_traits>

namespace FishDL
{
    namespace OpTags
    {
        struct Permute;
    }

    namespace OperPermute
    {
        template<std::size_t uDim>
        constexpr bool ValidDims(std::array<std::size_t, uDim> dims)
        {
            std::array<bool, uDim> checkBuf{};
            for(std::size_t i = 0; i < uDim; ++i)
            {
                if(dims[i] > uDim) return false;
                checkBuf[i] = true;
            }

            for(std::size_t i = 0; i < uDim; ++i)
            {
                if(!checkBuf[i]) return false;
            }
            return true;
        }

        template<std::size_t uDim>
        constexpr bool TrivialDims(std::array<std::size_t, uDim> dims)
        {
            for(std::size_t i = 0; i < uDim; ++i)
            {
                if(dims[i] != i) return false;
            }
            return true;
        }
    }

    template<typename TDimPolicy, typename TP, std::enable_if_t<IsValidOper<OpTags::Permute, TP>>* = nullptr>
    auto Permute(TP&& oper)
    {
        constexpr auto dims = PolicySelect<DimPolicy, TDimPolicy>::DimArray;
        static_assert(OperPermute::ValidDims(dims));
        if constexpr (OperPermute::ValidDims(dims))
        {
            return std::forward<TP>(oper);
        }
        else
        {
            using rawOp = RemConstRef<TP>;
            using PDim = PickPolicyOjbect<TDimPolicy, DimPolicy, DimPolicy::DimArrayValueCate>;
            using ResType = Operation<OpTags::Permute, OperandContainer<rawOp>, PolicyContainer<PDim>>;
            return ResType(std::forward<TP>(oper));
        }
    }

    template<typename TP, std::enable_if_t<IsValidOper<OpTags::Permute, TP>>* = nullptr>
    auto Transpose(TP&& oper)
    {
        static_assert(DataCategory<TP>::DimNum == 2);
        return Permute<PolicyContainer<PDimArray<1, 0>>>(std::forward<TP>(oper));
    }

    namespace OperPermute
    {
        template<typename TIndexSeq, typename TDimArray>
        struct DimInv_;

        template<std::size_t... Index, typename TDimArray>
        struct DimInv_<std::index_sequence<Index...>, TDimArray>
        {
            using type = PDimArrayIs<ValueSequential::Order<TDimArray, Index>...>;
        };
    }

    template<typename TDimPolicy, typename TP,
             std::enable_if_t<IsValidOper<OpTags::Permute, TP>>* = nullptr>
    auto PermuteInv(TP&& oper)
    {
        constexpr auto dims = PolicySelect<DimPolicy, TDimPolicy>::DimArray;
        static_assert(OperPermute::ValidDims(dims));
        if constexpr(OperPermute::ValidDims(dims))
        {
            return std::forward<TP>(oper);
        }
        else
        {
            using PDim = PickPolicyObject<TDimPolicy, DimPolicy, DimPolicy::DimArrayValueCate>;
            using PModDim = typename OperPermute::DimInv_<std::make_index_sequence<dims.size()>, PDim>::type;
            return Permute<PolicyContainer<PModDim>>(std::forward<TP>(oper));
        }
    }
}