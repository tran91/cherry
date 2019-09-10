#include "thread.h"
#include "vector.h"

struct thread_job
{
    id data;
    thread_job_callback callback;
};
make_type(thread_job);

static void thread_job_init(struct thread_job *p, key k)
{
    vector_new(&p->data);
    p->callback = NULL;
}

static void thread_job_clear(struct thread_job *p)
{
    release(p->data);
}

void thread_job_set_callback(id pid, thread_job_callback f)
{
    struct thread_job *raw;

    thread_job_fetch(pid, &raw);
    assert(raw != NULL);
    raw->callback = f;
}

void thread_job_add_arguments(id pid, id cid)
{
    struct thread_job *raw;

    thread_job_fetch(pid, &raw);
    assert(raw != NULL);
    vector_push(raw->data, cid);
}

void thread_job_run(id pid)
{
    struct thread_job *raw;
    unsigned len;
    signed i;
    id arg[8];

    thread_job_fetch(pid, &raw);
    assert(raw != NULL);
    if (!raw->callback) goto finish;

    vector_get_size(raw->data, &len);
    for (i = 0; i < len; ++i) {
        vector_get(raw->data, i, arg + i);
    }
    raw->callback(arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7]);
    raw->callback = NULL;

finish:
    release(raw->data);
}