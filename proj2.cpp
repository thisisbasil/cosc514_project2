//  COSC514 Project2 Bowie State
//  Group: Sophie Johnson-Shapoval, Basil Huffman, Fekadu Urga
//  Program description is in the accompanying design document

#include <string>
#include <iostream>
#include <chrono>
#include <fstream> //ofstream/read in files
#include <deque>
#include <array>
#include <iomanip>
#include <memory>
using namespace std;
using namespace std::chrono; //clock


struct MemoryNode //node in a linked list
    {
        int pid;
        int pageNum;
        string readInChars;
        int StartLine;
        int EndLine;
        double CPUtime;
        shared_ptr<MemoryNode> next;
        shared_ptr<MemoryNode> prev;

        MemoryNode(int _pid, int _pageNum, const string& _readInChars,
                   int _StartLine, int _EndLine, double _CPUtime,
                   shared_ptr<MemoryNode> _next = nullptr,
                   shared_ptr<MemoryNode> _prev = nullptr) :
                pid(_pid), pageNum(_pageNum), readInChars(_readInChars),
                StartLine(_StartLine), EndLine(_EndLine), CPUtime(_CPUtime),
                next(_next), prev(_prev) {}
        MemoryNode() = default;
    };

struct PCB
{
    int pid; //process ID [1..10]
    int state; // process state [1..9]
    int progrCounter; // the number of a line which is currenly being read in from the file
    shared_ptr<MemoryNode> headMem;
    shared_ptr<PCB> next;
    shared_ptr<PCB> prev;

    PCB() = default;
    PCB(int _pid, int _state, int _progrCounter,
        shared_ptr<MemoryNode> _headMem = nullptr,
        shared_ptr<PCB> _next = nullptr,
        shared_ptr<PCB> _prev = nullptr) :
        pid(_pid), state(_state), progrCounter(_progrCounter),
        headMem(_headMem), next(_next), prev(_prev) {}
};

template <typename T>
struct list
{
    shared_ptr<T> front;
    shared_ptr<T> back;
    int count;
    list() : front(nullptr), back(nullptr), count(0) { }
    void add_back(T node)
    {
        shared_ptr<T> newnode = make_shared<T>(node);
        if (back == nullptr)
        {
            back = newnode;
            front = back;
        }
        else
        {
            back->next = newnode;
            newnode->prev = back;
            back = newnode;
        }
        count++;
    }

    void add_front(T node)
    {
        shared_ptr<T> newnode = make_shared(node);
        if (front == nullptr)
        {
            front = newnode;
            back = front;
        }
        else
        {
            front->prev = newnode;
            newnode->next = front;
            front = newnode;
        }
        count++;
    }

    void remove_front()
    {
        if (front == nullptr) return;

        if (front == back)
        {
            front.reset();
            back.reset();
        }
        else front = front->next;
        count--;
    }

    void remove_back()
    {
        if (back == nullptr) return;
        if (front == back)
        {
            front.reset();
            back.reset();
        }
        else back = back->prev;
        count--;
    }
};

string printPCB(const list<PCB>& _list)
    {
        string out;
        //PCB  *p;
        shared_ptr<PCB> p;
        p=_list.front;
        int index=0;

        if (p == nullptr) { out="E"; return out; }
        else{
            while (p->next!=nullptr)
            {   out+=to_string(p->pid); out+="->";
                p=p->next; index++;
                }
                out+=to_string(p->pid);
        }
        return out;
    }

void printMemNodes(shared_ptr<MemoryNode> head)//print the list
    {
        shared_ptr<MemoryNode> p = head;
        int index=0;

        if (p==nullptr) {cout<<"E"; return;}//Empty
        else
        { cout<<"\n";
            while (p->next!=nullptr)
            {
                cout<<"pid: "<<p->pid<<" PageNum "<<p->pageNum<<" contains " <<p->readInChars<<" in lines " <<p->StartLine <<"-" << p->EndLine <<" read at " <<p->CPUtime<<"\n";
                p=p->next; index++;
            }

            cout<<"pid: "<<p->pid<<" PageNum "<<p->pageNum<<" contains " <<p->readInChars<<" in lines " <<p->StartLine <<"-";
            if (p->EndLine>101) cout<< "100"; else cout << p->EndLine; cout
            <<" read at " <<p->CPUtime;
        }

    }

// code for A is 65, so have a base of 64
inline char determineToken(int procNum)//used in createFiles() only
{
    return static_cast<char>(64+procNum);
}

void createFiles()     //create files (for the 10 processes) to be opened and read in later
{
    ofstream myfile;
    string token=""; //A thru J
    string file_name="";
    for (int k=1; k<11; k++)
    {   file_name="";
        file_name+="proc"; file_name+=to_string(k); file_name+=".txt";
        token=determineToken(k);

        myfile.open(file_name, ios::trunc);
        for (int j=1; j<101; j++)
        {
            if (j>0 &&j<10)
                myfile << token << to_string(j) << token << to_string(j)
                       << token << to_string(j) << token << to_string(j)
                       << token << to_string(j) << endl;
            else if (j==100)
                myfile << token << to_string(j) << token << to_string(j) << token << '1' << endl;
            else
                myfile << token << to_string(j) << token << to_string(j) << token << to_string(j) << token<<"\n";
        }
        myfile.close();
    }
}


string readLineNFile(string file_name, int startLine, int endLine)
{
    string readInOneLine;
    string lines=""; //concatenated result for lines between start and end
    int numCount=0;

    ifstream fileRand (file_name);
    if (fileRand.is_open())  // if succes, this is true
        {
            while ( !fileRand.eof()) //read in one line, readInOneLine, at a time
            {
            getline (fileRand,readInOneLine) ; numCount++;
            if (numCount>=startLine && numCount<=endLine) //if numCount is outside the bounds, lines stays ""
                    {lines+=readInOneLine;}
            }
        }
    fileRand.close();

    return lines;
}

void addToHashTable(array<shared_ptr<MemoryNode>, 10>& hashtable, int idx, const MemoryNode& mn)
{
    shared_ptr<MemoryNode> newnode = make_shared<MemoryNode>(mn);
    if (hashtable[idx] == nullptr)
    {
        hashtable[idx] = newnode;
    }
    else
    {
        shared_ptr<MemoryNode> temp = hashtable[idx];
        while (temp->next != nullptr) temp = temp->next;
        newnode->prev = temp;
        temp->next = newnode;
    }
}

//void printInstanceForTable(PCB * hReady,PCB *hCPU,PCB *hWait,PCB *hdisk,PCB *hDone, double time)
void printInstanceForTable(const list<PCB>& ready, const list<PCB>& cpu, const list<PCB>& wait,
                           const list<PCB>& disk, const list<PCB>& done, double time)
{
    cout<<"\n";
    cout<<left
    << setw(15) << time
    << setw(30) << printPCB(ready)
    << setw(10) << printPCB(cpu)
    << setw(30) << printPCB(wait)
    << setw(10) << printPCB(disk)
    << setw(10) << printPCB(done) ;

}

int main(int argc, const char * argv[]) {
    list<PCB> ready, cpu, wait, disk, done, mem;

    createFiles(); //create 10 files for the 10 processes, to be read in later in the program
    string file_name, lines;

    int HTSIZe=10;
	array<list<MemoryNode>,10> hashtable;
    //initially create the 10 processes and assign all of them to the Ready Queue
    for (int k=1; k<11; k++)
    {
        PCB temp { k, 0, 0, hashtable[k-1].front };
        ready.add_back(temp);
    }


    //print table header
    cout<<"\n------------------------------------------------------------------------------------------------------------------";
    cout<<left
    << setw(15) <<"\nClockStamp"
    << setw(30) <<"Ready"
    << setw(10) <<"CPU"
    << setw(30) <<"Wait"
    << setw(10) <<"Disk"
    << setw(10) <<"Done" ;

    steady_clock::time_point t1 = steady_clock::now(); //grab out initial time stamp
    printInstanceForTable(ready,cpu,wait,disk,done, 0);

    //Process management Pipeline:
    while ( true )
    {
        //Time Stamp
        steady_clock::time_point t2 = steady_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
        double time_span_count=time_span.count();

        if (disk.count > 0)
        {
            PCB temp(disk.front->pid, 1, disk.front->progrCounter,
                     disk.front->headMem);
            ready.add_back(temp);
            disk.remove_front();

        } else { cout << "Nothing to remove" << endl; }
        
        if (wait.count > 0 && disk.count == 0)
        {
            PCB temp { wait.front->pid, 4, wait.front->progrCounter,
                       wait.front->headMem };
            wait.remove_front();
            disk.add_back(temp);

            shared_ptr<MemoryNode> p = hashtable[disk.front->pid - 1].front;
            file_name.clear();
            file_name = "proc";
            file_name += to_string(disk.front->pid);
            file_name += ".txt";

            int Startline = 0, Endline = 0, Pagenum = 0;
            if (hashtable[disk.front->pid - 1].front == nullptr)
            {
                Startline = 1;
                Pagenum = 0;
            }
            else if (hashtable[disk.front->pid - 1].front->next == nullptr)
            {
                Startline = p->EndLine + 1;
                Pagenum = p->pageNum + 1;
            }
            else 
            {
                
                Startline = hashtable[disk.front->pid - 1].back->EndLine + 1;
                Pagenum = hashtable[disk.front->pid - 1].back->pageNum + 1;
                Endline = Startline + 7;
            }

            lines.clear();
            lines = readLineNFile(file_name, Startline, Endline);
            hashtable[disk.front->pid - 1].add_back(MemoryNode(disk.front->pid, Pagenum,
                                                        lines, Startline, Endline,
                                                        time_span_count));
            //In the PCB of the process, adjust the program counter
            if (Endline <= 100) disk.front->progrCounter = Endline;
            else if (Endline > 101) disk.front->progrCounter = 100;
        }

        if (cpu.count > 0 && cpu.front->progrCounter != 100)
        {
            PCB temp { cpu.front->pid, 3, cpu.front->progrCounter,
                       cpu.front->headMem };
            cpu.remove_front();
            wait.add_back(temp);
        }

         if (cpu.count > 0 && cpu.front->progrCounter >= 100)
         {
            PCB temp { cpu.front->pid, 5, cpu.front->progrCounter,
                       cpu.front->headMem };
            cpu.remove_front();
            done.add_back(temp);
         }

         if (cpu.count == 0 && ready.count > 0)
         {
            PCB temp { ready.front->pid, 2, ready.front->progrCounter,
                       ready.front->headMem };
            ready.remove_front();
            cpu.add_back(temp);
         }
         if (done.count == 10) break;
    } //end Pipeline


    //print the contents of the memory nodes as well
    cout<<"\n\nThe contents of the memory for each process-------------------------------------------------------------------------";
    for (int j=0;j<10;j++)
    {
        shared_ptr<MemoryNode> p=hashtable[j].front;
        cout<<"\n";
        printMemNodes(p);
    }


    cout<<"\n\nBYE!\n";

    return 0;
}
