//  COSC514 Project2 Bowie State
//  Group: Sophie Johnson-Shapoval, Basil Huffman, Fekadu Urga
//  Program description is in the accompanying design document (slides) and a report
#include <cassert>
#include <string>
#include <iostream>
#include <chrono>
#include <fstream> //ofstream/read in files
#include <array>
#include <iomanip>
#include <memory>
#include <random>
using namespace std;
using namespace std::chrono; //clock


struct MemoryNode //Node in a linked list. By the design choice, each process has a list of memory nodes organized 
	//in a hash table consisting of head ptrs to the processes' memory lists. The PCBs reference these head ptrs.
    {
        int pid; // process number
        int pageNum;  //page and frame are 1-1 mapping
        string readInChars; // this string contains the read-in data for the frame
        int StartLine; //ref the start lines in the proc's file
        int EndLine; // ref the end lines in the proc's file
        double CPUtime; // steady_clock() is used for timestamps inside the program
        MemoryNode* next; //in the final design, this is doubly-linked list; initially it was a singly-linked list
        MemoryNode* prev;

        MemoryNode(int _pid, int _pageNum, const string& _readInChars,
                   int _StartLine, int _EndLine, double _CPUtime
                   ,MemoryNode* _next = nullptr,
                   MemoryNode* _prev = nullptr) :
                pid(_pid), pageNum(_pageNum), readInChars(_readInChars),
                StartLine(_StartLine), EndLine(_EndLine), CPUtime(_CPUtime)
                ,next(_next), prev(_prev) {}
        MemoryNode() = default;
    };


void printMemNodes(MemoryNode* head)//print the list of memory nodes. This function is called to display to the user what was read in by each process.
    {
        MemoryNode* p = head;
        int index=0;
		cout << endl;
        if (p==nullptr) {cout<<"E"; return;}//Empty list
        else
        { 
	cout<<"\n";
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




struct PCB // this represents each of the 10 processes, these nodes are moved among the five queues in the pipeline
{
    int pid; //process number/ID [1..10]
    int state; // process states. 1: in Ready; 2: in CPU; 3: in Wait; 4: in Disk and 5: in Done.
    int progrCounter; // the number of a line which is currenly being read in from the file
    MemoryNode* headMem; // this is the head ptr to the memory list for a process.

    PCB() = default;

    PCB(int _pid, int _state, int _progrCounter,
        MemoryNode* _headMem = nullptr) :
        pid(_pid), state(_state), progrCounter(_progrCounter), headMem(_headMem) {}

};

// double ended, double linked list
// allows for O(1) insertions and deletions at beginning and end
template <typename T>
class list
{
public:
	
    struct NODE
    {
        T val;
        NODE* next;
        NODE* prev;
        NODE(T _val) : val(_val), next(nullptr), prev (nullptr) {}
    };
    NODE* front;
    NODE* back;

    int count;
    list() : front(nullptr), back(nullptr), count(0) { }

	// disallow copyinst and moving
    list(const list&) = delete;
    list(list&&) = delete;
    list& operator=(const list&) = delete;
    list& operator=(list&&) = delete;

    void add_back(T _val)
    {
        NODE* newnode = new NODE(_val);
        if (count == 0)
        {
            front = back = newnode;
        }
        else
        {
            back->next = newnode;
            newnode->prev = back;
            back = newnode;
        }
        count++;
    }

    void add_front(T _val)
    {
        NODE* newnode = new NODE(_val);
        if (front == nullptr)
        {
            front = back = newnode;
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
        NODE* newhead = front->next;
        delete front;
        front = newhead;
        if (front) front->prev = nullptr;
        count--;
        if (count == 0) back = front;
    }

    void remove_back()
    {
        if (back == nullptr) return;
        NODE* newback = back->prev;
        delete back;
        back = newback;
        if (back) back->next = nullptr;
        count--;
        if (count == 0) front = back;
    }

	// provide a means to access first and last node 
    T& peek_front() { return front->val; }
    T& peek_back() { return back->val;}

	// allow access to individual elements
	T& operator[](int idx)
	{
		assert(idx >= 0 && idx < count);

		// short circuit beginning and end, O(1)
		if (idx == 0) return front->val;
		if (idx == (count - 1)) return back->val;
		
		NODE* curr = front;
		while(idx--) curr = curr->next;
		return curr->val;
	}

	// clean up memory when finished
    ~list()
    {
        if (front == nullptr) return;
        NODE* curr = front;
        while (curr)
        {
            NODE* temp = curr->next;
            delete curr;
            curr = temp;
        }
    }
};

MemoryNode* InitHashTable(int HTSIZe, array<MemoryNode*,10>& hashtable) //this simply initializes the memory hash table to null's
{
    for(auto& j : hashtable)
    {
        j = nullptr;
    }
   return hashtable[HTSIZe];
}

// code for A is 65, so have a base of 64
inline char determineToken(int procNum)//used in createFiles() only; token is letter A-J for processes 1-10
{
    return static_cast<char>(64+procNum);
}

// generates a random number, using a uniform distribution,
// when needed, between min and max
int rng(int min = 0, int max = 100)
// each process's file consists of 100 lines; each line contains the token for the process 
//followed by the line number for example the 1st line in A's file is A1A1A1A1A1. Each line is 10 characters long

{
    static default_random_engine re {};
    static uniform_int_distribution<int> dist;
    return dist(re, uniform_int_distribution<int>::param_type {min, max}); 
}

void createFiles()     //create files (for the 10 processes) to be opened and read in later in the pipeline. 
//This function is simply for the ease and convenience to make sure there is no need for the user to enter the location of files or paths. 
{
    ofstream myfile;
    string token=""; //A thru J for proc 1-10
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


string readLineNFile(string file_name, int startLine, int endLine) // this function can read line x to line y in a given file. 
	//initially, the design included forcing internal fragmentation (not allowing the process to read all 8 lines for a page) 
	//due to ejecting the process at a random time between 5 and 8. However, the feature was not implemeted. This function is used
	//in the program to simply read in 8 lines at a time.
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

void addToHashTable(array<MemoryNode*, 10>& ht, int idx, const MemoryNode& mn) //insert a new node. insert @end , remove @ front of list
{
    MemoryNode* newnode = new MemoryNode(mn.pid, mn.pageNum, mn.readInChars, mn.StartLine, mn.EndLine, mn.CPUtime);
    if (ht[idx] == nullptr)
    {
        ht[idx] = newnode;
    }
    else
    {
        MemoryNode* temp = ht[idx];
        while (temp->next != nullptr) temp = temp->next;
        newnode->prev = temp;
        temp->next = newnode;
    }
}

string printPCB(const list<PCB>& temp) //print a list containing PCB nodes. This is used for priting the five queues
{
	string out;
	auto* p = temp.front;
	if (p == nullptr) out = 'E';
	else
	{
		while(p->next != nullptr)
		{
			out += to_string(p->val.pid) + "->";
			p = p->next;
		}
		out += to_string(p->val.pid);
	}
	return out;
}

void printInstanceForTable(const list<PCB>& ready, const list<PCB>& cpu, const list<PCB>& wait,
                          const list<PCB>& disk, const list<PCB>& done, double time) // print a header for display to the user
{
    cout<<"\n";
    cout << left
         << setw(15) << time
         << setw(30) << printPCB(ready)
         << setw(10) << printPCB(cpu)
         << setw(30) << printPCB(wait)
         << setw(10) << printPCB(disk)
         << setw(10) << printPCB(done);

}

int main(int argc, const char * argv[]) {

    list<PCB> ready, cpu, wait, disk, done, mem; // the five queues

    createFiles(); //create 10 files for the 10 processes, to be read in later in the program
    string file_name, lines;

    int HTSIZe=10;
    array<MemoryNode*,10> hashtable; // create the hash table
    hashtable[HTSIZe] = InitHashTable(HTSIZe, hashtable);

    //initially create the 10 processes and assign all of them to the Ready Queue
    for (int k=1; k<11; k++)
    {
        PCB temp { k, 1, 0, hashtable[k-1] };
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
	int CPUdelay = rng(1,5);
	cout << " CPU delay: " << CPUdelay;

    //Process management Pipeline:
    while ( true )
    {
        //Time Stamp
        steady_clock::time_point t2 = steady_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
        double time_span_count=time_span.count();

        if (disk.count > 0) // move process from the disk and add to the ready
        {
            PCB temp(disk.peek_front().pid, 1, disk.peek_front().progrCounter,
                     disk.peek_front().headMem);
            ready.add_back(temp);
            disk.remove_front();

        }

        if (wait.count > 0 && disk.count == 0) // if there is a process in the wait queue and there is none in the disk, move one to the disk, perform a read in
        {
            PCB temp { wait.peek_front().pid, 4, wait.peek_front().progrCounter,
                       wait.peek_front().headMem };
            wait.remove_front();
            disk.add_back(temp);

            MemoryNode* p = hashtable[disk.peek_front().pid - 1];
            file_name.clear();
            file_name = "proc" + to_string(disk.peek_front().pid) + ".txt";

            int Startline = 0, Endline = 0, Pagenum = 0;
            if (p == nullptr)
            {
                Startline = 1;
                Pagenum = 0;
            }
            else if (p->next == nullptr)
            {
                Startline = p->EndLine + 1;
                Pagenum = p->pageNum + 1;
            }
            else
            {
                while (p->next != nullptr) p = p->next;

                Startline = p->EndLine + 1;
                Pagenum = p->pageNum + 1;
             }
             Endline = Startline + 7;


            lines.clear();
            lines = readLineNFile(file_name, Startline, Endline);
            addToHashTable(hashtable,disk.peek_front().pid - 1, MemoryNode(disk.peek_front().pid, Pagenum,
                                                                     lines, Startline, Endline,
                                                                     time_span_count));

            //In the PCB of the process, adjust the program counter
            if (Endline <= 100) disk.peek_front().progrCounter = Endline;
            else if (Endline > 101) disk.peek_front().progrCounter = 100;
        }

        if (cpu.count > 0 && cpu.peek_front().progrCounter != 100 && CPUdelay == 0) // if the CPU contains a process, and for this process, 
		//we have not read in all 100 lines, and the delay is done, we can move the process out to the wait queue
        {
            PCB temp { cpu.peek_front().pid, 3, cpu.peek_front().progrCounter,
                       cpu.peek_front().headMem };
            cpu.remove_front();
            wait.add_back(temp);
			CPUdelay = rng(1,5);
			cout << " CPU delay: " << CPUdelay;
        }

         if (cpu.count > 0 && cpu.peek_front().progrCounter >= 100 && CPUdelay == 0)// if the CPU contains a process, and for this process, 
		//we have read in all 100 lines, and the delay is done, we can move the process out to the done queue
         {
            PCB temp { cpu.peek_front().pid, 5, cpu.peek_front().progrCounter,
                       cpu.peek_front().headMem };
            cpu.remove_front();
            done.add_back(temp);
			CPUdelay = rng(1,5);
			cout << " CPUdelay: " << CPUdelay;
         }

         if (cpu.count == 0 && ready.count > 0) // cpu is empty and ready has a process, assign the proc to the CPU
         {
            PCB temp { ready.peek_front().pid, 2, ready.peek_front().progrCounter,
                       ready.peek_front().headMem };
            ready.remove_front();
            cpu.add_back(temp);
         }

         printInstanceForTable(ready, cpu, wait, disk, done, time_span_count); // print out the processes in the queues for visual inspection
         CPUdelay--; //decrement the CPU delay
		 if (done.count == 10) break;
    } //end Pipeline


    //print the contents of the memory nodes as well
    cout<<"\n\nThe contents of the memory for each process-------------------------------------------------------------------------";

	for (auto* entry : hashtable)
	{
		printMemNodes(entry);
	}


    cout<<"\n\nBYE!\n";
    return 0;
}
