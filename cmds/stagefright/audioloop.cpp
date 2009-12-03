#include "SineSource.h"

#include <binder/ProcessState.h>
#include <media/stagefright/AudioPlayer.h>
#include <media/stagefright/MediaDebug.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/OMXClient.h>
#include <media/stagefright/OMXCodec.h>

using namespace android;

int main() {
    // We only have an AMR-WB encoder on sholes...
    static bool outputWBAMR = false;
    static const int32_t kSampleRate = outputWBAMR ? 16000 : 8000;
    static const int32_t kNumChannels = 1;

    android::ProcessState::self()->startThreadPool();

    OMXClient client;
    CHECK_EQ(client.connect(), OK);

    sp<MediaSource> source = new SineSource(kSampleRate, kNumChannels);

    sp<MetaData> meta = new MetaData;

    meta->setCString(
            kKeyMIMEType,
            outputWBAMR ? MEDIA_MIMETYPE_AUDIO_AMR_WB
                        : MEDIA_MIMETYPE_AUDIO_AMR_NB);

    meta->setInt32(kKeyChannelCount, kNumChannels);
    meta->setInt32(kKeySampleRate, kSampleRate);

    int32_t maxInputSize;
    if (source->getFormat()->findInt32(kKeyMaxInputSize, &maxInputSize)) {
        meta->setInt32(kKeyMaxInputSize, maxInputSize);
    }

    sp<OMXCodec> encoder = OMXCodec::Create(
            client.interface(),
            meta, true /* createEncoder */,
            source);

    sp<OMXCodec> decoder = OMXCodec::Create(
            client.interface(),
            meta, false /* createEncoder */,
            encoder);

#if 1
    AudioPlayer *player = new AudioPlayer(NULL);
    player->setSource(decoder);

    player->start();

    sleep(10);

    player->stop();

    delete player;
    player = NULL;
#else
    CHECK_EQ(decoder->start(), OK);

    MediaBuffer *buffer;
    while (decoder->read(&buffer) == OK) {
        // do something with buffer

        putchar('.');
        fflush(stdout);

        buffer->release();
        buffer = NULL;
    }

    CHECK_EQ(decoder->stop(), OK);
#endif

    return 0;
}