// header
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

int linearSearch( const vector<string> &wordList, const string& word )
{
	for( int i=0; i<wordList.size(); i++ )
	{
		int isEqual = word.compare(wordList[i]);
		if( isEqual == 0 ) return i;
	}
	// not found
	cout<<"Word not found !"<<endl;
	return -1;
}

int binarySearch( const vector<string> &wordList, const string& word, int begin, int end )
{	
	// no found
	if( end == begin ){
		cout<<"Word: "<<word<<" not found!"<<endl;
		return -1;
	}
	
	// end case
	if( (end-begin) == 1 )
	{
		if( word.compare( wordList[begin]) == 0 )
		{
			return begin;
		}
		else
		{
			cout<<"Word: "<<word<<" not found!"<<endl;
			return -1;
		}
	}
	//
	int mid = (begin+end)/2;
	if( word.compare(wordList[mid])==0 )
	{
		return mid;
	}
	// should find right part
	else if( word.compare(wordList[mid]) > 0 )
	{
		return binarySearch(wordList, word, mid+1, end);
	}
	else{
		return binarySearch(wordList, word, begin, mid);
	}
}

int main()
{
	fstream fh( "dict.txt", ios_base::in );
	if( fh )
	{
		string word = "zucchini";
		vector<string> wordList;
		for( string line; getline( fh, line); )
		{
			wordList.push_back(line);
		}
		
		int idx;
		//idx = linearSearch(wordList, word);
		//cout<<"Linear Search Word: "<<word<<" Index: "<<idx<<endl;
		
		idx = binarySearch( wordList, word, 0, wordList.size() );
		cout<<"Binary Search Word: "<<word<<" Index: "<<idx<<endl;
	} 
	else
	{
		cout<<"Something wrong with the file handelr"<<endl;
		return -1;
	}
	return 0;
}
