#include<string>
using namespace std;

class SuggestObject{
public:
	SuggestObject(string text, int minDist);

	SuggestObject(string text, int minDist,int wordFreq);

	int getMinDist() const { return minDist; }

	int getWordFreq() const {return wordFreq;}

	string getText() const {return text;}

	//bool WordSortPredicate(const SuggestObject& d1, const SuggestObject& d2);

private:
	string text;
	int minDist;
	int wordFreq;
};