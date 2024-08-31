#pragma once

#include <stack>
#include <string>
#include <type_traits>
#include <FishDL/utils/var_type_dict.h>
#include <FishDL/layers/utils/layer_in_map.h>
#include <FishDL/utils/cont_funcs/sequential.h>
#include <FishDL/data/utils/category_tags.h>
#include <FishDL/data/dynamic.h>

namespace FishDL
{
namespace LayerTraits
{
    template<typename TTypeMap, typename TKey, typename TCont>
    auto PickItemFromCont(TCont&& cont)
    {
        auto itemOri = std::forward<TCont>(cont).template Get<TKey>();
        static_assert(!std::is_same_v<decltype(itemOri), NullParameter>);

        using TAim = typename TTypeMap::template Find<TKey>;
        if constexpr (std::is_same_v<TAim, NullParameter>)
        {
            return itemOri;
        }
        else
        {
            if constexpr (!IsValidCategoryTag<TAim>)
            {
                return TAim(itemOri);
            }
            else if constexpr (IsDynamic<TAim>)
            {
                auto res = MakeDynamic(std::move(itemOri));
                static_assert(std::is_same_v<decltype(res), TAim>);
                return res;
            }
            else
            {
                static_assert(std::is_same_v<RemConstRef<decltype(itemOri)>, TAim>);
                return itemOri;
            }
        }
    }

    template<typename TStoreType, bool store>
    using LayerInternalBuf = std::conditional_t<store, std::stack<TStoreType>, NullParameter>;
    
    template <typename TData, typename TInput>
    auto Collapse(TInput&& p_input)
    {
        using AimCategory = DataCategory<TData>;
        if constexpr (IsValidCategoryTag<AimCategory>)
        {
            constexpr size_t AimDim = AimCategory::DimNum;
            constexpr size_t OriDim = DataCategory<TInput>::DimNum;
            static_assert(OriDim >= AimDim);
            if constexpr (OriDim == AimDim)
            {
                return std::forward<TInput>(p_input);
            }
            else
            {
                return ReduceSum<PolicyContainer<PModifyDimNumIs<OriDim - AimDim>>>(std::forward<TInput>(p_input));
            }
        }
        else
        {
            return NullParameter{};
        }
    }
    
    namespace ShapeCheckerImpl
    {
        template <typename TShape, bool bTrigger>
        class ShapeChecker_
        {
        public:
            template <typename TData>
            void PushDataShape(const TData&) {}
            
            template <typename TData>
            void CheckDataShape(const TData&) {}
            
            void AssertEmpty() const {}
            
            void Pop() {}
        };
        
        template <typename TShape>
        class ShapeChecker_<TShape, true>
        {
        public:
            template <typename TData>
            void PushDataShape(const TData& data)
            {
                m_buffer.push(data.Shape());
            }
            
            template <typename TData>
            void CheckDataShape(const TData& data)
            {
                if (m_buffer.empty())
                {
                    throw std::runtime_error("ShapeStack is empty, cannot check shape.");
                }
                if (!(data.Shape() == m_buffer.top()))
                {
                    throw std::runtime_error("Shape check fail.");
                }
            }
            
            void AssertEmpty() const
            {
                if (!m_buffer.empty())
                {
                    throw std::runtime_error("Shape checker is not empty.");
                }
            }
            
            void Pop()
            {
                if (m_buffer.empty())
                {
                    throw std::runtime_error("ShapeStack is empty, cannot check shape.");
                }
                m_buffer.pop();
            }
            
        private:
            std::stack<TShape> m_buffer;
        };

        template <typename TData, bool bTrigger>
        struct DataToShape_
        {
            using type = ShapeChecker_<void, false>;
        };

#ifdef FISHDL_CHECKSHAPE
        template <typename TData>
        struct DataToShape_<TData, true>
        {
            using type = ShapeChecker_<ShapeType<TData>, true>;
        };
#endif
    }

    template <typename TData, bool bTrigger>
    using ShapeChecker = typename ShapeCheckerImpl::DataToShape_<TData, bTrigger && (isValidCategoryTag<TData>)>::type;

    template <typename TShape, bool bTrigger>
    void PopoutFromStackHelper(ShapeCheckerImpl::ShapeChecker_<TShape, bTrigger>& stack)
    {
        stack.Pop();
    }

    template <typename... TDataStacks>
    void PopoutFromStack(TDataStacks&&... stacks)
    {
        (PopoutFromStackHelper(std::forward<TDataStacks>(stacks)), ...);
    }

    template <typename TShape, bool bTrigger>
    void CheckStackEmptyHelper(const ShapeCheckerImpl::ShapeChecker_<TShape, bTrigger>& stack)
    {
        stack.AssertEmpty();
    }

    template<typename... TDataStacks>
    void CheckStackEmpty(const TDataStacks&... stacks)
    {
        (CheckStackEmptyHelper(stacks),...);
    }
}
}