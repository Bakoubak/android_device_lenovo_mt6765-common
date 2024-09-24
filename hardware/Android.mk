LOCAL_PATH := $(my-dir)

ifeq ($(BOARD_HAS_MTK_HARDWARE),true)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
