
#ifndef FRAME_QUEUE_H
#define FRAME_QUEUE_H
#include <queue>
#include <SDL2\SDL_thread.h>


extern "C"{

#include <libavcodec\avcodec.h>

}

struct FrameQueue
{
	static const int capacity = 30;

	std::queue<AVFrame*> queue;

	uint32_t nb_frames;

	SDL_mutex* mutex;
	SDL_cond * cond;

	FrameQueue();
	bool enQueue(const AVFrame* frame);
	bool deQueue(AVFrame **frame);
};



#endif