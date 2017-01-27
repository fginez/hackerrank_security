#include <iterator>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <list>
#include <vector>
#include <set>
#include <iomanip>      // setfill, setw

#include "testinput.h"

using namespace std;

#define _AUTO_INPUT
#define __LOG_ENABLED
//#define __IGNORE_DECODE_DISAGREE

#ifdef __LOG_ENABLED
#define log(a) cout << a
#else
#define log(a)
#endif

#define fatal(a) cout << a; \
				 flush(cout); \
				 while(1)

class CException {
public:
	CException(string what) {
		m_what = what;
	}

	~CException(){}

	string what(){
		return m_what;
	}

private:
	string m_what;
};

/* MapCharToPosition
	Maps a input char code to integer position in array, such as:
	'a' -> 0
	'b' -> 1
	...
*/
int MapCharToPosition(char c) {
	if ( c >= 0x61 && c<= 0x7A ){
		return (c - 0x61);
	}
}

int main(int argc, char* argv[]) {	
	const string strFilename = "dictionary.lst";
	string strInput;
	vector<string> vecDictionary;
	vector<string> vecDictionary_lc;

	log( "Reading dictionary\n");
	try {
		bool one = false;
		std::ifstream fs(strFilename);
		while(!fs.eof()){
			std::string line;
			getline(fs, line);
			vecDictionary.push_back(line);
			std::transform(line.begin(), line.end(), line.begin(), ::tolower);
			vecDictionary_lc.push_back(line);
		}
		fs.close();
	}
	catch (...) {
		log("Exception reading dictionary file\n");
		return -1;
	}

#ifndef _AUTO_INPUT
	log( "Reading input\n");
	getline(cin, strInput);
#else
	strInput = string((const char*)g_aucTestInput);
#endif

	return 0;
}
