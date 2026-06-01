#include <media/AudioTrack.h>
#include <gui/IGraphicBufferProducer.h>
#include <media/stagefright/MediaMuxer.h>
#include <map>
#include <mutex>

using namespace android;

typedef void (*legacy_callback_t)(int event, void* user, void* info);

class ShimLegacyCallbackWrapper : public android::AudioTrack::IAudioTrackCallback {
private:
    legacy_callback_t mCallback;
    void* mData;
public:
    ShimLegacyCallbackWrapper(legacy_callback_t callback, void* user)
        : mCallback(callback), mData(user) {}

    size_t onMoreData(const android::AudioTrack::Buffer& buffer) override {
        android::AudioTrack::Buffer copy = buffer;
        mCallback(android::AudioTrack::EVENT_MORE_DATA, mData, static_cast<void*>(&copy));
        return copy.size();
    }
    void onUnderrun() override {
        mCallback(android::AudioTrack::EVENT_UNDERRUN, mData, nullptr);
    }
    void onLoopEnd(int32_t loopsRemaining) override {
        mCallback(android::AudioTrack::EVENT_LOOP_END, mData, &loopsRemaining);
    }
    void onMarker(uint32_t markerPosition) override {
        mCallback(android::AudioTrack::EVENT_MARKER, mData, &markerPosition);
    }
    void onNewPos(uint32_t newPos) override {
        mCallback(android::AudioTrack::EVENT_NEW_POS, mData, &newPos);
    }
    void onBufferEnd() override {
        mCallback(android::AudioTrack::EVENT_BUFFER_END, mData, nullptr);
    }
    void onNewIAudioTrack() override {
        mCallback(android::AudioTrack::EVENT_NEW_IAUDIOTRACK, mData, nullptr);
    }
    void onStreamEnd() override {
        mCallback(android::AudioTrack::EVENT_STREAM_END, mData, nullptr);
    }
    size_t onCanWriteMoreData(const android::AudioTrack::Buffer& buffer) override {
        android::AudioTrack::Buffer copy = buffer;
        mCallback(android::AudioTrack::EVENT_CAN_WRITE_MORE_DATA, mData, static_cast<void*>(&copy));
        return copy.size();
    }
};

static std::map<void*, sp<ShimLegacyCallbackWrapper>> gCallbackLockMap;
static std::mutex gCallbackLockMutex;

extern "C" {
    void _ZN7android10AudioTrackC1E19audio_stream_type_tj14audio_format_t20audio_channel_mask_tj20audio_output_flags_tRKNS_2wpINS0_19IAudioTrackCallbackEEEi15audio_session_tNS0_13transfer_typeEPK20audio_offload_info_tRKNS_7content22AttributionSourceStateEPK18audio_attributes_tbfi(
            void* thisptr,
            audio_stream_type_t streamType,
            uint32_t sampleRate,
            audio_format_t format,
            audio_channel_mask_t channelMask,
            size_t frameCount,
            audio_output_flags_t flags,
            const wp<android::AudioTrack::IAudioTrackCallback>& callbackWeak,
            int32_t notificationFrames,
            audio_session_t sessionId,
            android::AudioTrack::transfer_type transferType,
            const audio_offload_info_t *offloadInfo,
            const android::content::AttributionSourceState& attributionSource,
            const audio_attributes_t* pAttributes,
            bool doNotReconnect,
            float maxRequiredSpeed,
            audio_port_handle_t selectedDeviceId);

    void _ZN7android10AudioTrackC1E19audio_stream_type_tj14audio_format_tjj20audio_output_flags_tPFviPvS4_ES4_i15audio_session_tNS0_13transfer_typeEPK20audio_offload_info_tjiPK18audio_attributes_tbfi(
            void* thisptr,
            audio_stream_type_t streamType,
            uint32_t sampleRate,
            audio_format_t format,
            audio_channel_mask_t channelMask,
            size_t frameCount,
            audio_output_flags_t flags,
            legacy_callback_t cbf,
            void* user,
            int32_t notificationFrames,
            audio_session_t sessionId,
            android::AudioTrack::transfer_type transferType,
            const audio_offload_info_t *offloadInfo,
            uid_t uid,
            pid_t pid,
            const audio_attributes_t* pAttributes,
            bool doNotReconnect,
            float maxRequiredSpeed,
            audio_port_handle_t selectedDeviceId) {

        android::content::AttributionSourceState attributionSource;
        attributionSource.uid = uid;
        attributionSource.pid = pid;
        attributionSource.token = sp<BBinder>::make();

        sp<ShimLegacyCallbackWrapper> modernCallback = nullptr;
        if (cbf) {
            modernCallback = sp<ShimLegacyCallbackWrapper>::make(cbf, user);
            
            std::lock_guard<std::mutex> lock(gCallbackLockMutex);
            gCallbackLockMap[thisptr] = modernCallback;
        }

        wp<android::AudioTrack::IAudioTrackCallback> callbackWeak(modernCallback);

        _ZN7android10AudioTrackC1E19audio_stream_type_tj14audio_format_t20audio_channel_mask_tj20audio_output_flags_tRKNS_2wpINS0_19IAudioTrackCallbackEEEi15audio_session_tNS0_13transfer_typeEPK20audio_offload_info_tRKNS_7content22AttributionSourceStateEPK18audio_attributes_tbfi(
                     thisptr, streamType, sampleRate, format, channelMask, frameCount, flags, callbackWeak,
                     notificationFrames, sessionId, transferType, offloadInfo,
                     attributionSource, pAttributes,
                     doNotReconnect, maxRequiredSpeed, selectedDeviceId);
    }

    void _ZN7android10AudioTrackD1Ev(void* thisptr) {
        std::lock_guard<std::mutex> lock(gCallbackLockMutex);
        gCallbackLockMap.erase(thisptr);
    }

    void _ZN7android11AudioSourceC1EPK18audio_attributes_tRKNS_8String16Ejjjjii28audio_microphone_direction_tf(const audio_attributes_t *attr, const String16 &opPackageName, uint32_t sampleRate, uint32_t channelCount, uint32_t outSampleRate, uid_t uid, pid_t pid, audio_port_handle_t selectedDeviceId, audio_microphone_direction_t selectedMicDirection, float selectedMicFieldDimension);

    void _ZN7android11AudioSourceC1E14audio_source_tRKNS_8String16Ejjjjii28audio_microphone_direction_tf(__attribute__((unused)) audio_source_t inputSource, const String16 &opPackageName, uint32_t sampleRate, uint32_t channelCount, uint32_t outSampleRate, uid_t uid, pid_t pid, audio_port_handle_t selectedDeviceId, audio_microphone_direction_t selectedMicDirection, float selectedMicFieldDimension) {
         audio_attributes_t attr = AUDIO_ATTRIBUTES_INITIALIZER;
         attr.source = AUDIO_SOURCE_DEFAULT;
         _ZN7android11AudioSourceC1EPK18audio_attributes_tRKNS_8String16Ejjjjii28audio_microphone_direction_tf(&attr, opPackageName, sampleRate, channelCount, outSampleRate, uid, pid, selectedDeviceId, selectedMicDirection, selectedMicFieldDimension);
    }

    void _ZN7android7SurfaceC1ERKNS_2spINS_22IGraphicBufferProducerEEEbRKNS1_INS_7IBinderEEE(void* thisptr, const sp<IGraphicBufferProducer>& bufferProducer, bool controlledByApp, const sp<IBinder>& surfaceControlHandle);

    void _ZN7android7SurfaceC1ERKNS_2spINS_22IGraphicBufferProducerEEEb(void* thisptr, const sp<IGraphicBufferProducer> &bufferProducer, bool controlledByApp) {
         _ZN7android7SurfaceC1ERKNS_2spINS_22IGraphicBufferProducerEEEbRKNS1_INS_7IBinderEEE(thisptr, bufferProducer, controlledByApp, nullptr);
    }

    void _ZN7android10MediaMuxerC1EiNS_14MediaMuxerBase12OutputFormatE(void* thisptr, int fd, android::MediaMuxer::OutputFormat format);

    void _ZN7android10MediaMuxerC1EiNS0_12OutputFormatE(void* thisptr, int fd, android::MediaMuxer::OutputFormat format) {
         _ZN7android10MediaMuxerC1EiNS_14MediaMuxerBase12OutputFormatE(thisptr, fd, format);
    }
}
