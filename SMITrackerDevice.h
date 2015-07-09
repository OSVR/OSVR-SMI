/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 SMI (SensoMotoric Instruments).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_SMITrackerDevice_h_GUID_2AAE5722_96B3_4AA7_694E_426F0506DA54
#define INCLUDED_SMITrackerDevice_h_GUID_2AAE5722_96B3_4AA7_694E_426F0506DA54

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/EyeTrackerInterfaceC.h>

// Generated JSON header file
#include "com_osvr_SMI_EyeTracker_json.h"

// Library/third-party includes
#include "iViewHMDAPI.h"

// Standard includes
#include <iostream>
#include <memory>
#include <windows.h>

namespace {

class SMITrackerDevice {

  public:
    SMITrackerDevice(const SMITrackerDevice &);
    SMITrackerDevice &operator=(const SMITrackerDevice &);
    static SMITrackerDevice *createInstance(OSVR_PluginRegContext ctx);
    // assuming that m_instance is not null
    static SMITrackerDevice *getInstance();

    // default update callback, in this case, it shouldn't do anything
    OSVR_ReturnCode update();
    void reportData(smi_SampleHMDStruct *m_eyeData);

    osvr::pluginkit::DeviceToken m_dev;
    OSVR_EyeTrackerDeviceInterface m_eyetracker;

  private:
    SMITrackerDevice(OSVR_PluginRegContext ctx);
    static SMITrackerDevice *m_instance;
};

} // namespace
#endif // INCLUDED_SMITrackerDevice_h_GUID_2AAE5722_96B3_4AA7_694E_426F0506DA54
