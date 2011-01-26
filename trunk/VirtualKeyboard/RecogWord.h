#pragma once

#include<vector>
#include<string>
//#include<fstream>
//#include<iostream>
//#include<sstream>
//#include<map>

#include "TraceObj.h"
#include "widgetKeyBoard.h"

class RecogWord{

public:

	RecogWord();
	
	std::vector<std::string> RecogWordVector(std::vector<TraceObject> userTrace);

	std::vector<std::string> SplitString(const std::string& str);

	void ReadWordLexicon(std::string filePath);


	//void ReadNeighborDict(string filePath);

	//vector<char> SplitChar(const string& str);

private:

	std::vector<std::string> suggestWordArray;
	std::string wordLexiconPath;
	//string neighborDictPath;
	//map<char,vector<char>> neighborDict;
	std::vector<std::string> wordVector;
	int threshold;
	int suggestNum;


};