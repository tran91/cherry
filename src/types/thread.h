#ifndef __thread_h
#define __thread_h

#include "id.h"

type(thread);
/*
 * @pid: thread
 * @job: thread_job
 * 
 * example:
 *      static void callback(id user1, id user2, id user3)
 *      {
 * 
 *      }
 * 
 *      ...
 * 
 *      id u1, u2, u3;
 *      
 *      ...
 * 
 *      id pid;
 *      thread_new(&pid);
 *      
 *      ...
 * 
 *      id job;
 *      thread_job_new(&job);
 *      thread_job_set_callback(job, callback);
 *      thread_job_add_arguments(job, u1);
 *      thread_job_add_arguments(job, u2);
 *      thread_job_add_arguments(job, u3);
 *      thread_add_job(pid, job);
 */
void thread_add_job(id pid, id job);

typedef void(*thread_job_callback)(id,...);

type(thread_job);
void thread_job_set_callback(id pid, thread_job_callback f);
void thread_job_add_arguments(id pid, id cid);

#endif