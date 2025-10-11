#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>
#include <string>
#include <algorithm>
#include <vector>
#include "HashMap.h"
// helper to clean a token (keep original comments near the logic)
static std::string cleanWord(const std::string& raw) {
	// une regex qui reconnait les caractères anormaux (négation des lettres)
	static const std::regex re( R"([^a-zA-Z])");
	// élimine la ponctuation et les caractères spéciaux
	std::string w = std::regex_replace(raw, re, "");
	// passe en lowercase
	std::transform(w.begin(), w.end(), w.begin(), ::tolower);
	return w;
}

int main(int argc, char** argv) {
	using namespace std;
	using namespace std::chrono;

	// Allow filename as optional first argument, default to project-root/WarAndPeace.txt
	// Optional second argument is mode (e.g. "count" or "unique").
	string filename = "../WarAndPeace.txt";
	string mode = "count";
	if (argc > 1) filename = argv[1];
	if (argc > 2) mode = argv[2];

	ifstream input(filename);
	if (!input.is_open()) {
		cerr << "Could not open '" << filename << "'. Please provide a readable text file as the first argument." << endl;
		cerr << "Usage: " << (argc>0?argv[0]:"TME2") << " [path/to/textfile]" << endl;
		return 2;
	}
	cout << "Parsing " << filename << " (mode=" << mode << ")" << endl;
	
	auto start = steady_clock::now();
	
	// prochain mot lu
	string word;

	if (mode == "count") {
		size_t nombre_lu = 0;
	
		// default counting mode: count total words
		while (input >> word) {
			// élimine la ponctuation et les caractères spéciaux
			word = cleanWord(word);

			// word est maintenant "tout propre"
			if (nombre_lu % 100 == 0)
				// on affiche un mot "propre" sur 100
				cout << nombre_lu << ": "<< word << endl;
			nombre_lu++;
		}
	input.close();
	cout << "Finished parsing." << endl;
	cout << "Found a total of " << nombre_lu << " words." << endl;

	} else if (mode == "unique") {
		// skeleton for unique mode
		// before the loop: declare a vector "seen"
		std::vector<std::string> seen;
		
		

		while (input >> word) {
			// élimine la ponctuation et les caractères spéciaux
			word = cleanWord(word);
			bool vu=false;

			for(size_t i=0;i<seen.size();i++){
			
				if(word==seen[i]){
					vu=true;
					break;
				}

			}
			
			if(!vu){
				seen.push_back(word);
		
			}
			// add to seen if it is new
			
		}
	input.close();
	// TODO
	 cout << "Found " << seen.size() << " unique words." << endl;

	} 
	else if(mode =="freq"){
		std::vector<std::pair<std::string,int>> freq;
		
		

		while (input >> word) {
			// élimine la ponctuation et les caractères spéciaux
			word = cleanWord(word);
			bool v=false;
			for(size_t i=0;i<freq.size();i++){
				if(word == freq[i].first){
					freq[i].second++;
					v=true;
					break;
				}
			}

			if(!v){
				freq.push_back(std::make_pair(word,1));
			}
			
		}
	input.close();
	std::sort(freq.begin(), freq.end(),
          [](const std::pair<std::string,int>& a,
             const std::pair<std::string,int>& b) {
                return a.second > b.second; // tri décroissant
          });

	// TODO
	for(size_t i=0;i<freq.size();i++){
		if(freq[i].first == "war" || freq[i].first == "peace" || freq[i].first == "toto"){
			cout <<  freq[i].first << " : "<<freq[i].second << endl;
		}
	}
	cout<<"les dix mots les plus fréquents :"<<std::endl;
	for(size_t i=0;i<10;i++){
		
		cout <<  freq[i].first << " : "<<freq[i].second << endl;
	}
	 
	}
	else if(mode == "freqhash"){
		HashMap<std::string,int> freq(10000);
		while(input >> word){
			word = cleanWord(word);
			if(freq.get(word) == nullptr){
				freq.put(word,1);
			}else{
				freq.put(word,*freq.get(word)+1);
			}
		}
		std::vector<std::pair<std::string,int>> v=freq.toKeyValuePairs();
		cout<<"les dix mots les plus fréquents :"<<std::endl;
		for(size_t i=0;i<10;i++){
		
			cout <<  v[i].first << " : "<<v[i].second << endl;
		}
	}
	else if(mode == "freqstd"){
		std::unordered_map<std::string, int> freq;
		while(input >> word){
			word=cleanWord(word);
			if(freq.find(word) == freq.end()){
				freq[word]=1;
			}else{
				freq[word] += 1;
			}
		}

		 std::vector<std::pair<std::string,int>> vec;
    for(const auto& entry : freq){
        vec.push_back(entry);
    }

    // Tri décroissant par fréquence
    std::sort(vec.begin(), vec.end(),
        [](const auto& a, const auto& b){
            return a.second > b.second;
        });

    // Affichage
    for(size_t i = 0; i < 10; ++i){
        std::cout << vec[i].first << " : " << vec[i].second << std::endl;
    }
	}
	else {
		// unknown mode: print usage and exit
		cerr << "Unknown mode '" << mode << "'. Supported modes: count, unique" << endl;
		input.close();
		return 1;
	}

	// print a single total runtime for successful runs
	auto end = steady_clock::now();
	cout << "Total runtime (wall clock) : " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

	return 0;
}


