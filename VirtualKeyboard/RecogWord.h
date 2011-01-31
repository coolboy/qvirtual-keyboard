#pragma once

#include<vector>
#include<string>

#include "TraceObj.h"
#include "widgetKeyBoard.h"

class RecogWord{

public:

	RecogWord();
	
	std::vector<std::string> RecogWordVector(std::vector<TraceObject> userTrace);

	std::vector<std::string> SplitString(const std::string& str);

	void ReadWordLexicon(std::string filePath);
	

private:

	std::vector<std::string> suggestWordArray;
	std::string wordLexiconPath;
	std::vector<std::string> wordVector;
	std::map<std::string,int> wordMap;
	int threshold;
	int suggestNum;
	//bool spaceCtrl;
	std::vector<char> characterVector;
};