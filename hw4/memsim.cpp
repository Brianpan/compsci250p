// header
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <regex>
#include <unordered_map>
#include <bitset>
#include <sstream>
#include <math.h>

using namespace std;

int main(int argc, char* argv[])
{

	if( argc  != 2 )
	{
		cout<<"Param is wrong"<<endl;
	}

	unsigned indexSize = pow(2, 18);
	
	unsigned long cacheLine[indexSize][2];
	bool validBits[indexSize][2];
	memset(validBits, 0, sizeof(bool)*indexSize*2 );

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
	regex whiteSpace("\\s+");

	if( fh )
	{
		for( string line; getline( fh, line); )
		{
			vector<string> result{
				sregex_token_iterator( line.begin(), line.end(), whiteSpace, -1), {}
			};
			
			cacheAddress = result[0];
			accessType = result[1];
			
			stringstream ss;
			ss<<hex<<cacheAddress;
			unsigned n;
			ss>>n;
			bitset<32>b(n);

			auto cacheIndexBit = bitset<17>( (b>>4).to_ulong() );
			auto cacheTagBit = bitset<11>( (b>>21).to_ulong() );

			unsigned long cacheIndex = cacheIndexBit.to_ulong();
			unsigned long cacheTag = cacheTagBit.to_ulong();

			// check cache is hit or not 
			if( validBits[cacheIndex][0] == 0 )
			{
				cacheLine[cacheIndex][0] = cacheTag;
				cacheMiss += 1;
				validBits[cacheIndex][0] = 1;
			}
			else
			{
				if( cacheLine[cacheIndex][0] == cacheTag )
				{
					cacheHit += 1;
				}
				else
				{
					if(validBits[cacheIndex][1] == 0)
					{
						cacheLine[cacheIndex][1] = cacheTag;
						cacheMiss += 1;
						validBits[cacheIndex][1] = 1;
					}
					else
					{
						auto tmp = cacheLine[cacheIndex][0];
						if(cacheLine[cacheIndex][1] == cacheTag)
						{
							cacheHit += 1;
							cacheLine[cacheIndex][0] = cacheLine[cacheIndex][1];
							
						}
						else
						{
							cacheLine[cacheIndex][0] = cacheTag;
							cacheMiss += 1;
						}
						cacheLine[cacheIndex][1] = tmp;
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