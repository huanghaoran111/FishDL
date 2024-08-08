#pragma once

#include <FishDL/data/utils/traits.h>
#include <FishDL/operation/utils/operation_frame.h>

namespace FishDL
{
namespace OpTags
{
    struct Sigmoid;
}


// 如果传入诸如double类型，编译会出现错误
template<typename TP, std::enable_if_t<IsValidOper<OpTags::Sigmoid, TP>>* = nullptr>
auto Sigmoid(TP&& p_m)
{
    using rawM = RemConstRef<TP>;
    using ResType = Operation<OpTags::Sigmoid, OperandContainer<rawM>>;
    return ResType(std::forward<TP>(p_m));
}
}