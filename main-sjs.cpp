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
    {   int pid;
        int pageNum;
        string readInChars;
        int StartLine;
        int EndLine;
        double CPUtime;
        MemoryNode *next;
    };


struct PCB
{
    int pid; //process ID [1..10]
    int state; // process state [1..9]
    int progrCounter; // the number of a line which is currenly being read in from the file
    MemoryNode *headMem;
    PCB *next;
};


PCB* addPCBtoList(PCB *head, int pid, int state, int progrCounter, MemoryNode *headMem)//add a node at the end of the linked list
    {   PCB *newnode, *p=NULL;
        
        newnode = new PCB;
        
        if (head==NULL) //adding a node if it is the first node
        {
            head=newnode;
            newnode->next=NULL;
            newnode->pid=pid;
            newnode->state=state;
            newnode->progrCounter=progrCounter;
            newnode->headMem=headMem; // to be checked what it is
            p= head;
        }
        
        else if (head !=NULL) //adding a node after the 1st node
        {   p=head;
            while (p->next!=NULL) {p=p->next;}  //traverse to the end
            newnode->next=NULL;//
            p->next=newnode;//
            newnode->pid=pid;
            newnode->state=state;
            newnode->progrCounter=progrCounter;
            newnode->headMem=headMem; // to be checked what it is
            p=head;
        }
        return p;
    }



MemoryNode* addMemNodetoList(MemoryNode *head, int pid, int pageNum, string readInChars, int StartLine, int  EndLine, double CPUtime)
    {   MemoryNode *newnode, *p=NULL;
        newnode = new MemoryNode;
        
        if (head==NULL) //adding a node if it is the first node
        {
            head=newnode;
            newnode->next=NULL;
            newnode->pid=pid;
            newnode->pageNum=pageNum;
            newnode->readInChars=readInChars;
            newnode->StartLine=StartLine;
            newnode->EndLine=EndLine;
            newnode->CPUtime=CPUtime;
            p= head;
        }
        
        else if (head !=NULL) //adding a node after the 1st node
        {   p=head;
            while (p->next!=NULL) {p=p->next;}  //traverse to the end
            newnode->next=NULL;//
            p->next=newnode;//
            newnode->pid=pid;
            newnode->pageNum=pageNum;
            newnode->readInChars=readInChars;
            newnode->StartLine=StartLine;
            newnode->EndLine=EndLine;
            newnode->CPUtime=CPUtime;
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

        if (p==NULL) {out="E";return out;}
        else{
            while (p->next!=NULL)
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

        if (p==NULL) {cout<<"E"; return;}//Empty
        else
        { cout<<"\n";
            while (p->next!=NULL)
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

    if (head==NULL)
        {cout << "Nothing to delete."; return head;}
    
    else if (q->next==NULL ) // it is a 1-node list
    {
        q=NULL; delete q; head=NULL; delete head; return NULL;
    }
    
    else if (q->next!=NULL )
    {
        head=q->next;
        delete q;
    }
    return head;
}







MemoryNode* InitHashTable(int HTSIZe, array<MemoryNode*,10>& hashtable )
//MemoryNode *hashtable[HTSIZe] )//InitHashTable() initializes the hash table
{
    //for (int j=0;j<HTSIZe;j++)
    for(auto& j : hashtable)
    {
        //hashtable[j]=NULL;
		j = nullptr;
    }
    return hashtable[HTSIZe];
}





int sizeList(PCB *head) //find the size of the list
{   int sizeList=0;
    PCB *p;

    if (head==NULL) {return 0;} //list is empty, return 0
    else if (head->next==NULL) {return 1;} //size=1
    
    p=head;
    sizeList=1; //starting with the first node; size=1
    while (p->next!=NULL)
    {
        sizeList++; //traverse the list, incr. the size
        p=p->next;
    }

    return sizeList;
}


string determineToken(int procNum)//used in createFiles() only
{   string token="";
    if(procNum==1) token="A"; //process 1
    else if(procNum==2) token="B";//process 2, etc
    else if(procNum==3) token="C";
    else if(procNum==4) token="D";
    else if(procNum==5) token="E";
    else if(procNum==6) token="F";
    else if(procNum==7) token="G";
    else if(procNum==8) token="H";
    else if(procNum==9) token="I";
    else if(procNum==10) token="J";
    return token;
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
    PCB * hReady=NULL, *hCPU=NULL, *hWait=NULL, *hDisk=NULL, *hDone=NULL;
    
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
        hReady=addPCBtoList(hReady, k, 1, 0, hashtable[k-1]); //pid, state, progrCounter, ptr to MemList

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
        if (hDisk!=NULL)
    {
        hReady=addPCBtoList(hReady, hDisk->pid, 1,hDisk->progrCounter, hDisk->headMem); //add to Ready
        hDisk=delPCBFromList(hDisk); //rem from Disk
    }
     
    if(hWait!=NULL && hDisk==NULL)
        {   hDisk=addPCBtoList(hDisk, hWait->pid, 4,hWait->progrCounter, hWait->headMem); //add to Disk
            hWait=delPCBFromList(hWait); //rem from Wait

            //perform reading in of one page
            MemoryNode *p=hashtable[hDisk->pid-1];
            file_name=""; file_name+="proc"; file_name+=to_string(hDisk->pid); file_name+=".txt";
            
            int Startline=0, EndLine=0, PageNum=0;
            if (p==NULL) { Startline=1; PageNum=0;}
            else if (p->next==NULL){Startline=p-> EndLine+1; PageNum=p->pageNum+1;}
            else
            { while (p->next!=NULL) { p=p->next;}
            Startline=p-> EndLine+1;PageNum=p->pageNum+1;}
            EndLine=Startline+7;
            

            lines=""; lines=readLineNFile(file_name, Startline,EndLine);
            hashtable[hDisk->pid-1]=addMemNodetoList(hashtable[hDisk->pid-1], hDisk->pid, PageNum, lines, Startline, EndLine, time_span_count);
            
            //In the PCB of the process, adjust the program counter
            if (EndLine<=100) hDisk->progrCounter=EndLine;
            else if (EndLine>101)hDisk->progrCounter=100;
        }
       
    if (hCPU!=NULL && hCPU->progrCounter!=100) //not done
    {
        hWait=addPCBtoList(hWait, hCPU->pid, 3,hCPU->progrCounter, hCPU->headMem); //add to Wait
        hCPU=delPCBFromList(hCPU); //rem from CPU
    }
   
    if (hCPU!=NULL && hCPU->progrCounter>=100) //done
        {
            hDone=addPCBtoList(hDone, hCPU->pid, 5,hCPU->progrCounter, hCPU->headMem); //add to Done
            hCPU=delPCBFromList(hCPU); //rem from CPU
        }
     if (hCPU==NULL && hReady!=NULL)
     {
         hCPU=addPCBtoList(hCPU, hReady->pid, 2,hReady->progrCounter, hReady->headMem); //add to CPU
         hReady=delPCBFromList(hReady); //rem from Read
     }
        
        printInstanceForTable(hReady,hCPU,hWait,hDisk,hDone, time_span_count);

        if  (sizeList(hDone)==10) break; //break when all are  Done

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
