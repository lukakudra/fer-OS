#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

static int pid=0;
static int sig[]={SIGUSR1, SIGUSR2, SIGTERM, SIGQUIT};

void prekidna_rutina(int signal) {
	kill(pid, SIGKILL);
	exit(0);
}

int main(int argc, char *argv[]) {
	int i;
	int j;
	pid=atoi(argv[1]);
	sigset(SIGINT, prekidna_rutina);
	
	srand((unsigned)time(NULL));

	while(1) {
		i=rand()%4;
		j=rand()%3+3;
		sleep(j);
		kill(pid, sig[i]);
	}
	return 0;
}
