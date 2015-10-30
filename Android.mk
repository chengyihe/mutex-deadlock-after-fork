LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := MutexDeadlockAfterFork.cpp
LOCAL_STATIC_LIBRARIES += \
	libcutils \
	libutils \
	liblog

LOCAL_MODULE := mutex-deadlock-after-fork
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
