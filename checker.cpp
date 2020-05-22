#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "sockop.h"
#define BUF_SIZE 1024
using namespace std;

bool log_static[100]={0};
string my_id[100] = {""};
vector<string>log_id;

struct save_area{
    vector<int>start;
    vector<int>end;
}save_areas;

struct city{
    int id;
    string name;
    int mask[3];
};

struct store{
    int num;
    string name;
    vector<city> citys;
};

struct file{
    int num;
    int day;
    int alcohol;
    int ori_alcohol;
    vector<store> stores;
}mydata;

struct log_file{
    int index;
    int day;
    int connfd;
    string command;
    string results;
};

int string2int(string input){
    stringstream ss;
    int output;
    ss << input;
    ss >> output;
    return output;
}

string int2string(int input){
    stringstream ss;
    string output;
    ss << input;
    ss >> output;
    return output;
}

string data2string(){
    string msg, temp;
    stringstream ss;
    msg = "";
    for (int i = 0; i < mydata.num; i++)
    {
        msg = msg+mydata.stores[i].name+" ";
        //msg = msg+int2string(mydata.stores[i].num)+"\n";
        for (int j = 0; j < mydata.stores[i].citys.size(); j++)
        {
            msg = msg+int2string(mydata.stores[i].citys[j].id)+" ";
            msg = msg+mydata.stores[i].citys[j].name+" ";
            for(int k = 0 ; k<3 ; k++){
                msg = msg+int2string(mydata.stores[i].citys[j].mask[k])+" ";
            }
            //msg = msg + " ";
        }
    }
    return msg;
}

void read_file(string argv){
    fstream read_file;
    read_file.open(argv.c_str(), ios::in);
    if (!read_file){
        cout << "can't open input file\n";
        exit(1);
    }
    string data = "";
    //stringstream ss;
    read_file >> data;
    mydata.num = string2int(data);
    for (int i = 0; i < mydata.num; i++){
        store temp_store;
        read_file >> data;
        temp_store.name = data;
        read_file >> data;
        temp_store.num = string2int(data);
        for (int j = 0; j < temp_store.num; j++){
            city temp_city;
            read_file >> data;
            temp_city.id = string2int(data);
            read_file >> data;
            temp_city.name = data;
            for (int k = 0; k < 3; k++){
                read_file >> data;
                temp_city.mask[k] = string2int(data);
            }
            temp_store.citys.push_back(temp_city);
        }
        mydata.stores.push_back(temp_store);
    }
}

void show_data(){
    for (int j = 0; j < mydata.num; j++)
    {
        cout << mydata.stores[j].name << endl;
        cout << mydata.stores[j].num << endl;
        for (int k = 0; k < mydata.stores[j].citys.size(); k++)
        {
            cout << mydata.stores[j].citys[k].id << " ";
            cout << mydata.stores[j].citys[k].name << " ";
            cout << mydata.stores[j].citys[k].mask[0] << " ";
            cout << mydata.stores[j].citys[k].mask[1] << " ";
            cout << mydata.stores[j].citys[k].mask[2] << endl;
        }
    }
    cout<<mydata.day<<" "<<mydata.alcohol<<endl;
}

string vector2string(vector<string> read_from_server){
    string msg = "";
    int count = 0;
    int num = mydata.num;
    //msg = msg+ read_from_server[0] + "\n";
    for(int i=0 ; i<num ; i++){
        msg = msg+read_from_server[count]+"\n";
        count++;
        int num2 = mydata.stores[i].num;
        //msg = msg+read_from_server[count]+"\n";
        //count++;
        for(int j=0 ; j<num2 ; j++){
            for(int k=0 ; k<5 ; k++){
                msg = msg+read_from_server[count]+" ";
                count++;
            }
            msg = msg + "\n";
        }
    }
    return msg;
}

void update(){
    if(mydata.day>7){
        mydata.day = 1;
        log_id.clear();
    }
    if(mydata.alcohol <5)
        mydata.alcohol = mydata.ori_alcohol;
}

void show_log(vector<log_file> log_files){
    cout<<"size:"<<log_files.size()<<endl;
    for(int i=0 ; i<log_files.size() ; i++){
        cout<<log_files[i].index<<" "<<log_files[i].day<<" "<<log_files[i].connfd<<" ";
        cout<<log_files[i].command<<" ";
        for(int j=0 ; j<log_files[i].results.size() ; j++){
            cout<<log_files[i].results[j]<<" ";
        }
        cout<<endl;
    }
}

vector<string> string2vector(string p){
    string s;
    stringstream ss;
    vector<string>read_from_server;
    ss<<p;
    while(!ss.eof()){
        ss>>s;
        read_from_server.push_back(s);
    }
    return read_from_server;
}

vector<log_file>read_log_file(string argv){
    ifstream file;
    vector<log_file> log;
    char *p;
    file.open(argv.c_str(), ifstream::in);
    while (!file.eof()){
        char ss[1024];
        log_file temp;
        file.getline(ss, 1024);
        if(strlen(ss)==0)
            break;
        p = strtok(ss, ",");
        temp.index = atoi(p);
        p = strtok(NULL, ",");
        temp.day = atoi(p);
        p = strtok(NULL, ",");
        temp.connfd = atoi(p);
        p = strtok(NULL, ",");
        temp.command = p;
        p = strtok(NULL, ",");
        temp.results = p;
        log.push_back(temp);
    }
    return log;
}

bool judge_correction(string msg){
    if(msg.size() >10){
        cout<<"the length is excessed 10\n";
        return 0;
    }
    int num[10];
    num[0] = msg[0]-55;
    switch(msg[0]){
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'W':
            num[0] = msg[0]-55;
            break;
        case 'I':
            num[0] = msg[0]-39;
            break;
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
            num[0] = msg[0]-56;
            break;
        case 'O':
            num[0] = msg[0]-44;
            break;
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'Z':
            num[0] = msg[0]-57;
            break;
        case 'X':
        case 'Y':
            num[0] = msg[0]-58;
            break;
    }
    for(int i=1 ; i<msg.size() ; i++){
        string s;
        s.push_back(msg[i]);
        num[i] = string2int(s);
    }
    int Y = num[0]/10 + (num[0]%10)*9;
    int sum = Y;
    for(int i=1 ; i<9 ; i++){
        sum+=num[i]*(9-i);
    }
    int Z = 10-sum%10;
    if(Z == num[9])
        return 1;
    return 0;
}

void correct_answer(int index, string answer, string wrong){
    cout<<"------------------------------------------"<<endl;
    cout<<"index :"<<index<<endl;
    cout<<"the correction answer is : \n"<<answer<<endl<<endl;
    cout<<"your answer is : \n"<<wrong<<endl;
    cout<<"------------------------------------------"<<endl;
}

bool judge_day(string id){
    int today = mydata.day;
    int n = id[9]-48;
    if(today==7){
        return true;
    }
    else if((today%2==1 && n%2==1) || (today%2==0 && n%2==0)){
            return true;
        }
    return false;
}

bool check_history(string id){
    for(int i=0 ; i<log_id.size() ; i++){
        if(id == log_id[i])
            return false;
    }
    return true;
}

bool deal_buying(vector<string>msg){
    int array[4], sum = 0;
    bool flag = false;
    for(int i=0 ; i<4 ; i++){
        array[i] = string2int(msg[i+2]);
    }
    array[0] -=1;
    sum = array[1]+array[2]+array[3];
    if(sum>3)
        return false;
    for(int i=0 ; i<mydata.num ; i++){
        if(msg[1] == mydata.stores[i].name ){
            if(array[0]>=mydata.stores[i].num)
                return false;
            for(int j=0 ; j<3 ; j++){
                if(mydata.stores[i].citys[array[0]].mask[j]-array[j+1]<0)
                    return false;
            }
            for(int j=0 ; j<3 ; j++){
                mydata.stores[i].citys[array[0]].mask[j] = mydata.stores[i].citys[array[0]].mask[j]-array[j+1];
            }
            flag = true;
        }
    }
    if(flag)
        return true;
    return false;
}

void check_1(vector<log_file> log_files){
    string command;
    int count = 0;
    int not_check=5;
    for(int i=0 ; i<log_files.size() ; i++){
        command = log_files[i].command;
        if(mydata.day!=log_files[i].day && not_check == 0){
            correct_answer(log_files[i].index,"Day "+int2string(mydata.day), "Day "+int2string(log_files[i].day));
            count++;
            //break;
        }
        if(strncmp(command.c_str(), "login", 5)==0){
            string msg = command.substr(6,command.length()-6);
            bool flag = judge_correction(msg);
            if(flag && log_static[log_files[i].connfd]==0){
                if(strncmp(log_files[i].results.c_str(),"Login successful.", strlen("Login successful."))){
                    count++;
                    correct_answer(log_files[i].index,"Login successful.", log_files[i].results);
                    //break;
                }
                my_id[log_files[i].connfd] = msg;
                log_static[log_files[i].connfd]=1;
            }
            else{
                if(strncmp(log_files[i].results.c_str(), "Login failed.",strlen("Login failed"))){
                    count++;
                    correct_answer(log_files[i].index,"Login failed.", log_files[i].results);
                    //break;
                }
            }
        }
        else if(strcmp(command.c_str(),"logout")==0){
            if(log_static[log_files[i].connfd] == 0){
                if(strncmp(log_files[i].results.c_str(),"Logout failed.", strlen("Logout failed."))){
                    count++;
                    correct_answer(log_files[i].index,"Logout failed.", log_files[i].results);
                    //break;
                }
            }
            else{
                if(strncmp(log_files[i].results.c_str(),"Logout successful.", strlen("Logout successful."))){
                    count++;
                    correct_answer(log_files[i].index,"Logout successful.", log_files[i].results);
                    //break;
                }
                log_static[log_files[i].connfd]=0;
                log_id.push_back(my_id[log_files[i].connfd]);
                //mydata.day++;
                update();
            }
        }
        else if(strncmp(command.c_str(), "buy", 3)==0){
            vector<string>buy_msg;
            string tmp;
            stringstream ss;
            int connfd = log_files[i].connfd;
            ss << command;
            while(!ss.eof()){
                ss>>tmp;
                buy_msg.push_back(tmp);
            }
            if(buy_msg.size()!=6 || log_static[connfd]==0 || !judge_day(my_id[connfd]) || !check_history(my_id[connfd])){
                if(strncmp(log_files[i].results.c_str(),"Buy failed.", strlen("Buy failed."))){
                    //cout<<judge_day(my_id[connfd])<<" "<<check_history(my_id[connfd])<<endl;
                    count++;
                    correct_answer(log_files[i].index,"Buy failed.", log_files[i].results);
                    //break;
                }
            }
            else{
                if(deal_buying(buy_msg)){
                    mydata.alcohol--;
                    mydata.day++;
                    log_static[log_files[i].connfd]=0;
                    log_id.push_back(my_id[log_files[i].connfd]);
                    update();
                    save_areas.end.push_back(i);
                    for(int j = i-1 ; j>=0 ; j--){
                        if(log_files[j].connfd == log_files[i].connfd){
                            save_areas.start.push_back(j);
                            break;
                        }
                    }
                    if(strncmp(log_files[i].results.c_str(),"Disinfecting hands with alcohol. Putting masks into envelope. Thank you for your purchase.", strlen("Disinfecting hands with alcohol. Putting masks into envelope. Thank you for your purchase."))){
                        count++;
                        correct_answer(log_files[i].index,"Disinfecting hands with alcohol. Putting masks into envelope. Thank you for your purchase.", log_files[i].results);
                        //break;
                    }
                    not_check = 4;
                }
                else{
                    if(strncmp(log_files[i].results.c_str(),"Buy failed.", strlen("Buy failed."))){
                        count++;
                        correct_answer(log_files[i].index,"Buy failed.", log_files[i].results);
                        //break;
                    }
                }
            }
        }
        else if(strcmp(command.c_str(), "show")==0){
        }
        else if(strcmp(command.c_str(), "statics")==0){
        }
        else{
            if(strncmp(log_files[i].results.c_str(),"Input format not valid.", strlen("Input format not valid."))){
                count++;
                correct_answer(log_files[i].index,"Input format not valid.", log_files[i].results);
                //break;
            }
        }
        if(not_check>0){
            // not_check--;
        }
    }
}

bool check_flag(int index){
    for(int i=0 ; i<save_areas.start.size() ; i++){
        if(index > save_areas.start[i] && index<save_areas.end[i])
            return false;
    }
    return true;
}

void check_2(vector<log_file> log_files){
    string command;
    int count = 0;
    bool flag;
    int not_check=5;
    for(int i=0 ; i<log_files.size() ; i++){
        flag = true;
        command = log_files[i].command;
        if(mydata.day!=log_files[i].day && not_check == 0){
            correct_answer(log_files[i].index,"Day "+int2string(mydata.day), "Day "+int2string(log_files[i].day));
            count++;
            //break;
        }
        if(strncmp(command.c_str(), "login", 5)==0){
            string msg = command.substr(6,command.length()-6);
            bool flag = judge_correction(msg);
            if(flag && log_static[log_files[i].connfd]==0){
                if(strncmp(log_files[i].results.c_str(),"Login successful.", strlen("Login successful."))){
                    count++;
                    correct_answer(log_files[i].index,"Login successful.", log_files[i].results);
                    //break;
                }
                my_id[log_files[i].connfd] = msg;
                log_static[log_files[i].connfd]=1;
            }
            else{
                if(strncmp(log_files[i].results.c_str(), "Login failed.",strlen("Login failed"))){
                    count++;
                    correct_answer(log_files[i].index,"Login failed.", log_files[i].results);
                    //break;
                }
            }
        }
        else if(strcmp(command.c_str(),"logout")==0){
            if(log_static[log_files[i].connfd] == 0){
                if(strncmp(log_files[i].results.c_str(),"Logout failed.", strlen("Logout failed."))){
                    count++;
                    correct_answer(log_files[i].index,"Logout failed.", log_files[i].results);
                    //break;
                }
            }
            else{
                if(strncmp(log_files[i].results.c_str(),"Logout successful.", strlen("Logout successful."))){
                    count++;
                    correct_answer(log_files[i].index,"Logout successful.", log_files[i].results);
                    //break;
                }
                log_static[log_files[i].connfd]=0;
                log_id.push_back(my_id[log_files[i].connfd]);
                //mydata.day++;
                update();
            }
        }
        else if(strncmp(command.c_str(), "buy", 3)==0){
            vector<string>buy_msg;
            string tmp;
            stringstream ss;
            int connfd = log_files[i].connfd;
            ss << command;
            while(!ss.eof()){
                ss>>tmp;
                buy_msg.push_back(tmp);
            }
            if(buy_msg.size()!=6 || log_static[connfd]==0 || !judge_day(my_id[connfd]) || !check_history(my_id[connfd])){
                if(strncmp(log_files[i].results.c_str(),"Buy failed.", strlen("Buy failed."))){
                    //cout<<judge_day(my_id[connfd])<<" "<<check_history(my_id[connfd])<<endl;
                    count++;
                    correct_answer(log_files[i].index,"Buy failed.", log_files[i].results);
                    //break;
                }
            }
            else{
                if(deal_buying(buy_msg)){
                    mydata.alcohol--;
                    mydata.day++;
                    log_static[log_files[i].connfd]=0;
                    log_id.push_back(my_id[log_files[i].connfd]);
                    update();
                    if(strncmp(log_files[i].results.c_str(),"Disinfecting hands with alcohol. Putting masks into envelope. Thank you for your purchase.", strlen("Disinfecting hands with alcohol. Putting masks into envelope. Thank you for your purchase."))){
                        count++;
                        correct_answer(log_files[i].index,"Disinfecting hands with alcohol. Putting masks into envelope. Thank you for your purchase.", log_files[i].results);
                        //break;
                    }
                    not_check = 4;
                }
                else{
                    if(strncmp(log_files[i].results.c_str(),"Buy failed.", strlen("Buy failed."))){
                        count++;
                        correct_answer(log_files[i].index,"Buy failed.", log_files[i].results);
                        //break;
                    }
                }
            }
        }
        else if(strcmp(command.c_str(), "show")==0){
            string s = data2string();
            //vector<string>temp = string2vector(log_files[i].results);
            //string msg = vector2string(temp);
            string msg = log_files[i].results;
            flag = check_flag(i);
            if(s!=msg && flag){
                count++;
                correct_answer(log_files[i].index, s, msg);
                //break;
            }
        }
        else if(strcmp(command.c_str(), "statics")==0){
            int x = atoi(log_files[i].results.c_str());
            flag = check_flag(i);
            if(x != mydata.alcohol && flag){
                count++;
                correct_answer(log_files[i].index,int2string(mydata.alcohol), int2string(x));
                //break;
            }
        }
        else{
            if(strncmp(log_files[i].results.c_str(),"Input format not valid.", strlen("Input format not valid."))){
                count++;
                correct_answer(log_files[i].index,"Input format not valid.", log_files[i].results);
                //break;
            }
        }
    }
    if(count ==0){
        cout<<"Congratulations!!! They are all correct!!!"<<endl;
    }
    else if (count==1){
        cout<<"There is "<<count<<" error"<<endl;
        cout<<"Maybe you can find it ><"<<endl;
    }
    else{
        cout<<"There are "<<count<<" errors QQQQ"<<endl;
        cout<<"Please keep on going and never give up."<<endl;
    }
    if(not_check>0){
        // not_check--;
    }
}

void clear(){
    log_id.clear();
    for(int i=0 ; i<100 ; i++){
        log_static[i] = 0;
        my_id[i] = "";
    }
}

string server, port;
vector<string> commands[30];

double timeStart, timeEnd;
timespec time1, time2;

timespec diff(timespec start, timespec end) {
    timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

void read_command(string filename){
    ifstream file;
    file.open(filename, ifstream::in);
    if (!file){
        cerr<<"Error opening file.\n";
        exit(1);
    }
    while(!file.eof()){
        string str;
        getline(file, str);
        if(file.eof()){
            break;
        }
        stringstream ss(str);
        string idx, client_idx, cmd;
        getline(ss, idx, ',');
        getline(ss, client_idx, ',');
        getline(ss, cmd, ',');
        commands[atoi(client_idx.c_str())].push_back(cmd);
        // cout<<"/ "<<idx<<" / "<<client_idx<<" / "<<cmd<<endl;
    }
    file.close();
}


void myclient(){
    int connfd[30];
    for(int i = 0 ; i < 30 ; i++){
        if(!commands[i].empty()){
            connfd[i] = connectsock(server.c_str(), port.c_str(), "tcp");
        }
    }
    int closed_fd = 0;
    while(closed_fd < 30){
        // cout<<closed_fd<<endl;
        for(int i = 0 ; i < 30 ; i++){
            if(!commands[i].empty()){
                string snd;
                snd = commands[i][0];
                commands[i].erase(commands[i].begin()); 
                if(write(connfd[i], snd.c_str(), snd.length()) < 0){
                    cerr<<"Error write.\n";
                }
                // cout<<snd<<endl;
            }
            else if(connfd[i] != -1){
                close(connfd[i]);
                closed_fd++;
                connfd[i] = -1;
            }
            // usleep(10);
        }
        for(int i = 0 ; i < 30 ; i++){

            if(connfd[i] != -1){
                char rcv[BUF_SIZE];
                memset(rcv, 0, BUF_SIZE);
                int n = read(connfd[i], rcv, BUF_SIZE);
                if(n == 0){
                    cout<<connfd[i]<<" disconnect\n";
                    break;
                }
                else if(n == -1){
                    perror("Error read :");
                    break;
                }
                string msg (rcv);
                // cout<<msg<<endl;
                if( msg.find("Disinfecting") != -1){
                    while((msg.find("purchase.") == -1)){
                        memset(rcv, 0, BUF_SIZE);
                        n = read(connfd[i], rcv, BUF_SIZE);
                        if(n == 0){
                            cout<<connfd[i]<<" disconnect\n";
                            break;
                        }
                        else if(n == -1){
                            perror("Error read :");
                            break;
                        }
                        msg = rcv;
                        // cout<<msg<<endl;
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]){
    clock_gettime(CLOCK_REALTIME, & time1);
    if(argc != 6){
        cerr<<"Please enter correct input: ./checker <ip> <port> <day of week> <alcohol amount> <command file>\n";
        exit(1);
    }

    // signal(SIGINT, handler);
    server = argv[1];
    port = argv[2];
    read_command(argv[5]);
    myclient();

    clock_gettime(CLOCK_REALTIME, & time2);
    cout << diff(time1, time2).tv_sec << "." << diff(time1, time2).tv_nsec <<" s"<< endl;
    cout << "Please stop the server, and press enter\n";
    string str;
    getline(cin, str);
    mydata.alcohol = atoi(argv[4]);
    mydata.ori_alcohol = atoi(argv[4]);
    mydata.day = atoi(argv[3]);
    read_file("inventory.txt");
    file temp = mydata;
    vector<log_file> log_files = read_log_file("result.txt");
    //check_log_file(log_files);
    check_1(log_files);
    //cout<<save_areas.start.size()<<endl;
    clear();
    mydata = temp;
    check_2(log_files);
    return 0;
}