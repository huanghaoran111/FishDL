#pragma once

#include <random>
#include <stdexcept>
#include <type_traits>
#include <FishDL/model/param_initializer/utils/fill_with_spec_dist.h>
#include <FishDL/model/param_initializer/utils/policies.h>

namespace FishDL
{
/*
    struct VarScaleFillerPolicy
    {
        using MajorClass = VarScaleFillerPolicy;

        struct DistributeTypeCate
        {
            struct Uniform;
            struct Norm;
        };
        using Distribute = DistributeTypeCate::Uniform;

        struct ScaleModeTypeCate
        {
            struct FanIn;
            struct FanOut;
            struct FanAvg;
        };
        using ScaleMode = ScaleModeTypeCate::FanAvg;
    };
#include <FishDL/policies/policy_macro_begin.h>

#include <FishDL/policies/policy_macro_end.h>
// */

}