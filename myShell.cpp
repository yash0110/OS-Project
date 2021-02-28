#include <bits/stdc++.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

using namespace std;

set<char> DELIM{' ','\r','\t','\n','\a'};

deque<string> records;
map<string,char**> aliases;

struct job{
    int pid;
    string path;
    vector<string> command;

    job(int x, char* y, char** z){
        pid=x;
        int i=0;
        while(y[i]!='\0')
            path.push_back(y[i++]);
        path.push_back('\0');
        
        i=0;
        while(z[i]){
            int j=0;
            string temp;
            while(z[i][j]!='\0')
                temp.push_back(z[i][j++]);
            temp.push_back('\0');
            command.push_back(temp);
            i++;
        }
    }

    void print_command(){
        for(auto it:command)
            cout<<it;
        cout<<endl;
    }

    void print_job(){
        cout<<" "<<pid;
        int l=10-(to_string(pid)).size();
        while(l--)
            cout<<" ";
        cout<<path;
        l=35-path.size();
        while(l--)
            cout<<" ";
        print_command();
    }
};

list<job> curr_jobs;
int find_pid,curr_fgp=-1;

bool found(const job& j){
    return (j.pid==find_pid);
}

bool remove_from_list(int pid){
    find_pid=pid;
    int old_size=curr_jobs.size();

    curr_jobs.remove_if(found);
    int new_size=curr_jobs.size();

    return (new_size!=old_size);
}

char *builtinstrs[]={   
    (char*)"cd",
    (char*)"help",
    (char*)"exit",
    (char*)"bg",
    (char*)"joblist",
    (char*)"kill",
    (char*)"to_fg",
    (char*)"history",
    (char*)"alias"
};

int num_builtins=sizeof(builtinstrs)/sizeof(char*);

bool cd(char** args){
    if(args[1]==NULL){
        cerr<<"more arguements expected in the command \"cd\""<<endl;
        return true;
    }
    if(args[2]!=NULL){
        cerr<<"more than expected arguements in the command \"cd\""<<endl;
        return true;
    }

    if(chdir(args[1])<0)
        cerr<<"No such directory present!"<<endl;
    return true;
}

bool help(char** args){
    if(args[1]!=NULL){
        cerr<<"more than expected arguements in the command \"help\""<<endl;
        return true;
    }

    cout<<"Yash Gandhi's Shell:"<<endl;
    cout<<"Type commands and their arguements to run them."<<endl;
    cout<<"These are the builtin commands: "<<endl;
    for(int i=0;i<num_builtins;i++)
        cout<<builtinstrs[i]<<endl;

    return true;
}

bool exit(char** args){
    return false;
}

bool bg(char** args){
    if(args[1]==NULL){
        cerr<<"more arguements expected in the command \"bg\""<<endl;
        return true;
    }
    args++;
    
    pid_t child=fork();
    if(child<0){
        cerr<<"fork() failed"<<endl;
        exit(1);
    }
    else if(child==0){
        int status=execvp(args[0],args);
        if(status<0){
            cerr<<"execvp() failed"<<endl;
            exit(1);
        }
    }
    else{
        char path[1000];
        getcwd(path,sizeof(path));

        job temp(child,path,args);
        curr_jobs.push_back(temp);
    }

    return true;
}

bool joblist(char** args){
    if(args[1]){
        cerr<<"more arguements expected in the command \"joblist\""<<endl;
        return true;
    }

    cout<<"====================================================================================="<<endl;
    cout<<" PID       WORKING DIRECTORY                  COMMAND"<<endl;
    for(auto it:curr_jobs)
        it.print_job();
    cout<<"Number of background jobs = "<<curr_jobs.size()<<endl;
    cout<<"====================================================================================="<<endl;

    return true;
}

bool kill(char** args){
    if(args[1]==NULL){
        cerr<<"more arguements expected in the command \"kill\""<<endl;
        return true;
        
    }
    if(args[2]!=NULL){
        cerr<<"more than expected arguements in the command \"kill\""<<endl;
        return true;
    }

    int id=atoi(args[1]);
    if(remove_from_list(id)){
        if(kill(id,SIGTERM)<0)
            cerr<<"kill() failed"<<endl;
    }
    else
        cout<<"No process with id "<<id<<" running in the background!"<<endl;
    
    return true;
}

bool to_fg(char** args){
    if(args[1]==NULL){
        cerr<<"more arguements expected in the command \"to_fg\""<<endl;
        return true;
        
    }
    if(args[2]!=NULL){
        cerr<<"more than expected arguements in the command \"to_fg\""<<endl;
        return true;
    }

    int pid=atoi(args[1]);
    
    find_pid=pid;
    auto it=find_if(curr_jobs.begin(),curr_jobs.end(),found);
    if(it==curr_jobs.end()){
        cerr<<"No process with pid "<<pid<<" running in the background"<<endl;
        return true;
    }

    it->print_command();
    remove_from_list(pid);

    int status;
    do{
        waitpid(pid,&status,WUNTRACED);
    }while(!WIFEXITED(status)&&!WIFSIGNALED(status));

    return true;
}

bool history(char** args){
    if(args[1]!=NULL){
        cerr<<"more than expected arguements in the command \"help\""<<endl;
        return true;
    }
    int i=1;
    for(auto it:records){
        cout<<"  ";
        if(i<10)
            cout<<"  ";
        else if(i<100)
            cout<<" ";
        cout<<i++<<"  ";
        cout<<it<<endl;
    }

    return true;
}

bool alias(char** args){
    if(args[1]==NULL){
        cerr<<"more arguements expected in the command \"alias\""<<endl;
        return true;
        
    }
    if(args[2]!=NULL){
        cerr<<"more than expected arguements in the command \"alias\""<<endl;
        return true;
    }

    string temp;
    for(int i=0;args[1][i]!='\0';i++)
        temp.push_back(args[1][i]);

    // string lhs,rhs;
    string lhs,right;
    char* rhs=new char[100];
    int i=0,j=0;
    for(;temp[i]!='='&&i<temp.length();i++)
        lhs.push_back(temp[i]);

    i++;
    if(i+1>=temp.length()||temp[i]!='\''||temp.back()!='\''){
        cerr<<"invalid format"<<endl;
        return true;
    }

    j=0;
    i++;
    for(;i<temp.length()-1;i++){
        rhs[j++]=temp[i];
        right.push_back(temp[i]);
    }
    rhs[j]='\0';
    
    if(lhs==right)
        return true;

    char** newargs=new char*;
    newargs[0]=rhs;
    newargs[1]=NULL;
    
    aliases[lhs]=newargs;

    return true;
}

bool (*builtinfuncs[])(char**)={
    &cd,
    &help,
    &exit,
    &bg,
    &joblist,
    &kill,
    &to_fg,
    &history,
    &alias
};

bool cmnds(char** args){
    pid_t pid;
    int status;

    pid=fork();
    if(pid<0){
        cerr<<"fork() failed"<<endl;
        exit(1);
    }
    else if(pid==0){
        int execstatus=execvp(args[0],args);
        if(execstatus==-1){
            cerr<<"execvp() failed"<<endl;
            return false;
        }
    }
    else{
        curr_fgp=pid;
        do{
            waitpid(pid,&status,WUNTRACED);
        }while(!WIFEXITED(status)&&!WIFSIGNALED(status)&&!WIFSTOPPED(status));
    }

    return true;
}

bool execute(char** args){
    if(args[0]==NULL){
        cerr<<"No commands entered!"<<endl;
        return true;
    }

    if(args[1]==NULL){
        string temp;
        for(int i=0;args[0][i]!='\0';i++)
            temp.push_back(args[0][i]);
        
        if(aliases.count(temp))
            return execute(aliases[temp]);
    }

    for(int i=0;i<num_builtins;i++){
        if(strcmp(args[0],builtinstrs[i])==0)
            return (*builtinfuncs[i])(args);
    }
    
    return cmnds(args);
}

void broadcastTermination(int pid, int status){
    cout<<"Process with pid "<<pid<<" ";
    if(WIFEXITED(status))
        cout<<"exited , status = "<<WEXITSTATUS(status)<<endl;
    else if(WIFSIGNALED(status))
        cout<<"killed by signal "<<WTERMSIG(status)<<endl;
    else if(WIFSTOPPED(status))
        cout<<"stopped by signal "<<WSTOPSIG(status)<<endl;
    else if(WIFCONTINUED(status))
        cout<<"continued"<<endl;
    
    remove_from_list(pid);
}

static void signalHandler1(int sig){
    int status;
    int pid=waitpid(-1,&status,WUNTRACED);
    
    if(pid<0)
        return;

    broadcastTermination(pid,status);
}

// static void signalHandler2(int sig){
//     kill(curr_fgp,SIGSTOP);
//     cout<<"stopped"<<endl;
//     sleep(15);
//     cout<<"continued"<<endl;
//     kill(curr_fgp,SIGCONT);
// }

void myShell(){

    int status;
    char path[1000];
    cout<<endl;

    signal(SIGCHLD,signalHandler1);
    // signal(SIGTSTP,signalHandler2);

    do{
        getcwd(path,sizeof(path));
        cout<<"\033[1;34mMyShell:\033[1;32m";
        cout<<path<<"\033[1;31m>>\033[0m ";

        //READ LINE
        string line;
        getline(cin,line);

        if(line.size()){
            if(records.size()==999)
                records.pop_front();
            records.push_back(line);
        }

        //PARSING THE LINE
        char *args[100];
        int curr=0,i=0;
        string s;

        for(int i=0;i<line.size();i++){
            if(DELIM.count(line[i])){
                if(s.size()){
                    char* temp=new char[100];
                    for(int j=0;j<s.size();j++)
                        temp[j]=s[j];
                    temp[s.size()]='\0';
                    args[curr++]=temp;
                }
                s.clear();
            }
            else
                s.push_back(line[i]);
        }
        if(s.size()){
            char* temp=new char[100];
            for(int j=0;j<s.size();j++)
                temp[j]=s[j];
            temp[s.size()]='\0';
            args[curr++]=temp;
        }
        args[curr]=NULL;

        //EXECUTING THE COMMAND
        status=execute(args);
    }while(status);

    cout<<endl;
}

int main(int argc, char* argv[]){

    myShell();

    return 0;
}