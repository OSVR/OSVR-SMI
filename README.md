# OSVR-SMI 
> Maintained at <https://github.com/OSVR/OSVR-SMI>
>
> For details, see <http://osvr.github.io>

This is a plugin for OSVR that provides access the eye tracker data for SMI Eye Tracking Devices

It is Windows-only, and requires the SMI SDK to build. See <http://www.smivision.com/en/gaze-and-eye-tracking-systems/support/software-download.html> for details.

You will need to install the SMI [SDK](Eye Tracking HMD Upgrade Software) software on your computer before using this plugin. Additionally you will need to download Oculus Runtime and SDK available at <https://developer.oculus.com/downloads/>

Then, just put the `.dll` file from this plugin in the same directory of your OSVR server as the other plugins - usually something like `osvr-plugins-0` - and use the config file provided here as an example.


## Licenses
This plugin: Licensed under the Apache License, Version 2.0.

Note that since this builds on the SMI SDK, you are responsible for complying with the terms of that license.
