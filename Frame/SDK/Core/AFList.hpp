﻿/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#pragma once

#include "SDK/Core/AFPlatform.hpp"

namespace ark
{

    template <typename T>
    class AFList
    {
    public:
        virtual ~AFList() = default;
        bool Add(const T& id);
        bool Find(const T& id);
        bool Remove(const T& id);
        bool ClearAll();

        bool First(T& id);
        bool Next(T& id);
        bool Get(const int32_t index, T& id);
        int Count();

    private:
        using TList = std::list<T>;
        TList mtObjConfigList;
        typename std::list<T>::iterator mCurIter;
    };

    template<typename T>
    bool AFList<T>::Add(const T& id)
    {
        mtObjConfigList.push_back(id);
        return true;
    }

    template<typename T>
    bool AFList<T>::Remove(const T& id)
    {
        if (Find(id))
        {
            mtObjConfigList.remove(id);
            return true;
        }

        return false;
    }

    template < typename T >
    bool AFList<T>::ClearAll()
    {
        mtObjConfigList.clear();
        return true;
    }

    template < typename T >
    bool AFList<T>::First(T& id)
    {
        if (mtObjConfigList.size() <= 0)
        {
            return false;
        }

        mCurIter = mtObjConfigList.begin();

        if (mCurIter != mtObjConfigList.end())
        {
            id = *mCurIter;
            return true;
        }

        return false;
    }

    template < typename T >
    bool AFList<T>::Next(T& id)
    {
        if (mCurIter == mtObjConfigList.end())
        {
            return false;
        }

        ++mCurIter;

        if (mCurIter != mtObjConfigList.end())
        {
            id = *mCurIter;
            return true;
        }

        return false;
    }

    template < typename T >
    bool AFList<T>::Find(const T& id)
    {
        auto it = std::find(mtObjConfigList.begin(), mtObjConfigList.end(), id);

        if (it != mtObjConfigList.end())
        {
            return true;
        }

        return false;
    }

    template < typename T >
    bool AFList<T>::Get(const int32_t index, T& id)
    {
        if (index >= mtObjConfigList.size())
        {
            return false;
        }

        auto it = this->mtObjConfigList.begin();
        std::advance(it, index);

        id = *it;

        return true;
    }

    template < typename T >
    int AFList<T>::Count()
    {
        return (int)(mtObjConfigList.size());
    }

}