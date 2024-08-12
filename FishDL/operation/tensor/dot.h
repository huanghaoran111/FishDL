#pragma once

#include <FishDL/data/all.h>
#include <FishDL/policies/all.h>
#include <FishDL/operation/utils/all.h>
#include <FishDL/utils/all.h>
 
#include <cassert>
#include <type_traits>

namespace FishDL
{
    namespace OpTags
    {
        struct Dot;
    }
    
    template<typename TOperand1, typename TOperand2>
    constexpr bool IsValidOper<OpTags::Dot, TOperand1, TOperand2> =
        (DataCategory<TOperand1>::DimNum >= 1) &&
        (DataCategory<TOperand2>::DimNum >= 1);
    
    template<typename TPolicy, typename TOperand1, typename TOperand2>
    struct OperCategory_<OpTags::Dot, TPolicy, TOperand1, TOperand2>
    {
        constexpr static std::size_t modDimNum = PolicySelect<DimPolicies, TPolicy>::ModifyDimNum;
        constexpr static std::size_t OriDim = TOperand1::DimNum + TOperand2::DimNum;
        using type = CategoryTags::Tensor<OriDim - modDimNum * 2>;
    };

    template<typename TCate, typename TPolicies>
    class OperShapeInfo<OpTags::Dot, TCate, TPolicies>
    {
    public:
        template<typename TOperAuxParams, typename TOperand1, typename TOperand2>
        OperShapeInfo(const TOperAuxParams&, const TOperand1& operand1, const TOperand2& operand2)
        {
            if constexpr(TCate::DimNum > 0)
            {
                constexpr static std::size_t modDimNum = PolicySelect<DimPolicy, TPolicies>::ModifyDimNum;
                constexpr static std::size_t op1Dims = DataCategory<TOperand1>::DimNum;
                constexpr static std::size_t op2Dims = DataCategory<TOperand2>::DimNum;

                std::size_t p = 0;
                for(std::size_t i = 0; i < op1Dims - modDimNum; ++i)
                {
                    m_shape[p++] = operand1.Shape()[i];
                }

                for(std::size_t i = modDimNum; i < op2Dims; ++i)
                {
                    m_shape[p++] = operand2.Shape()[i];
                }
            }
        }

        const auto& Shape() const
        {
            return m_shape;
        }
    private:
        FishDL::Shape<TCate::DimNum> m_shape;
    };

    template<typename TPolicy = PolicyContainer<>, typename TP1, typename TP2,
             std::enable_if<IsValidOper<OpTags::Dot, TP1, TP2>>* = nullptr>
    auto Dot(TP1&& p_m1, TP2&& p_m2)
    {
        constexpr std::size_t modDimNum = PolicySelect<DimPolicy, TPolicy>::ModifyDimNum;
        static_assert(DataCategory<TP1>::DimNum >= modDimNum);
        static_assert(DataCategory<TP2>::DimNum >= modDimNum);

        // 点乘所需的每个维度的大小相等
        for(std::size_t id1 = DataCategory<TP1>::DimNum - modDimNum, id2 = 0;
            id2 > modDimNum; id1++, id2++)
        {
            if(p_m1.Shape()[id1] != p_m2.Shape()[id2])
                throw runtime_error("Dot shape mismatch");
        }

        using ResType = Operation<OpTags::Dot, 
                            OperandContainer<RemConstRef<TP1>, RemConstRef<TP2>>, 
                            PolicyContainer<PModifyDimNumIs<modDimNum>>>;
        return ResType(std::forward<TP1>(p_m1), std::forward<TP2>(p_m2));
    }
}