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

#include "SDK/Interface/AFINet.h"
#include "SDK/Core/AFQueue.hpp"
#include "SDK/Core/AFRWLock.hpp"
#include <brynet/net/SocketLibFunction.h>
#include <brynet/net/TCPService.h>
#include <brynet/net/Connector.h>

#pragma pack(push, 1)

namespace ark
{

    class AFCWebSocktClient : public AFINet
    {
    public:
        AFCWebSocktClient(brynet::net::TcpService::PTR server = nullptr, brynet::net::AsyncConnector::PTR connector = nullptr);

        template<typename BaseType>
        AFCWebSocktClient(BaseType* pBaseType, void (BaseType::*handleRecieve)(const ARK_PKG_BASE_HEAD& xHead, const int, const char*, const size_t, const AFGUID&), void (BaseType::*handleEvent)(const NetEventType, const AFGUID&, const int))
        {
            mRecvCB = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
            mEventCB = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

            brynet::net::base::InitSocket();
            m_pTcpService = brynet::net::TcpService::Create();
        }

        ~AFCWebSocktClient() override;

        void Update() override;
        bool Start(const int target_busid, const std::string& ip, const int port, bool ip_v6 = false) override;
        bool Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6 = false) override
        {
            return false;
        }

        bool Shutdown() override final;
        bool SendRawMsg(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID = 0, const AFGUID& xPlayerID = 0) override;
        bool CloseNetEntity(const AFGUID& xClient) override;
        bool IsServer() override;
        bool Log(int severity, const char* msg) override;

    protected:
        bool SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient = 0);

        bool DismantleNet(AFHttpEntity* pEntity);
        void ProcessMsgLogicThread();
        void ProcessMsgLogicThread(AFHttpEntity* pEntity);
        bool CloseSocketAll();

        int DeCode(const char* strData, const size_t len, ARK_PKG_CS_HEAD& xHead);
        int EnCode(const ARK_PKG_CS_HEAD& xHead, const char* strData, const size_t len, std::string& strOutData);

    private:
        std::unique_ptr<AFHttpEntity> m_pClientEntity{ nullptr };
        std::string mstrIPPort{};
        int mnTargetBusID{ 0 };
        NET_PKG_RECV_FUNCTOR mRecvCB{ nullptr };
        NET_EVENT_FUNCTOR mEventCB{ nullptr };
        AFCReaderWriterLock mRWLock;

        brynet::net::TcpService::PTR m_pTcpService{ nullptr };
    };

}

#pragma pack(pop)