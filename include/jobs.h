#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

typedef enum {
    JOB_RUNNING,
    JOB_STOPPED,
    JOB_DONE
} JobStatus;

typedef struct Job {
    int id;
    pid_t pid;
    JobStatus status;
    char *command;
    struct Job *next;
} Job;

int add_job(pid_t pid, JobStatus status, const char *command);

void remove_job(pid_t pid);

void remove_job_by_id(int id);

Job *find_job(pid_t pid);

Job *find_job_by_id(int id);

void update_job_status(pid_t pid, JobStatus status);

void print_jobs();

void check_zombies();

void free_jobs();

#endif
