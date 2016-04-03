#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

char* signame(int sig){
	if (sig == SIGUSR1){
		return "SIGUSR1";
	} else if (sig == SIGUSR2){
		return "SIGUSR2";
	} 
}

void uaction(int sig, siginfo_t *info, void *context){
	printf("%s from %d\n", signame(sig), info->si_uid);
}

int main(){
	struct sigaction act;
	act.sa_sigaction = &uaction;
	act.sa_flags = SA_SIGINFO | SA_RESETHAND;
	int sigs[] = {SIGUSR1, SIGUSR2};
	size_t i;		
	for (i = 0; i < sizeof(sigs) / sizeof(int); i++){
		if (sigaction(sigs[i], &act, NULL) < 0) {
			printf ("error in handler while handled %s", signame(sigs[i]));
			return 1;
		}	
	}	
	if (sleep(10) == 0){
		printf("No signals were caught\n");	
	} 
	return 0;
}


