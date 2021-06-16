/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "ffparse.h"
#include <thread>



#include "ff/ff.h"
#include "ff/mediacapture.h"
#include "base/define.h"
#include "base/test.h"


#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>

#define MAX_CHUNK_SIZE 10240*8
// maximum send buffer 262144  =1024 *256

#define highWaterMark  8 * 1048576
//maximum buffer = 16 *1048576 where  1024*1024 =1048576




#define IOBUFSIZE 40960
//40960*6

namespace base {
    namespace fmp4 {


        // based on https://ffmpeg.org/doxygen/trunk/remuxing_8c-example.html

        static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt, const char *tag) {
            AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

            // printf("%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
            //        tag,
            //        av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
            //        av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
            //        av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
            //        pkt->stream_index);
        }

        int mediaMuxCallback(void *opaque, uint8_t *buf, int bufSize) {
            //outputData.insert(outputData.end(), buf, buf + bufSize);
            FFParse *obj = (FFParse *) opaque;

          //  obj->pc->sendDataBinary((const uint8_t *) buf, bufSize);
           // std::this_thread::sleep_for(std::chrono::milliseconds(15));

            //  obj->outputData.insert(obj->outputData.end(), buf, buf + bufSize);
            //static size_t kMaxQueuedSendDataBytes = 16 * 1024 * 1024;

            //   if(obj->pc->data_channel_->buffered_amount()  <  1024 )
            //       std::this_thread::sleep_for(std::chrono::milliseconds(5));
            //   
            //   else( obj->pc->data_channel_->buffered_amount()  >  8 * 1024 * 1024 )
            //       

            return bufSize;
        }

        FFParse::FFParse() :  fragmp4_filter("fragmp4"), fragmp4_muxer("fragmp4muxer", &fragmp4_filter), info("info", &fragmp4_muxer) {

            fragmp4_muxer.activate();

        }

        FFParse::~FFParse() {
            SInfo << "~FFParse( )";
        }

        /*
        void FFParse::run() {

            int64_t startTime = time::hrtime();



            std::ifstream bunnyFile;
            bunnyFile.open("/var/tmp/test.mp4", std::ios_base::in | std::ios_base::binary);

            char buf[ MAX_CHUNK_SIZE];

            memset(buf, 'A', MAX_CHUNK_SIZE);

            while (!stopped()) {


                // dc->sendDataMsg("ravind");

                pc->sendDataBinary((const uint8_t *) buf, MAX_CHUNK_SIZE);

                do {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                     SInfo << "slee for 5 mlsec";
                } while (pc->data_channel_->buffered_amount() > highWaterMark);

                // while( pc->data_channel_->buffered_amount()  > highWaterMark )
                // std::this_thread::sleep_for(std::chrono::milliseconds(10));

                int64_t lastTimestamp = time::hrtime(); //nanosecondtime
                auto nsdelay = lastTimestamp - startTime;
                

                SInfo << "Sent message seed MByte " <<  (pc->data_channel_->bytes_sent()*1000)/nsdelay ;
            }





            SInfo << "fmp4 thread exit";

            // fileName = "/var/tmp/videos/test.mp4";
            fileName = "/var/tmp/kunal720.mp4";
            //fmp4(fileName.c_str(), "fragTmp.mp4");
            //fmp4(fileName.c_str());
        }
         */

        void FFParse::run() {


            parseH264("/experiment/live/testProgs/test.264");
            //fmp4("/experiment/fmp4/test.264", "fragTmp.mp4");
//            
//                        std::ifstream bunnyFile;
//                        bunnyFile.open("/tmp/output2.mp4", std::ios_base::in | std::ios_base::binary);
//            
//                        char buf[ MAX_CHUNK_SIZE];
//            
//                        while (bunnyFile.good() && !stopped() ) {
//                          bunnyFile.read(buf,  MAX_CHUNK_SIZE);
//                          int nRead = bunnyFile.gcount();
//                          if (nRead > 0) {
//                           // dc->sendDataMsg("ravind");
//            
//                            pc->sendDataBinary((const uint8_t *)buf, nRead);
//            
//                            do
//                            {
//                                std::this_thread::sleep_for(std::chrono::milliseconds(15));
//                            } while( pc->data_channel_->buffered_amount()  >  highWaterMark );
//                    
//                            // while( pc->data_channel_->buffered_amount()  >  12 * 1024 * 1024 )
//                             // std::this_thread::sleep_for(std::chrono::milliseconds(10));
//                    
//                          }
//            
//                          SInfo << "Sent message of size " << nRead ;
//                        }


            SInfo << "fmp4 thread exit";

            // fileName = "/var/tmp/videos/test.mp4";
            fileName = "/var/tmp/kunal720.mp4";
            //fmp4(fileName.c_str(), "fragTmp.mp4");
            //fmp4(fileName.c_str());
        }

        int FFParse::fmp4(const char *in_filename, const char *out_filename, bool fragmented_mp4_options) {

            const AVOutputFormat *ofmt = NULL;
            AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
            AVPacket pkt;

            int ret, i;
            int stream_index = 0;
            int *stream_mapping = NULL;
            int stream_mapping_size = 0;


            if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
                SError << "Could not open input file " << in_filename;
                return -1;
            }

            if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
                SError << "Failed to retrieve input stream information";
                return -1;
            }

            av_dump_format(ifmt_ctx, 0, in_filename, 0);

            ////////////////////////////////////////////////////////////////////// for file 

            //    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
            //    if (!ofmt_ctx) {
            //        fprintf(stderr, "Could not create output context\n");
            //        ret = AVERROR_UNKNOWN;
            //        goto end;
            //    }


            ////////////////////////////////////////////////////////////////////////for buffer 
            std::fstream file;
            if (out_filename) {
                file.open(out_filename, std::ios::out | std::ios::binary);
                if (!file.is_open()) {
                    std::cout << "Couldn't open file " << out_filename << std::endl;
                    return 2;
                }
            }


            AVIOContext *ioCtxt;
            uint8_t *ioBuffer;

            AVOutputFormat *outputFormat = av_guess_format("mp4", nullptr, nullptr);


            avformat_alloc_output_context2(&ofmt_ctx, outputFormat, NULL, NULL);
            if (!ofmt_ctx) {
                SError << "Could not create output context";
                ret = AVERROR_UNKNOWN;
                // goto end;
                return -1;
            }


            if ((ioBuffer = (uint8_t*) av_malloc(IOBUFSIZE)) == nullptr) {
                SError << "Couldn't allocate I/O buffer";
                return 8;
            }
            if ((ioCtxt = avio_alloc_context(ioBuffer, IOBUFSIZE, 1, (void*) this, nullptr, mediaMuxCallback, nullptr)) == nullptr) {
                SError << "Couldn't initialize I/O context";
                return 9;
            }

            //Set video stream data

            ofmt_ctx->pb = ioCtxt;

            /////////////////////////////////////////////////////////////////////////////////

            stream_mapping_size = ifmt_ctx->nb_streams;
            stream_mapping = (int*) av_mallocz_array(stream_mapping_size, sizeof (*stream_mapping));
            if (!stream_mapping) {
                ret = AVERROR(ENOMEM);
                return -1;
            }

            ofmt = ofmt_ctx->oformat;

            for (i = 0; i < ifmt_ctx->nb_streams; i++) {
                AVStream *out_stream;
                AVStream *in_stream = ifmt_ctx->streams[i];
                AVCodecParameters *in_codecpar = in_stream->codecpar;

                if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
                        in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
                        in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
                    stream_mapping[i] = -1;
                    continue;
                }

                stream_mapping[i] = stream_index++;

                out_stream = avformat_new_stream(ofmt_ctx, NULL);
                if (!out_stream) {
                    SError << "Failed allocating output stream";
                    ret = AVERROR_UNKNOWN;
                    return -1;
                }

                ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
                if (ret < 0) {
                    SError << "Failed to copy codec parameters";
                    return -1;
                }
                out_stream->codecpar->codec_tag = 0;
            }
            av_dump_format(ofmt_ctx, 0, out_filename, 1);

            //    if (!(ofmt->flags & AVFMT_NOFILE)) {
            //        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
            //        if (ret < 0) {
            //            fprintf(stderr, "Could not open output file '%s'", out_filename);
            //            goto end;
            //        }
            //    }

            AVDictionary* opts = NULL;

            if (fragmented_mp4_options) {
                // https://developer.mozilla.org/en-US/docs/Web/API/Media_Source_Extensions_API/Transcoding_assets_for_MSE
                av_dict_set(&opts, "movflags", "frag_keyframe+empty_moov+default_base_moof", 0);
            }
            // 

            ret = avformat_write_header(ofmt_ctx, &opts);
            if (ret < 0) {
                SError << "Error occurred when opening output file";
                return -1;
            }

            while (!stopped()) {
                AVStream *in_stream, *out_stream;

                ret = av_read_frame(ifmt_ctx, &pkt);
                if (ret < 0)
                    break;

                in_stream = ifmt_ctx->streams[pkt.stream_index];
                if (pkt.stream_index >= stream_mapping_size ||
                        stream_mapping[pkt.stream_index] < 0) {
                    av_packet_unref(&pkt);
                    continue;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));


                pkt.stream_index = stream_mapping[pkt.stream_index];
                out_stream = ofmt_ctx->streams[pkt.stream_index];
                log_packet(ifmt_ctx, &pkt, "in");

                /* copy packet */
                pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, static_cast<AVRounding> (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, static_cast<AVRounding> (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
                pkt.pos = -1;
                log_packet(ofmt_ctx, &pkt, "out");

                ret = av_write_frame(ofmt_ctx, &pkt);
                if (ret < 0) {
                    SError << "Error muxing packet";
                    break;
                }
                av_packet_unref(&pkt);

                while (outputData.size() > 1024) {
                   // pc->sendDataBinary(&outputData[0], 1024);
                    outputData.erase(outputData.begin(), outputData.begin() + 1024);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                // obj->outputData.insert(obj->outputData.end(), buf, buf + bufSize);

            }

            //  av_write_trailer(ofmt_ctx);

            ret = av_write_frame(ofmt_ctx, nullptr); //Flush if something has been left

            if (outputData.size()) {
//                pc->sendDataBinary(&outputData[0], outputData.size());
                outputData.erase(outputData.begin(), outputData.begin() + outputData.size());
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            if (out_filename) {
                // Write media data in container to file
                file.open("my_mp4.mp4", std::ios::out | std::ios::binary);
                if (!file.is_open()) {
                    std::cout << "Couldn't open output file " << std::endl;
                    return 12;
                }

                file.write((char*) outputData.data(), outputData.size());
                if (file.fail()) {
                    std::cout << "Couldn't write to file" << std::endl;
                    return 13;
                }


                file.close();
            }




            avformat_close_input(&ifmt_ctx);

            ///////////////// for buffer
            av_free(ioBuffer);
            av_free(ioCtxt);
            ////////////////////
            /////////////////for enabl following
            /* close output */
            //   if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
            //        avio_closep(&ofmt_ctx->pb);


            avformat_free_context(ofmt_ctx);

            av_freep(&stream_mapping);

            if (ret < 0 && ret != AVERROR_EOF) {
                //fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
                return 1;
            }

            SInfo << "exited fMp4 thread";

            return 0;
        }

        void FFParse::decode(AVCodecContext *cdc_ctx, AVFrame *frame, AVPacket *pkt, FILE *fp_out) {
            int ret = 0;
            int y;

            if ((ret = avcodec_send_packet(cdc_ctx, pkt)) < 0) {
                fprintf(stderr, "avcodec_send_packet failed.\n");
                //exit(1);
                return;
            }

            while ((ret = avcodec_receive_frame(cdc_ctx, frame)) >= 0) {
                printf("Write 1 frame.\n");

                for (y = 0; y < frame->height; y++)
                    fwrite(&frame->data[0][y * frame->linesize[0]], 1, frame->width, fp_out);
                for (y = 0; y < frame->height / 2; y++)
                    fwrite(&frame->data[1][y * frame->linesize[1]], 1, frame->width / 2, fp_out);
                for (y = 0; y < frame->height / 2; y++)
                    fwrite(&frame->data[2][y * frame->linesize[2]], 1, frame->width / 2, fp_out);
            }

            if ((ret != AVERROR(EAGAIN)) && (ret != AVERROR_EOF)) {
                fprintf(stderr, "avcodec_receive_packet failed.\n");
                return;
            }
        }

        void FFParse::parseH264(const char *input_file) {
            int ret = 0;
            AVCodec *codec = NULL;
            AVCodecContext *cdc_ctx = NULL;
            AVPacket *pkt = NULL;
            AVFrame *frame = NULL;
            FILE *fp_in, *fp_out;
            AVFormatContext *fmt_ctx = NULL;
            AVCodecParserContext *parser = NULL;

            if ((codec = avcodec_find_decoder(AV_CODEC_ID_H264)) == NULL) {
                fprintf(stderr, "avcodec_find_decoder failed.\n");
                //  goto ret1;
                return;
            }

            if ((cdc_ctx = avcodec_alloc_context3(codec)) == NULL) {
                fprintf(stderr, "avcodec_alloc_context3 failed.\n");
                // goto ret1;
                return;
            }

            if ((ret = avcodec_open2(cdc_ctx, codec, NULL)) < 0) {
                fprintf(stderr, "avcodec_open2 failed.\n");
                // goto ret2;
                return;
            }

            if ((pkt = av_packet_alloc()) == NULL) {
                fprintf(stderr, "av_packet_alloc failed.\n");
                //goto ret3;
                return;
            }

            if ((frame = av_frame_alloc()) == NULL) {
                fprintf(stderr, "av_frame_alloc failed.\n");
                //goto ret4;
                return;
            }

            if ((fp_in = fopen(input_file, "rb")) == NULL) {
                fprintf(stderr, "fopen %s failed.\n", input_file);
                // goto ret7;
                return;
            }

            if ((parser = av_parser_init(codec->id)) == NULL) {
                fprintf(stderr, "av_parser_init failed.\n");
                //goto ret8;
                return;
            }

            
             u_int8_t*         fReceiveBuffer;
            long unsigned     nbuf;       ///< Size of bytebuffer

            char*             fStreamId;
           // FrameFilter&      framefilter;
            SetupFrame        setupframe;  ///< This frame is used to send subsession information
            BasicFrame        basicframe;  ///< Data is being copied into this frame
            int               subsession_index;

            while (feof(fp_in) == 0) {
                char inbuf[1024] = {0};
                const uint8_t *data = (const uint8_t *) inbuf;
                int data_size = 0;

                data_size = fread(inbuf, 1, 1024, fp_in);

                while (data_size > 0) {
                    /*Only input video data*/
                    if ((ret = av_parser_parse2(parser, cdc_ctx, &pkt->data, &pkt->size,
                            data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0)) < 0)
                    {
                        fprintf(stderr, "av_parser_parse2 failed.\n");
                        //goto ret8;
                        return;
                    }

                    // avcodec_decode_video2

                    data += ret;
                    data_size -= ret;

                    if (pkt->size > 0) {
                        
                        basicframe.copyFromAVPacket(pkt);
                        
                        info.run(&basicframe);
                        int x = 0;
                       // decode(cdc_ctx, frame, pkt, fp_out);
                    }
                }

            }

            fclose(fp_in);
            av_frame_free(&frame);
            av_packet_free(&pkt);
            avcodec_close(cdc_ctx);
            avcodec_free_context(&cdc_ctx);


        }








    }

}