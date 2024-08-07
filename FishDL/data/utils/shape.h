#include "traits.h"
#include <FishDL/utils/helper.h>
#include <FishDL/data/utils/category_tags.h>

#include <cinttypes>
#include <array>
#include <cassert>
#include <stdexcept>
#include <algorithm>

namespace FishDL
{
    namespace
    {
        template<std::size_t uDimNum,
                 typename TCurIndexType, typename... TRemainIndexType>
        std::size_t IndexToOffset(const std::array<std::size_t, uDimNum>& dims,
                                  std::size_t& gap, TCurIndexType curIdx,
                                  TRemainIndexType... remIndex)
        {
            constexpr std::size_t indexPos = uDimNum - sizeof...(TRemainIndexType) - 1;
            if constexpr(sizeof...(TRemainIndexType) == 0)
            {
                return static_cast<std::size_t>(curIdx);
            }
            else
            {
                std::size_t curGap = 1;
                std::size_t res = IndexToOffset(dims, curGap, remIdx...);
                gap = curGap * dims[indexPos + 1];
                res += static_cast<std::size_t>(curIdx) * gap;
                return res;
            }
        }

        template<std::size_t uDimNum, int... N>
        std::size_t IndexToOffset(const std::array<std::size_t, uDimNum>& dims,
                                  const std::array<std::size_t, uDimNum>& indexes,
                                  Helper::IndexSequence<N...>*)
        {
            std::size_t gap = 1;
            return IndexToOffset(dims, gap, std::get<N>(indexes)...);
        }

        template<std::size_t ID, typename TShape>
        void FillShape(TShape& pShape)
        {
            return ;
        }

        template<std::size_t ID, typename TShape, typename TCurParam, typename... TShapeParameter>
        void FillShape(TShape& pShape, TCurParam curParam, TShapeParameter... shapes)
        {
            pShape[ID] = static_cast<std::size_t>(curParam);
            FillShape<ID + 1>(pShape, shapes...);
        }

    }
    template<std::size_t uDimNum>
    class Shape
    {
        static_assert(uDimNum > 0);
    public:
        constexpr static std::size_t DimNum = uDimNum;
        explicit Shape() = default;

        template <typename... TIntTypes,
                  std::enable_if_t<(std::is_convertible_v<TIntTypes, size_t> && ...)>* = nullptr>
        explicit Shape(TIntTypes... shapes)
        {
            static_assert(sizeof...(TIntTypes) == uDimNum);
            FillShape<0>(m_dims, shapes...);
        }
        
        template<typename... TIntTypes,
                enable_if_t<(std::is_convertible_v<TIntTypes, std::size_t> && ...)>* = nullptr>
        std::size_t IndexToOffset(TIntTypes... indexes) const
        {
            std::size_t gap = 1;
            return IndexToOffset(m_dims, gap, indexes...);
        }
        std::size_t IndexToOffset(const array<std::size_t, DimNum>& indexes) const
        {
            using TSeq = Helper::MakeIndexSequence<DimNum>;
            return IndexToOffset(m_dims, indexes, (TSeq*)nullptr);
        }

        std::array<std::size_t, DimNum> OffsetToIndex(std::size_t offset) const
        {
            std::array<std::size_t, DimNum> res;
            for(int i = (int)DimNum - 1; i >= 0; i--)
            {
                res[i] = offset % m_dims[i];
                offset = (offset - res[i]) / m_dims[i];
            }
            if(offset != 0)
            {
                throw std::runtime_error("Offset out of bound");
            }
            return res;
        }

        void ShiftIndex(array<std::size_t, DimNum>& indexes, int carry = 1) const
        {
            if(carry == 0) return;
            if(carry > 0)
            {
                size_t uCarry = (std::size_t)carry;
                for(int i = (int)DimNum - 1; i >= 0; --i)
                {
                    indexes[i] += uCarry;
                    uCarry = indexes[i] / m_dims[i];
                    indexes[i] %= m_dims[i];
                    if(uCarry == 0) return;
                }
                if(uCarry)
                {
                    throw std::runtime_error("Overflow");
                }
            }
            else
            {
                throw std::runtime_error("Not implemented yet.");
            }
        }

        std::size_t Count() const
        {
            return std::accumulate(std::begin(m_dims), std::end(m_dims), static_cast<size_t>(1), std::multiplies<>());
        }
        std::size_t operator[](std::size_t idx) const
        {
            assert(idx < DimNum)
            return m_dims[idx];
        }
        std::size_t& operator[](std::size_t idx)
        {
            assert(idx < DimNum);
            return m_dims[idx];
        }
        decltype(auto) begin()
        {
            return m_dims.begin();
        }
        decltype(auto) begin() const
        {
            return m_dims.begin();
        }
        decltype(auto) end()
        {
            return m_dims.end();
        }
        decltype(auto) end() const
        {
            return m_dims.end();
        }
        decltype(auto) rbegin()
        {
            return m_dims.rbegin();
        }
        decltype(auto) rbegin() const
        {
            return m_dims.rbegin();
        }
        decltype(auto) rend()
        {
            return m_dims.rend();
        }
        decltype(auto) rend() const
        {
            return m_dims.rend();
        }

        bool operator==(const Shape& val) const
        {
            return m_dims == val.m_dims;
        }
        template<std::size_t vDimNum>
        bool operator==(const Shape<vDimNum>&) const
        {
            return false;
        }
    private:
        std::array<std::size_t, uDimNum> m_dims{};
    };

    template<>
    class Shape<0>
    {
    public:
        constexpr static std::size_t DimNum = 0;

        explicit Shape() = default;
        bool operator==(const Shape& val) const
        {
            return true;
        }

        template<std::size_t vDimNum>
        bool operator==(const Shape<vDimNum>& const)
        {
            return false;
        }

        std::size_t Count() const
        {
            return 1;
        }

        decltype(auto) size() const
        {
            return 0;
        }
    };

    template<std::size_t v1, std::size_t v2>
    bool operator!=(const Shape<v1> val1, const Shape<v2> val2)
    {
        return !(val1 == val2);
    }
    template<typename... TShapeParameter,
             std::enable_if_t<(std::is_convertible_v<TShapeParameter, std::size_t> && ...)>* = nullptr>
    explicit Shape(TShapeParameter...) -> Shape<sizeof...(TShapeParameter)>;
}
