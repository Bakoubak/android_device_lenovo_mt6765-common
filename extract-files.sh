#!/bin/bash
#
# Copyright (C) 2016 The CyanogenMod Project
# Copyright (C) 2017-2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

set -e

# Load extract_utils and do some sanity checks
MY_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "${MY_DIR}" ]]; then MY_DIR="${PWD}"; fi

ANDROID_ROOT="${MY_DIR}/../../.."

HELPER="${ANDROID_ROOT}/tools/extract-utils/extract_utils.sh"
if [ ! -f "${HELPER}" ]; then
    echo "Unable to find helper script at ${HELPER}"
    exit 1
fi
source "${HELPER}"

# Default to sanitizing the vendor folder before extraction
CLEAN_VENDOR=true

ONLY_FIRMWARE=
ONLY_COMMON=
ONLY_TARGET=
KANG=
SECTION=

while [ "${#}" -gt 0 ]; do
    case "${1}" in
        --only-common )
                ONLY_COMMON=true
                ;;
        --only-target )
                ONLY_TARGET=true
                ;;
        -n | --no-cleanup )
                CLEAN_VENDOR=false
                ;;
        -k | --kang )
                KANG="--kang"
                ;;
        -s | --section )
                SECTION="${2}"; shift
                CLEAN_VENDOR=false
                ;;
        * )
                SRC="${1}"
                ;;
    esac
    shift
done

if [ -z "${SRC}" ]; then
    SRC="adb"
fi

function blob_fixup() {
    case "${1}" in
        lib/libshowlogo.so)
            grep -q "libshim_showlogo.so" "${2}" || "${PATCHELF}" --add-needed "libshim_showlogo.so" "${2}"
            ;;
        lib/libsink.so)
            "${PATCHELF}" --add-needed libshim_vtservice.so "${2}"
            ;;
        lib64/libmtkavenhancements.so)
            grep -q "libshim_mtkavenhancements.so" "${2}" || "${PATCHELF}" --add-needed "libshim_mtkavenhancements.so" "${2}"
            ;;
        lib64/extractors/libmtkmkvextractor.so)
            grep -q "libshim_extractors.so" "${2}" || "${PATCHELF}" --add-needed "libshim_extractors.so" "${2}"
            ;;
        vendor/bin/hw/android.hardware.wifi@1.0-service-lazy-mediatek)
            "${PATCHELF}" --replace-needed "libwifi-hal.so" "libwifi-hal-mtk.so" "${2}"
            "${PATCHELF}" --add-needed "libcompiler_rt.so" "${2}"
            ;;
        vendor/bin/hw/hostapd)
            "${PATCHELF}" --add-needed "libcompiler_rt.so" "${2}"
            ;;
        vendor/bin/hw/wpa_supplicant)
            "${PATCHELF}" --add-needed "libcompiler_rt.so" "${2}"
            "${PATCHELF}" --replace-needed "libcrypto.so" "libcrypto-v33.so" "${2}"
            "${PATCHELF}" --replace-needed "libssl.so" "libssl-v33.so" "${2}"
            ;;
        lib/libsink.so)
            "${PATCHELF}" --add-needed libshim_vtservice.so "${2}"
            ;;
        vendor/lib*/libutinterface_custom_md.so)
            "${PATCHELF}" --add-needed libutinterface_md.so "${2}"
            ;;
        vendor/lib*/libudf.so)
            "${PATCHELF}" --replace-needed "libunwindstack.so" "libunwindstack-v30.so" "${2}"
            ;;
        vendor/lib/libgeofence.so)
            "${PATCHELF}" --add-needed libshim_gps.so "${2}"
            ;;
        vendor/lib/libmnl.so)
            "${PATCHELF}" --add-needed libshim_gps.so "${2}"
            ;;
        vendor/lib*/libmtkcam_stdutils.so)
            "${PATCHELF}" --replace-needed "libutils.so" "libutils-v30.so" "${2}"
            ;;
        vendor/lib*/libpixelflinger.so)
            "${PATCHELF}" --add-needed libshim_memset.so "${2}"
            ;;
        vendor/lib*/hw/audio.primary.mt6765.so)
            "${PATCHELF}" --replace-needed "libmedia_helper.so" "libmedia_helper-v29.so" "${2}"
            "${PATCHELF}" --replace-needed "libalsautils.so" "libalsautils-v29.so" "$2"
            ;;
        vendor/lib*/hw/vendor.mediatek.hardware.pq@2.3-impl.so)
            "${PATCHELF}" --replace-needed libutils.so libutils-v30.so "${2}"
            ;;
        vendor/lib*/hw/libSoftGatekeeper.so)
            "${PATCHELF}" --replace-needed "libgatekeeper.so" "libgatekeeper-v29.so" "${2}"
            ;;
        vendor/lib64/libmtkcam_ulog.so)
            "${PATCHELF}" --replace-needed "libunwindstack.so" "libunwindstack-v30.so" "${2}"
            "${PATCHELF}" --replace-needed libutils.so libutils-v30.so "${2}"
            ;;
        vendor/lib64/libcam.feature_utils.so|vendor/lib64/libcam.iopipe.so)
            "${PATCHELF}" --replace-needed "libunwindstack.so" "libunwindstack-v30.so" "${2}"
            "${PATCHELF}" --replace-needed libutils.so libutils-v30.so "${2}"
            ;;
        vendor/lib/libMtkOmxVdecEx.so)
            "${PATCHELF}" --replace-needed "libui.so" "libui-v32.so" "${2}"
            ;;
        vendor/lib*/libdlbdsservice.so|vendor/lib*/soundfx/libswdap.so)
            "${PATCHELF}" --replace-needed "libstagefright_foundation.so" "libstagefright_foundation-v33.so" "${2}"
        ;;
    esac
}


if [ -z "${ONLY_TARGET}" ]; then
    # Initialize the helper for common device
    setup_vendor "${DEVICE_COMMON}" "${VENDOR}" "${ANDROID_ROOT}" true "${CLEAN_VENDOR}"

    extract "${MY_DIR}/proprietary-files.txt" "${SRC}" "${KANG}" --section "${SECTION}"
fi

if [ -z "${ONLY_COMMON}" ] && [ -s "${MY_DIR}/../${DEVICE}/proprietary-files.txt" ]; then
    # Reinitialize the helper for device
    source "${MY_DIR}/../${DEVICE}/extract-files.sh"
    setup_vendor "${DEVICE}" "${VENDOR}" "${ANDROID_ROOT}" false "${CLEAN_VENDOR}"

    extract "${MY_DIR}/../${DEVICE}/proprietary-files.txt" "${SRC}" "${KANG}" --section "${SECTION}"
    extract_firmware "${MY_DIR}/proprietary-firmware.txt" "${SRC}"
fi

"${MY_DIR}/setup-makefiles.sh"
