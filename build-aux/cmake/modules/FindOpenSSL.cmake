##
## FindSdkEngine.cmake: This file is part of cmake-aux.
## Copyright (C) Gostai S.A.S., 2006-2008.
##
## This software is provided "as is" without warranty of any kind,
## either expressed or implied, including but not limited to the
## implied warranties of fitness for a particular purpose.
##
## See the LICENSE file for more information.
## For comments, bug reports and feedback: http://www.urbiforge.com
##
##-----------------------------------------------------------------
## CMake PACKAGE - URBI SDK Engine
## Find URBI Sdk Engine libraries and paths.
#
# Current FindOpenSSL module provided by CMake is laking:
#    * support for choosing between static/shared library to search for
#    * search of the libcrypto
# that's why we provide this alternative package waiting for better solution
# coming from upstream.
#
##-----------------------------------------------------------------

set(PACKAGE_FULLNAME "OpenSSL (TLS)")

##-----------------------------------------------------------------
# This package tries to respect CMake guidelines to be easily
# reusable.
# This module handles the following variables. Please note XXX is
# the name specified in the PACKAGE_NAME variable (just above).
#
# XXX_INCLUDE_DIRS   The final set of include directories listed in
#                    one variable for use by client code.
#
# XXX_LIBRARY_DIRS   Optionally, the final set of library
#                    directories listed in one variable for use
#                    by client code.
#
# XXX_LIBRARIES      The libraries to link against to use XXX. These
#                    should include full paths.
#
# XXX_YY_FOUND       FALSE/Unidefined if YY part of XXX package has
#                    not been found.
#
# XXX_FOUND          FALSE/Undefined if the whole package has not
#                    been found.
#                    Set it manually to FALSE don't want to use XXX.
#
# XXX_ROOT_DIR       Paths you want to add in the path list in order
#                    to help cmake find your package on your computer.
#
# QUIET option       You can ask the package not to complain if there
#                    is library/path not found. This means no output.
#                    If you specify both QUIET and REQUIRED option,
#                    the QUIET option has a lower priority and
#                    critical messages are displayed.
#
# REQUIRED option    You can ask the package to throw a FATAL_ERROR
#                    if the whole package has not been found.
#
##-----------------------------------------------------------------

# Retrieve PACKAGE_NAME and PACKAGE_FILENAME
include(Package-toolbox)
set_package_name(${CMAKE_CURRENT_LIST_FILE})

if(NOT ${PACKAGE_NAME}_FOUND)

    # Load search macro and set additional options
    include(Search)
    set(SEARCH_OPTIONS)

    if(${PACKAGE_FILENAME}_FIND_QUIETLY)
        list(APPEND SEARCH_OPTIONS QUIET)
    else(${PACKAGE_FILENAME}_FIND_QUIETLY)
        message(STATUS "Loading package " ${PACKAGE_FULLNAME} " ("
                       ${PACKAGE_NAME} ")")
    endif(${PACKAGE_FILENAME}_FIND_QUIETLY)

    set(${PACKAGE_NAME}_FOUND FALSE)
    set(${PACKAGE_NAME}_LIBRARY_DIRS ${CMAKE_LIBRARY_PATH})

    # Search for the associated crypto library.
    search(LIBRARY OPEN_SSL_CRYPTO_LIBRARY crypto
           FULLNAME "SDK Engine's Cryptography"
           PACKAGE ${PACKAGE_FULLNAME}
           PATHS ${${PACKAGE_NAME}_ROOT_DIR}
           ${SEARCH_OPTIONS}
           STATIC)
    list(APPEND ${PACKAGE_NAME}_LIBRARIES ${OPEN_SSL_CRYPTO_LIBRARY})

    # Search for the associated dl library.
    search(LIBRARY OPEN_SSL_DL_LIBRARY dl
           FULLNAME "SDK Engine's dl"
           PACKAGE ${PACKAGE_FULLNAME}
           PATHS ${${PACKAGE_NAME}_ROOT_DIR}
           ${SEARCH_OPTIONS}
           SHARED)
    list(APPEND ${PACKAGE_NAME}_LIBRARIES ${SDK_ENGINE_DL_LIBRARY})

    if(SDK_ENGINE_URBI_LIBRARY AND SDK_ENGINE_INCLUDE )
        set(${PACKAGE_NAME}_FOUND TRUE)
    else(SDK_ENGINE_URBI_LIBRARY AND SDK_ENGINE_INCLUDE)
        if(${PACKAGE_FILENAME}_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find the whole package. Use -D"
                                ${PACKAGE_NAME}_ROOT_DIR "to specify paths to help"
                                "CMake find this package.")
        endif(${PACKAGE_FILENAME}_FIND_REQUIRED)
    endif(SDK_ENGINE_URBI_LIBRARY AND SDK_ENGINE_INCLUDE)

endif(NOT ${PACKAGE_NAME}_FOUND)

