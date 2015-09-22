#[=======================================================================[.rst:
FindSMI
-----------

Find the SMI SDK components.

Imported Targets
^^^^^^^^^^^^^^^^

Several :ref:`imported targets <Imported targets>` are provided
if the SDK is found:

``SMI::HMDAPI``
  Main library for interacting with SMI HMD API.

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

``SMI_FOUND``
  True if the SDK was found, false otherwise.
``SMI_INCLUDE_DIRS``
  Include directories needed to find SMI headers.
``SMI_LIBRARIES``
  The list of all SMI libraries.

It is recommended to link against the imported targets, rather than using the
latter two variables above.

Cache Variables
^^^^^^^^^^^^^^^

This module uses the following cache variables:

``SMI_HMDAPI_LIBRARY``
  The location of the SMI HMD API driver library.
``SMI_INCLUDE_DIR``
  The location of the SMI SDK include directory containing ``iViewHMDAPI.h``.
``SMI_ROOT_DIR``
  An optional root to start looking for the SMI SDK, if it``s not in its
  conventional location (x86 Program Files directory +
  ``SMI/iViewNG-HMD/SDK/``)

The cache variables should not be used by project code.
They may be set by end users to point at SMI SDK components, though the script
can find the current (as of this writing) version of the SDK in its default
location.

#]=======================================================================]

# Original Author:
# 2015 Georgiy Frolov <georgiy@sensics.com>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(SMI_ROOT_DIR
	"${SMI_ROOT_DIR}"
	CACHE
	PATH
	"Directory to search for SMI HMD API SDK")

if(WIN32)
    # Test 32/64 bits
    if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
        # Get desired program files directory
        set(_PF86_ENV "ProgramFiles(x86)")
        set(_PF86 $ENV{${_PF86_ENV}})
        
        if(NOT "$ENV{${_PF86}}" STREQUAL "")
        # 32-bit dir: only set on win64
            file(TO_CMAKE_PATH "$ENV{_PF86}" _progfiles)
        else()
        # 32-bit dir on win32
            file(TO_CMAKE_PATH "$ENV{ProgramFiles}" _progfiles)
        endif()

        find_library(SMI_HMDAPI_LIBRARY
            NAMES
            iViewHMDAPI
            PATHS
            "${SMI_ROOT_DIR}"
            "${_progfiles}/SMI/iViewNG-HMD/SDK/libs")

        if(SMI_HMDAPI_LIBRARY)
            get_filename_component(_libdir "${SMI_HMDAPI_LIBRARY}" DIRECTORY)
        endif()

        find_path(SMI_INCLUDE_DIR
            NAMES
            iViewHMDAPI.h
            HINTS
            "${_libdir}/../include"
            "${_libdir}/../../include"
            PATHS
            "${SMI_ROOT_DIR}"
            "${_progfiles}/SMI/iViewNG-HMD/SDK/include"
            PATH_SUFFIXES
            inc
        )
    endif()
endif() # WIN32

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SMI FOUND_VAR SMI_FOUND
    REQUIRED_VARS
    SMI_HMDAPI_LIBRARY
    SMI_INCLUDE_DIR)

if(SMI_FOUND)
    set(APPEND SMI_LIBRARIES ${SMI_HMDAPI_LIBRARY})
    set(SMI_INCLUDE_DIRS ${SMI_INCLUDE_DIR})
    mark_as_advanced(SMI_ROOT_DIR)
    if(NOT TARGET SMI::HMDAPI)
        add_library(SMI::HMDAPI UNKNOWN IMPORTED)
        set_target_properties(SMI::HMDAPI PROPERTIES
            IMPORTED_LOCATION "${SMI_HMDAPI_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SMI_INCLUDE_DIRS}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "C")
    endif()
endif()

mark_as_advanced(SMI_INCLUDE_DIR
    SMI_HMDAPI_LIBRARY)
