﻿/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "Common/AFApp.hpp"
#include "SDK/Interface/AFINet.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFIModule.h"

class AFINetModule : public AFIModule
{
protected:
    AFINetModule() = default;

public:
    virtual ~AFINetModule() = default;

    template<typename BaseType>
    bool AddRecvCallback(const int nMsgID, BaseType* pBase, void (BaseType::*handleRecv)(const ARK_PKG_BASE_HEAD&, const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_PKG_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        NET_PKG_RECV_FUNCTOR_PTR functorPtr = std::make_shared<NET_PKG_RECV_FUNCTOR>(functor);

        return AddRecvCallback(nMsgID, functorPtr);
    }

    template<typename BaseType>
    bool AddRecvCallback(BaseType* pBase, void (BaseType::*handleRecv)(const ARK_PKG_BASE_HEAD&, const int, const char*, const uint32_t, const AFGUID&))
    {
        NET_PKG_RECV_FUNCTOR functor = std::bind(handleRecv, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        NET_PKG_RECV_FUNCTOR_PTR functorPtr = std::make_shared<NET_PKG_RECV_FUNCTOR>(functor);

        return AddRecvCallback(functorPtr);
    }

    virtual bool AddRecvCallback(const int nMsgID, const NET_PKG_RECV_FUNCTOR_PTR& cb)
    {
        if (mxRecvCallBack.find(nMsgID) != mxRecvCallBack.end())
        {
            return false;
        }
        else
        {
            mxRecvCallBack.insert(std::make_pair(nMsgID, cb));
            return true;
        }
    }

    virtual bool AddRecvCallback(const NET_PKG_RECV_FUNCTOR_PTR& cb)
    {
        mxCallBackList.push_back(cb);

        return true;
    }

    template<typename BaseType>
    bool AddEventCallBack(BaseType* pBase, void (BaseType::*handler)(const NetEventType, const AFGUID&, const int))
    {
        NET_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        NET_EVENT_FUNCTOR_PTR functorPtr(new NET_EVENT_FUNCTOR(functor));

        return AddEventCallBack(functorPtr);
    }

    virtual bool AddEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb)
    {
        mxEventCallBackList.push_back(cb);

        return true;
    }

protected:
    void OnRecvBaseNetPack(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        auto it = mxRecvCallBack.find(nMsgID);

        if (mxRecvCallBack.end() != it)
        {
            (*it->second)(xHead, nMsgID, msg, nLen, xClientID);
        }
        else
        {
            for (auto iter : mxCallBackList)
            {
                (*iter)(xHead, nMsgID, msg, nLen, xClientID);
            }
        }
    }

    void OnSocketBaseNetEvent(const NetEventType eEvent, const AFGUID& xClientID, int nServerID)
    {
        for (auto it : mxEventCallBackList)
        {
            (*it)(eEvent, xClientID, nServerID);
        }
    }

private:
    std::map<int, NET_PKG_RECV_FUNCTOR_PTR> mxRecvCallBack;
    std::list<NET_EVENT_FUNCTOR_PTR> mxEventCallBackList;
    std::list<NET_PKG_RECV_FUNCTOR_PTR> mxCallBackList;
};