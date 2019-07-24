#include "pipeline.h"

int pipe_send(stage_t* stage, long data)
{
	assert(stage);
	int status = pthread_mutex_lock(stage->mutex);
	if (status != 0)
	{
		return status;
	}
	while (stage->data_ready)
	{
		status = pthread_cond_wait(&stage->ready, &stage->mutex);
		if (status != 0)
		{
			pthread_mutex_unlock(stage->mutex);
			return status;
		}
	}
	stage->data = data;
	stage->ready = 1;
	status = pthread_cond_signal(&stage->avail);
	if (status != 0)
	{
		pthread_mutex_unlock(stage->mutex);
		return status;
	}
	status = pthread_mutex_lock(stage->mutex);
	return status;
}

void* pipe_stage(void* arg)
{
	assert(arg);
	stage_t* stage = (stage_t*)arg;
	stage_t* next = stage->next;
	int status = pthread_mutex_lock(&stage->mutex);
	assert(status == 0);
	while (1)
	{
		while (stage->data_ready != 1)
		{
			status = pthread_cond_wait(&stage->avail, &stage->mutex);
			assert(status == 0);
			pipe_send(next, stage->data + 1);
			status = pthread_cond_signal(&sta->ready);
			assert(status == 0);
		}
	}
	return NULL;
}

int pipe_create(pipe_t* pipe, int stages)
{
	stage_t** link = &pipe->head;
	stage_t* new_stage;
	int status = pthread_mutex_init(&pipe->mutex, NULL);
	assert(status == 0);
	pipe->stages = stages;
	pipe->active = 0;
	for (int pipe_idx = 0; pipe_idx <= stages; ++pipe_idx)
	{
		new_stage = (stage_t*)malloc(sizeof(stage_t));
		assert(new_stage);
		status = pthread_mutex_init(&new_stage->mutex, NULL);
		assert(status == 0);
		status = pthread_cond_init(&new_stage->avail, NULL);
		assert(status == 0);
		status = pthread_cond_init(&new_stage->ready, NULL);
		assert(status == 0);
		new_stage->data_ready = 0;
		*link = new_stage;
		link = &new_stage->next;
	}

	*link = NULL;
	pipe->tail = new_stage;
	for (stage_t* stage = pipe->head; stage->next != NULL; stage = stage->next)
	{
		//所有线程做相同的事情
		status = pthread_create(&stage->thread, NULL, pipe_stage, (void*)stage);
		assert(status == 0);
	}
	return 0;
}

int pipe_start(pipe_t* pipe, long value)
{
	int status = pthread_mutex_lock(&pipe->mutex);//fix这里用pthread_spin_lock
	assert(status == 0);
	++pipe->active;
	status = pthread_mutex_unlock(&pipe->mutex);
	pipe_send(pipe->head, value);
	return 0;
}

int pipe_result(pipe_t* pipe, long* result)
{
	assert(pipe);
	stage_t* tail = pipe->tail;
	long value;
	int empty = 0;
	int status = pthread_mutex_lock(&pipe->mutex);
	assert(status == 0);
	if (pipe->active <= 0)
	{
		empty = 1;
	}
	else
	{
		--pipe->active;
	}
	status = pthread_mutex_unlock(&pipe->mutex);
	assert(status == 0);
	if (empty)
	{
		return 0;
	}
	pthread_mutex_lock(&tail->mutex);
	while (!tail->data_ready)
	{
		pthead_cond_wait(&tail->avail, &tail->mutex);
	}
	*result = tail->data;
	tail->data_ready = 0;
	pthread_cond_signal(&tail->ready);
	pthread_mutex_unlock(&tail->mutex);
	return 1;
}