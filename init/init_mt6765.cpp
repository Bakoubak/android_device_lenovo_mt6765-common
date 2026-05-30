/*
   Copyright (c) 2014, The Linux Foundation. All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>

#include <android-base/properties.h>
#include "vendor_init.h"

using android::base::GetProperty;
using android::base::ReadFileToString;

void property_override(char const prop[], char const value[])
{
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void check_variant_and_set_props() {
    if (access("/dev/ccci_aud", F_OK) == 0) {
        android::base::SetProperty("ro.boot.radio.type", "lte");
    } else {
        android::base::SetProperty("ro.boot.radio.type", "wifi");
        
        android::base::SetProperty("ro.radio.noril", "yes");
	android::base::SetProperty("vendor.ril.mtk", "0");
    }
}

void set_go_default_props() {
    struct sysinfo sys;
    sysinfo(&sys);

    if (sys.totalram <= 3072ull * 1024 * 1024) {
        property_override("ro.config.low_ram", "true");
        property_override("ro.config.low_ram2g", "true");
        property_override("ro.lmk.use_psi", "true");
        property_override("ro.lmk.use_minfree_levels", "false");

        property_override("ro.lmk.kill_heaviest_task", "false");

        property_override("ro.lmk.swap_util_max", "96");
        property_override("ro.lmk.swap_free_low_percentage", "5");

        property_override("ro.lmk.thrashing_limit", "100");
        property_override("ro.lmk.thrashing_limit_decay", "25");
        property_override("ro.launcher.blur.appLaunch", "0");
        property_override("ro.surface_flinger.supports_background_blur", "0");
        property_override("ro.sf.blurs_are_expensive", "1");
        property_override("persist.sys.sf.disable_blurs", "1");
        property_override("ro.sf.blurs_are_caro", "1");
        property_override("fw.max_users", "1");
        property_override("fw.power_user_switcher", "0");
        property_override("fw.show_guest_on_lockscreen", "0");
        property_override("fw.show_hidden_users", "0");
        property_override("fw.show_multiuserui", "0");
        property_override("fw.show_user_switcher_on_lockscreen", "0");
        property_override("fw.user_creation_disabled", "1");

        property_override("ro.HOME_APP_ADJ", "0");

        property_override("dalvik.vm.heapstartsize", "8m");
        property_override("dalvik.vm.heapgrowthlimit", "128m");
        property_override("dalvik.vm.heapsize", "192m");
        property_override("dalvik.vm.heaptargetutilization", "0.75");
        property_override("dalvik.vm.heapminfree", "1m");
        property_override("dalvik.vm.heapmaxfree", "8m");

        property_override("ro.config.small_battery", "true");
    }
    else
    {
        property_override("ro.launcher.blur.appLaunch", "0");
        property_override("ro.surface_flinger.supports_background_blur", "0");
        property_override("ro.sf.blurs_are_expensive", "1");
	property_override("ro.lmk.min_adj", "0");
	property_override("ro.lmk.psi_critical_stall_ms", "70");
        property_override("ro.lmk.use_psi", "false");
        property_override("ro.lmk.use_minfree_levels", "true");
	property_override("ro.lmk.psi_low", "10");
	property_override("ro.lmk.psi_medium", "20");
	property_override("ro.lmk.psi_critical", "25");
        property_override("ro.lmk.kill_heaviest_task", "true");
        property_override("ro.lmk.swap_util_max", "80");
        property_override("ro.lmk.swap_free_low_percentage", "5");
        property_override("ro.lmk.thrashing_limit", "30");
        property_override("ro.lmk.thrashing_limit_decay", "50");
    }
}

void vendor_load_properties() {
    set_go_default_props();
    check_variant_and_set_props();
}
