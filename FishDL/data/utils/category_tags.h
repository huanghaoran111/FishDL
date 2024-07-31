#pragma once

#include "traits.h"

namespace FishDL
{
    namespace CategoryTags
    {
        struct OutOfCategory;

        template<std::size_t uDim>
        struct Tensor
        {
            constexpr staic size_t DimNum = uDim;
        };

        using Scalar = Tensor<0>;
        using Vector = Tensor<1>;
        using Matrix = Tensor<2>;
    }

    template<typename T>
    constexpr bool isValidCategoryTag = false;

    template<std::size_t uDim>
    constexpr bool isValidCategoryTag<CategoryTags::Tensor<uDim>> = true;

    template <typename TElem, typename TDevice, std::size_t uDim>
    struct PrincipalDataType_
    {
        using type = Tensor<TElem, TDevice, uDim>;
    };

    template <typename TCategory, typename TElem, typename TDevice>
    using PrincipalDataType = typename PrincipalDataType_<TElem, TDevice, TCategory::DimNum>::type;
    
    // 给定数据类型，可以返回对应的数据类型的类别
    template <typename T>
    struct DataCategory_
    {
        // 如果有这个类别则会进入第一个重载版本
        template <typename R>
        static typename R::CategoryTag Test(typename R::CategoryTag*);
        // 无类别则返回OutOfCategory
        template <typename R>
        static CategoryTags::OutOfCategory Test(...);

        using type = decltype(Test<RemConstRef<T>>(nullptr));
    };

    template <typename T>
    using DataCategory = typename DataCategory_<T>::type;

    template<typename T, std::size_t uDim>
    constexpr bool IsTensorWithDim = std::is_same_v<DataCategory<T>, CategoryTags::Tensor<uDim>>;

    #define HELPER(Name, uDim) template<typename T> constexpr bool Name = IsTensorWithDim<T, uDim>;
    HELPER(IsScalar, 0)
    HELPER(IsVector, 1)
    HELPER(IsMatrix, 2)
    HELPER(IsThreeDArray, 3) 
    #undef HELPER
}