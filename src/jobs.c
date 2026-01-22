#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> 

#include "jobs.h"

static Job *job_list = NULL;
static int next_job_id = 1;

int add_job(pid_t pid, JobStatus status, const char *command) {
    Job *new_job = malloc(sizeof(Job));
    if (!new_job) {
        perror("malloc");
        return -1;
    }
    new_job->id = next_job_id++;
    new_job->pid = pid;
    new_job->status = status;
    new_job->command = strdup(command);
    new_job->next = NULL;

    if (job_list == NULL) {
        job_list = new_job;
    } else {
        Job *current = job_list;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_job;
    }
    return new_job->id;
}

void remove_job(pid_t pid) {
    Job *current = job_list;
    Job *prev = NULL;

    while (current != NULL) {
        if (current->pid == pid) {
            if (prev == NULL) {
                job_list = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->command);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void remove_job_by_id(int id) {
     Job *current = job_list;
    Job *prev = NULL;

    while (current != NULL) {
        if (current->id == id) {
            if (prev == NULL) {
                job_list = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->command);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

Job *find_job(pid_t pid) {
    Job *current = job_list;
    while (current != NULL) {
        if (current->pid == pid) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

Job *find_job_by_id(int id) {
    Job *current = job_list;
    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void update_job_status(pid_t pid, JobStatus status) {
    Job *job = find_job(pid);
    if (job) {
        job->status = status;
    }
}

void print_jobs() {
    Job *current = job_list;
    while (current != NULL) {
        const char *status_str = "Unknown";
        switch (current->status) {
            case JOB_RUNNING: status_str = "Running"; break;
            case JOB_STOPPED: status_str = "Suspended"; break;
            case JOB_DONE: status_str = "Done"; break;
        }
        printf("[%d]  %s \t\t %s\n", current->id, status_str, current->command);
        current = current->next;
    }
}

void check_zombies() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            Job *j = find_job(pid);
            if (j) {
                printf("[%d]  Done \t\t %s\n", j->id, j->command);
                remove_job(pid);
            }
        }
        if (WIFSTOPPED(status)) {
             Job *j = find_job(pid);
             if (j && j->status != JOB_STOPPED) {
                 j->status = JOB_STOPPED;
                 printf("[%d]  Suspended \t %s\n", j->id, j->command);
             }
        }
    }
}

void free_jobs() {
    Job *current = job_list;
    while (current != NULL) {
        Job *next = current->next;
        free(current->command);
        free(current);
        current = next;
    }
    job_list = NULL;
}