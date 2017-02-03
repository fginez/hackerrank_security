////////////////////////////////////////////////////////////////////////////////////////////////
// Hackerhank security challenges: Basic Cryptanalysis
//
// Este programa realiza uma cripto-analise de um buffer de entrada contra um dicionario de palavras
// possiveis. A tecnica escolhida foi a separacao das strings de acordo com o seu tamanho, e da maior
// para a menor, realiza-se uma busca por padrões e em seguida (após a exaustao do primeiro metodo) por
// casamento utilizando expressoes regulares. Os padroes sao determinados pela repeticao de caracteres
// e pela primeira ocorrencia de cada caracter dentro da string (mais detalhes no codigo).
//
// This program performs a cryptanalysis of an input buffer against a possible word dictionary. The chosen
// methodology is based in length separated strings, and then from higher length to lower, searches for a
// pattern and then (after the exaustive search by first method) for matches using regex. The patterns are
// build using repetition characters and first occurrency of each of them into the string (more detais 
// are shown in the source code).
//
// Author:
// Fernando Ginez da Silva
// fginez@gmail.com
//
// Date: 30/Jan/2017
//
//////////////////////////////////////////////////////////////////////////////////////////////////

//Set this macro to enable automatic input (no need to type input encoded words)
#define _AUTO_INPUT

#include <iterator>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <regex>

#ifdef _AUTO_INPUT
#include "testinput.h"
#endif

using namespace std;

/*
 * CreatePattern
 *
 * Pattern is consisted of two parts:
 * +------------------+------------------------+
 * |Repetition pattern|First occurrency pattern|
 * +------------------+------------------------+
 *
 * Repetition pattern: 
 * If there are any repeated character in string, a pattern is build using the following format:
 * charX_1st_rep_index,charX_2nd_rep_index,...;charY_1st_rep_index,charY_2nd_rep_index,...;
 *
 * First occurency pattern:
 * A pattern is build taking the index of first occurrency for each char contained into the string.
 *
 * Example:
 * word: sananas
 * repetition pattern: 0,6;1,3,5;2,4;
 * first occurency pattern: 0121210
 * complete pattern: 0,6;1,3,5;2,4;0121210
 */
string CreatePattern(string word) {
	string pattern = "";
	vector<char> letters;

	// Build unique letters vector
	for(unsigned int i=0; i<word.length(); i++){
		vector<char>::iterator it = find(letters.begin(), letters.end(), word[i]);
		if ( letters.end() == it ) {
			letters.push_back(word[i]);
		}
	}

	// Build first block: repetitions pattern
	for ( auto l:letters ) {
		string repetitions;
		for (unsigned int i=0; i<word.length(); i++){
			if(word[i] == l) {			
				if ( repetitions.length() ) {
					repetitions += ",";
				}
				repetitions += to_string(i);
			}
		}

		if ( repetitions.length() > 1){
			if (pattern.length()){
				pattern += ";";
			}
			pattern+=repetitions;
		}
	}

	// Build second block: first occurrency pattern
	if ( pattern.length() ) {
		pattern += ";";
	}

	for ( auto w:word ) {
		vector<char>::iterator it = find(letters.begin(), letters.end(), w);
		pattern+=to_string((int) (it - letters.begin()));
	}

	return pattern;
}

/*
 * findPattern
 * 
 * Takes as input a map and a pattern. If a pattern **full-match** is found into input map,
 * a counter is incremented and the matching pair is inserted into output map.
 */
int findPattern(map<string, string> input, string pattern, map<string, string> &output){
	int itensFound = 0;
	for ( auto item:input ){
		if ( item.second == pattern ) {
			itensFound++;
			output.insert(item);
		}
	}
	return itensFound;
}

/*
 * updateTranslationMap
 *
 * Takes a pair of strings (encoded and its related clear text) and adds each char to
 * translation map.
 */
void updateTranslationMap(string encoded, string clear, map<char, char> &mapTranslation){

	if ( encoded.length() != clear.length() ) {
		return;
	}

	for(unsigned int i=0; i<encoded.length(); i++) {
		char c;
		try {
			c = mapTranslation.at(encoded[i]);
			if ( c != clear[i] ) {
				cout << "char disagree\n";
			}
		} catch (...) {
			mapTranslation.insert(pair<char, char>(encoded[i], clear[i]));
		}
	}
}

/*
 * buildTestWord
 *
 * Takes a partial clear text string (preclear) and our translation map, and build
 * an output string with all already known chars into mapTranslation. Unknown chars
 * are inserted as a dot '.' (this is a regex "coringa").
 */
int buildTestWord(string preclear, map<char, char> &mapTranslation, string &output) {
	int unknown = 0;
	for ( auto c:preclear ) {
		char d;
		try {
			d = mapTranslation.at(c);
			output += d;
		} catch(...) {
			output += ".";
			unknown++;
		}
	}

	return unknown;
}

/*
 * translateWord
 *
 * Takes an input encoded string and translates it using mapTranslation.
 */
string translateWord(string encoded, map<char, char> mapTranslation) {
	string output="";
	for(char c:encoded){
		try {
			output+=mapTranslation.at(c);
		} catch(...) {
			output+=".";
		}
	}
	return output;
}

int main(int argc, char* argv[]) {	
	const string strFilename = "dictionary.lst";
	const int totalLetters = 26;
	string strInput;
	map<string, string> mapInput[20];
	map<string, string> mapDictionary[20];
	map<string, string> mapDictionary_lc[20];
	map<char, char> mapTranslation;

	try {
		bool one = false;
		ifstream fs(strFilename);
		while(!fs.eof()){
			string line;
			getline(fs, line);
			int len = line.length();			
			mapDictionary[len].insert(pair<string, string>(line, CreatePattern(line)));
			transform(line.begin(), line.end(), line.begin(), ::tolower);
			mapDictionary_lc[len].insert(pair<string, string>(line, CreatePattern(line)));
		}
		fs.close();
	}
	catch (...) {
		return -1;
	}

#ifndef _AUTO_INPUT
	getline(cin, strInput);
#else
	int i=0;
	do {

		strInput += string((const char*)g_aucTestInput[i]);
		strInput += " ";
		i++;
	} while (i<100);
#endif

	std::istringstream iss(strInput);
	do {
		string token;
		iss >> token;
		int len = token.length();
		mapInput[len].insert(pair<string, string>(token, CreatePattern(token)));
	} while(iss);

	for ( int searchlen=19; searchlen > 0; searchlen-- ) {
		if ( mapInput[searchlen].size() &&
			 mapDictionary_lc[searchlen].size() ) {

			for (auto input:mapInput[searchlen]) {
				int nItensFound;
				map<string, string> itensFound;
				string searchInput   = input.first;
				string searchPattern = input.second;
				
				nItensFound=findPattern(mapDictionary_lc[searchlen], searchPattern, itensFound);
				if ( 1 == nItensFound ) {
					updateTranslationMap(searchInput, itensFound.begin()->first, mapTranslation);
				}
				else if ( 1 < nItensFound ) {
					string testWord;
					int unknownLetters = buildTestWord(searchInput, mapTranslation, testWord);
					if ( unknownLetters ) {
						for ( auto f:itensFound ) {
							if ( regex_match( f.first.c_str(), regex(testWord.c_str()) )) {
								updateTranslationMap(searchInput, f.first, mapTranslation);
								break;
							}
						}
					}
				}
			}
		}
	}

	istringstream iss2(strInput);
	do {
		string token;
		iss2 >> token;
		cout << translateWord(token, mapTranslation);
		if (iss2)
			cout << " ";
	} while(iss2);

	return 0;
}
