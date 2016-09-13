//ffmpeg -i rtsp://10.6.0.79/ufirststream -vf fps=fps=1/10 -update 1 img.jpg 
//g++ -o testvid videorplaka.cpp  -lavcodec -lavdevice -lavfilter -lavformat -lavutil -logg  -lswscale -lx264 -lpthread -lvorbis -lopenalpr -L/usr/lib 
//RAW DATADAN ALAIOZ HULOGGGGGGGGGG!
#include "alpr.h"
#include <stdio.h>
#include <dirent.h>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <string.h>
#include <iostream>
#include <time.h>
#include <pthread.h>
//#include "stateidentifier.h"

#define MAXBUF 1024

extern "C" {
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include "libavcodec/avcodec.h"
#include <libswscale/swscale.h>
}
struct arguments {
SwsContext *img_convert_ctx;
AVFrame* pic;
AVCodecContext* ccontext;
AVFrame*  picrgb;
char* country;
char* region;
int frame;

};
char blue[] = { 0x1b, '[', '1', ';', '3', '4', 'm', 0 };
char normal[] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

void* thrRead(void *argz ){

struct arguments * args =(struct arguments *) argz ;
sws_scale(args->img_convert_ctx, args->pic->data, args->pic->linesize, 0, args->ccontext->height, args->picrgb->data, args->picrgb->linesize);
              std::stringstream name;
              //name << "testcam" << ".jpeg";
              //--------------------------------------------------------------------------------------
              alpr::Alpr openalpr(args->country);
              openalpr.setTopN(20);
              openalpr.setDefaultRegion(args->region);
              char * extended = (char*)malloc(sizeof(char) * MAXBUF);

              //strcpy(extended,directory);
             // strcat(extended,);

              

              //--------------------------------------------------------------------------------------
              //myfile.open(name.str());
              alpr::AlprRegionOfInterest regionlar(0,args->ccontext->height/4,args->ccontext->width, args->ccontext->height*3/8);

              std::vector<alpr::AlprRegionOfInterest> a ; 
              a.push_back(regionlar);


              alpr::AlprResults results =  openalpr.recognize((args->picrgb->data[0]), 3, args->ccontext->width, args->ccontext->height, a);
             // alpr::StateIdentifier 
             // alpr::AlprResults recognizer = openalpr.
 // Iterate through the results.  There may be multiple plates in an image,
 // and each plate return sthe top N candidates.
                 //--------------------------------------------------------------------------------------
              if (openalpr.isLoaded() == false)
              {

                std::cerr << "Error loading OpenALPR" << std::endl;
                exit(1);
              }
              std::cout <<  results.plates.size()<<" plate(s) found in frame " << args->frame << ":" <<std::endl;
              if(results.plates.size()){


                for (int i = 0; i < results.plates.size(); i++)
                {

                  alpr::AlprPlateResult plate = results.plates[i];

                  std::cout << "plate" << i << ": " << plate.topNPlates.size() << " results "  <<plate.region<< std::endl;

                  for (int k = 0; k < plate.topNPlates.size(); k++)
                  {
                    alpr::AlprPlate candidate = plate.topNPlates[k];
                //if(candidate.matches_template)
                //  std::cout << "    - " << candidate.characters << "\t confidence: " << candidate.overall_confidence << std::endl;
                    std::cout <<  "    - " << candidate.characters << "\t confidence: " << candidate.overall_confidence;
                    std::cout << "\t pattern_match: " << candidate.matches_template << std::endl;


                  }
                }
              }

}

void readplate(char * pics, char * directory,char * country,char * region){
  alpr::Alpr openalpr(country);
  openalpr.setTopN(20);
  char * extended = (char*)malloc(sizeof(char) * MAXBUF);

  strcpy(extended,directory);
  strcat(extended,pics);

  openalpr.setDefaultRegion(region);

  alpr::AlprResults results = openalpr.recognize(extended);


 // Iterate through the results.  There may be multiple plates in an image,
 // and each plate return sthe top N candidates.
  if (openalpr.isLoaded() == false)
  {
    std::cerr << "Error loading OpenALPR" << std::endl;
    exit(1);
  }

  std::cout << blue << results.plates.size()<<" matches found:" << normal << std::endl;
  for (int i = 0; i < results.plates.size(); i++)
  {

    alpr::AlprPlateResult plate = results.plates[i];

    std::cout <<blue << "plate" << i << ": " << plate.topNPlates.size() << " results" << normal << std::endl;

    for (int k = 0; k < plate.topNPlates.size(); k++)
    {
      alpr::AlprPlate candidate = plate.topNPlates[k];
      //if(candidate.matches_template)
      //  std::cout << "    - " << candidate.characters << "\t confidence: " << candidate.overall_confidence << std::endl;
      std::cout << blue << "    - " << candidate.characters << "\t confidence: " << candidate.overall_confidence;
      std::cout << "\t pattern_match: " << candidate.matches_template << normal<< std::endl;


    }
  }
  /* code */

}



int main(int argc, char const *argv[])
{
  char * directory="./",
  *region="az", 
  *country= "eu";
  char debug = 0;
  unsigned short int i = 0, j = 0,k =0;
  unsigned int fps  = 10;
  char * link  = (char*) "rtsp://10.6.0.79/ufirststream";
  if(argc > 1){

    for(i = 1; i < argc; i++)
    {

      if(!strcmp("-debug",argv[i])){
        debug = 1;
      }
      else if(!strcmp(argv[i],"-reg")){
        region = (char*)argv[i+1];
      }
      else if(!strcmp(argv[i],"-src")){
        link = (char*)argv[i+1];
      }
      else if(!strcmp(argv[i],"-ctry"))
      {
        country = (char*)argv[i+1]; 
      }
      else if(!strcmp(argv[i],"-fps"))
      //else if()
      {
        fps = atoi(argv[i+1]);

      }
    }

  }

    printf("\n\n-=-=-=-=-==-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    printf("url: %s\ncountry: %s\n",link,country);
    printf("-=-=-=-=-==-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n\n");

 
  
  const AVCodec *decoder  = avcodec_find_decoder(AV_CODEC_ID_H264) ;
  const AVCodec *codec;

  SwsContext *img_convert_ctx;
  AVFormatContext* context = avformat_alloc_context();
  AVCodecContext* ccontext = avcodec_alloc_context3(decoder);
  int video_stream_index;

  av_register_all();
  avformat_network_init();

      //open rtsp
  if(avformat_open_input(&context, link,NULL,NULL)){
    return EXIT_FAILURE;
  }

  if(avformat_find_stream_info(context,NULL) < 0){
    return EXIT_FAILURE;
  }
  if(debug)printf("Retrieving from %s\n",link);
  


  //search video stream
  for(int i =0;i<context->nb_streams;i++){
    if(context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
      video_stream_index = i;
  }

  AVPacket packet;
  av_init_packet(&packet);
  AVFormatContext* oc = avformat_alloc_context();
  AVStream* stream=NULL;
  int cnt = 0;
    //start reading packets from stream and write them to file
    av_read_play(context);//play RTSP
    if(debug) printf("play!\n");
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) 
      {if(debug)printf("loljk\n");
    exit(1);
  }
  avcodec_get_context_defaults3(ccontext, codec);
  avcodec_copy_context(ccontext,context->streams[video_stream_index]->codec);
  std::ofstream myfile;
  if(debug)printf("hi\n");
  if (avcodec_open2(ccontext, codec,NULL) < 0) exit(1);

  img_convert_ctx = sws_getContext(ccontext->width, ccontext->height, ccontext->pix_fmt, ccontext->width, ccontext->height,
    AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

  int size = avpicture_get_size(AV_PIX_FMT_YUV420P, ccontext->width, ccontext->height);


  uint8_t* picture_buf = (uint8_t*)(av_malloc(size));
  AVFrame* pic = av_frame_alloc() ;
  AVFrame*  picrgb= av_frame_alloc() ;
  int size2 = avpicture_get_size(AV_PIX_FMT_RGB24, ccontext->width, ccontext->height);
  uint8_t* picture_buf2 = (uint8_t*)(av_malloc(size2));
  avpicture_fill((AVPicture *) pic, picture_buf, AV_PIX_FMT_YUV420P, ccontext->width, ccontext->height);
  avpicture_fill((AVPicture *) picrgb, picture_buf2, AV_PIX_FMT_RGB24, ccontext->width, ccontext->height);
  if(debug) printf("lmao\n");

  while(av_read_frame(context,&packet)>=0 )
    {//read 100 frames

     if(debug)std::cout << "1 Frame: " << cnt << std::endl;
        if(packet.stream_index == video_stream_index){//packet is video
         if(debug) std::cout << "2 Is Video" << std::endl;
         if(stream == NULL)
            {//create stream in file
              if(debug)std::cout << "3 create stream" << std::endl;
              stream = avformat_new_stream(oc,context->streams[video_stream_index]->codec->codec);
              avcodec_copy_context(stream->codec,context->streams[video_stream_index]->codec);
              stream->sample_aspect_ratio = context->streams[video_stream_index]->codec->sample_aspect_ratio;
            }
            int check = 0;
            packet.stream_index = stream->id;
            if(debug) std::cout << "4 decoding" << std::endl;
            int result = avcodec_decode_video2(ccontext, pic, &check, &packet);
           // std::cout << "Bytes decoded " << result << " check " << check << std::endl;
            if(!(cnt % fps))
            {
              clock_t t;
              t=clock();
              pthread_t thread;
              //thrRead(img_convert_ctx, pic,ccontext, picrgb,country, region );
              struct arguments * arg = ( struct arguments *)malloc(sizeof(struct arguments));

              arg->img_convert_ctx =img_convert_ctx;
              arg->pic = pic;

              arg->ccontext=ccontext;
              arg->picrgb=picrgb;
              arg->country=country;
              arg->region=region;
              arg->frame = cnt;
               int rc = pthread_create(&thread, NULL, thrRead, (void*)arg);
                  if (rc){
                   std::cout << "Error:unable to create thread," << rc << std::endl;
                   exit(-1);}
                 //pthread_join(NULL);  
                 rc = pthread_join(thread,NULL);
                if (rc){
                   std::cout << "Error:unable to join," << rc << std::endl;
                   exit(-1);
                }
                std::cout <<"---------------------------------------------------------------" <<(((float)(clock() - t))/CLOCKS_PER_SEC )<< "seconds"<<std::endl;


              }
            cnt++;
          }
          av_free_packet(&packet);
          av_init_packet(&packet);
        }
        av_free(pic);
        av_free(picrgb);
        av_free(picture_buf);
        av_free(picture_buf2);

        av_read_pause(context);
        avio_close(oc->pb);
        avformat_free_context(oc);
//    return (EXIT_SUCCESS);
        return 0;
      }