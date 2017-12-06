#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>

using namespace std;
class solution {

private:

bool DEBUG;
int clck;
vector<string> vect_lines;
vector<int>* t_vars;		

public :

solution(ifstream &file_in,int clck_in = 10 ,bool DEBUG_in = false);
void dbg(const string &msg);
vector<int>* alu();
int mips_clock();

};



int solution::mips_clock() {
chrono::milliseconds timespan(clck); 

this_thread::sleep_for(timespan);
static int cycle = 0;
if (cycle == 0 )
	cycle = 1;
else 
	cycle = 0;
return cycle;
}

