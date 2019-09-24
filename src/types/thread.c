#include "thread.h"
#include "vector.h"
#include <pthread.h>
#if OS != WEB
    #include <openssl/err.h>
#endif

void thread_job_run(id pid);

struct thread
{
    id jobs;
    pthread_mutex_t barrier;
    pthread_cond_t cond;
    char running;
};
make_type(thread);

static void *run(id *pid);

static void thread_init(struct thread *p, key k)
{
    vector_new(&p->jobs);
    pthread_mutex_init(&p->barrier, NULL);
    pthread_cond_init(&p->cond, NULL);
    p->running = 0;
}

static void thread_clear(struct thread *p)
{
    pthread_mutex_lock(&p->barrier);
    release(p->jobs);
    pthread_cond_signal(&p->cond);
    pthread_mutex_unlock(&p->barrier);

    pthread_mutex_destroy(&p->barrier);
    pthread_cond_destroy(&p->cond);    
}

static void *run(id *pid)
{
    id job;
    struct thread *raw;

check:
    retain(*pid);
    thread_fetch(*pid, &raw);
    if (!raw) goto finish;

    pthread_mutex_lock(&raw->barrier);
    vector_get(raw->jobs, 0, &job);
    retain(job);
    vector_remove(raw->jobs, 0);
    pthread_mutex_unlock(&raw->barrier);
    if (id_validate(job)) {
        thread_job_run(job);
        release(job);
    }

    pthread_mutex_lock(&raw->barrier);
    vector_get(raw->jobs, 0, &job);
    if (!id_validate(job)) {
        pthread_mutex_unlock(&raw->barrier);
        release(*pid);

        thread_fetch(*pid, &raw);
        if (raw) {
            pthread_mutex_lock(&raw->barrier);
            pthread_cond_wait(&raw->cond, &raw->barrier);
            pthread_mutex_unlock(&raw->barrier);
        } 

        retain(*pid);
        thread_fetch(*pid, &raw);
        if (!raw) goto finish;
        release(*pid);
        
        goto check;
    } else {
        pthread_mutex_unlock(&raw->barrier);
        release(*pid);
        goto check;   
    }

finish:
    free(pid);
    #if OS != WEB
        ERR_remove_state(1);
    #endif
    return (void *)1;
}

static char do_wait = 0;
static struct {
    pthread_t *start;
    unsigned int len;
    volatile unsigned int barrier;
} tids = {NULL, 0, 0};

static void wait()
{
    signed i;
    if (tids.start) {
        for (i = 0; i < tids.len; i++) {
            pthread_join(tids.start[i], NULL);
        }
        free(tids.start);
        tids.start = NULL;
        tids.len = 0;
    }
}

void thread_add_job(id pid, id cid)
{
    pthread_t tid;
    struct thread *raw;
    id *ext;

    thread_fetch(pid, &raw);
    assert(raw != NULL);

    pthread_mutex_lock(&raw->barrier);
    vector_push(raw->jobs, cid);
    if (!raw->running) {
        raw->running = 1;
        ext = malloc(sizeof(id));
        *ext = pid;
        pthread_create(&tid, NULL, (void*(*)(void*))run, (void*)ext);
        lock(&thread_barrier);
        tids.start = realloc(tids.start, sizeof(pthread_t) * (tids.len + 1));
        tids.start[tids.len] = tid;
        tids.len++;
        if (!do_wait) {
            do_wait = 1;
            unlock(&thread_barrier);
            atexit(wait);
        } else {
            unlock(&thread_barrier);
        }
    }
    pthread_mutex_unlock(&raw->barrier);

    pthread_mutex_lock(&raw->barrier);
    pthread_cond_signal(&raw->cond);
    pthread_mutex_unlock(&raw->barrier);
}