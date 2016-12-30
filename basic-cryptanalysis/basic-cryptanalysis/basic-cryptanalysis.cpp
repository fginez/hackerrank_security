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
#include <iomanip>      // std::setfill, std::setw

#define __LOG_ENABLED
#define __IGNORE_DECODE_DISAGREE

#ifdef __LOG_ENABLED
#define log(a) std::cout << a
#else
#define log(a)
#endif

std::string vecToString(std::vector<int> input) {
	std::stringstream s;
	for ( auto i:input ) {
		s << i << " ";
	}
	return s.str();
}

/*
 *
 *
 */
class Word {
private:
	int m_iLen;
	int m_iRepeatedCharsCount;
	std::string m_strWord;
	std::map<char, std::vector<int>*>* m_mapRepeated;

	// Implementar o operator :=
	// Para testar se duas palavras contém o mesmo conteudo na string

	// Talvez seja melhor criar uma estrutura do seguinte tipo:
	// Um mapa:
	// char, vector<int>(repeated_char, {repeated positions})
	// Pois assim é possivel diferenciar se mais de um caractere se repetir
	// na mesma palavra.
	void buildVectors() {
		//for( char c='A'; c<='z'; c++ ) {
		for( char c='a'; c<='z'; c++ ) {
			int counter = 0;
			std::string::iterator it = m_strWord.begin();
			do {
				if (*it == c) {
					counter +=1;
				}
				it++;
			} while ( it != m_strWord.end() );

			if ( counter > 1 ) {
				std::vector<int>* pvec = NULL;
				pvec = new std::vector<int>();
				it = m_strWord.begin();
				do {
					if (*it == c) {
						int pos = it - m_strWord.begin();
						pvec->push_back(pos);
					}
					it++;
				} while ( it != m_strWord.end() );

				m_mapRepeated->insert(std::make_pair(c, pvec));
			}
		}

		m_iRepeatedCharsCount = m_mapRepeated->size();
	}

public:
	Word(std::string word){
		m_mapRepeated = new std::map<char, std::vector<int>*>();
		m_strWord = word;
		m_iLen = m_strWord.length();
		buildVectors();

		//log("Creating word=" << word << " len=" << m_iLen << " repeatedCount=" << m_iRepeatedCharsCount << "\n");
	}

	Word(const char* word){
		m_mapRepeated = new std::map<char, std::vector<int>*>();
		m_strWord = std::string(word);
		m_iLen = m_strWord.length();
		buildVectors();
	}

	int getLen(){
		return m_iLen;
	}

	int getRepeatedCharsCount(){
		return m_iRepeatedCharsCount;
	}

	std::map<char, std::vector<int>*>* getRepeatedCharsMap(){
		return m_mapRepeated;
	}

	std::string getWord(){
		return m_strWord;
	}
};

 class Decoder{
private:
	std::map<char, char> m_DecodingMap;

public:
	Decoder(){
	}

	~Decoder(){
	}

	int getRemainingCount(){
		int total = 'z' - 'a' + 1;
		for (char c='a'; c<='z'; c++ ) {
			if ( isDecoded(c) )
				total -= 1;
		}

		return total;
	}

	char decode(char input){
		try {
			return m_DecodingMap.at(input);
		} catch(...) { 

		}
		return ' ';
	}

	void decode(std::string* input){
		std::string::iterator it = input->begin();
		do {
			*it = decode(*it);
		} while ( it != input->end() );
	}

	void addDecodingElement(char encoded, char clear){
		try {
			if (!isDecoded(encoded) ) {
				m_DecodingMap.insert(std::pair<char, char>(encoded, clear));
				log ("\t  Added dec. elem. " << encoded << "->" << clear << "\n");
			} else {
				if ( clear != decode(encoded) ) {
					log ("\t*** FATAL: New element disagree ***\n");
					log ("\tOld pair: " << encoded << "->" << decode(encoded) << "\n");
					log ("\tNew pair: " << encoded << "->" << clear << "\n");

#ifndef __IGNORE_DECODE_DISAGREE
					while(1);
#endif
				}
			}
		} catch (...) {
			// TODO
		}
	}

	void addDecodingElement(std::string encoded, std::string clear) {
		if ( encoded.size() == clear.size() ) {

			std::string::iterator it_enc = encoded.begin();
			std::string::iterator it_clr = clear.begin();

			do {
				addDecodingElement(*it_enc, *it_clr);
				it_enc++; it_clr++;
			} while (it_enc != encoded.end());
		}
	}

	bool isDecoded(char input){
		try {
			char ch = m_DecodingMap.at(input);
			return true;
		} catch (...){

		}
		return false;
	}
};

 /*class EncodedWord : Word {
private:		
	int m_iEncodedRemaining;
	int m_iDecodedCounter;
	std::vector<int> m_iVecDecodedPos;
	Decoder* m_poDec;

public:
	EncodedWord(std::string word, Decoder *dec): Word(word){
		m_poDec = dec;
	}

	~EncodedWord(){

	}

	int getEncodedRemaining() {
		return m_iEncodedRemaining;
	}

	int getDecodedCounter() {
		return m_iDecodedCounter;
	}

	void refresh() {
		std::vector<char> chars(std::begin(getWord()), std::end(getWord()));
		int pos = 0;
		m_iVecDecodedPos.clear();
		for ( char c:chars ) {
			if ( m_poDec->isDecoded(c) ) {
				m_iVecDecodedPos.push_back(pos);
			}
			pos++;
		}

		m_iDecodedCounter = m_iVecDecodedPos.size();
		m_iEncodedRemaining = getWord().size() - m_iDecodedCounter;
	}

};*/

class WordBank {
private:
	std::list<Word*> *m_WordList;
	int m_iMaxRepeated;
	int m_iMaxLen;

public:
	WordBank(){
		m_WordList = new std::list<Word*>();
	}

	WordBank(std::list<Word*> pInput) {
		m_WordList = new std::list<Word*>();
		std::copy(pInput.begin(), pInput.end(), std::back_inserter(*m_WordList));
	}

	~WordBank(){
		m_WordList->clear();
		delete m_WordList;
	}

	int getMaxRepeated() {
		return getMaxRepeated(m_WordList);
	}

	static int getMaxRepeated(std::list<Word*>* input) {
		int iMaxRepeated = 0;
		for (auto w:*input) {
			if (w->getRepeatedCharsCount() > iMaxRepeated) {
				iMaxRepeated = w->getRepeatedCharsCount();
			}
		}
		return iMaxRepeated;
	}

	int getMaxLen() {
		return getMaxLen(m_WordList);		
	}

	static int getMaxLen(std::list<Word*>* input) {		
		int iMaxLen = 0;
		for (auto w:*input) {
			if (w->getLen() > iMaxLen) {
				iMaxLen = w->getLen();
			}
		}
		return iMaxLen;
	}

	void insertWord(std::string word){
		Word* w = new Word(word);
		m_WordList->push_back(w);

		if (w->getRepeatedCharsCount() > m_iMaxRepeated) {
			m_iMaxRepeated = w->getRepeatedCharsCount();
		}

		if (w->getLen() > m_iMaxLen) {
			m_iMaxLen = w->getLen();
		}
	}

	void insertWord(Word* word){
		m_WordList->push_back(word);

		if (word->getRepeatedCharsCount() > m_iMaxRepeated) {
			m_iMaxRepeated = word->getRepeatedCharsCount();
		}

		if (word->getLen() > m_iMaxLen) {
			m_iMaxLen = word->getLen();
		}
	}

	bool contains(std::string word) {
		for ( auto i=m_WordList->begin(); i != m_WordList->end(); i++ ){
			if ( (*i)->getWord() == word ) {
				return true;
			}
		}
		return false;
	}

	static void getListByLen(std::list<Word*>* origin, std::list<Word*>* output, int len) {
		for ( auto i=origin->begin(); i != origin->end(); i++ ){
			if ( (*i)->getLen() == len ) {
				output->push_back(*i);
			}
		}
	}

	void getListByLen(std::list<Word*>* pOutput, int len){
		getListByLen(m_WordList, pOutput, len);
	}

	static void getListByRepeatedCount(std::list<Word*>* origin, std::list<Word*>* output, int iCount) {
		for ( auto i=origin->begin(); i != origin->end(); i++ ) {
			if ( (*i)->getRepeatedCharsCount() == iCount ) {
				output->push_back(*i);
			}
		}
	}

	void getListByRepeatedCount(std::list<Word*>* pOutput, int iCount) {
		getListByRepeatedCount(m_WordList, pOutput, iCount);
	}

	static void getListByRepeatedPositions(std::list<Word*>* origin, std::list<Word*>* output, std::vector<int>* piVecPositions) {
		for ( auto i=origin->begin(); i != origin->end(); i++ ) {
			std::map<char, std::vector<int>*>* map = (*i)->getRepeatedCharsMap();
			if (map->size()){
				auto imap = map->begin();
				do {
					if ( *imap->second == *piVecPositions ){
						output->push_back(*i);
						break;
					}
					imap++;
				} while ( imap != map->end() );
			}
		}
	}

	void getListByRepeatedPositions(std::list<Word*>* pOutput, std::vector<int>* piVecPositions) {
		getListByRepeatedPositions(m_WordList, pOutput, piVecPositions);
	}
};

class Dictionary: public WordBank{
private:

public:
	Dictionary(std::string filename){
		try {
			bool one = false;
			std::ifstream fs(filename);
			while(!fs.eof()){
				std::string line;
				getline(fs, line);
				std::transform(line.begin(), line.end(), line.begin(), ::tolower);
				insertWord(line);
			}
			fs.close();
		}
		catch (...) {

		}
	}

	~Dictionary(){

	}
};

class DecoderBuilder{
private:
	Dictionary *m_dic;
	Decoder*    m_decoder;
	std::vector<Word*> m_vecEncodedWords;

public:
	DecoderBuilder(Dictionary *dic, std::string input_text){
		m_dic = dic;
		m_decoder = new Decoder();

		std::list<std::string> words;

		// What the fuck???
		std::istringstream is(input_text);

		std::copy(std::istream_iterator<std::string>(is), 
			      std::istream_iterator<std::string>(),
				  std::back_inserter(words));

		for(auto x:words) {
			m_vecEncodedWords.push_back(new Word(x));
		}

		//printHistogram(m_EncodedMap);

		this->process();
	}

	Decoder* getDecoder(){
		return m_decoder;
	}

	void process(){
		WordBank wb;
		std::set<int> wordLen;

		// Para cada palavra codificada...
		for ( auto word:m_vecEncodedWords ) {
			wb.insertWord((Word*) word);
		}

		int r = wb.getMaxRepeated();
		int l = wb.getMaxLen();
		log("WB  R=" << r <<" L=" << l << "\n");

		int rd = m_dic->getMaxRepeated();
		int ld = m_dic->getMaxLen();
		log("DIC R=" << rd <<" L=" << ld << "\n");

		for (int len=l; len>0; len--) {
			std::list<Word*> listByLen;
			wb.getListByLen(&listByLen, len);

			for(int rep=WordBank::getMaxRepeated(&listByLen); rep>0; rep--) {
				std::list<Word*> listByLenByRep;
				WordBank::getListByRepeatedCount(&listByLen, &listByLenByRep, rep);

				int iRemaining = m_decoder->getRemainingCount();
				log("DECODER: " << iRemaining << " to go!\n");
				if ( iRemaining == 0 ) {
					log("DECODER: good job!");
					return;
				}

				int iFound = listByLenByRep.size();
				if (iFound) {
					log("* Found " << iFound << " with len=" << len << " rep=" << rep << "\n");
					// Find in dictionary a matching pair
					std::list<Word*> dicByLen;
					m_dic->getListByLen(&dicByLen, len);
					if ( dicByLen.size() ) {
						std::list<Word*> dicByLenByRep;
						WordBank::getListByRepeatedCount(&dicByLen, &dicByLenByRep, rep);

						int iDicFound = dicByLenByRep.size();
						if (iDicFound) {
							log("\t=> Dictionary contains " << iDicFound << " similar entries\n");

							if ( 1 == iDicFound && 1 == iFound ) {
								log("\t** Perfect match\n");
								m_decoder->addDecodingElement( listByLenByRep.front()->getWord(),
															   dicByLenByRep.front()->getWord() );
							} else {
								for ( auto word:listByLenByRep ){
									log("\t\tSearching for a matching pair for " << word->getWord() << "\n");

									//Get encoded word repeated chars map
									std::map<char, std::vector<int>*>* map = word->getRepeatedCharsMap();
									std::map<char, std::vector<int>*>::iterator it = map->begin();

									//Find in dictionary for a possible element
									std::list<Word*> dicByLenByRepByPos;
									WordBank::getListByRepeatedPositions(&dicByLenByRep, &dicByLenByRepByPos, it->second);

									if( 1 == dicByLenByRepByPos.size()) {
										log("\t\t** Match  found\n");
										m_decoder->addDecodingElement( word->getWord(), dicByLenByRepByPos.front()->getWord() );
									} else {
										bool bStopSearch = false;
										log("\t\t   Found " << dicByLenByRepByPos.size() << " possibilities" << "\n");

										// for each possible word in you found dictionary...
										for (auto dicword:dicByLenByRepByPos) {
											bool bMatch = true;
											
											// Set start conditions
											std::map<char, std::vector<int>*>* dicmap = dicword->getRepeatedCharsMap();
											std::map<char, std::vector<int>*>::iterator it_dic = dicmap->begin();

											it = map->begin();
											// Swap encoded word positions
											do{											
												it_dic = dicmap->begin();
												log("\t\t   '" << it->first << "' repeats at positions " << vecToString(*it->second) << "\n");
												bool bFound = false;
												do {
													log("\t\t\t" << "Current dictionary word has reps @ " << vecToString(*it_dic->second) << "\n");
													if ( *it->second == *it_dic->second ) {
														bFound = true;
														log("\t\t\t  Partial match found!\n");
														break;
													}
													it_dic++;
												} while ( it_dic != dicmap->end() );

												if ( !bFound ) {
													log("\t\t\t   Does not match!\n");
													break;
												}

												it++;

												if (bFound && it == map->end()){
													log("\t\t** Match  found\n");
													m_decoder->addDecodingElement( word->getWord(), dicword->getWord() );
													bStopSearch = true;
												}
											} while (it != map->end() );

											if (bStopSearch)
												break;
										}
									}
								}
							}
						}
					}					
				}
			}
		}
	}
};

int main(int argc, char* argv[])
{
	log( "Reading input\n");
	std::string input;
	std::getline(std::cin, input);

	log( "Reading dictionary\n");
	Dictionary* dic = new Dictionary("dictionary.lst");

	log( "Building decoder\n");
	DecoderBuilder* db = new DecoderBuilder(dic, input);
	Decoder* dec = db->getDecoder();

	log( "Decoding\n");
	for(auto c:input) {
		std::cout << dec->decode(c);
	}
	return 0;
}
