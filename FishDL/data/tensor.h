#pragma once

#include <FishDL/data/utils/shape.h>
#include <FishDL/data/utils/traits.h>
#include <FishDL/data/utils/continuous_memory.h>
#include <FishDL/data/utils/device.h>
#include <FishDL/data/utils/lower_access.h>

namespace FishDL
{
    namespace
    {
        template<typename TShape, typename TCurIndex, typename TNextParam, typename... TRemainParam>
        auto OffsetAndVal(const TShape& shape, std::size_t& gap, TCurIndex curIdx, TNextParam nextParam, TRemainParam... remPara)
        {
            constexpr std::size_t uDimNum = TShape::DimNum;
            constexpr std::size_t indexPos = uDimNum - sizeof...(TRemainParam) - 1;
            if(static_cast<std::size_t>(curIdx) >= shape[indexPos])
                throw std::runtime_error("Invalid dimension index.");
            
            if constexpr(sizeof...(TRemainParam) == 0)
            {
                return std::pair(static_cast<std::size_t>(curIdx), nextParam);
            }
            else
            {
                std::size_t curGap = 1;
                auto [pos, val] = OffsetAndVal(shape, curGap, nextParam, remPara...);
                gap = curGap * shape[indexPos + 1];
                pos += static_cast<std::size_t>(curIdx) * gap;
                return std::pair(pos, val);
            }
        }
    }

    template<typename TElem, typename TDevice, std::size_t uDim>
    class Tensor
    {
        static_cast(std::is_same_v<RemConstRef<TElem>, TElem>);
    public:
        using CategoryTag = CategoryTags::Tensor<uDim>;
        using ElementType = TElem;
        using DeviceType = TDevice;

        friend struct LowerAccessImpl<Tensor>;

    private:
        bool AvailableForWrite() const { return !m_mem.IsShared(); }

    public:
        template<typename... TShapeParameter>
        explicit Tensor(TShapeParameter... shapes)
            : m_shape(shapes...)
            , m_mem(m_shape.Count()) { }
        
        explicit Tensor(ContinuousMemory<ElementType, DeviceTag> p_mem,
                        FishDL::Shape<uDim> p_shape)
            : m_shape(std::move(p_shape))
            , m_mem(std::move(p_mem)) { }

        const auto& Shape() const noexcept
        {
            return m_shape;
        }

        bool operator==(const Tensor& val) const
        {
            return (m_shape == val.m_shape) &&
                   (m_mem == val.m_mem);
        }

        // 求值注册
        auto EvalRegister() const
        {
            // return MakeConstEvalHandle(*this);
        }

        template<typename... TPosValParams>
        void SetValue(TPosValParams... posValParams)
        {
            // 留出一位来保证是设置的值，即 张量位置+值
            static_assert(uDim + 1 == sizeof...(posValParams));
            assert(AvailableForWrite());
            std::size_t gap = 1;
            auto [pos, val] = OffsetAndVal(m_shape, gap, posValParams...);
            (m_mem.RawMemory())[pos] = static_cast<ElementType>(val);
        }

        template<typename... TPosParams>
        ElementType operator()(TPosParams... posParams) const
        {
            auto pos = m_shape.IndexToOffset(posParams...);
            return (m_mem.RawMemory())[pos];
        }

        const auto operator[](std::size_t id) const
        {
            if constexpr (uDim > 1)
            {
                using AimType = Tensor<ElementType, DeviceType, uDim - 1>;
                FishDL::Shape<uDim - 1> aimShape;
                std::size_t count = 1;
                for(std::size_t i = 1; i < uDim; ++i)
                {
                    const std::size_t curDim = m_shape[i];
                    aimShape[i - 1] = curDim;
                    count *= curDim;
                }

                const std::size_t pos = id * count;
                if(pos >= shape.Count())
                {
                    thorw std::runtime_error("ID out of bound.");
                }
                return AimType(m_mem.Shift(pos), aimShape);
            }
            else
            {
                using AimType = Tensor<ElementType, DeviceType, 0>;
                if(id >= m_shape[0])
                {
                    throw std::runtime_error("ID out of bound.");
                }
                return AimType(m_mem.Shift(id));
            }
        }
        
    private:
        FishDL::Shape<uDim> m_shape;                // 记录形状相关的信息
        ContinuousMemory<TElem, TDevice> m_mem;     // 保存的数据元素
    };

    template<typename TElem, typename TDevice>
    class Tensor<TElem, TDevice, 0>
    {
        static_assert(std::is_same<RemConstRef<TElem>, TElem>::value);

    public:
        using CategoryTag = CategoryTags::Tensor<0>;
        using ElementType = TElem;
        using DeviceType = TDevice;

        friend struct LowerAccessImpl<Tensor>;

    public:        
        explicit Tensor(ElementType elem = ElementType())
            : m_mem(1)
        {
            SetValue(elem);
        }
        
        explicit Tensor(MetaNN::Shape<0>)
            : Tensor() {}

        explicit Tensor(ContinuousMemory<ElementType, DeviceType> p_mem)
            : m_mem(std::move(p_mem))
        {
            assert(m_mem.Size() >= 1);
        }

        const auto& Shape() const noexcept
        {
            const static MetaNN::Shape<0> shape;
            return shape;
        }

        bool AvailableForWrite() const
        {
            return !m_mem.IsShared();
        }

        void SetValue(ElementType val)
        {
            assert(AvailableForWrite());
            (m_mem.RawMemory())[0] = val;
        }

        auto Value() const noexcept
        {
            return (m_mem.RawMemory())[0];
        }
    
        bool operator== (const Tensor& val) const noexcept
        {
            return (Value() == val.Value());
        }

        auto EvalRegister() const
        {
            return MakeConstEvalHandle(*this);
        }

    private:
        ContinuousMemory<ElementType, DeviceType> m_mem;
    };

    template<typename TElement, typename TDevice, std::size_t uDim>
    struct LowerAccessImpl<Tensor<TElement, TDevice, uDim>>
    {
        LowerAccessImpl(Tensor<TElement, TDevice, uDim> p)
            : m_data(std::move(p)) { }
        
        TElement* MutableRawMemory()
        {
            return m_data.m_mem.RawMemory();
        }

        const TElement* MutableRawMemory() const
        {
            return m_data.m_mem.RawMemory();
        }

    private:
        Tensor<TElement, TDevice, uDIm> m_data;
    };

    #define HELPER(Name, uDim) template<typename TElem, typename TDevice> using Name = Tensor<TElem, TDevice, uDim>;
    HELPER(Scalar, 0)
    HELPER(Vector, 1)
    HELPER(Matrix, 2)
    HELPER(ThreeDArray, 3) 
    #undef HELPER
}
