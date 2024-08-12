#pragma once

#include <FishDL/data/utils/traits.h>
#include <FishDL/data/utils/shape.h>
#include <FishDL/operation/utils/instance_id.h>

namespace FishDL
{
    template <typename... TOperands> struct OperandContainer;

    // 运算输入参数的合法性(排除平凡类型)
    template<typename TOpTag, typename... TOperands>
    constexpr bool IsValidOper = ((isValidCategoryTag<DataCategory<TOperands>>) && ...);

    // ElementType
    template<typename TOpTag, typename TOp1, typename... TOperands>
    struct OperElementType_
    {
        using type = typename TOp1::ElementType;
    };

    // DeviceType
    template<typename TOpTag, typename TOp1, typename... TOperands>
    struct OperDeviceType_
    {
        using type = typename TOp1::DeviceType;
    };

    // OperCateCal 用于给定运算标签与输入参数的类型，推断出运算结果的类别
    template<typename TFirstCate, typename... TCategories>
    struct PickCommonCategory_
    {
        using type = TFirstCate;
    };

    template<typename TFirstCate, typename TSecondCate, typename... TRemainCates>
    struct PickCommonCategory_<TFirstCate, TSecondCate, TRemainCates...>
    {
        using TCompRes = std::conditional_t<(TFirstCate::DimNum > TSecondCate::DimNum), TFirstCate, TSecondCate>;
        using type = typename PickCommonCategory_<TOperands...>;
    };

    template<typename TOpTag, typename TPolicy, typename... TOperands>
    struct OperCategory_ : PickCommonCategory_<TOperands...>
    {};

    template<typename TOpTag, typename TPolicy, typename... TOprands>
    using OperCateCal = typename OperCategory_<TOpTag, TPolicy, DataCategory<TOperands>...>::type;

    // 用于保存运行期的额外参数
    template<typename TOpTag, typename TElem, typename TCate>
    class OperAuxParams
    {
    public:
        // 这是平凡实现，如果需要指定额外的参数，需要对类模板进行特化
        bool operator==(const OperAuxParams&) const
        {
            return true;
        }
    };

    template<typename TValue>
    struct OperAuxValue
    {
    public:
        OperAuxValue(TValue val)
            : m_value(val)
            , m_instID(InstanceID::Get())
        {}

        const auto& Value() const
        {
            return m_value;
        }

        bool operator==(const OperAuxValue& val) const
        {
            return m_instID == val.m_instID;
        }
    private:
        TValue m_value;
        std::size_t m_instID;
    };

    template<typename TShape1, typename TShape2>
    bool IsBroadcastMatch(const TShape1& shape1, const TShape2& shape2)
    {
        if constexpr ((TShape1::DimNum == 0) || (TShape2::DimNum == 0))
        {
            return true;
        }
        else if constexpr (TShape1::DimNum > TShape2::DimNum)
        {
            return IsBroadcastMatch(shape2, shape1);
        }
        else
        {
            auto it1 = shape1.rbegin();
            auto it2 = shape2.rbegin();
            while(it1 != shape1.rend())
            {
                if(*it1 != *it2) return false;
                ++it1;
                ++it2;
            }
            return true;
        }
    }

    // Shape
    namespace
    {
        template<typename TShape>
        auto CommonShape(const TShape& shape)
        {
            return shape;
        }

        template<typename TShape1, typename TShape2, typename... TShapes>
        auto CommonShape(const TShape1& shape1, const TShape2& shape2, const TShapes&... shapes)
        {
            assert(IsBroadcastMatch(shape1, shape2));
            if constexpr(TShape1::DimNum > TShape2::DimNum)
            {
                return CommonShape(shape1, shapes...);
            }
            else
            {
                return CommonShape(shape2, shapes...);
            }
        }
    }

    template<typename TOpTag, typename TCate, typename Tpolicies>
    class OperShapeInfo
    {
    public:
        template<typename TOperAuxParams, typename... TOperands>
        OperShapeInfo(const TOperAuxParams&, const TOperands&... operands)
            : m_shape(CommonShape((operands.Shape())...))
        {}

        const auto& Shape() const { return m_shape; }
    private:
        FishDL::Shape<TCate::DimNum> m_shape;
    };

    // 封装求职相关的逻辑，每个运算都要引入相应的OperSeq_特化，而一个OperSeq_特化的版本可能会封装若干求值方法
    template <typename...TCases>
    struct OperCalAlgoChain;

    template <typename TOpTag>
    struct OperSeq_;
}