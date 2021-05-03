//  COSC514 Project2 Bowie State
//  Group: Sophie Johnson-Shapoval, Basil Huffman, Fekadu Urga
//  Program description is in the accompanying design document (slides) and a report
#include <thread>
#include <list>
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

// Memory nodes for each hash table list
struct MemoryNode 
{
	int pid; // process number
    int pageNum;  //page and frame are 1-1 mapping
    string readInChars; // this string contains the read-in data for the frame
    int StartLine; //ref the start lines in the proc's file
    int EndLine; // ref the end lines in the proc's file
    double CPUtime; // steady_clock() is used for timestamps inside the program
    MemoryNode(int _pid, int _pageNum, const string& _readInChars,
               int _StartLine, int _EndLine, double _CPUtime) :
               pid(_pid), pageNum(_pageNum), readInChars(_readInChars),
               StartLine(_StartLine), EndLine(_EndLine), CPUtime(_CPUtime) {}
};

void printMemNodes(const list<MemoryNode>& _list)
{
	int idx = 0;
	cout << endl;
	if (_list.size() == 0)
	{
		cout << "E";
		return;
	}
	cout << endl;
	for (const auto& i : _list)
	{
		cout << "pid : " << i.pid << " PageNum " << i.pageNum << " contains " 
             << i.readInChars << " in lines " << i.StartLine << "-";
		if (++idx < _list.size())
		{
			cout << i.EndLine << " read at " << i.CPUtime << endl;
		}
		else
		{
			if (i.EndLine > 101) cout << "100";
			else cout << i.EndLine;
			cout << " read at " << i.CPUtime;
		}
	}
}

// PCB represents a process, 10 will exist within the system and are
// moved amongst the various state queues
struct PCB 
{
    int pid; //process number/ID [1..10]
    int state; // process states. 1: in Ready; 2: in CPU; 3: in Wait; 4: in Disk and 5: in Done.
    int progrCounter; // the number of a line which is currenly being read in from the file
	list<MemoryNode>& headMem; // reference to the associated hash table entry
    PCB(int _pid, int _state, int _progrCounter,
		list<MemoryNode>& _headMem) :
        pid(_pid), state(_state), progrCounter(_progrCounter), headMem(_headMem) {}
};

// generates a random number, using a uniform distribution,
// when needed, between min and max
int rng(int min = 0, int max = 100)
{
    static random_device rd;
    static mt19937 gen { rd() };
    uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

void createFiles()     //create files (for the 10 processes) to be opened and read in later in the pipeline.
{
    ofstream myfile;
    string token=""; //A thru J for proc 1-10
    string file_name="";
    for (int k=1; k<11; k++)
    {   file_name.clear();
        file_name+="proc" + to_string(k) + ".txt";
        token = static_cast<char>(64+k); // 10 proceses, cast to chars representing A-J

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
    string readInOneLine, lines; //concatenated result for lines between start and end
    int numCount=0;

    ifstream fileRand (file_name);
    if (fileRand.is_open())  // if succes, this is true
    {
    	while ( !fileRand.eof()) //read in one line, readInOneLine, at a time
     	{
     		getline (fileRand,readInOneLine); 
			numCount++;
         	if (numCount>=startLine && numCount<=endLine) //if numCount is outside the bounds, lines stays ""
         		lines+=readInOneLine;
     	}
    }
    fileRand.close();

    return lines;
}

//print a list containing PCB nodes. This is used for priting the five queues
string printPCB(const list<PCB>& temp)
{
    string out;
    if (temp.size() == 0) return string("E");
    else
    {
        for (const auto& i : temp)
        {
            out += to_string(i.pid) + "->";
        }
    }
    return out.substr(0,out.size() - 2);
}

void printInstanceForTable(const list<PCB>& ready, const list<PCB>& cpu, const list<PCB>& wait,
                           const list<PCB>& disk, const list<PCB>& done, double time) // print a header for display to the user
{
    cout << endl << left << setw(15) << time << setw(30) << printPCB(ready) 
         << setw(10) << printPCB(cpu) << setw(30) << printPCB(wait) 
         << setw(10) << printPCB(disk) << setw(10) << printPCB(done);
}

int main(int argc, const char * argv[]) {
    list<PCB> ready, cpu, wait, disk, done;
    ofstream checkrand;
    checkrand.open("checkrand.out");
    createFiles(); //create 10 files for the 10 processes, to be read in later in the program
    string file_name, lines;

    int HTSIZe=10;
    array<list<MemoryNode>,10> hashtable; // create the hash table

    //initially create the 10 processes and assign all of them to the Ready Queue
    for (int k=1; k<11; k++)
    {
        PCB temp { k, 1, 0, hashtable[k-1] };
        ready.push_back(temp);
    }

    //print table header
    cout << "\n------------------------------------------------------------------------------------------------------------------"
         << left << setw(15) << "\nClockStamp" << setw(30) << "Ready" << setw(10) << "CPU"
         << setw(30) << "Wait" << setw(10) << "Disk" << setw(10) << "Done" ;

    steady_clock::time_point t1 = steady_clock::now(); //grab out initial time stamp
    printInstanceForTable(ready,cpu,wait,disk,done, 0);
    int CPUdelay = rng(1,5);
    checkrand << CPUdelay << ' ';
    cout << " CPU delay: " << CPUdelay;

    //Process management Pipeline:
    while (done.size() != 10)
    {
        //Time Stamp
        steady_clock::time_point t2 = steady_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
        double time_span_count = time_span.count();

        if (disk.size() > 0)
        {
            PCB temp { disk.front().pid, 1, disk.front().progrCounter, disk.front().headMem };
            ready.push_back(temp);
            disk.pop_front();
        }

        if (wait.size() > 0 && disk.size() == 0)
        {
            PCB temp { wait.front().pid, 4, wait.front().progrCounter, wait.front().headMem };
            wait.pop_front();
            disk.push_back(temp);

            file_name.clear();
            file_name = "proc" + to_string(disk.front().pid) + ".txt";

            int Startline = 0, Endline = 0, Pagenum = 0;
			if (hashtable[disk.front().pid - 1].size() == 0)
            {
                Startline = 1;
                Pagenum = 0;
            }
            else
            {
				Startline = hashtable[disk.front().pid - 1].back().EndLine + 1;
				Pagenum = hashtable[disk.front().pid - 1].back().pageNum + 1;
            }
            Endline = Startline + 7;

            lines.clear();
            lines = readLineNFile(file_name, Startline, Endline);
			hashtable[disk.front().pid - 1].push_back(MemoryNode(disk.front().pid, Pagenum, lines, 
																 Startline, Endline, time_span_count));

            //In the PCB of the process, adjust the program counter
            if (Endline <= 100) disk.front().progrCounter = Endline;
            else if (Endline > 101) disk.front().progrCounter = 100;
        }

        if (cpu.size() > 0 && cpu.front().progrCounter != 100 && CPUdelay == 0) // if the CPU contains a process, and for this process,
        //we have not read in all 100 lines, and the delay is done, we can move the process out to the wait queue
        {
            PCB temp { cpu.front().pid, 3, cpu.front().progrCounter, cpu.front().headMem };
            cpu.pop_front();
            wait.push_back(temp);
            CPUdelay = rng(1,5);
            cout << " CPU delay: " << CPUdelay;
            checkrand << CPUdelay << ' ';
        }

        if (cpu.size() > 0 && cpu.front().progrCounter >= 100 && CPUdelay == 0)// if the CPU contains a process, and for this process,
        //we have read in all 100 lines, and the delay is done, we can move the process out to the done queue
        {
            PCB temp { cpu.front().pid, 5, cpu.front().progrCounter, cpu.front().headMem };
            cpu.pop_front();
            done.push_back(temp);
            CPUdelay = rng(1,5);
            cout << " CPU delay: " << CPUdelay;
            checkrand << CPUdelay << ' ';
        }

        if (cpu.size() == 0 && ready.size() > 0) // cpu is empty and ready has a process, assign the proc to the CPU
        {
            PCB temp { ready.front().pid, 2, ready.front().progrCounter, ready.front().headMem };
            ready.pop_front();
            cpu.push_back(temp);
        }

        printInstanceForTable(ready, cpu, wait, disk, done, time_span_count); // print out the processes in the queues for visual inspection
        CPUdelay--; //decrement the CPU delay
        //this_thread::sleep_for(50ms);
    } //end Pipeline

    //print the contents of the memory nodes as well
    cout<<"\n\nThe contents of the memory for each process-------------------------------------------------------------------------";
    for (const auto& entry : hashtable) printMemNodes(entry);
    cout<<"\n\nBYE!\n";

    return 0;
}
