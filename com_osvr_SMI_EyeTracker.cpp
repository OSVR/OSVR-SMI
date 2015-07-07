/** @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics Inc.
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

// Anonymous namespace to avoid symbol collision
namespace {

OSVR_MessageType eyeTrackerMessage;

class EyeTrackerDevice {
  public:
    EyeTrackerDevice(OSVR_PluginRegContext ctx) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        // tracker = trackerInst;
        osvrDeviceEyeTrackerConfigure(opts, &m_eyetracker, 1);

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

        // simulate data by default for now
        bool simulateData = true;

        apiCallRC(rc = smi_startStreaming(simulateData, &params));

        /* END SMI part */

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    void apiCallRC(int rc) {
        std::cout << std::endl
                  << smi_rcToString(rc) << std::endl;
    }

    OSVR_ReturnCode update() { return OSVR_RETURN_SUCCESS; }

	static void CALLBACK myCallback(smi_CallbackDataStruct *result) {

		smi_SampleHMDStruct *m_eyeData = (smi_SampleHMDStruct *)result->result;

		OSVR_TimeValue times;

		osvrTimeValueGetNow(&times);

		OSVR_EyeGazePosition2DState por;
		por.data[0] = m_eyeData->por.x;
		por.data[1] = m_eyeData->por.y;
		osvrDeviceEyeTrackerReport2DGaze(m_dev, m_eyetracker, por, 0, &times);
		/*
		OSVR_EyeGazeDirectionState left_dir;
		left_dir.data[0] = m_eyeData->left.gazeDirection.x;
		left_dir.data[1] = m_eyeData->left.gazeDirection.y;
		left_dir.data[2] = m_eyeData->left.gazeDirection.z;
		osvrDeviceEyeTrackerReport3DGazeDirection(m_dev, m_eyetracker, left_dir, 0, &times);

		OSVR_EyeGazeDirectionState right_dir;
		right_dir.data[0] = m_eyeData->right.gazeDirection.x;
		right_dir.data[1] = m_eyeData->right.gazeDirection.y;
		right_dir.data[2] = m_eyeData->right.gazeDirection.z;
		osvrDeviceEyeTrackerReport3DGazeDirection(m_dev, m_eyetracker, right_dir, 1, &times);

		OSVR_EyeGazeBasePoint3DState left_basePoint;
		left_basePoint.data[0] = m_eyeData->left.gazeBasePoint.x;
		left_basePoint.data[1] = m_eyeData->left.gazeBasePoint.y;
		left_basePoint.data[2] = m_eyeData->left.gazeBasePoint.z;
		osvrDeviceEyeTrackerReport3DGaze(m_dev, m_eyetracker, left_dir, left_basePoint, 0, &times);

		OSVR_EyeGazeBasePoint3DState right_basePoint;
		right_basePoint.data[0] = m_eyeData->right.gazeBasePoint.x;
		right_basePoint.data[1] = m_eyeData->right.gazeBasePoint.y;
		right_basePoint.data[2] = m_eyeData->right.gazeBasePoint.z;
		osvrDeviceEyeTrackerReport3DGaze(m_dev, m_eyetracker, right_dir, right_basePoint, 1, &times);
		*/
	}

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_EyeTrackerDeviceInterface m_eyetracker;
    smi_SampleHMDStruct *m_eyeData;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;

        // always detect HMD for now, until proper hardware detection
        if (0 == 1) {
            std::cout << "PLUGIN: We have NOT detected Eye Tracker "
                      << std::endl;
            return OSVR_RETURN_FAILURE;
        }

        m_found = true;

        std::cout << "PLUGIN: We have detected SMI HMD! " << std::endl;
        /// Create our device object
        osvr::pluginkit::registerObjectForDeletion(ctx,
                                                   new EyeTrackerDevice(ctx));

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