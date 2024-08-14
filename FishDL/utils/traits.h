#pragma once

#include <iterator>
#include <cstddef>
#include <type_traits>

namespace FishDL
{
    template<typename T>
    struct Identity_
    {
        using type = T;
    };

    template<typename T>
    using RemConstRef = std::remove_cv_t<std::remove_reference_t<T>>;

    // And
    template <bool cur, typename TNext>
    constexpr static bool AndValue = false;

    template <typename TNext>
    constexpr static bool AndValue<true, TNext> = TNext::value;

    // Or
    template <bool cur, typename TNext>
    constexpr static bool OrValue = true;

    template <typename TNext>
    constexpr static bool OrValue<false, TNext> = TNext::value;

    template <typename... T>
    constexpr static bool DependencyFalse = false;

    // 编译期进行 switch 分支选择
    template<typename TBooleanCont, typename TFunCont>
    struct CompileTimeSwitch_;

    template<bool curBool, bool... TBools,
             template<typename...> class TFunCont, typename curFunc, typename... TFuncs>
    struct CompileTimeSwitch_<std::integer_sequence<bool, curBool, TBools...>, TFunCont<curFunc, TFuncs...>>
    {
        static_assert((sizeof...(TBools) == sizeof...(TFuncs)) ||
                      (sizeof...(TBools) + 1 == sizeof...(TFuncs)));
        using type = typename std::conditional_t<curBool,
                                                 Identity_<curFunc>,
                                                 CompileTimeSwitch_<std::integer_sequence<bool, TBools...>,
                                                                    TFunCont<TFuncs...>>>::type;
    };

    template<template<typename...> class TFunCont, typename curFunc>
    struct CompileTimeSwitch_<std::integer_sequence<bool>, TFunCont<curFunc>>
    {
        using type = CurFunc;
    };

    template<typename TBooleanCont, typename TFunCont>
    using CompileTimeSwitch = typename CompileTimeSwitch_<TBooleanCont, TFunCont>::type;
}
