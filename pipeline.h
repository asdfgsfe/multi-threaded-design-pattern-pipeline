#ifndef __PIPE_LINE_H__
#define __PIPE_LINE_H_

typedef struct stage_tag
{
	pthread_mutex_t mutex;
	pthread_cond_t avail;
	pthread_cond_t ready;
	int data_ready;
	long data;
	pthread_t thread;
	struct stage_tag* next;
} stage_t;

typedef struct pipe_tag
{
	pthread_mutex_t mutex;
	stage_t* head;
	stage_t* tail;
	int stages;
	int active;
}pipe_t;


int pipe_send(stage_t* stage, long data);
void* pipe_stage(void* arg);

#endif //__PIPE_LINE_H_
