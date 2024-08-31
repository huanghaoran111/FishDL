#pragma once 
#include <FishDL/utils/cont_funcs/helpers.h>
#include <FishDL/utils/cont_funcs/sequential.h>
#include <tuple>

namespace FishDL
{
namespace Map
{
    namespace CreateFromItemsAuxImpl
    {
        template<template<typename> typename KeyPicker>
        struct KVCreator
        {
            template <typename TItem>
            struct apply
            {
                using type = Helper::KVBinder<typename KeyPicker<TItem>::type, TItem>;
            };
        };
    }

    namespace FindImpl
    {
        template<typename TCon, typename TDefault>
        struct map_;

        template<template<typename...> typename TCon, typename... TItem, typename TDefault>
        struct map_<TCon<TItem...>, TDefault> : TItem...
        {
            using TItem::apply...;
            static TDefault apply(...);
        };
    }

    template<typename TCon, typename TKey, typename TDefault>
    struct Find_
    {
        using type = decltype(FindImpl::map_<TCon, TDefault>::apply((TKey*)nullptr));
    };

    template<typename TCon, typename TKey, typename TDefault = void>
    using Find = typename Find_<TCon, TKey, TDefault>::type;

}
}