// gcc -o convert final_ffmpeg_frame_grabber.c -lavformat -lavcodec -lswscale -lturbojpeg -lz
extern "C" {
//#include <libavutil/imgutils.h>
 #include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
/*extern "C" {
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include "libavcodec/avcodec.h"
#include <libswscale/swscale.h>8
}*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <alpr.h>
#include <string>
#include <time.h>
#include <fstream>

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

using namespace std;

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {

  FILE *pFile;
  char szFilename[32];
  const int JPEG_QUALITY = 100;
  const int COLOR_COMPONENTS = pFrame->linesize[0];
  long unsigned int _jpegSize = 0;
  unsigned char* _compressedImage = NULL;

     // tjhandle _jpegCompressor = tjInitCompress();

      //tjCompress2(_jpegCompressor, pFrame->data[0], width, 0, height, TJPF_RGB,
                 // &_compressedImage, &_jpegSize, TJSAMP_444, JPEG_QUALITY,
                //  TJFLAG_FASTDCT);

      //tjDestroy(_jpegCompressor);

  
  sprintf(szFilename, "./switzerland1/frame%d.jpg", iFrame);

  pFile=fopen(szFilename, "wb");
  if(pFile == NULL)
    return;

      // Write pixel data
  fwrite(_compressedImage, sizeof(unsigned char), _jpegSize, pFile);

      // Close file
  fclose(pFile);

    //  tjFree(_compressedImage);
}


int main(int argc, char *argv[]) {

      // Initalizing these to NULL prevents segfaults!
  AVFormatContext   *pFormatCtx = NULL;
  int               i, videoStream;
  AVCodecContext    *pCodecCtxOrig = NULL;
  AVCodecContext    *pCodecCtx = NULL;
  AVCodec           *pCodec = NULL;
  AVFrame           *pFrame = NULL;
  AVFrame           *pFrameRGB = NULL;
  AVPacket          packet;
  int               frameFinished;
  int               numBytes;
  uint8_t           *buffer = NULL;
  struct SwsContext *sws_ctx = NULL;
  
  struct stat st = {0};
  if (stat("./switzerland1", &st) == -1) 
  {
    mkdir("./switzerland1", 0700);
  }
  ofstream myfile;
  myfile.open ("./switzerland/swit1.txt");
  myfile << "begin " << endl;
  if(argc < 2) {
    printf("Please provide a movie file\n");
    return -1;
  }
      // Register all formats and codecs
  av_register_all();

      // Open video file
  if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)!=0)
        return -1; // Couldn't open file

      // Retrieve stream information
      if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        return -1; // Couldn't find stream information

      // Dump information about file onto standard error
      av_dump_format(pFormatCtx, 0, argv[1], 0);

      // Find the first video stream
      videoStream = -1;
      for(i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
          videoStream=i;
          break;
        }
        if(videoStream==-1)
        return -1; // Didn't find a video stream

      // Get a pointer to the codec context for the video stream
      pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;
      // Find the decoder for the video stream
      pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
      if(pCodec==NULL) {
        fprintf(stderr, "Unsupported codec!\n");
        return -1; // Codec not found
      }
      // Copy context
      pCodecCtx = avcodec_alloc_context3(pCodec);
      if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
        fprintf(stderr, "Couldn't copy codec context");
        return -1; // Error copying codec context
      }

      // Open codec
      if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
        return -1; // Could not open codec

      // Allocate video frame
      pFrame=av_frame_alloc();

      // Allocate an AVFrame structure
      pFrameRGB=av_frame_alloc();
      if(pFrameRGB==NULL)
        return -1;

      // Determine required buffer size and allocate buffer
      numBytes=avpicture_get_size(AV_PIX_FMT_RGB24 , pCodecCtx->width,
       pCodecCtx->height);
      buffer=(uint8_t *)malloc(numBytes*sizeof(uint8_t));

      // Assign appropriate parts of buffer to image planes in pFrameRGB
      // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
      // of AVPicture
      avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24 ,
       pCodecCtx->width, pCodecCtx->height);

      // initialize SWS context for software scaling
      sws_ctx = sws_getContext(pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        pCodecCtx->width,
        pCodecCtx->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
        );
//------------------------------------------------------------------------------------------------------

		 // Initialize the library using United States style license plates.
		 // You can use other countries/regions as well (for example: "eu", "au", or "kr")
      myfile << "------OPEN ALPR------" << "\n\n-Country:EU\n-Region Detection:Active\n-Top N Results:10 \n\n "; 
      
      alpr::Alpr openalpr("us","/etc/openalpr/openalpr.conf");
      

 		//openalpr.setDefaultRegion("fl");
		 //Optionally specify the top N possible plates to return (with confidences).  Default is 10
		 //openalpr.setDetectRegion(true);		
      openalpr.setTopN(10);
      

		 // Optionally, provide the library with a region for pattern matching.  This improves accuracy by
		 // comparing the plate text with the regional pattern.
		//openalpr.setDefaultRegion("si");

		 // Make sure the library loaded before continuing.
		 // For example, it could fail if the config/runtime_data is not found
      if (openalpr.isLoaded() == false)
      {
       std::cerr << "Error loading OpenALPR" << std::endl;
       return 0;
     }
//------------------------------------------------------------------------------------------------------------



     int numberofplates,reg;	 
     numberofplates=0;
     reg=0;	
     int isim;
     isim=0;
      // Read frames and save first five frames to disk
     i=0;
     long int fra;
     fra = 0;
     clock_t ini;
     ini = clock();

     myfile << "---------------------------------------------------------------------------"<<endl;
     while(av_read_frame(pFormatCtx, &packet)>=0) {
        // Is this a packet from the video stream?
      if(packet.stream_index==videoStream) {
          // Decode video frame
        avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

          // Did we get a video frame?
        if(frameFinished) {
    	// Convert the image from its native format to RGB
         sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
          pFrame->linesize, 0, pCodecCtx->height,
          pFrameRGB->data, pFrameRGB->linesize);
         int load;
         load=0;
         
         
    	// Save the frame to disk
         if( 1)
         {	
    	//SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height,i);
          
           clock_t t;
           t = clock();
           fra++;

           alpr::AlprRegionOfInterest regionlar(0  , (pCodecCtx->height/4),
            pCodecCtx->width/2,315);
    //alpr::AlprRegionOfInterest regionlar2((pCodecCtx->width/2),  (pCodecCtx->height/4),(pCodecCtx->width/7),(pCodecCtx->height/7)*2);
           std::vector<alpr::AlprRegionOfInterest> a ; 
           a.push_back(regionlar);
   //  a.push_back(regionlar2);


           alpr::AlprResults results = openalpr.recognize(pFrameRGB->data[0], 3
            ,pCodecCtx->width, pCodecCtx->height ,a);

           
           if(results.plates.size()){
             //
           
           for (int i = 0; i < results.plates.size(); i++)
           {
             load=0;
             numberofplates++;
             alpr::AlprPlateResult plate = results.plates[i];
		   //std::cout << "  plate" << i << ": " << plate.topNPlates.size() << " results" << std::endl;
             std::cout << "region: " << plate.region << "\t region confidence:" << plate.regionConfidence <<  std::endl;
             myfile <<"Region: " << plate.region << "\tRegion confidence: " << plate.regionConfidence << 
             "\tBest Plate: " << plate.bestPlate.characters << std::endl;
             if(plate.region.compare("")!=0 ) 
             {
               isim++;
               reg++;
               //SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height,isim);
               myfile << "For frame"<< isim << " (region found):" << endl; 
             }
             else
             {
               isim++;
              // SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height,isim);
               myfile << "For frame"<< isim << " :" << endl;
             }
             


             for (int k = 0; k < plate.topNPlates.size(); k++)
             {
               
              alpr::AlprPlate candidate = plate.topNPlates[k];
              std::cout << "    - " << candidate.characters << "\t\t confidence: " << candidate.overall_confidence;
              std::cout << "\t pattern_match: " << candidate.matches_template << std::endl;
              
              myfile <<"\t   " << "confidence: " << candidate.overall_confidence << "\t pattern_match: " 
              << candidate.matches_template << "\tRegion:"<< plate.region << "\t\t| "  
              << candidate.characters << std::endl;
              
            }
            
            myfile << "------------------------------------------------------------------------"<< 
            (float(clock()-t))/CLOCKS_PER_SEC  << " seconds." << endl;
            
          }
        }
        else printf("No plate found!!!");
          
          std::cout << "--------------" << (float(clock()-t))/CLOCKS_PER_SEC  << " seconds." <<std::endl; 
          
        }
      }
    }
	  // Free the packet that was allocated by av_read_frame
    av_free_packet(&packet);
  }
  printf("\nNumber of plates read:%d \nNumber of region detected:%d \n",numberofplates,reg);
  std::cout <<"Average process time : " << (float(clock()-ini))/(CLOCKS_PER_SEC*fra) << "sec" << std::endl;
  myfile << "\nNumber of plates read: "<<numberofplates << "\nNumber of regions detected: " << reg  <<
  "\nAverage process time : " << (float(clock()-ini))/(CLOCKS_PER_SEC*fra) << "seconds" <<  endl;  
  myfile.close();

      // Free the RGB image
  free(buffer);
  av_frame_free(&pFrameRGB);

      // Free the YUV frame
  av_frame_free(&pFrame);

      // Close the codecs
  avcodec_close(pCodecCtx);
  avcodec_close(pCodecCtxOrig);

      // Close the video file
  avformat_close_input(&pFormatCtx);

  return 0;
}
