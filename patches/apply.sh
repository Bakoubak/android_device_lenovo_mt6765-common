#!/bin/bash
cd vendor/lineage
patch -p1 < ../../device/lenovo/mt6765-common/patches/vendor/lineage/0001-vendor-lineage-build-soong-Let-the-user-build-withou.patch
cd ../..
cd packages/modules/Connectivity
patch -p1 < ../../../device/lenovo/mt6765-common/patches/packages/modules/Connectivity/0001-connectivity-clat_coordinator-Disable-SELinux-contex.patch
