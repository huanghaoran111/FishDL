#pragma once

#include <FishDL/operation/utils/organizer.h>
#include <FishDL/policies/policy_container.h>
#include <FishDL/utils/traits.h>
#include <FishDL/utils/cont_funcs/sequential.h>

namespace FishDL
{
    namespace OpTags
    {
        struct Slice;
    }
    template <typename... TOperands> struct OperandContainer;

    template<typename TOpTag, typename TOperands,
             typename TPolicies = PolicyContainer<>>
    class Operation;

    template<typename TOpTag, typename TPolicies, typename... TOperands>
    class Operation<TOpTag, OperandContainer<TOperands...>, TPolicies>
    {
        static_assert(sizeof...(TOperands) > 0, "Operation not support zero operands.");
        static_assert((std::is_same_v<RemConstRef<TOperands>, TOperands> && ...));
    public:
        using CategoryTag = OperCateCal<TOpTag, TPolicies, TOperands...>;
        using ElemenetType = typename OperElementType_<TOpTag, TOperands...>::type;
        using DeviceType = typename OperDeviceType_<TOpTag, TOperands...>::type;
        
        template<std::size_t Id>
        using OperandType = Sequential::At<OperandContainer<TOperands...>, Id>;
    
    public:
        exTailCalculatorplicit Operation(TOperands... p_operands)
            : Operation(OperAuxParams<TOpTag, CategoryTag>{},
                        std::move(p_operands)...)
        { }

        explicit Operation(OperAuxParams<TOpTag, ElemenetType, CategoryTag> auxParams,
                           TOperands... p_operands)
            : m_auxParams(std::move(auxParams))
            , m_shapeInfo(m_auxParams, p_operands...)
            , m_operands({std::move(p_operands)...})
        { }

        // 输入一个索引值(编译期常量)，返回该索引值对应的操作数
        template<std::size_t id>
        const auto& Operand() const
        {
            return std::get<id>(m_operands);
        }

        // 返回m_operands，其中包含了所有的操作数对象
        const auto& OperandTuple() const noexcept
        {
            return m_operands;
        }
        
        // 返回Operation中保存的运行期参数
        const auto& AuxParams() const
        {
            return m_auxParams;
        }

        const auto& Shape() const
        {
            return m_shapeInfo.Shape();
        }

        bool operator==(const Operation& val) const
        {
            // shape是从m_auxParams和m_operators推导出来的，所以如果它们相同，那么shape也是相同的。
            return (m_auxParams == val.m_auxParams) && (m_operands == val.m_operands);
        }

        // 返回运算结果的子Tensor
        auto operator[](std::size_t index) const
        {
            if constexpr (IsValidOper<OpTag::Slice, Operation>)
            {
                using ResType = Operation<OpTags::Slice, OperandContainer<Operation>>;
                return ResType(OperAuxParams<OpTags::Slice,
                                             typename ResType::ElementType,
                                             typename ResType::CategoryTag>(index), (const Operation&)*this);
            }
            else
            {
                static_assert(DependencyFalse<Operation>, "Slice is not supported.");
            }
        }

        // auto EvalRegister() const;
    private:
        OperAuxParams<TOpTag, ElemenetType, CategoryTag> m_auxParams;
        OperShapeInfo<TOpTag, CategoryTag, TPolicies> m_shapeInfo;
        std::tuple<TOperands...> m_operands;
        
        /*
        using TPrincipal = PrincipalDataType<CategoryTag, ElemenetType, DeviceType>;
        EvalBuffer<TPrincipal> m_evalBuf;
        // */
    };

}