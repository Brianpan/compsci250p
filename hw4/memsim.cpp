// header
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <regex>
#include <unordered_map>
using namespace std;

string getCacheTag(string cacheAddress)
{
	string cacheTag = "";
	cacheTag += cacheAddress[0];
	cacheTag += cacheAddress[1];
	
	// hex to int
	char *c = (char*) malloc(1);
	memcpy( c, &cacheAddress[2], 1);
	int number = (int)strtol( (const char*)c, NULL, 16);
	
	string binary;
	int mask = 1;

	// int to binary string
	for(int i=0; i<4;i++)
	{
		if(mask&number)
			binary = "1" + binary;
		else
			binary = "0" + binary;
		mask<<=1;

	}

	// fetch left 3 bits
	for(int i= 0; i<3;i++)
	{
		cacheTag += binary[i];
	}

	free(c);
	return cacheTag;
}

string getCacheIndex(string cacheAddress)
{
	string cacheIndex = "";
	// hex to int
	char *c = (char*) malloc(1);
	memcpy( c, &cacheAddress[2], 1);
	int number = (int)strtol( (const char*)c, NULL, 16);
	
	string binary;
	int mask = 1;

	// int to binary string
	for(int i=0; i<4;i++)
	{
		if(mask&number)
			binary = "1" + binary;
		else
			binary = "0" + binary;
		mask<<=1;
	}

	cacheIndex += binary[3];

	for(int i=3;i<7;i++)
	{
		cacheIndex += cacheAddress[i];
	}

	free(c);
	return cacheIndex;
}

int main(int argc, char* argv[])
{

	if( argc  != 2 )
	{
		cout<<"Param is wrong"<<endl;
	}

	unordered_map<string,vector<string>> cacheLine;
	string accessType;
	string cacheAddress;
	string cacheTag;
	string cacheIndex;

	int totalTrace = 0;
	int readCount = 0;
	int writeCount = 0;
	int cacheHit = 	0;
	int cacheMiss = 0;

	fstream fh( argv[1], ios_base::in );
	if( fh )
	{
		for( string line; getline( fh, line); )
		{
			regex whiteSpace("\\s+");
			vector<string> result{
				sregex_token_iterator( line.begin(), line.end(), whiteSpace, -1), {}
			};
			
			cacheAddress = result[0];
			accessType = result[1];

			cacheTag = getCacheTag(cacheAddress);
			cacheIndex = getCacheIndex(cacheAddress);
			
			// check cache is hit or not 
			auto isFoundIter = cacheLine.find(cacheIndex);
			if( isFoundIter == cacheLine.end() )
			{
				cacheLine[cacheIndex].push_back(cacheTag);
				cacheMiss += 1;
			}
			else
			{
				if(cacheLine[cacheIndex].size() == 1)
				{
					if( cacheLine[cacheIndex][0] == cacheTag )
					{
						cacheHit += 1;
					}
					else
					{
						cacheLine[cacheIndex].push_back(cacheTag);
						string tmp = cacheLine[cacheIndex][0];
						cacheLine[cacheIndex][0] = cacheLine[cacheIndex][1];
						cacheLine[cacheIndex][1] = tmp;
						cacheMiss += 1;
					}
					
				}
				else
				{
					// cache line is full
					if( cacheLine[cacheIndex][0] == cacheTag )
					{
						cacheHit += 1;
					}
					else if( cacheLine[cacheIndex][1] == cacheTag )
					{
						// should swap
						string tmp = cacheLine[cacheIndex][0];
						cacheLine[cacheIndex][0] = cacheLine[cacheIndex][1];
						cacheLine[cacheIndex][1] = tmp;
						cacheHit += 1;
					}
					//cache miss
					else
					{
						// remove 1 & swap
						cacheLine[cacheIndex][1] = cacheLine[cacheIndex][0];
						cacheLine[cacheIndex][0] = cacheTag;

						cacheMiss += 1;
					}
				}
			}

			// update counter
			if( accessType == "R" )
			{
				readCount += 1;
			}
			if( accessType == "W" )
			{
				writeCount += 1;
			}
			totalTrace += 1;
		}	
	}

	cout<<"Total traces: "<<totalTrace<<endl;
	cout<<"Reads: "<<readCount<<endl;
	cout<<"Writes: "<<writeCount<<endl;
	cout<<"Cache hits: "<<cacheHit<<endl;
	cout<<"Cache misses: "<<cacheMiss<<endl;

	return 0; 
}