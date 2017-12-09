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
vector<int> _tmpVars;

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
	// set clck time
	clck = clck_in;

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
		_tmpVars.push_back(val);

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
	bool isStall = false;
	bool isEnd = false;
	int cycle = 0;

	int piplineIdx[3];
	piplineIdx[0] = -1;
	piplineIdx[1] = -1;
	piplineIdx[2] = -1;

	while(true)
	{
		if(!mips_clock()) {
			continue;
		}
		// transition state
		// Branch & JUMP should not need write back
		if( piplineIdx[1] == -1 || (_vect_types[piplineIdx[1]] != JUMP_TYPE && _vect_types[piplineIdx[1]] != BRANCH_TYPE) )
			piplineIdx[2] = piplineIdx[1];
		else
			piplineIdx[2] = -1;

		piplineIdx[1] = piplineIdx[0];

		// end should not in pipeline
		if(!isStall)
		{
			if( idx < (_vect_lines.size()-1) )
			{
				piplineIdx[0] = idx;
				// should stall if JUMP or BRANCH
				if( _vect_types[piplineIdx[0]] == JUMP_TYPE || _vect_types[piplineIdx[0]] == BRANCH_TYPE )
				{
					isStall = true;
				}
				else
				{
					idx += 1;
				}
			}
			else
			{
				if(piplineIdx[1] == -1 && piplineIdx[2] == -1)
				{
					break;
				}
				piplineIdx[0] = -1;
			}
		}
		// update isStall flag
		else
		{
			piplineIdx[0] = -1;
			isStall = false;
		}

		//
		cout<<endl;
		cout<<"clockcycle: "<<cycle<<endl;
		// fetch step
		if(piplineIdx[0] >= 0)
			cout<<"Fetch : "<<_vect_lines[piplineIdx[0]]<<endl;
		// execution step
		if(piplineIdx[1] >= 0)
		{
			cout<<"Execute : "<<_vect_lines[piplineIdx[1]]<<endl;
			switch(_vect_types[piplineIdx[1]])
			{
				case OPERATOR_TYPE:
					processOp(_inst_operator[piplineIdx[1]]);
					break;
				case LABEL_TYPE:
					processOp(_inst_operator[piplineIdx[1]]);
					break;
				case JUMP_TYPE:
					jumpIdx = _label_dict[_inst_operator[piplineIdx[1]][0]];
					idx = jumpIdx;
					break;
				case BRANCH_TYPE:
					jumpIdx = shouldBranch(_inst_operator[piplineIdx[1]]);
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
					isEnd = true;
					break;
			}

		}

		// write back step
		if(piplineIdx[2] >= 0)
		{
			cout<<"Write_back : "<<_vect_lines[piplineIdx[2]]<<endl;
			// write back to _vars
			// print status
			for(int i=0;i<7;i++)
			{
				cout<<_vars[i]<<",";
				_vars[i] = _tmpVars[i];
			}
			cout<<_vars[7];
			_vars[7] = _tmpVars[7];

			cout<<endl;

			// instruction before end finish
			if( piplineIdx[2] == (_vect_lines.size()-2) )
			{
				break;
			}

			piplineIdx[2] = -1;
		}

		cycle += 1;
	}

	cout<<"end"<<endl;

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
		_tmpVars[destIdx]= _tmpVars[v0Idx] + _tmpVars[v1Idx];
	}
	else if(op == "addi")
	{
		int constant = stoi(registers[3]);
		_tmpVars[destIdx] = _tmpVars[v0Idx] + constant;
	}
	else if(op == "sub")
	{
		_tmpVars[destIdx]= _tmpVars[v0Idx] - _tmpVars[v1Idx];
	}
	else if(op == "mul")
	{
		_tmpVars[destIdx]= _tmpVars[v0Idx] * _tmpVars[v1Idx];
	}
	// div
	else
	{
		_tmpVars[destIdx]= _tmpVars[v0Idx] / _tmpVars[v1Idx];
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
		shouldJump = (_tmpVars[v0Idx] == _tmpVars[v1Idx]);
	}
	else
	{
		shouldJump = (_tmpVars[v0Idx] != _tmpVars[v1Idx]);
	}

	if(shouldJump)
	{
		jumpIdx =_label_dict[registers[3]];
	}

	return jumpIdx;
}
