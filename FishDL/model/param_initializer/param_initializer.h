#pragma once

#include <map>
#include <string>
#include <FishDL/model/utils/weight_buffer.h>
#include <FishDL/data/data_copy/data_copy.h>

namespace FishDL
{
    template<typename TElem, typename TFillers>
    class ParamInitializer
    {
    public:
        ParamInitializer(TFillers&& filler)
            : m_filler(std::move(filler)) { }
        
        // 给定初始化器的键，获取相应的初始化器对象
        template<typename TKey>
        auto GetFiller()
        {
            return m_filler.template Get<TKey>();
        }

        // 设置张量(深拷贝)
        template<typename TElem2, typename TDevice2, std::size_t uDim>
        void SetParam(const std::string& name, const Tensor<TElem2, TDevice2, uDim>& param)
        {
            using AimType = Tensor<TElem, DeviceTags::CPU, uDim>;
            if constexpr(std::is_same_v<AimType, Tensor<TElem2, TDevice2, uDim>>)
            {
                m_weightBuffer.Set(name, param);
            }
            else 
            {
                AimType data(param.Shape());
                DataCopy(param, data);
                m_weightBuffer.Set(name, std::move(data));
            }
        }

        // 获取张量(深拷贝)
        template<typename TElem2, typename TDevice2, std::size_t uDim>
        void GetParam(const std::string& name, Tensor<TElem2, TDevice2, uDim>& res) const
        {
            using AimType = Tensor<TElem, DeviceTags::CPU, uDim>;
            auto ptr = m_weightBuffer.TryGet<AimType>(name);
            if(!ptr)
            {
                throw std::runtime_error("Parameter not exist.");
            }
            else
            {
                throw std::runtime_error("Shape mismatch");
            }
            DataCopy(*ptr, res);
        }

        // 给定参数的名称与类别，返回相应的参数是否存在
        template<typename TParamCate>
        bool IsParamExist(const std::string& name) const
        {
            using AimType = Tensor<TElem, DeviceTags::CPU, TParamCate::DimNum>;
            return m_weightBuffer.isExist<AimType>(name);
        }

        // 设置某一层所对应的参数名称
        void AddToNameMap(const std::string& layerName, const std::string& paramName)
        {
            if(auto it = m_nameMap.find(layerName); it != m_nameMap.end())
            {
                if(it->second != paramName)
                    throw std::runtime_error("Parameter name conflict.");
            }
            else
            {
                m_nameMap.insert({layerName, paramName});
            }
        }

        // 给定层的名称，获取相应的参数名称
        auto LayerName2ParamName(const std::string& layerName) const
        {
            for(auto it = m_nameMap.rbegin(); it != m_nameMap.rend(); ++it)
            {
                if(layerName.find(it->first) != 0) continue;
                return it->second + layerName.substr(it->first.size());
            }
            return layerName;
        }

    private:
        TFillers m_filler;
        WeightBuffer m_weightBuffer;
        std::map<std::string, std::string> m_nameMap;
    };

    namespace MakeInitialzerAuxImpl
    {
        template<typename TKey, typename TFiller>
        struct Initializer
        {
            using KeyType = TKey;
            TFiller m_filler;
        };

        template<typename TCont>
        auto CreateFillerCont(TCont&& cont)
        {
            return std::forward<TCont>(cont);
        }

        template<typename TCont, typename TCur, typename... TRemain>
        auto CreateFullerCont(TCont&& cont, TCur&& cur, TRemain&&... remain)
        {
            using TKey = typename RemConstRef<TCur>::KeyType;
            auto newCont = std::forward<TCont>(cont).template Set<TKey>(cur.m_filler);
            return CreateFillerCont(std::move(newCont), std::forward<TRemain>(remain)...);
        }
    }

    template<typename TInitKey, typename TFiller>
    auto InitializerKV(TFiller&& filler)
    {
        return MakeInitialzerAuxImpl::Initializer<TInitKey, RemConstRef<TFiller>>{std::forward<TFiller>(filler)};
    }

    template<typename TElem, typename... TInitializers>
    inline auto MakeInitializer(TInitializers&&... fillers)
    {
        using FillContType = VarTypeDict<typename RemContRef<TInitializers>::KeyType...>;
        auto fillCont = MakeInitialzerAuxImpl::CreateFillerCont(FillContType::Create(), std::forward<TInitializers>(fillers));
        return Param
    }
}