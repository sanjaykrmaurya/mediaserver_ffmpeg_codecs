/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "webrtc/fmp4.h"
#include "webrtc/peer.h"

#include "ff/ff.h"
#include "ff/mediacapture.h"
#include "base/define.h"
#include "base/test.h"
#include <thread>

extern "C"
{
//#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}

#define AUDIOFILE  "/var/tmp/songs/hindi.pcm"               
#define VIDEOFILE  "/experiment/live/testProgs/test.264"
//#define VIDEOFILE  "/experiment/fmp4/kunal720.264"
#include "webrtc/ffparse.h"

#define MAX_CHUNK_SIZE 10240*8
// maximum send buffer 262144  =1024 *256

#define highWaterMark  8 * 1048576
//maximum buffer = 16 *1048576 where  1024*1024 =1048576




#define IOBUFSIZE 40960
//40960*6

namespace base {
    
    fmp4::ReadMp4 *self;

    namespace fmp4 {

        ReadMp4::ReadMp4() {
            
            
         ffparser = new FFParse(this,AUDIOFILE, VIDEOFILE);
         ffparser->start(); 
            
         self = this;

        }

        ReadMp4::~ReadMp4() {
            SInfo << "~ReadMp4( )";
            ffparser->stop();
            ffparser->join();
            delete ffparser;
        }
        
        void ReadMp4::run()
        {
            
        }
        
        void ReadMp4::add(wrtc::PeerfMp4 *obj)
        {
            g_num_mutex.lock(); 
            setfmp4Peers.insert(obj );
            g_num_mutex.unlock();
        }
        
        void ReadMp4::remove(wrtc::PeerfMp4 *obj)
        {
            g_num_mutex.lock();
            if( setfmp4Peers.find(obj) != setfmp4Peers.end())
            {
                setfmp4Peers.erase(obj);
            }
            g_num_mutex.unlock();
        }
        
        
         void ReadMp4::broadcast(const char * data, int size, bool binary   )
         {
             g_num_mutex.lock();
              
             std::set<wrtc::PeerfMp4*>::iterator it;
            for (it = setfmp4Peers.begin(); it != setfmp4Peers.end(); ++it) {
                
                wrtc::PeerfMp4 *obj = *it; 
                obj->pc->sendDataBinary((const uint8_t *) data, size);
            }
            
            g_num_mutex.unlock();

             
         }
        
        
    }
    
    
    
    namespace wrtc {

        PeerfMp4::PeerfMp4(Peer *pc) : pc(pc) {
            
           
        }

        PeerfMp4::~PeerfMp4() {
            SInfo << "~PeerfMp4( )";
        }
        
        
        void PeerfMp4::onConnect()
        {
           SInfo << " datachannel onConnect" ;
              
            self->add(this);
        }
        
        void PeerfMp4::ondisConnect()
        {
           SInfo << " datachannel onDisConnect" ;
            self->remove(this);
        }
        
        
        void PeerfMp4::onMessage(std::string str)
        {
            SInfo << " datachannel onMessage" << str;
                
                //  m_ping_timeout_timer.Reset();
                //  m_packet_mgr.put_payload(std::string(data,sz));
            if( str == "reset")
                self->ffparser->reset();    
        }

        
//        void PeerfMp4::run()
//        {
//            
//        }
//        
#if 0
        
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
            ReadMp4 *obj = (ReadMp4 *) opaque;

           // obj->pc->sendDataBinary((const uint8_t *) buf, bufSize);
            std::this_thread::sleep_for(std::chrono::milliseconds(15));

            //  obj->outputData.insert(obj->outputData.end(), buf, buf + bufSize);
            //static size_t kMaxQueuedSendDataBytes = 16 * 1024 * 1024;

            //   if(obj->pc->data_channel_->buffered_amount()  <  1024 )
            //       std::this_thread::sleep_for(std::chrono::milliseconds(5));
            //   
            //   else( obj->pc->data_channel_->buffered_amount()  >  8 * 1024 * 1024 )
            //       

            return bufSize;
        }

        void ReadMp4::run() {

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
       
        void ReadMp4::run() 
        {


            std::ifstream bunnyFile;
            bunnyFile.open("/var/tmp/test.mp4", std::ios_base::in | std::ios_base::binary);

            char buf[ MAX_CHUNK_SIZE];

            while (bunnyFile.good() && !stopped() ) {
              bunnyFile.read(buf,  MAX_CHUNK_SIZE);
              int nRead = bunnyFile.gcount();
              if (nRead > 0) {
               // dc->sendDataMsg("ravind");

                pc->sendDataBinary((const uint8_t *)buf, nRead);

                do
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(15));
                } while( pc->data_channel_->buffered_amount()  >  highWaterMark );
        
                // while( pc->data_channel_->buffered_amount()  >  12 * 1024 * 1024 )
                 // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
              }

              SInfo << "Sent message of size " << nRead ;
            }
    
    
            SInfo << "fmp4 thread exit"; 
    
           // fileName = "/var/tmp/videos/test.mp4";
            fileName = "/var/tmp/kunal720.mp4";
            //fmp4(fileName.c_str(), "fragTmp.mp4");
            //fmp4(fileName.c_str());
        }    
         



        int ReadMp4::fmp4(const char *in_filename, const char *out_filename, bool fragmented_mp4_options) {



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
                    pc->sendDataBinary(&outputData[0], 1024);
                    outputData.erase(outputData.begin(), outputData.begin() + 1024);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                // obj->outputData.insert(obj->outputData.end(), buf, buf + bufSize);

            }

            //  av_write_trailer(ofmt_ctx);

            ret = av_write_frame(ofmt_ctx, nullptr); //Flush if something has been left

            if (outputData.size()) {
                pc->sendDataBinary(&outputData[0], outputData.size());
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



#endif





    }

}
