#include "decoder.h"

#include <iostream>
#include <libavformat/avformat.h>

namespace rgbd_streamer
{
ColorDecoder::ColorDecoder(AVCodecID av_codec_id)
    : packet_(nullptr), codec_(nullptr), codec_parser_context_(nullptr), codec_context_(nullptr)
{
    av_log_set_level(AV_LOG_INFO);

    packet_ = av_packet_alloc();
    if (!packet_) {
        std::cout << "av_packet_alloc failed" << std::endl;
        return;
    }

    avcodec_register_all();
    codec_ = avcodec_find_decoder(av_codec_id);
    if (!codec_) {
        std::cout << "avcodec_find_decoder failed" << std::endl;
        return;
    }

    codec_parser_context_ = av_parser_init(codec_->id);
    if (!codec_parser_context_) {
        std::cout << "codec_parser_context not found" << std::endl;
        return;
    }

    codec_context_ = avcodec_alloc_context3(codec_);
    if (!codec_context_) {
        std::cout << "avcodec_alloc_context3 failed" << std::endl;
        return;
    }

    /* open it */
    if (avcodec_open2(codec_context_, codec_, nullptr) < 0) {
        std::cout << "avcodec_open2 failed" << std::endl;
    }
}

ColorDecoder::~ColorDecoder()
{
    if (packet_)
        av_packet_free(&packet_);

    if (codec_parser_context_)
        av_parser_close(codec_parser_context_);

    if (codec_context_)
        avcodec_free_context(&codec_context_);
}

void decodePacket(std::vector<ColorDecoderFrame>& output, AVCodecContext* codec_context, AVPacket* pkt)
{
    int send_packet_result = avcodec_send_packet(codec_context, pkt);
    if (send_packet_result < 0) {
        //-1094995529 means AVERROR_INVALIDDATA
        std::cout << "avcodec_send_packet failed in FfmpegDecoder::decodePacket... error code: " << send_packet_result << std::endl;
        throw std::exception("avcodec_send_packet failed in FfmpegDecoder::decodePacket");
    }

    while (true) {
        ColorDecoderFrame av_frame(av_frame_alloc());
        if (!av_frame.av_frame()) {
            throw std::exception("av_frame_alloc failed in FfmpegDecoder::decodePacket...");
        }
        av_frame.av_frame()->format = AV_PIX_FMT_YUV420P;

        int receive_frame_result = avcodec_receive_frame(codec_context, av_frame.av_frame());
        if (receive_frame_result == AVERROR(EAGAIN) || receive_frame_result == AVERROR_EOF) {
            return;
        }
        else if (receive_frame_result < 0) {
            std::cout << "avcodec_receive_frame failed in FfmpegDecoder::decodePacket... error code: " << receive_frame_result << std::endl;
            throw std::exception("avcodec_send_packet failed in FfmpegDecoder::decodePacket");
        }

        fflush(stdout);

        /* the picture is allocated by the decoder. no need to free it */
        output.push_back(std::move(av_frame));
    }

    return;
}

ColorDecoderFrame ColorDecoder::decode(const std::vector<uint8_t>& av_frame)
{
    std::vector<ColorDecoderFrame> output_frames;
    /* use the parser to split the data into frames */
    size_t data_size = av_frame.size();
    // Adding a buffer padding is important!
    // Removing this results in crashes that happens in a way hard to debug!!! (I know it since it happened to me...)
    // When it happens, it happens with av_parser_parse2.
    std::unique_ptr<uint8_t> padded_data(new uint8_t[data_size + AV_INPUT_BUFFER_PADDING_SIZE]);
    memcpy(padded_data.get(), av_frame.data(), data_size);
    memset(padded_data.get() + data_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    uint8_t* data = padded_data.get();

    while (data_size > 0) {
        // Returns the number of bytes used.
        int size = av_parser_parse2(codec_parser_context_,
            codec_context_,
            &packet_->data,
            &packet_->size,
            data,
            data_size,
            AV_NOPTS_VALUE,
            AV_NOPTS_VALUE,
            0);
        if (size < 0) {
            std::cout << "Error while parsing" << std::endl;
            throw std::exception("Error while parsing");
        }
        data += size;
        data_size -= size;

        if (packet_->size)
            decodePacket(output_frames, codec_context_, packet_);

        if (codec_->capabilities & AV_CODEC_CAP_DELAY) {
            //Read the following comment which came from the comments above AV_CODEC_CAP_DELAY to understand why the following code is here.
            // * Decoders:
            // * The decoder has a non-zero delay and needs to be fed with avpkt->data=NULL,
            // * avpkt->size=0 at the end to get the delayed data until the decoder no longer
            // * returns frames.
            if (data_size == 0) {
                size = av_parser_parse2(codec_parser_context_,
                    codec_context_,
                    &packet_->data,
                    &packet_->size,
                    nullptr,
                    0,
                    AV_NOPTS_VALUE,
                    AV_NOPTS_VALUE,
                    0);

                if (size < 0) {
                    std::cout << "Error while parsing" << std::endl;
                    throw std::exception("Error while parsing");
                }

                decodePacket(output_frames, codec_context_, packet_);
            }
        }
    }

    if (output_frames.size() != 1) {
        std::cout << "Found " << output_frames.size() << " frames while there should be one." << std::endl;
        throw std::exception("Found non-single frames.");
    }

    return std::move(output_frames[0]);
}

std::unique_ptr<ColorDecoder> createColorDecoder()
{
    return std::make_unique<ColorDecoder>(AVCodecID::AV_CODEC_ID_VP8);
}
}