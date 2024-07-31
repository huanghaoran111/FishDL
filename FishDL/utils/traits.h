#pragma once

#include <type_traits>



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

