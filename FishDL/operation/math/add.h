#pragma once

#include <FishDL/utils/all.h>
#include <FishDL/data/utils/traits.h>
#include <FishDL/operation/utils/all.h>

namespace FishDL
{
namespace OpTags
{
    struct Add;             // 张量相加
    struct AddWithNum;      // 使用数值加张量的情况
}

namespace OperAddWithNum
{
    template<typename TOp1, typename TOp2>
    constexpr bool Valid()
    {
        if constexpr (IsValidCategoryTag<DataCategory<TOp1>> && !IsValidCategoryTag<DataCategory<TOp2>>)
        {
            return std::is_constructible_v<typename RemConRef<TOp1>::ElementType, TOp2>;
        }
        else if constexpr (!isValidCategoryTag<DataCategory<TOp1>> && isValidCategoryTag<DataCategory<TOp2>>)
        {
            return std::is_constructible_v<typename RemConstRef<TOp2>::ElementType, TOp1>;
        }
        else
        {
            return false;
        }
    }
}

template<typename TOp1, typename TOp2>
constexpr bool IsValidOper<OpTags::AddWithNum, TOp1, TOp2> = OperAddWithNum::Valid<TOp1, TOp2>();


template <typename TElem, typename TCate>
struct OperAuxParams<OpTags::AddWithNum, TElem, TCate> : public OperAuxValue<TElem>
{
    using TBase = OperAuxValue<TElem>;
    using TBase::TBase;
    using TBase::operator==;
};

// interface
template<typename TP1, typename TP2,
         std::enable_if_t<IsValidOper<OpTags::Add, TP1, TP2> ||
                          IsValidOper<OpTags::AddWithNum, TP1, TP2>>* = nullptr>
auto operator+(TP1&& p_m1, TP2&& p_m2)
{
    if constexpr(IsValidOper<OpTags::Add, TP1, TP2>)
    {
        using rawOp1 = RemConstRef<TP1>;
        using rawOp2 = RemConstRef<TP2>;
        using ResType = Operation<OpTags::Add, OperandContainer<rawOp1, rawOp2>>;
        return ResType(std::forward<TP1>(p_m1), std::forward<TP2>(p_m2));
    }
    else if constexpr(IsValidOper<OpTags::AddWithNum, TP1, TP2>)
    {
        if constexpr(isValidCategoryTag<DataCategory<TP1>> && isValidCategoryTag<DataCategory<TP2>>)
        {
            using rawOp = RemConstRef<TP2>;
            using ResType = Operation<OpTags::AddWithNum, OperandContainer<rawOp>>;
            OperAuxParams<OpTags::AddWithNum, 
                          typename rawOp::ElementType, 
                          OperCateCal<OpTags::AddWithNum, PolicyContainer<>, rawOp>> params(p_m1);
            return ResType(std::move(params), std::forward<TP2>(p_m2));
        }
        else if constexpr(isValidCategoryTag<DataCategory<TP1>> && !isValidCategoryTag<DataCategory<TP2>>)
        {
            using rawOp = RemConstRef<TP1>;
            using ResType = Operation<OpTags::AddWithNum, OperandContainer<rawOp>>;
            OperAuxParams<OpTags::AddWithNum, typename rawOp::ElementType,
                          OperCateCal<OpTags::AddWithNum, PolicyContainer<>, rawOp>> params(p_m2);
            return ResType(std::move(params), std::forward<TP1>(p_m1));
        }
        else
        {
            static_assert(DependencyFalse<TP1, TP2>);
        }
    }
    else
    {
        static_assert(DependencyFalse<TP1, TP2>);
    }
}

}