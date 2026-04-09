#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: 2024 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from extract_utils.file import File
from extract_utils.fixups_blob import (
    BlobFixupCtx,
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.fixups_lib import (
    lib_fixup_remove,
    lib_fixups,
    lib_fixups_user_type,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)
from extract_utils.tools import (
    llvm_objdump_path,
)
from extract_utils.utils import (
    run_cmd,
)

namespace_imports = [
    "hardware/google/interfaces",
    "hardware/google/pixel",
    "hardware/mediatek",
    "hardware/mediatek/libmtkperf_client",
    "device/lenovo/mt6765-common/lights",
    "device/lenovo/mt6765-common/libshims",
    "hardware/lineage/interfaces/power-libperfmgr",
    "vendor/lenovo/amar_row_lte",
    "device/lenovo/mt6765-common/vndk",
]

blob_fixups: blob_fixups_user_type = {
    'lib/libshowlogo.so': blob_fixup()
        .add_needed('libshim_showlogo.so'),
    
    'lib/libsink.so': blob_fixup()
        .add_needed('libshim_vtservice.so'),
    
    'lib64/libmtkavenhancements.so': blob_fixup()
        .add_needed('libshim_mtkavenhancements.so'),
    
    'lib64/extractors/libmtkmkvextractor.so': blob_fixup()
        .add_needed('libshim_extractors.so'),
    
    'vendor/bin/hw/android.hardware.wifi@1.0-service-lazy-mediatek': blob_fixup()
        .replace_needed('libwifi-hal.so', 'libwifi-hal-mtk.so')
        .add_needed('libcompiler_rt.so'),
    
    ('vendor/bin/hw/hostapd', 'vendor/bin/hw/wpa_supplicant'): blob_fixup()
        .add_needed('libcompiler_rt.so'),
    
    'vendor/bin/hw/wpa_supplicant': blob_fixup()
        .replace_needed('libcrypto.so', 'libcrypto-v33.so')
        .replace_needed('libssl.so', 'libssl-v33.so'),
    
    ('vendor/lib/libudf.so', 'vendor/lib64/libudf.so'): blob_fixup()
        .replace_needed('libunwindstack.so', 'libunwindstack-v30.so'),
    
    ('vendor/lib/libgeofence.so', 'vendor/lib/libmnl.so'): blob_fixup()
        .add_needed('libshim_gps.so'),
    
    ('vendor/lib/libmtkcam_stdutils.so', 'vendor/lib64/libmtkcam_stdutils.so'): blob_fixup()
        .replace_needed('libutils.so', 'libutils-v30.so'),
    
    ('vendor/lib/libpixelflinger.so', 'vendor/lib64/libpixelflinger.so'): blob_fixup()
        .add_needed('libshim_memset.so'),
    
    ('vendor/lib/hw/audio.primary.mt6765.so', 'vendor/lib64/hw/audio.primary.mt6765.so'): blob_fixup()
        .replace_needed('libmedia_helper.so', 'libmedia_helper-v29.so')
        .replace_needed('libalsautils.so', 'libalsautils-v29.so'),
    
    ('vendor/lib/hw/vendor.mediatek.hardware.pq@2.3-impl.so', 'vendor/lib64/hw/vendor.mediatek.hardware.pq@2.3-impl.so'): blob_fixup()
        .replace_needed('libutils.so', 'libutils-v30.so'),
    
    ('vendor/lib/hw/libSoftGatekeeper.so', 'vendor/lib64/hw/libSoftGatekeeper.so'): blob_fixup()
        .replace_needed('libgatekeeper.so', 'libgatekeeper-v29.so'),
    
    ('vendor/lib/libmtkcam_ulog.so', 'vendor/lib64/libmtkcam_ulog.so', 'vendor/lib64/libcam.feature_utils.so', 'vendor/lib64/libcam.iopipe.so'): blob_fixup()
        .replace_needed('libunwindstack.so', 'libunwindstack-v30.so')
        .replace_needed('libutils.so', 'libutils-v30.so'),
    
    'vendor/lib/libMtkOmxVdecEx.so': blob_fixup()
        .replace_needed('libui.so', 'libui-v32.so'),
    
    ('vendor/lib/libdlbdsservice.so', 'vendor/lib64/libdlbdsservice.so', 'vendor/lib/soundfx/libswdap.so', 'vendor/lib64/soundfx/libswdap.so'): blob_fixup()
        .replace_needed('libstagefright_foundation.so', 'libstagefright_foundation-v33.so'),

    ('vendor/lib/libutinterface_custom_md.so', 'vendor/lib64/libutinterface_custom_md.so'): blob_fixup()
        .add_needed('libutinterface_md.so'),
    
    'vendor/lib/libvcodec_oal.so': blob_fixup()
        .clear_symbol_version('__aeabi_memcpy')
        .clear_symbol_version('__aeabi_memset')
        .clear_symbol_version('__gnu_Unwind_Find_exidx'),

    'vendor/lib/libvc1dec_sa.ca7.so': blob_fixup()
        .clear_symbol_version('__aeabi_memclr')
        .clear_symbol_version('__aeabi_memclr4')
        .clear_symbol_version('__aeabi_memcpy')
        .clear_symbol_version('__aeabi_memmove')
        .clear_symbol_version('__aeabi_memset')
        .clear_symbol_version('__gnu_Unwind_Find_exidx'),

    'vendor/lib/libmp4enc_sa.ca7.so': blob_fixup()
        .clear_symbol_version('__aeabi_memclr')
        .clear_symbol_version('__aeabi_memcpy')
        .clear_symbol_version('__aeabi_memset')
        .clear_symbol_version('__gnu_Unwind_Find_exidx'),

    'vendor/lib/libvp8dec_sa.ca7.so': blob_fixup()
        .clear_symbol_version('__aeabi_memcpy')
        .clear_symbol_version('__aeabi_memcpy4')
        .clear_symbol_version('__aeabi_memmove')
        .clear_symbol_version('__aeabi_memset')
        .clear_symbol_version('__gnu_Unwind_Find_exidx'),

    'vendor/lib/libthha.so': blob_fixup()
        .clear_symbol_version('__aeabi_memcpy')
        .clear_symbol_version('__aeabi_memset')
        .clear_symbol_version('__gnu_Unwind_Find_exidx'),

    ('vendor/lib/libwifi-hal-mtk.so', 'vendor/lib64/libwifi-hal-mtk.so'): blob_fixup()
        .fix_soname(),

    ('vendor/lib/libalsautils-v29.so', 'vendor/lib64/libalsautils-v29.so'): blob_fixup()
        .fix_soname(),
}

lib_fixups: lib_fixups_user_type = {
    'libprotobuf-cpp-lite-3.9.1': lambda lib, partition: f'{lib}-vendorcompat',
    'libprotobuf-cpp-full-3.9.1': lambda lib, partition: f'{lib}-vendorcompat',
    'libprotobuf-cpp-lite': lambda lib, partition: f'{lib}-vendorcompat',
    'libprotobuf-cpp-full': lambda lib, partition: f'{lib}-vendorcompat',
    'libclang_rt.ubsan_standalone-aarch64-android': lambda lib, partition: f'{lib}-v32',
}

module = ExtractUtilsModule(
    'mt6765-common',
    'lenovo',
    blob_fixups=blob_fixups,
    lib_fixups=lib_fixups,
    namespace_imports=namespace_imports,
    add_firmware_proprietary_file=True,
)

if __name__ == '__main__':
    utils = ExtractUtils.device(module)
    utils.run()