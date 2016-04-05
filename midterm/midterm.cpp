#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

using namespace std;

int cur_proc;

void uaction(int sig, siginfo_t* info, void* context){
	kill(cur_proc, SIGINT);
}

int main(){
	string rest = "";
	while (true){
	write(1,"$\n",2);
	int const BUFF_SIZE = 1024;
	char buf[BUFF_SIZE];
	int red_size = 1;
	string command = "";
	rest = "";
	size_t i;
	while (red_size > 0) {
		red_size = read(0, buf, BUFF_SIZE);
		string subbuf;
		for (i = 0; i <= red_size; i++){
			if (buf[i] == '\n'){
				subbuf = string(buf).substr(0, i);
				rest = string(buf).substr(i,red_size);
				break;
			}
			
		}
		if (subbuf.length() == 0){
			command += string(buf);
		}  else {
			command += string(subbuf);
			break;
		}
	}
	i = 0;	
	struct sigaction act;
	act.sa_sigaction = &uaction;
	act.sa_flags = SA_SIGINFO;	
   	int outfd[2];
 	int fromfd[2];
	int status;
	bool terminate;
	while (true){
		string cmd = "";
		string param = "";
		size_t begin = i;
		while (command[i] != ' ' && i < command.length()){
			i++;
		}
		cmd = command.substr(begin, i - begin);
		size_t param_begin = i + 1;
		while (command[i] != '|' && i < command.length()){
			i++;
		}
		if (param_begin < command.length()){
			param = command.substr(param_begin, i - param_begin - (i >= command.length() ? 0 : 1) );
		}
		pipe(outfd);
		bool last = i >= command.length();
		cur_proc = fork();
	/*	if (sigaction(SIGINT, &act, NULL) < 0) {
			printf ("error in handler while SIGINT ");
			return 1;
		}	*/
    		if (!cur_proc){
			if (begin != 0){
				dup2(fromfd[0],0);
				close(fromfd[1]);
       				close(fromfd[0]);
			}
			if (!last){
				dup2(outfd[1],1);	
			}
       			execlp(cmd.c_str(),cmd.c_str(),param == "" ? NULL : param.c_str(),NULL);
    		} else {
			if (begin != 0){
        			close(fromfd[1]);
       				close(fromfd[0]);
			}
			if (last){
				close (outfd[1]);
				close (outfd[0]);
			}
		}
		i+=2;
		
		fromfd[0] = outfd[0];
		fromfd[1] = outfd[1];
			
		while (wait(&status) > 0){
		}

		if (last){
			break;
		}
	}	
}
}

