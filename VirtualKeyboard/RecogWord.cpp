#include "stdafx.h"
#include "RecogWord.h"
#include "widgetKeyBoard.h"
#include "distance.h"
#include "SuggestObject.h"
#include <cctype>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std; 

RecogWord::RecogWord(){
	this->threshold = 4;
	this->wordLexiconPath = "word_freq.txt";
	this->suggestNum = 5;
	this->ReadWordLexicon(this->wordLexiconPath);
//	this->spaceCtrl = false;

	for(int i = int('A'); i <= int('Z'); i++)
		this->characterVector.push_back(char(i));	
}

vector<string> RecogWord::RecogWordVector(vector<TraceObject> userTrace){

	//user's raw input
	string rawInput;
	for(vector<int>::size_type i = 0; i < userTrace.size(); i++)
			rawInput += userTrace[i].getCharcter().at(0).toAscii();

	//final output for users, vector's len is determined by "this->suggestNum"
	vector<string> suggestText;

	//--------------------------------------------------------------
    //step1: filter by first char and last char
	//--------------------------------------------------------------
	char first_char = userTrace.begin()->getCharcter().at(0).toAscii();
	char last_char = userTrace.rbegin()->getCharcter().at(0).toAscii();

	vector<string> first_last_filterVector;
	for(vector<int>:: size_type i = 0; i < this->wordVector.size(); i++){
		
		char first = this->wordVector[i][0];
		char last = this->wordVector[i][this->wordVector[i].size()-1];

		if(first == first_char && last == last_char && this->wordVector[i].size()!=1){
			first_last_filterVector.push_back(this->wordVector[i]);
		}
	}

	//if there is no result for step1, just return the raw input
	if(first_last_filterVector.size()==0)
	{
		//if(this->spaceCtrl)
		//	rawInput = " " + rawInput;
		//else
		//	this->spaceCtrl = true;

		suggestText.push_back(rawInput);
		return suggestText;
	}


	//--------------------------------------------------------------
    //step2: filter by non shown up char
	//--------------------------------------------------------------
	vector<char> nonShowUp;
	for(vector<int>::size_type i = 0; i < this->characterVector.size(); i++){
		if(std::find(rawInput.begin(), rawInput.end(), characterVector[i]) == rawInput.end())
			nonShowUp.push_back(this->characterVector[i]);
	}

	vector<string> shownUpVector;
	for(vector<int>::size_type j = 0; j < first_last_filterVector.size();j++){
		bool flag = true;
		for(vector<int>::size_type i = 0; i < nonShowUp.size(); i++){
			if(std::find(first_last_filterVector[j].begin(), first_last_filterVector[j].end(), nonShowUp[i]) != first_last_filterVector[j].end()){
				flag = false;
				break;
			}
		}

		if(flag)
			shownUpVector.push_back(first_last_filterVector[j]);
	}

	//if there is no result for step2, just return the raw input
	if(shownUpVector.size()==0)
	{
		//if(this->spaceCtrl)
		//	rawInput = " " + rawInput;
		//else
		//	this->spaceCtrl = true;

		suggestText.push_back(rawInput);
		return suggestText;
	}

	//--------------------------------------------------------------
    //step3: filter by hanging time
	//--------------------------------------------------------------
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
	//transform(refinedString.begin(), refinedString.end(),refinedString.begin(), ::tolower);

	//--------------------------------------------------------------
    //step4: LD compare and rank
	//--------------------------------------------------------------
	vector<SuggestObject> minLDVector;
	Distance dist;
	for(vector<int>::size_type i = 0; i != shownUpVector.size(); ++i){
		int distance = dist.LD(refinedString.c_str(),shownUpVector[i].c_str());
		SuggestObject sug = SuggestObject(shownUpVector[i], distance,this->wordMap[shownUpVector[i]]);
		minLDVector.push_back(sug);
	}

	//sort the minLDVector by value
	std::sort(minLDVector.begin(), minLDVector.end(), [](const SuggestObject& left, const SuggestObject& right)->bool{
		//if( left.getMinDist() == right.getMinDist()){
		//	return left.getWordFreq()< right.getWordFreq();
		//}
		//else
			return left.getMinDist() < right.getMinDist();
	});


	//--------------------------------------------------------------
    //step5: rank by freq from dict
	//--------------------------------------------------------------
	//add this information in sort above

	if(minLDVector.size()< this->suggestNum)
		this->suggestNum = minLDVector.size();

	for(int i = 0; i < this->suggestNum; i++)
		suggestText.push_back(minLDVector[i].getText());

	//if(this->spaceCtrl){
	//	for(vector<int>::size_type i = 0; i < suggestText.size(); i++)
	//		suggestText[i] = " " + suggestText[i];
	//}			
	//else
	//	this->spaceCtrl = true;

	return suggestText;
}p

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

		vector<string> tmp = this->SplitString(s);
		std::transform(tmp[0].begin(), tmp[0].end(),tmp[0].begin(), ::toupper);
		wordVector.push_back(tmp[0]);
		stringstream convert(tmp[1]);
		convert >> wordMap[tmp[0]];
	}
}