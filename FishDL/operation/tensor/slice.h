#pragma once

#include <FishDL/data/utils/traits.h>
#include <FishDL/operation/utils/all.h>
#include <FishDL/utils/all.h>
#include <cassert>
#include <type_traits>

namespace FishDL
{
    namespace OpTags
    {
        struct Slice;
    }

    template<typename TPolicy, typename TOperand>
    struct OperCategory_<OpTags::Slice, TPolicy, TOperand>
    {
        using type = CategoryTags::Tensor<TOperand::DimNum - 1>;
    };

    template<typename TElem, typename TCate>
    struct OperAuxParams<OpTags::Slice, TElem, TCate>
    {
        OperAuxParams(std::size_t p_elemID)
            : m_elemID(p_elemID) { }
        
        const std::size_t m_elemID;

        bool operator==(const OperAuxParams& val) const
        {
            return (m_elemID == val.m_elemID);
        }
    };

    template<typename TCate, typename TPolicies>
    class OperShapeInfo<OpTags::Slice, TCate, TPolicies>
    {
    public:
        template<typename TOperAuxParams, typename TOperand>
        OperShapeInfo(const OperAuxParams&, const TOperand& operand)
        {
            static_assert(TCate::DimNim + 1 == DataCategory<TOperand>::DimNum);
            if constexpr (TCate::DimNum != 0)
            {
                for(std::size_t i = 0; i < TCate::DimNum; ++i)
                {
                    m_shape[i] = operand.Shape()[i + 1];
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
}