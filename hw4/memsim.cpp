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

	cout<<"cacheAddress:"<<cacheTag<<" binary:"<<binary<<endl;
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
	return cacheIndex;
}

int main()
{

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

	fstream fh( "test0.txt", ios_base::in );
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

			cout<<cacheAddress<<" ";
			cacheTag = getCacheTag(cacheAddress);
			cacheIndex = getCacheIndex(cacheAddress);
			cout<<" Cache Index: "<<cacheIndex;
			cout<<endl;
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