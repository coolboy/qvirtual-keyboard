#pragma once

#include<vector>
#include<string>
#include<fstream>
#include<iostream>
#include <sstream>
#include<map>

#include "TraceObj.h"
#include "widgetKeyBoard.h"

using namespace std; 

class RecogWord{

public:

	RecogWord();
	
	vector<string> RecogWordVector(vector<TraceObject> userTrace);

	vector<string> SplitString(const string& str);

	void ReadWordLexicon(string filePath);


	//void ReadNeighborDict(string filePath);

	//vector<char> SplitChar(const string& str);

private:

	vector<string> suggestWordArray;
	string wordLexiconPath;
	//string neighborDictPath;
	//map<char,vector<char>> neighborDict;
	vector<string> wordVector;
	int threshold;
	int suggestNum;


};