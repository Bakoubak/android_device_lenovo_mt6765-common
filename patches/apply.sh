#!/bin/bash
cd vendor/lineage
patch -p1 < ../../device/lenovo/mt6765-common/patches/vendor/lineage/0001-PATCH-Permit-building-with-a-prebuilt-kernel.patch
cd ../..
cd packages/modules/Connectivity
patch -p1 < ../../../device/lenovo/mt6765-common/patches/packages/modules/Connectivity/0001-PATCH-Allow-booting-on-4.9-kernels.patch
