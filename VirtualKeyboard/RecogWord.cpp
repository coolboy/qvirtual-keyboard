#include "stdafx.h"

#include "RecogWord.h"
#include "widgetKeyBoard.h"
#include "distance.h"
#include "SuggestObject.h"

//#include <map>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std; 

RecogWord::RecogWord(){
	this->threshold = 4;
	this->wordLexiconPath = "word_freq.txt";
	this->suggestNum = 5;
	//this->neighborDictPath = "NeighborDict.txt";
	this->ReadWordLexicon(this->wordLexiconPath);
	//this->ReadNeighborDict(this->neighborDictPath);
}

vector<string> RecogWord::RecogWordVector(vector<TraceObject> userTrace){

	//step1: filter by first char and last char
	char first_char = tolower(userTrace.begin()->getCharcter().at(0).toAscii());
	char last_char = tolower(userTrace.rbegin()->getCharcter().at(0).toAscii());

	vector<string> first_last_filterVector;

	for(vector<int>:: size_type i = 0; i < this->wordVector.size(); i++){
		
		char first = this->wordVector[i][0];
		char last = this->wordVector[i][this->wordVector[i].size()-1];

		if(first == first_char && last == last_char ){
			first_last_filterVector.push_back(this->wordVector[i]);
		}
	}

	//step2: filter by hanging time
	vector<TraceObject> refinedUserInput;
	int traceSize = userTrace.size();
	//consider the first char
	string refinedString = userTrace[0].getCharcter().toStdString();

	//element between first and last char
	for(vector<int>::size_type i = 1; i != traceSize-1;++i){
		if(userTrace[i].getWeight() < this->threshold)
			continue;

		//store the weight
		refinedUserInput.push_back(userTrace[i]);
		//store the char
		refinedString += userTrace[i].getCharcter().toStdString();
	}

	//consider the last char
	refinedString += userTrace[userTrace.size()-1].getCharcter().toStdString();
	//change refinedString into lowercase
	transform(refinedString.begin(), refinedString.end(),refinedString.begin(), ::tolower);

	//string initialString = userTrace[0].getCharcter().toStdString() + userTrace[userTrace.size()-1].getCharcter().toStdString();
	
	
	//step3: find result
	vector<SuggestObject> minLDVector;
	Distance dist;
	for(vector<int>::size_type i = 0; i != first_last_filterVector.size(); ++i){
		int distance = dist.LD(refinedString.c_str(),first_last_filterVector[i].c_str());
		SuggestObject sug = SuggestObject(first_last_filterVector[i], distance);
		minLDVector.push_back(sug);
	}

	//sort the minLDVector by value
	std::sort(minLDVector.begin(), minLDVector.end(), [](const SuggestObject& left, const SuggestObject& right)->bool{
		return left.getMinDist() < right.getMinDist();
	});
	

	vector<string> suggestText;
	for(int i = 0; i < this->suggestNum; i++)
		suggestText.push_back(minLDVector[i].getText());

	return suggestText;

}

vector<string> RecogWord::SplitString(const string& str){
	vector<string> tmpVector;
	istringstream iss(str);
	
	string sub;
	while (iss>>sub){
		tmpVector.push_back(sub);
	}

	return tmpVector;
}

void RecogWord::ReadWordLexicon(string filePath){
	ifstream fin(filePath);  
	string s; 
	while( getline(fin,s) ){

		wordVector.push_back(this->SplitString(s)[0]);
	}
}