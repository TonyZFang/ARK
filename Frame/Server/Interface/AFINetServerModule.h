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

#include "SDK/Core/AFGUID.hpp"
#include "SDK/Core/AFQueue.hpp"
#include "SDK/Core/AFDataNode.hpp"
#include "SDK/Interface/AFIModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Net/AFCNetServer.h"
#include "SDK/Proto/AFProtoCPP.hpp"
#include "Server/Interface/AFINetModule.h"

class ServerData
{
public:
    ServerData()
    {
        pData = std::make_shared<AFMsg::ServerInfoReport>();
    }

    ~ServerData()
    {

    }

    void Init(const AFGUID& xClientID, const AFMsg::ServerInfoReport& xData)
    {
        xClient = xClientID;
        *(pData) = xData;
    }

    AFGUID xClient{ 0 };
    ARK_SHARE_PTR<AFMsg::ServerInfoReport> pData{ nullptr };
};

class SessionData
{
public:
    SessionData()
    {
    }

    int32_t mnLogicState{ 0 };
    int32_t mnGameID{ 0 };
    AFGUID mnUserID{ 0 };
    AFGUID mnClientID{ 0 };
    AFGUID mnHashIdentID{ 0 };
    std::string mstrAccout{ "" };
};

class AFINetServerModule : public AFINetModule
{
protected:
    AFINetServerModule()
    {
        nLastTime = GetPluginManager()->GetNowTime();
        m_pNet = NULL;
    }

public:
    template<typename BaseType>
    AFINetServerModule(AFIPluginManager* p, BaseType* pBase, void (BaseType::*handleRecieve)(const int, const int, const char*, const uint32_t, const AFGUID))
    {
        pPluginManager = p;
        nLastTime = GetPluginManager()->GetNowTime();
        m_pNet = NULL;
    }

    explicit AFINetServerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
        nLastTime = GetPluginManager()->GetNowTime();
        m_pNet = NULL;
    }

    virtual ~AFINetServerModule()
    {
        if (m_pNet)
        {
            m_pNet->Final();
        }

        ARK_DELETE(m_pNet);
    }

    //as server
    template<class ClassNetServerType = AFCNetServer>
    int Start(const int nServerID, const std::string strIP, const unsigned short nPort, const int thread_count, const unsigned int nMaxClient)
    {
        std::string strIPAndPort;
        std::string strPort;
        AFMisc::ARK_TO_STR(strPort, nPort);
        strIPAndPort = strIP + ":" + strPort;
        m_pNet = ARK_NEW ClassNetServerType(this, &AFINetServerModule::OnReceiveNetPack, &AFINetServerModule::OnSocketNetEvent);
        return m_pNet->Start(nServerID, strIPAndPort, thread_count, nMaxClient);
    }

    virtual bool Update()
    {
        if (m_pNet == nullptr)
        {
            return false;
        }

        KeepAlive();

        m_pNet->Update();
        return true;
    }

    bool SendMsgToAllClientWithOutHead(const int nMsgID, const std::string& msg, const AFGUID& nPlayerID)
    {
        return m_pNet->SendMsgToAllClient(msg.c_str(), msg.length(), nPlayerID);
    }

    bool SendMsgPBToAllClient(const uint16_t nMsgID, const google::protobuf::Message& xData, const AFGUID& nPlayerID)
    {
        std::string strMsg;

        if (!xData.SerializeToString(&strMsg))
        {
            char szData[MAX_PATH] = { 0 };
            ARK_SPRINTF(szData, MAX_PATH, "Send Message to all Failed For Serialize of MsgData, MessageID: %d\n", nMsgID);

            return false;
        }

        return SendMsgToAllClientWithOutHead(nMsgID, strMsg, nPlayerID);
    }

    bool SendMsgPB(const uint16_t nMsgID, const google::protobuf::Message& xData, const AFGUID& xClientID, const AFGUID nPlayer, const std::vector<AFGUID>* pClientIDList = NULL)
    {
        std::string xMsgData;

        if (!xData.SerializeToString(&xMsgData))
        {
            char szData[MAX_PATH] = { 0 };
            ARK_SPRINTF(szData, MAX_PATH, "Send Message to %s Failed For Serialize of MsgData, MessageID: %d\n", xClientID.ToString().c_str(), nMsgID);

            return false;
        }

        return SendMsgPB(nMsgID, xMsgData, xClientID, nPlayer, pClientIDList);
    }

    bool SendMsgPB(const uint16_t nMsgID, const std::string& strData, const AFGUID& xClientID, const AFGUID& nPlayer, const std::vector<AFGUID>* pClientIDList = NULL)
    {
        if (m_pNet != nullptr)
        {
            char szData[MAX_PATH] = { 0 };
            ARK_SPRINTF(szData, MAX_PATH, "Send Message to %s Failed For NULL Of Net, MessageID: %d\n", xClientID.ToString().c_str(), nMsgID);
            //LogSend(szData);
            return false;
        }

        if (pClientIDList != nullptr)
        {
            //playerid主要是网关转发消息的时候做识别使用，其他使用不使用
            AFMsg::BrocastMsg xMsg;
            AFMsg::PBGUID* pPlayerID = xMsg.mutable_entity_id();
            *pPlayerID = AFINetModule::GUIDToPB(nPlayer);
            xMsg.set_msg_data(strData.data(), strData.length());
            xMsg.set_msg_id(nMsgID);

            for (size_t i = 0; i < pClientIDList->size(); ++i)
            {
                const AFGUID& ClientID = (*pClientIDList)[i];

                AFMsg::PBGUID* pData = xMsg.add_target_entity_list();

                if (pData)
                {
                    *pData = AFINetModule::GUIDToPB(ClientID);
                }
            }

            std::string strMsg;

            if (!xMsg.SerializeToString(&strMsg))
            {
                return false;
            }

            return m_pNet->SendMsgWithOutHead(AFMsg::EGMI_GTG_BROCASTMSG, strMsg.data(), strMsg.size(), xClientID, nPlayer);
        }
        else
        {
            return m_pNet->SendMsgWithOutHead(nMsgID, strData.data(), strData.size(), xClientID, nPlayer);
        }
    }

    AFINet* GetNet()
    {
        return m_pNet;
    }

    bool PackTableToPB(AFDataTable* pTable, AFMsg::EntityDataTableBase* pEntityTableBase)
    {
        if (pTable == nullptr || pEntityTableBase == nullptr)
        {
            return false;
        }

        for (size_t i = 0; i < pTable->GetRowCount(); i++)
        {
            AFMsg::DataTableAddRow* pAddRowStruct = pEntityTableBase->add_row();
            pAddRowStruct->set_row(i);

            for (size_t j = 0; j < pTable->GetColCount(); j++)
            {
                AFMsg::PBCellData* pAddData = pAddRowStruct->add_cell_list();

                AFCData xRowColData;

                if (!pTable->GetValue(i, j, xRowColData))
                {
                    continue;
                }

                AFINetServerModule::TableCellToPBCell(xRowColData, i, j, *pAddData);
            }
        }

        return true;
    }

    bool AddTableToPB(AFDataTable* pTable, AFMsg::EntityDataTableList* pPBData)
    {
        if (!pTable || !pPBData)
        {
            return false;
        }

        AFMsg::EntityDataTableBase* pTableBase = pPBData->add_data_table_list();
        pTableBase->set_table_name(pTable->GetName());

        if (!PackTableToPB(pTable, pTableBase))
        {
            return false;
        }

        return true;
    }


    bool TableListToPB(AFGUID self, ARK_SHARE_PTR<AFIDataTableManager> pTableManager, AFMsg::EntityDataTableList& xPBData, const AFFeatureType nFeature)
    {
        AFMsg::EntityDataTableList* pPBData = &xPBData;
        *(pPBData->mutable_entity_id()) = AFINetModule::GUIDToPB(self);

        if (!pTableManager)
        {
            return false;
        }

        size_t nTableCount = pTableManager->GetCount();

        for (size_t i = 0; i < nTableCount; ++i)
        {
            AFDataTable* pTable = pTableManager->GetTableByIndex(i);

            if (pTable == nullptr)
            {
                continue;
            }

            AFFeatureType xResult = (pTable->GetFeature() & nFeature);

            if (xResult.any())
            {
                AddTableToPB(pTable, pPBData);
            }
        }

        return true;
    }

    bool NodeListToPB(AFGUID self, ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager, AFMsg::EntityDataNodeList& xPBData, const AFFeatureType nFeature)
    {
        if (!pNodeManager)
        {
            return false;
        }

        for (size_t i = 0; i < pNodeManager->GetNodeCount(); i++)
        {
            AFDataNode* pNode = pNodeManager->GetNodeByIndex(i);

            if (pNode == nullptr)
            {
                continue;
            }

            AFFeatureType xResult = (pNode->GetFeature() & nFeature);

            if (pNode->Changed() && xResult.any())
            {
                AFMsg::PBNodeData* pData = xPBData.add_data_node_list();
                AFINetModule::DataNodeToPBNode(pNode->GetValue(), pNode->GetName(), *pData);
            }
        }

        return true;
    }

protected:
    void OnReceiveNetPack(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID)
    {
        OnReceiveBaseNetPack(xHead, nMsgID, msg, nLen, xClientID);
    }

    void OnSocketNetEvent(const NetEventType eEvent, const AFGUID& xClientID, int nServerID)
    {
        OnSocketBaseNetEvent(eEvent, xClientID, nServerID);
    }

    void KeepAlive()
    {
        //Do nothing whe m_pNet as server
        //if(!m_pNet)
        //{
        //    return;
        //}

        //if(m_pNet->IsServer())
        //{
        //    return;
        //}

        //if(nLastTime + 10 > GetPluginManager()->GetNowTime())
        //{
        //    return;
        //}

        //nLastTime = GetPluginManager()->GetNowTime();

        //AFMsg::ServerHeartBeat xMsg;
        //xMsg.set_count(0);

        //SendMsgPB(AFMsg::EGameMsgID::EGMI_STS_HEART_BEAT, xMsg, AFGUID(0), AFGUID(0));
    }



private:
    AFINet* m_pNet;
    int64_t nLastTime;
    int64_t nServerID;
};