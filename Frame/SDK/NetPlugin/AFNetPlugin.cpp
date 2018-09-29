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

#include "AFNetPlugin.h"
#include "AFCNetServerManagerModule.h"
#include "AFCNetClientManagerModule.h"

namespace ark
{

    ARK_DLL_PLUGIN_ENTRY(AFNetPlugin)
    ARK_DLL_PLUGIN_EXIT(AFNetPlugin)

    //////////////////////////////////////////////////////////////////////////

    int AFNetPlugin::GetPluginVersion()
    {
        return 0;
    }

    const std::string AFNetPlugin::GetPluginName()
    {
        return GET_CLASS_NAME(AFNetPlugin)
    }

    void AFNetPlugin::Install()
    {
        RegisterModule<AFINetServerManagerModule, AFCNetServerManagerModule>();
        RegisterModule<AFINetClientManagerModule, AFCNetClientManagerModule>();
    }

    void AFNetPlugin::Uninstall()
    {
        DeregisterModule<AFINetClientManagerModule, AFCNetClientManagerModule>();
        DeregisterModule<AFINetServerManagerModule, AFCNetServerManagerModule>();
    }

}