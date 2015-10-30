#define LOG_TAG "DeadlockForkMutex"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex;

bool thread_init = false;
pthread_mutex_t thread_init_mutex;
pthread_cond_t thread_init_cond;

void *thread_routine(void *)
{
    pthread_mutex_lock(&mutex);

    pthread_mutex_lock(&thread_init_mutex);
    thread_init = true;
    pthread_cond_signal(&thread_init_cond);
    pthread_mutex_unlock(&thread_init_mutex);

    while (1) {
        printf("%d:%d, %s\n", getpid(), gettid(), __func__);
        usleep(1000000);
    }

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main()
{
    pthread_t thread;
    int ret;
    pid_t pid;

    printf("%d:%d, mutex-deadlock-after-fork starts\n", getpid(), gettid());
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&thread_init_mutex, NULL);
    pthread_cond_init(&thread_init_cond, NULL);

    ret = pthread_create(&thread, NULL, thread_routine, NULL);
    if (ret < 0) {
        printf("failed to pthread_create, error = %s\n", strerror(errno));
        return 1;
    }

    pthread_mutex_lock(&thread_init_mutex);
    while (!thread_init) {
        pthread_cond_wait(&thread_init_cond, &thread_init_mutex); } pthread_mutex_unlock(&thread_init_mutex);
    pthread_mutex_unlock(&thread_init_mutex);

    pid = fork();
    if (pid == 0) {
        printf("%d:%d, child process after fork\n", getpid(), gettid());
        printf("%d:%d, child process is ready to get mutex\n", getpid(), gettid());
        pthread_mutex_lock(&mutex);
        printf("%d:%d, child process successfully gets mutex\n", getpid(), gettid());
        pthread_mutex_unlock(&mutex);
        printf("%d:%d, child process releases the mutex\n", getpid(), gettid());
    } else {
        printf("%d:%d, parent process after fork\n", getpid(), gettid());
        pthread_join(thread, NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&thread_init_mutex);
    pthread_cond_destroy(&thread_init_cond);
    printf("%d:%d, mutex-deadlock-after-fork ends\n", getpid(), gettid());

    return 0;
}
