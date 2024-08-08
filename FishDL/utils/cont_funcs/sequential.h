#pragma once

#include <FishDL/utils/cont_funcs/helpers.h>

namespace FishDL::Sequential
{
// At ---------------------
    namespace AtImpl
    {
        template<typename ignore>
        struct impl;

        template<int... ignore>
        struct impl<Helper::IndexSequence<ignore...>>
        {
            template<typename nth>
            static nth apply(decltype(ignore, (void*)nullptr)..., nth*, ...);
        };
    }
    
    template<typename TCon, int N>
    struct At_;

    template<template<typename...> typename TCon, typename... TParams, int N>
    struct At_<TCon<TParams...>, N>
    {
        using type = decltype(AtImpl::impl<Helper::MakeIndexSequence<N>>::apply((TParams*)nullptr...));
    };

    template<typename TCon, int N>
    using At = typename At_<TCon, N>::type;
// At ---------------------
}