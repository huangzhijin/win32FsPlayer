// FsPlayerTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

extern "C" {

#include <libavcodec\avcodec.h>
#include <libavformat\avformat.h>
#include <libswscale\swscale.h>
#include <libswresample\swresample.h>
#include <SDL2\SDL.h>
#include <SDL2\SDL_thread.h>
#include <SDL2\SDL_video.h>
#include <libavformat\avformat.h>
#include <libavutil\rational.h>

}



#include "PacketQueue.h"
#include "Audio.h"
#include "Video.h"
#include "Media.h"
#include "VideoDisplay.h"
using namespace std;

bool quit = false;
   int thread_exit = 0;
  int thread_pause = 0;

int main(int argc, char* argv[])
{
	av_register_all();

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

	//char* filename ="Titanic.ts";
	//char* filename = "F:\\test.rmvb";
	//char filename[] = "Titanic.ts";
	char filename[500] = { 0 };
	strcpy(filename, argv[1]);

	MediaState media(filename);

	if (media.openInput())
		SDL_CreateThread(decode_thread, "", &media); // 创建解码线程，读取packet到队列中缓存
	//media.video->video_ctx->width = media.pFormatCtx->streams[media.video->stream_index]->codec->width;
	//media.video->video_ctx->height = media.pFormatCtx->streams[media.video->stream_index]->codec->height;

	media.audio->audio_play(); // create audio thread

	media.video->video_play(&media); // create video thread

	AVStream *audio_stream = media.pFormatCtx->streams[media.audio->stream_index];
	AVStream *video_stream = media.pFormatCtx->streams[media.video->stream_index];

	
	double audio_duration = audio_stream->duration * av_q2d(audio_stream->time_base);
	double video_duration = video_stream->duration * av_q2d(video_stream->time_base);

	cout << "audio时长：" << audio_duration << endl;
	cout << "video时长：" << video_duration << endl;

	SDL_Event event;
	while (true) // SDL event loop
	{
		
		SDL_WaitEvent(&event);
		printf("%d==%d==SDL_WaitEvent===\n", thread_pause, event.type);
		switch (event.type)
		{
		case FF_QUIT_EVENT:
		case SDL_QUIT:
			quit = 1;
			SDL_Quit();
			//SDL_CloseAudio();
			return 0;
			break;

		case SDL_WINDOWEVENT:
			//If Resize
			SDL_GetWindowSize(media.video->window,&(media.video->screen_w), &(media.video->screen_h));
			break;
		case FF_REFRESH_EVENT:
			//video_refresh_timer(&media);
			if (thread_pause==0) {
				video_refresh_timer(&media);
			}
			
			printf("%d=======\n", thread_pause);
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_SPACE) {
				thread_pause = !thread_pause;
				SDL_PauseAudio(thread_pause);
			  if (thread_pause == 0) {
				  printf("%d==%d==------------------SDL_KEYDOWN------------------------===\n", thread_pause, event.type);
				schedule_refresh(&media,1);
			  }
			}
				

			break;
		case FF_STOP_EVENT:
		//	thread_exit = 1;
			break;
		default:
			break;
		}
	}

	getchar();
	return 0;
}