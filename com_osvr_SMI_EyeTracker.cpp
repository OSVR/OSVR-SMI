/** @date 2015

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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <SMITrackerDevice.h>

// Standard Includes
#include <stdint.h>

// Anonymous namespace to avoid symbol collision
namespace {

OSVR_MessageType eyeTrackerMessage;
SMITrackerDevice *SMITrackerDevice::m_instance;

static void CALLBACK myCallback(smi_CallbackDataStruct *result) {

    smi_SampleHMDStruct *m_eyeData = (smi_SampleHMDStruct *)result->result;

    if (SMITrackerDevice::getInstance() == nullptr) {
        // skip this report, occurs once when mycallback is registered
    } else {
        SMITrackerDevice::getInstance()->reportData(m_eyeData);
    }
}

/// helper function for SMI API calls
static void apiCallRC(int rc) {
    std::cout << std::endl
              << smi_rcToString(rc) << std::endl;
}

SMITrackerDevice *SMITrackerDevice::createInstance(OSVR_PluginRegContext ctx) {
    if (m_instance == nullptr) {
        m_instance = new SMITrackerDevice(ctx);
    }
    return m_instance;
}

// assuming that m_instance is not null
SMITrackerDevice *SMITrackerDevice::getInstance() { return m_instance; }

// default update callback, in this case, it shouldn't do anything
OSVR_ReturnCode SMITrackerDevice::update() { return OSVR_RETURN_SUCCESS; }

/// Report data from this function
void SMITrackerDevice::reportData(smi_SampleHMDStruct *m_eyeData) {

    OSVR_TimeValue times;
    osvrTimeValueGetNow(&times);

    OSVR_EyeGazePosition2DState por;
    por.data[0] = m_eyeData->por.x;
    por.data[1] = m_eyeData->por.y;
    osvrDeviceEyeTrackerReport2DGaze(m_eyetracker, por, 0, &times);

    OSVR_EyeGazeDirectionState left_dir;
    left_dir.data[0] = m_eyeData->left.gazeDirection.x;
    left_dir.data[1] = m_eyeData->left.gazeDirection.y;
    left_dir.data[2] = m_eyeData->left.gazeDirection.z;
    osvrDeviceEyeTrackerReport3DGazeDirection(m_eyetracker, left_dir, 0,
                                              &times);

    OSVR_EyeGazeDirectionState right_dir;
    right_dir.data[0] = m_eyeData->right.gazeDirection.x;
    right_dir.data[1] = m_eyeData->right.gazeDirection.y;
    right_dir.data[2] = m_eyeData->right.gazeDirection.z;
    osvrDeviceEyeTrackerReport3DGazeDirection(m_eyetracker, right_dir, 1,
                                              &times);

    OSVR_EyeGazeBasePoint3DState left_basePoint;
    left_basePoint.data[0] = m_eyeData->left.gazeBasePoint.x;
    left_basePoint.data[1] = m_eyeData->left.gazeBasePoint.y;
    left_basePoint.data[2] = m_eyeData->left.gazeBasePoint.z;
    osvrDeviceEyeTrackerReport3DGaze(m_eyetracker, left_dir, left_basePoint, 0,
                                     &times);

    OSVR_EyeGazeBasePoint3DState right_basePoint;
    right_basePoint.data[0] = m_eyeData->right.gazeBasePoint.x;
    right_basePoint.data[1] = m_eyeData->right.gazeBasePoint.y;
    right_basePoint.data[2] = m_eyeData->right.gazeBasePoint.z;
    osvrDeviceEyeTrackerReport3DGaze(m_eyetracker, right_dir, right_basePoint,
                                     1, &times);
}

SMITrackerDevice::SMITrackerDevice(OSVR_PluginRegContext ctx) {

    /// Create the initialization options
    OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

    /// @todo specify number of sensors (1, 2)
    osvrDeviceEyeTrackerConfigure(opts, &m_eyetracker, 2);

    /// Create the sync device token with the options
    m_dev.initSync(ctx, "SMI", opts);

    /// Send JSON descriptor
    m_dev.sendJsonDescriptor(com_osvr_SMI_EyeTracker_json);

    /* SMI part */

    int rc;

    apiCallRC(rc = smi_setCallback(myCallback));

    smi_TrackingParameterStruct params;
    memset(&params, 0, sizeof(smi_TrackingParameterStruct));
    params.mappingDistance = 1500; // map to vergence distance

    bool simulateData = false;

    apiCallRC(rc = smi_startStreaming(simulateData, &params));

    /* END SMI part */

    /// Register update callback
    m_dev.registerUpdateCallback(this);
}

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        // if device was already detected, we don't need to go thru hardware
        // detection again
        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;

        int rc;
        rc = smi_checkHardware();
        if (rc != 1) {
            return OSVR_RETURN_FAILURE;
        }
        m_found = true;

        std::cout << "PLUGIN: We have detected SMI HMD! " << std::endl;
        /// Create our device object
        osvr::pluginkit::registerObjectForDeletion(
            ctx, SMITrackerDevice::createInstance(ctx));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    bool m_found;
};
} // namespace

OSVR_PLUGIN(com_osvr_SMI_EyeTracker) {

    osvrDeviceRegisterMessageType(ctx, "EyeTrackerMessage", &eyeTrackerMessage);

    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}