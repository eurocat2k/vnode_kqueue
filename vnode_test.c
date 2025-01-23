#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <signal.h>
 
// constants 
#define NUM_EVENT_SLOTS 2
#define NUM_EVENT_FDS 2

// SIGNAL context
struct context {
	void (*handler)(int sig);
};
// SIGNAL handler
void unix_signal_handler(int sig);
 
char *flagstring(int flags);

int kq = -1;         // kqueue handler
int event_fd = -1;   // the file to be monitored

extern int main(int argc, char *argv[]) {
    char *path = argv[1];
    struct kevent events_to_monitor[NUM_EVENT_FDS];
    struct kevent event_data[NUM_EVENT_SLOTS];
    void *user_data;
    struct timespec timeout;
    unsigned int vnode_events;
    struct context ctx = {.handler = unix_signal_handler };
 
    if (argc != 2) {
        fprintf(stderr, "Usage: monitor <file_path>\n");
        exit(-1);
    }
 
    /* Open a kernel queue. */
    if ((kq = kqueue()) < 0) {
        fprintf(stderr, "Could not open kernel queue.  Error was %s.\n", strerror(errno));
    }
 
    /*
       Open a file descriptor for the file/directory that you
       want to monitor.
     */
    event_fd = open(path, O_RDONLY | O_NONBLOCK);
    if (event_fd <=0) {
        fprintf(stderr, "The file %s could not be opened for monitoring.  Error was %s.\n", path, strerror(errno));
        exit(-1);
    }

    sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigprocmask(SIG_BLOCK, &mask, NULL);

    EV_SET(&event_data[0], SIGINT, EVFILT_SIGNAL, EV_ADD | EV_ENABLE, 0, 0, &ctx);
	assert(0 == kevent(kq, event_data, 1, NULL, 0, NULL));
 
    /*
       The address in user_data will be copied into a field in the
       event.  If you are monitoring multiple files, you could,
       for example, pass in different data structure for each file.
       For this example, the path string is used.
     */
    user_data = path;
 
    /* Set the timeout to wake us every half second. */
    timeout.tv_sec = 0;        // 0 seconds
    timeout.tv_nsec = 500000000;    // 500 milliseconds
 
    /* Set up a list of events to monitor. */
    vnode_events = NOTE_DELETE | NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_RENAME | NOTE_REVOKE | NOTE_OPEN | NOTE_CLOSE | NOTE_CLOSE_WRITE;
    EV_SET( &events_to_monitor[0], event_fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, vnode_events, 0, user_data);
 
    /* Handle events. */
    int num_files = 1;
    // int continue_loop = 40; /* Monitor for twenty seconds. */
    // while (--continue_loop) {
    while(1) {
        int event_count = kevent(kq, events_to_monitor, NUM_EVENT_SLOTS, event_data, num_files, &timeout);
        if (event_count < 0  && errno == EINTR) {
            continue;
        }
        if ((event_count < 0) || (event_data[0].flags == EV_ERROR)) {
            /* An error occurred. */
            fprintf(stderr, "An error occurred (event count %d).  The error was %s.\n", event_count, strerror(errno));
            break;
        }
        if (event_count) {
            for (int i = 0; i < event_count; i += 1) {
                if (event_data[i].filter == EVFILT_SIGNAL) {
                    int sig = event_data[i].ident;
                    ctx.handler(sig);
                } else if (event_data[i].filter == EVFILT_VNODE) {
                    printf("Event %" PRIdPTR " occurred.  Filter %d, flags %d, filter flags %s, filter data %" PRIdPTR ", path %s\n",
                        event_data[i].ident,
                        event_data[i].filter,
                        event_data[i].flags,
                        flagstring(event_data[i].fflags),
                        event_data[i].data,
                        (char *)event_data[i].udata
                    );
                }
            }
        }
        // } else {
        //     printf("No event.\n");
        // }
 
        /* Reset the timeout.  In case of a signal interrruption, the
           values may change. */
        timeout.tv_sec = 0;        // 0 seconds
        timeout.tv_nsec = 500000000;    // 500 milliseconds
    }
    close(event_fd);
    return 0;
}
 
/* A simple routine to return a string for a set of flags. */
char *flagstring(int flags) {
    static char ret[512];
    char *or = "";
 
    ret[0]='\0'; // clear the string.
    if (flags & NOTE_DELETE) {strcat(ret,or);strcat(ret,"NOTE_DELETE");or="|";}
    if (flags & NOTE_WRITE) {strcat(ret,or);strcat(ret,"NOTE_WRITE");or="|";}
    if (flags & NOTE_EXTEND) {strcat(ret,or);strcat(ret,"NOTE_EXTEND");or="|";}
    if (flags & NOTE_ATTRIB) {strcat(ret,or);strcat(ret,"NOTE_ATTRIB");or="|";}
    if (flags & NOTE_LINK) {strcat(ret,or);strcat(ret,"NOTE_LINK");or="|";}
    if (flags & NOTE_RENAME) {strcat(ret,or);strcat(ret,"NOTE_RENAME");or="|";}
    if (flags & NOTE_REVOKE) {strcat(ret,or);strcat(ret,"NOTE_REVOKE");or="|";}
    if (flags & NOTE_OPEN) {strcat(ret,or);strcat(ret,"NOTE_OPEN");or="|";}
    if (flags & NOTE_CLOSE) {strcat(ret,or);strcat(ret,"NOTE_CLOSE");or="|";}
    if (flags & NOTE_CLOSE_WRITE) {strcat(ret,or);strcat(ret,"NOTE_CLOSE_WRITE");or="|";}
 
    return ret;
}

void unix_signal_handler(int sig) {
	printf("Received UNIX signal via kqueue: %d\n", sig);
    switch (sig) {
        case SIGINT:
            if (event_fd != -1) {
                printf("[I] Closing monitored descriptor \033[1;36m%d\033[0m\n", event_fd);
                close(event_fd);
            }
            if (kq != -1) {
                printf("[I] Closing kqueue handler \033[1;35m%d\033[0m\n", kq);
                close(kq);
            }
            printf("Exiting...\n");
            _exit(0);
        break;
        default:
            _exit(sig);
    }
}
