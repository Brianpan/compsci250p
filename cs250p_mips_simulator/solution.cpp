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

unordered_map<string, int> _register_map;

void processOp(const vector<string> &registers);
int shouldBranch(const vector<string> &registers);

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

	// init register map
	_register_map["$0"] = 0;
	_register_map["$1"] = 1;
	_register_map["$2"] = 2;
	_register_map["$3"] = 3;
	_register_map["$4"] = 4;
	_register_map["$5"] = 5;
	_register_map["$6"] = 6;
	_register_map["$7"] = 7;

	// check correctness
	// for(int i =0;i<_vect_lines.size();i++)
	// {
	// 	cout<<_vect_lines[i]<<"|"<<_vect_types[i]<<endl;
	// 	for(int j=0;j<_inst_operator[i].size();j++)
	// 	{
	// 		cout<<_inst_operator[i][j]<<" ";
	// 	}
	// 	cout<<"----"<<endl;
	// }
}

vector<int>* solution::alu(){
	int idx = 0;
	int jumpIdx;
	while(idx<_vect_lines.size())
	{
		switch(_vect_types[idx])
		{
			case OPERATOR_TYPE:
				processOp(_inst_operator[idx]);
				idx += 1;
				break;
			case LABEL_TYPE:
				processOp(_inst_operator[idx]);
				idx += 1;
				break;
			case JUMP_TYPE:
				jumpIdx = _label_dict[_inst_operator[idx][0]];
				idx = jumpIdx;
				break;
			case BRANCH_TYPE:
				jumpIdx = shouldBranch(_inst_operator[idx]);
				if(jumpIdx >= 0)
				{
					idx = jumpIdx;
				}
				else
				{
					idx += 1;
				}
				break;
			case END_TYPE:
				idx += 1;
				break;
		}
	}

	return &_vars;
}

void solution::processOp(const vector<string> &registers){
	string op = registers[0];
	int destIdx = _register_map[registers[1]];
	int v0Idx = _register_map[registers[2]];
	int v1Idx;
	if(op != "addi")
	{
		v1Idx = _register_map[registers[3]];
	}

	if(op == "add")
	{
		_vars[destIdx]= _vars[v0Idx] + _vars[v1Idx];
	}
	else if(op == "addi")
	{
		int constant = stoi(registers[3]);
		_vars[destIdx] = _vars[v0Idx] + constant;
	}
	else if(op == "sub")
	{
		_vars[destIdx]= _vars[v0Idx] - _vars[v1Idx];
	}
	else if(op == "mul")
	{
		_vars[destIdx]= _vars[v0Idx] * _vars[v1Idx];
	}
	// div
	else
	{
		_vars[destIdx]= _vars[v0Idx] / _vars[v1Idx];
	}

}

int solution::shouldBranch(const vector<string> &registers){
	int jumpIdx = -1;
	string op = registers[0];
	int v0Idx = _register_map[registers[1]];
	int v1Idx = _register_map[registers[2]];

	bool shouldJump = false;
	if(op == "beq")
	{
		shouldJump = (_vars[v0Idx] == _vars[v1Idx]);
	}
	else
	{
		shouldJump = (_vars[v0Idx] != _vars[v1Idx]);
	}

	if(shouldJump)
	{
		jumpIdx =_label_dict[registers[3]];
	}

	return jumpIdx;
}