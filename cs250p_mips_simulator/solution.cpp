#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <regex>

#define OPERATOR_TYPE 0
#define LABEL_TYPE 1
#define JUMP_TYPE 2
#define BRANCH_TYPE 3
#define END_TYPE 4

using namespace std;
class solution {

private:

bool DEBUG;
int clck;
vector<string> _vect_lines;
vector<int> _vect_types;

vector<int> _vars;
vector<int>* t_vars;
vector<vector<string>> _inst_operator;

// use for noting label
unordered_map<string,int> _label_dict;


public :

solution(ifstream &file_in,int clck_in = 10 ,bool DEBUG_in = false);
void dbg(const string &msg);

vector<int>* alu(){};

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


// solution construct
solution::solution(ifstream &file_in,int clck_in ,bool DEBUG_in){
	// get init value
	string initVals;
	getline(file_in, initVals);
	regex Comma(",");
	vector<string> result{
		sregex_token_iterator( initVals.begin(), initVals.end(), Comma, -1), {}
	};
	for(int i = 0; i<result.size();i++)
	{
		int val = stoi(result[i]);
		_vars.push_back(val);

	}
	regex Label("label(.*)");
	regex Jump("b\\s(.*)");
	regex Branch("(beq|bnq)(.*)");
	regex whiteSpace("\\s+");
	// put instruction into string
	for( string line; getline( file_in, line); )
	{
		vector<string> inst_operator;
		_vect_lines.push_back(line);
		// parse inst type
		vector<string> result{
			sregex_token_iterator( line.begin(), line.end(), whiteSpace, -1), {}
		};

		string parse_register;
		if(line == "end")
		{
			_vect_types.push_back(END_TYPE);
			inst_operator.push_back(line);
			_inst_operator.push_back(inst_operator);
			continue;
		}
		else if( regex_match(line, Label) )
		{
			_vect_types.push_back(LABEL_TYPE);

			int idx = _vect_types.size() - 1;
			_label_dict[result[0]] = idx;

			inst_operator.push_back(result[1]);
			parse_register = result[2];
		}
		else if( regex_match(line, Jump) )
		{
			_vect_types.push_back(JUMP_TYPE);
			inst_operator.push_back(result[1]);
			_inst_operator.push_back(inst_operator);
			continue;
		}
		else if( regex_match(line, Branch) )
		{
			_vect_types.push_back(BRANCH_TYPE);
			inst_operator.push_back(result[0]);
			parse_register = result[1];
		}
		else
		{
			_vect_types.push_back(OPERATOR_TYPE);
			inst_operator.push_back(result[0]);
			parse_register = result[1];
		}

		// parse registers info
		vector<string> comma_parser{
			sregex_token_iterator( parse_register.begin(), parse_register.end(), Comma, -1), {}
		};
		for(int i=0;i<comma_parser.size();i++)
		{
			inst_operator.push_back(comma_parser[i]);
		}
		_inst_operator.push_back(inst_operator);
	}
	// check correctness
	for(int i =0;i<_vect_lines.size();i++)
	{
		cout<<_vect_lines[i]<<"|"<<_vect_types[i]<<endl;
		for(int j=0;j<_inst_operator[i].size();j++)
		{
			cout<<_inst_operator[i][j]<<" ";
		}
		cout<<"----"<<endl;
	}
}
