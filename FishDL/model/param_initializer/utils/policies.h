#pragma once

#include <random>
#include <FishDL/policies/policy_macro_begin.h>
namespace FishDL
{
struct InitPolicy
{
    using MajorClass = InitPolicy;

    struct RandEngineTypeCate;
    using RandEngine = std::mt19937;
};

TypePolicyTemplate(PRandomGeneratorIs, InitPolicy, RandEngine);
}
#include <FishDL/policies/policy_macro_end.h>