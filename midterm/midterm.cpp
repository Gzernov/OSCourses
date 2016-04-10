#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>

using namespace std;

vector<int> runned_proc;

void kill_all(){
	//cout<<"kill_all" << runned_proc.size()<<"\n";
	while (!runned_proc.empty()){
		kill(runned_proc.back(), SIGINT);
		runned_proc.pop_back();	
	}
}

void uaction(int sig, siginfo_t* info, void* context){
	//cout<<"SIGINT\n";
	kill_all();
}

int main(){
	string rest = "";
	struct sigaction act;
	act.sa_sigaction = &uaction;
	act.sa_flags = SA_SIGINFO;	
	if (sigaction(SIGINT, &act, NULL) < 0) {
		printf ("error in handler while SIGINT ");
		return 1;
	}
	while (true){
		write(1,"$\n",2);
		runned_proc = vector<int>();
		int const BUFF_SIZE = 1024;
		char buf[BUFF_SIZE];
		int red_size = 1;
		string command = "";
		//cout<<"rest:|"<<rest<<"|\n";
		size_t i = 0;
		while (red_size > 0) {
			red_size = read(0, buf, BUFF_SIZE);
			if (red_size == -1){
				break;
			}
			string subbuf;
			for (i = 0; i < red_size; i++){
				if (buf[i] == '\n'){
					subbuf = string(buf).substr(0, i);
					rest = i == (red_size - 1) ? "" : string(buf).substr(i,red_size - 1);
					//cout<<"i "<<i<<"rs "<< red_size<<"\n";
					//cout << "rest: >" << rest << "<";
					
					break;
				}
				
			}
			if (subbuf.length() == 0){
				command += string(buf);
			} else{
				command += string(subbuf);
				break;
			}
		}
		i = 0;	
		int outfd[2];
		int fromfd[2];
		int status;
		bool terminate = false;
		while (true){
			if (command.length() == 0){
				break;
			}
			string cmd = "";
			string param = "";
			size_t begin = i;
			while (command[i] != ' ' && i < command.length()){
				i++;
			}
			cmd = command.substr(begin, i - begin);
			size_t param_begin = i + 1;
			i++;
			while (command[i] != '|' && i < command.length()){
				i++;
			}
			//cout<<"b :"<<param_begin<<"e :"<<i;
			if (param_begin < command.length() && param_begin != i){
				param = command.substr(param_begin, i - param_begin - (i >= command.length() ? 0 : 1) );
			}
			if (cmd == "exit"){
				terminate = true;
				break;
			}
			//cout << "cmd: |" << cmd << "| param: >" << param <<"<\n";
			pipe(outfd);
			bool last = i >= command.length();
			int cur_proc = fork();
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
				return 0;
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
			runned_proc.insert(runned_proc.end(), cur_proc);
			
			fromfd[0] = outfd[0];
			fromfd[1] = outfd[1];
		
			while (wait(&status) > 0){
			}

			if (last){
				break;
			}
		}
		if (terminate){
			break;
		}
		//kill_all();
	}
}

