#!/bin/bash

# Copyright (c) 2014 Aerys

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.


# Install Android SDK/NDK.
# Usage: ./install_jni.sh

# Please change ANDROID_SDK_VERSION and ANDROID_NDK_VERSION to install a specific version.

# Author: Warren Seine (warren.seine@aerys.in), Aerys, 2014.
# Project: Minko (minko.io).
# License: MIT.

set -e

if [[ -z "${ANDROID_HOME}" ]]; then
	if [[ -z "${ANDROID}" ]]; then
		echo "Environment variable ANDROID_HOME is not defined."
		exit 1
	else
		ANDROID_HOME="${ANDROID}"
	fi
fi

ANDROID_SDK_VERSION="android-19"
ANDROID_NDK_VERSION="android-ndk-r10d"
ANDROID_TOOLCHAIN="arm-linux-androideabi-4.9"
ANDROID_SYSTEM="linux-x86_64"

if [[ $OSTYPE == cygwin* ]]; then
	ANDROID_HOME=`cygpath -u "${ANDROID_HOME}"`
	ANDROID_SYSTEM="windows-x86_64"
elif [[ $OSTYPE == darwin* ]]; then
	ANDROID_SYSTEM="darwin-x86_64"
fi

if [[ -n "${ANDROID_HOME}" ]]; then
	if [[ ! -e "${ANDROID_HOME}/tools/lib/sdk-common.jar" ]]; then
		echo "Environment variable ANDROID_HOME is defined but does not look like an Android SDK."
		exit 1
	elif [[ ! -d "${ANDROID_HOME}/ndk/${ANDROID_NDK_VERSION}" ]]; then
		echo "Environment variable ANDROID is defined but does not contain NDK ${ANDROID_NDK_VERSION}."
		exit 1
	fi
fi

ANDROID_NDK_HOME="${ANDROID_HOME}/toolchains/${ANDROID_TOOLCHAIN}"

# Build a standalone toolchain
pushd "${ANDROID_HOME}/ndk/${ANDROID_NDK_VERSION}" > /dev/null
build/tools/make-standalone-toolchain.sh --system=${ANDROID_SYSTEM} --platform=${ANDROID_SDK_VERSION} --toolchain=${ANDROID_TOOLCHAIN} --install-dir=${ANDROID_NDK_HOME}
popd > /dev/null

# Link default NDK.
pushd "${ANDROID_HOME}/toolchains/" > /dev/null
ln -s ${ANDROID_TOOLCHAIN} default
popd > /dev/null