/*
 * Header to daemonize progam
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>

/* Change this to whatever your daemon is called */
#define DAEMON_NAME "server-daemon"

/* Change this to the user under which to run */
#define RUN_AS_USER "root"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define LOG_FILE "/var/log/server-daemon.log"

static void child_handler(int signum)
{
    switch(signum) {
        case SIGALRM: exit(EXIT_FAILURE); break;
        case SIGUSR1: exit(EXIT_SUCCESS); break;
        case SIGCHLD: exit(EXIT_FAILURE); break;
    }
}

static void daemonize( const char *lockfile )
{
    pid_t pid, sid, parent;
    int lfp = -1;
	
    /* already a daemon */
    if ( getppid() == 1 ) return;
	
    /* Create the lock file as the current user */
    if ( lockfile && lockfile[0] ) {
        lfp = open(lockfile,O_RDWR|O_CREAT,0640);
        if ( lfp < 0 ) {
            syslog( LOG_ERR, "unable to create lock file %s, code=%d (%s)",
                lockfile, errno, strerror(errno) );
            exit(EXIT_FAILURE);
        }
    }
	
    /* Drop user if there is one, and we were run as root */
    if ( getuid() == 0 || geteuid() == 0 ) {
        struct passwd *pw = getpwnam(RUN_AS_USER);
        if ( pw ) {
            syslog( LOG_NOTICE, "setting user to " RUN_AS_USER );
            setuid( pw->pw_uid );
        }
    }
	
    /* Trap signals that we expect to recieve */
    signal(SIGCHLD,child_handler);
    signal(SIGUSR1,child_handler);
    signal(SIGALRM,child_handler);
	
    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        syslog( LOG_ERR, "unable to fork daemon, code=%d (%s)",
            errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {
		
        /* Wait for confirmation from the child via SIGTERM or SIGCHLD, or
           for two seconds to elapse (SIGALRM).  pause() should not return. */
        alarm(2);
        pause();
		
        exit(EXIT_FAILURE);
    }
	
    /* At this point we are executing as the child process */
    parent = getppid();
	
    /* Cancel certain signals */
    signal(SIGCHLD,SIG_DFL); /* A child process dies */
    signal(SIGTSTP,SIG_IGN); /* Various TTY signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP, SIG_IGN); /* Ignore hangup signal */
    signal(SIGTERM,SIG_DFL); /* Die on SIGTERM */
	
    /* Change the file mode mask */
    umask(0);
	
    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        syslog( LOG_ERR, "unable to create a new session, code %d (%s)",
           errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }
	
    /* Change the current working directory.  This prevents the current
	 directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        syslog( LOG_ERR, "unable to change directory to %s, code %d (%s)",
            "/", errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }
	
    /* Redirect standard files to /dev/null */
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
	
    /* Tell the parent process that we are A-okay */
    kill( parent, SIGUSR1 );
}

static pid_t pid;

static int just_restarted = 0;

static void
getpwnam_alarm(int signo)
{
    kill(pid, SIGKILL);
    just_restarted++;
}

static void
die(const char *msg)
{
    if (errno)
        perror(msg);
    else
        printf(msg);
	exit(1);
}

int main( int argc, char *argv[] ) {

    /* One may wish to process command line arguments here */
    struct timeval  tv;
    time_t lt;
    const char userid[32];
    int timeout;
    int interval;
    struct sigaction sa;
    char timestamp[80];
    FILE *lf;
	
    /* Daemonize */
    daemonize( "/var/lock/subsys/" DAEMON_NAME );
	
    /* Now we are a daemon -- do the work for which we were paid */
    int debug = 0; /* Setup the debug variable */


    void error(char *msg)
    {
        perror(msg);
        exit(1);
    }

    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
    	sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while(1){
    	newsockfd = accept(sockfd, 
        	(struct sockaddr *) &cli_addr, 
        	&clilen);
    	if (newsockfd < 0) 
        	error("ERROR on accept");

        bzero(buffer,256);
        n = read(newsockfd,buffer,255);

        if (n < 0) error("ERROR reading from socket");
        if ((lf=fopen(LOG_FILE,"a")) == NULL) {
            printf("\n Cant open log file\n");
            return(0);
        }
        fprintf(lf,"Here is the message: %s\n",buffer);
        fclose(lf);

        n = write(newsockfd,"I got your message",18);
        if (n < 0) error("ERROR writing to socket");
    }
	return 0;
}
