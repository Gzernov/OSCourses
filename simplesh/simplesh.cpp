#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <string.h>

using namespace std;

vector<int> runned_proc;

void kill_all(){
	while (!runned_proc.empty()){
		kill(runned_proc.back(), SIGINT);
		runned_proc.pop_back();	
	}
}

void uaction(int sig, siginfo_t* info, void* context){
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
		string command = rest;
		size_t i = 0;
		bool empty = true;
		while (red_size > 0) {
			red_size = read(0, buf, BUFF_SIZE);
			if (red_size == -1){
				break;
			}
			if (red_size != 0){
				empty = false;
			}
			char cur_str[red_size];
			memcpy (cur_str,  buf, red_size);
			command += string (cur_str, red_size);
			size_t new_line = command.find("\n");
			if (new_line != string::npos){
				rest = (new_line == command.length() - 1) ? "" : command.substr(new_line + 1, command.length() - new_line - 1); 
				command = command.substr(0, new_line);
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
				terminate = empty;
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
			if (param_begin < command.length() && param_begin != i){
				param = command.substr(param_begin, i - param_begin - (i >= command.length() ? 0 : 1) );
			}
			if (cmd == "exit"){
				terminate = true;
				break;
			}
			
			
			pipe(outfd);
			bool last = i >= command.length();
			int cur_proc = fork();
			
			if (!cur_proc){
				vector<int> spaces = vector<int>();
				size_t j = 0;
				for (; j < param.length(); j++){
					if (param[j] == ' '){
						spaces.push_back(j);
					}
				}
				char* args[3 + spaces.size()];
				args[0] = (char*) cmd.c_str();
				size_t beg = 0;
				for (j = 0; j < spaces.size(); j++){
					args[j + 1] = strdup((param.substr(beg, spaces[j] - beg)).c_str());
					beg = spaces[j] + 1;
				}
				args[1 + spaces.size()] = param == "" ? NULL : strdup((param.substr(beg, param.length() - beg)).c_str());
				args[2 + spaces.size()] = NULL;
				if (begin != 0){
					dup2(fromfd[0],0);
					close(fromfd[1]);
					close(fromfd[0]);
				}
				if (!last){
					dup2(outfd[1],1);	
				}
				execvp(args[0], args);
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
	}
}

