//  COSC514 Project2 Bowie State
//  Created by Sophie Johnson-Shapoval on 4/18-21/2021
//  Program description is in the accompanying design document

#include <string>
#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono; //clock
#include <fstream> //ofstream/read in files
#include <deque>
#include <array>
#include <iomanip>


struct MemoryNode //node in a linked list
    {   
		int pid;
        int pageNum;
        string readInChars;
        int StartLine;
        int EndLine;
        double CPUtime;				
        MemoryNode *next;
        
        MemoryNode(int _pid, int _pageNum, const string& _readInChars,
                   int _StartLine, int _EndLine, double _CPUtime, 
				   MemoryNode* _next = nullptr) :
				pid(_pid), pageNum(_pageNum), readInChars(_readInChars),
                StartLine(_StartLine), EndLine(_EndLine), CPUtime(_CPUtime),
                next(_next) {}
        MemoryNode() = default;
    };


struct PCB
{
    int pid; //process ID [1..10]
    int state; // process state [1..9]
    int progrCounter; // the number of a line which is currenly being read in from the file
    MemoryNode *headMem;
    PCB *next;

	PCB() = default;
	PCB(int _pid, int _state, int _progrCounter, 
        MemoryNode* _headMem = nullptr, PCB* _next = nullptr) :
		pid(_pid), state(_state), progrCounter(_progrCounter),
		headMem(_headMem), next(_next) {}
    
    friend ostream& operator<<(ostream& out, const PCB* other)
    {
        return out;
    }
};


PCB* addPCBtoList(PCB *head, int pid, int state, int progrCounter, MemoryNode *headMem)//add a node at the end of the linked list
    {   
		PCB *newnode, *p=nullptr;
        
        newnode = new PCB(pid, state, progrCounter, headMem);
        
        if (head == nullptr) //adding a node if it is the first node
        {
            head=newnode;
            p= head;
        }
        
        else if (head != nullptr) //adding a node after the 1st node
        {   p=head;
            while (p->next != nullptr) p = p->next;  //traverse to the end
            p->next=newnode;//
            p=head;
        }
        return p;
    }



MemoryNode* addMemNodetoList(MemoryNode *head, int pid, int pageNum, string readInChars, int StartLine, int  EndLine, double CPUtime)
    {   MemoryNode *newnode, *p = nullptr;
	 	newnode = new MemoryNode(pid,pageNum,readInChars, StartLine, EndLine, CPUtime);
        
        if (head == nullptr) //adding a node if it is the first node
        {
			head = newnode;
            p= head;
        }
        
        else if (head != nullptr) //adding a node after the 1st node
        {   p=head;
            while (p->next != nullptr) p=p->next;  //traverse to the end
            p->next=newnode;//
            p=head;
        }
        return p;
    }


string printPCB(PCB* head)//print a list consisting of PCBs
    {
        string out="";
        PCB  *p;
        p=head;
        int index=0;

        if (p==nullptr) {out="E";return out;}
        else{
            while (p->next!=nullptr)
            {   out+=to_string(p->pid); out+="->";
                p=p->next; index++;
                }
                out+=to_string(p->pid);
        }
        return out;
    }


void printMemNodes(MemoryNode* head)//print the list
    {
        MemoryNode  *p=head;
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




PCB* delPCBFromList(PCB *head)// deletea node from the front
{
    PCB *q=head;

    if (head==nullptr)
        {cout << "Nothing to delete."; return head;}
    
    else if (q->next==nullptr ) // it is a 1-node list
    {
        q=nullptr; delete q; head=nullptr; delete head; return nullptr;
    }
    
    else if (q->next!=nullptr )
    {
        head=q->next;
        delete q;
    }
    return head;
}







MemoryNode* InitHashTable(int HTSIZe, array<MemoryNode*,10>& hashtable )
{
    for(auto& j : hashtable)
    {
		j = nullptr;
    }
    return hashtable[HTSIZe];
}




int sizeList(PCB *head) //find the size of the list
{   int sizeList=0;
    PCB *p;

    if (head==nullptr) {return 0;} //list is empty, return 0
    else if (head->next==nullptr) {return 1;} //size=1
    
    p=head;
    sizeList=1; //starting with the first node; size=1
    while (p->next!=nullptr)
    {
        sizeList++; //traverse the list, incr. the size
        p=p->next;
    }

    return sizeList;
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


void printInstanceForTable(PCB * hReady,PCB *hCPU,PCB *hWait,PCB *hdisk,PCB *hDone, double time)
{
    cout<<"\n";
    cout<<left
    << setw(15) <<time
    << setw(30) <<printPCB(hReady)
    << setw(10) <<printPCB(hCPU)
    << setw(30) << printPCB(hWait)
    << setw(10) <<printPCB(hdisk)
    << setw(10) <<printPCB(hDone) ;
    
}

int main(int argc, const char * argv[]) {
    PCB * hReady=nullptr, *hCPU=nullptr, *hWait=nullptr, *hDisk=nullptr, *hDone=nullptr;
   
	deque<PCB> readyQ, cpuQ, waitQ, diskQ, doneQ, memQ;
 
    createFiles(); //create 10 files for the 10 processes, to be read in later in the program
    string file_name="";
    string lines="";
    
    int HTSIZe=10;
    //MemoryNode *hashtable[10];
	array<MemoryNode*,10> hashtable;	
    hashtable[HTSIZe]=InitHashTable( HTSIZe, hashtable );

    //initially create the 10 processes and assign all of them to the Ready Queue
    for (int k=1; k<11; k++)
    {
        //hReady=addPCBtoList(hReady, k, 1, 0, hashtable[k-1]); //pid, state, progrCounter, ptr to MemList
		PCB temp { k, 0, 0, hashtable[k-1] };
		readyQ.push_back(temp);
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
    printInstanceForTable(hReady,hCPU,hWait,hDisk,hDone, 0);

    //Process management Pipeline:
    while ( true )
    {
        //Time Stamp
        steady_clock::time_point t2 = steady_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
        double time_span_count=time_span.count();
/*        if (hDisk!=nullptr)
    {
        hReady=addPCBtoList(hReady, hDisk->pid, 1,hDisk->progrCounter, hDisk->headMem); //add to Ready
        hDisk=delPCBFromList(hDisk); //rem from Disk
    }*/
		if (diskQ.size() != 0)
		{
			if (diskQ.size() == 0) cout << "Nothing to delete.";
			else
			{
				PCB temp { diskQ.front().pid, 1, diskQ.front().progrCounter, 
						   diskQ.front().headMem };
				readyQ.push_back(temp);
				diskQ.pop_front();
			}	
			
		}
     
    /*if(hWait!=nullptr && hDisk==nullptr)
        {   hDisk=addPCBtoList(hDisk, hWait->pid, 4,hWait->progrCounter, hWait->headMem); //add to Disk
            hWait=delPCBFromList(hWait); //rem from Wait

            //perform reading in of one page
            MemoryNode *p=hashtable[hDisk->pid-1];
            file_name=""; file_name+="proc"; file_name+=to_string(hDisk->pid); file_name+=".txt";
            
            int Startline=0, EndLine=0, PageNum=0;
            if (p==nullptr) { Startline=1; PageNum=0;}
            else if (p->next==nullptr){Startline=p-> EndLine+1; PageNum=p->pageNum+1;}
            else
            { while (p->next!=nullptr) { p=p->next;}
            Startline=p-> EndLine+1;PageNum=p->pageNum+1;}
            EndLine=Startline+7;
            

            lines=""; lines=readLineNFile(file_name, Startline,EndLine);
            hashtable[hDisk->pid-1]=addMemNodetoList(hashtable[hDisk->pid-1], hDisk->pid, PageNum, lines, Startline, EndLine, time_span_count);
            
            //In the PCB of the process, adjust the program counter
            if (EndLine<=100) hDisk->progrCounter=EndLine;
            else if (EndLine>101)hDisk->progrCounter=100;
        }*/
	if (waitQ.size() != 0 && diskQ.size() == 0)
	{
		PCB temp { waitQ.front().pid, 4, waitQ.front().progrCounter,
				   waitQ.front().headMem };
		waitQ.pop_front();
		diskQ.push_back(temp);

		MemoryNode* p = hashtable[diskQ.front().pid - 1];
		file_name.clear();
		file_name = "proc" + to_string(diskQ.front().pid) + ".txt";

		int Startline = 0, Endline = 0, Pagenum = 0;

		// REALLY NEED TO THINK ABOUT HOW THE MEMORY LIST IS IMPLEMENTED
		if (memQ.size() == 0) { Startline = 1; Pagenum = 0; }
		else if (1) 
		{ 
			//Startline = p->Endline+1; 
			//Pagenum = p->pageNum + 1;
		}
		else
		{
			while (p->next != nullptr) p = p->next;
			//Startline = p->				
		}
	}
       
    /*if (hCPU!=nullptr && hCPU->progrCounter!=100) //not done
    {
        hWait=addPCBtoList(hWait, hCPU->pid, 3,hCPU->progrCounter, hCPU->headMem); //add to Wait
        hCPU=delPCBFromList(hCPU); //rem from CPU
    }*/
	if (cpuQ.size() && cpuQ.front().progrCounter != 100)
	{
		PCB temp { cpuQ.front().pid, 3, cpuQ.front().progrCounter, 
				   cpuQ.front().headMem };
		cpuQ.pop_front();
		waitQ.push_back(temp);
	}
   
    /*if (hCPU!=nullptr && hCPU->progrCounter>=100) //done
        {
            hDone=addPCBtoList(hDone, hCPU->pid, 5,hCPU->progrCounter, hCPU->headMem); //add to Done
            hCPU=delPCBFromList(hCPU); //rem from CPU
        }*/

	 if (cpuQ.size() && cpuQ.front().progrCounter >= 100)
	 {
		PCB temp { cpuQ.front().pid, 5, cpuQ.front().progrCounter,
				   cpuQ.front().headMem };
		cpuQ.pop_front();
		doneQ.push_back(temp);
	 }

     /*if (hCPU==nullptr && hReady!=nullptr)
     {
         hCPU=addPCBtoList(hCPU, hReady->pid, 2,hReady->progrCounter, hReady->headMem); //add to CPU
         hReady=delPCBFromList(hReady); //rem from Read
     }*/
     if (cpuQ.size() == 0 && readyQ.size())
	 {
		PCB temp { readyQ.front().pid, 2, readyQ.front().progrCounter,
				   readyQ.front().headMem };
		readyQ.pop_front();
		cpuQ.push_back(temp);
	 } 
        //printInstanceForTable(hReady,hCPU,hWait,hDisk,hDone, time_span_count);

        //if  (sizeList(hDone)==10) break; //break when all are  Done
		if (doneQ.size() == 10) break;
    } //end Pipeline

    
    //print the contents of the memory nodes as well
    cout<<"\n\nThe contents of the memory for each process-------------------------------------------------------------------------";
    for (int j=0;j<10;j++)
    { MemoryNode *p=hashtable[j];
        cout<<"\n";printMemNodes(p);
    }
    
    
    cout<<"\n\nBYE!\n";
    
    return 0;
}
