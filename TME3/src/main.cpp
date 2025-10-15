#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <ios>
#include "HashMap.h"
#include "FileUtils.h"
#include <thread>
#include <mutex>
#include <atomic>
#include "HashMapMT.h"
#include "HashMapFine.h"


using namespace std;

int main(int argc, char **argv)
{
        using namespace std::chrono;

        // Allow filename as optional first argument, default to project-root/WarAndPeace.txt
        // Optional second argument is mode (e.g. "freqstd" or "freq").
        // Optional third argument is num_threads (default 4).
        string filename = "../WarAndPeace.txt";
        string mode = "freqstd";
        int num_threads=4;
        if (argc > 1)
                filename = argv[1];
        if (argc > 2)
                mode = argv[2];
        if (argc > 3) {
                
                if (num_threads >= 0) {
                       num_threads = std::stoi(argv[3]); 
        
                }
        }

        // Check if file is readable
        ifstream check(filename, std::ios::binary);
        if (!check.is_open())
        {
                cerr << "Could not open '" << filename << "'. Please provide a readable text file as the first argument." << endl;
                cerr << "Usage: " << (argc > 0 ? argv[0] : "TME3") << " [path/to/textfile] [mode] [num threads]" << endl;
                return 2;
        }
        check.seekg(0, std::ios::end);
        std::streamoff file_size = check.tellg();
        check.close();

        cout << "Preparing to parse " << filename << " (mode=" << mode << " N=" << num_threads << "), containing " << file_size << " bytes" << endl;

        auto start = steady_clock::now();

        std::vector<std::pair<std::string, int>> pairs;

        if (mode == "freqstd") {
                ifstream input(filename, std::ios::binary);
                size_t total_words = 0;
                size_t unique_words = 0;
                std::unordered_map<std::string, int> um;
                std::string word;
                while (input >> word) {
                        word = pr::cleanWord(word);
                        if (!word.empty()) {
                                total_words++;
                                ++um[word];
                        }
                }
                unique_words = um.size();
                pairs.reserve(unique_words);
                for (const auto& p : um) pairs.emplace_back(p);
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");

        } else if (mode == "freqstdf") {
                size_t total_words = 0;
                size_t unique_words = 0;
                std::unordered_map<std::string, int> um;
                pr::processRange(filename, 0, file_size, [&](const std::string& word) {
                        total_words++;
                        um[word]++;
                });
                unique_words = um.size();
                pairs.reserve(unique_words);
                for (const auto& p : um) pairs.emplace_back(p);
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");

        } else if (mode == "freq") {

                size_t total_words = 0;
                size_t unique_words = 0;
                HashMap<std::string, int> hm;
                pr::processRange(filename, 0, file_size, [&](const std::string& word) {
                        total_words++;
                        hm.incrementFrequency(word);
                });
                pairs = hm.toKeyValuePairs();
                unique_words = pairs.size();
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");

        }else if (mode == "partition") {
                size_t total_words = 0;
                size_t unique_words = 0;
                std::unordered_map<std::string, int> um;
                std::vector<std::streamoff> partition=pr::partition(filename, file_size, num_threads);
                for(size_t i=0;i<partition.size()-1;i++){
                        pr::processRange(filename, partition[i], partition[i+1], [&](const std::string& word) {
                        total_words++;
                        um[word]++;
                        });
                }
                
                unique_words = um.size();
                pairs.reserve(unique_words);
                for (const auto& p : um) pairs.emplace_back(p);
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");

        }else if (mode == "mt_naive") {
                
                size_t total_words = 0;
                size_t unique_words = 0;
                std::unordered_map<std::string, int> um;
                std::vector<std::streamoff> partition=pr::partition(filename, file_size, num_threads);
                std::vector<std::thread> threads;
                for(size_t i=0;i<partition.size()-1;i++){
                       threads.emplace_back([&, i]() {
                        pr::processRange(filename, partition[i], partition[i + 1], [&](const std::string& word) {
                        
                        total_words++;
                        um[word]++;
                        });
                });
                }
                for(auto& t : threads){
                        t.join();
                }
                unique_words = um.size();
                pairs.reserve(unique_words);
                for (const auto& p : um) pairs.emplace_back(p);
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");
        }else if (mode == "mt_hnaive") {

                size_t total_words = 0;
                size_t unique_words = 0;
                HashMap<std::string, int> hm;
                std::vector<std::streamoff> partition=pr::partition(filename, file_size, num_threads);
                std::vector<std::thread> threads;
                for(size_t i=0;i<partition.size()-1;i++){
                       threads.emplace_back([&, i]() {
                        pr::processRange(filename, partition[i], partition[i + 1], [&](const std::string& word) {
                        
                        total_words++;
                        hm.incrementFrequency(word);
                        });
                });
                }
                for(auto& t : threads){
                        t.join();
                }
                pairs = hm.toKeyValuePairs();
                unique_words = pairs.size();
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");
        }else if (mode == "mt_atomic") {
                
                std::atomic total_words = 0;
                size_t unique_words = 0;
                std::unordered_map<std::string, int> um;
                std::vector<std::streamoff> partition=pr::partition(filename, file_size, num_threads);
                std::vector<std::thread> threads;
                for(size_t i=0;i<partition.size()-1;i++){
                       threads.emplace_back([&, i]() {
                        pr::processRange(filename, partition[i], partition[i + 1], [&](const std::string& word) {
                        
                        total_words++;
                        um[word]++;
                        });
                });
                }
                for(auto& t : threads){
                        t.join();
                }
                unique_words = um.size();
                pairs.reserve(unique_words);
                for (const auto& p : um) pairs.emplace_back(p);
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");
        }else if (mode == "mt_hatomic") {

                atomic total_words = 0;
                size_t unique_words = 0;
                HashMap<std::string, int> hm;
                std::vector<std::streamoff> partition=pr::partition(filename, file_size, num_threads);
                std::vector<std::thread> threads;
                for(size_t i=0;i<partition.size()-1;i++){
                       threads.emplace_back([&, i]() {
                        pr::processRange(filename, partition[i], partition[i + 1], [&](const std::string& word) {
                        
                        total_words++;
                        hm.incrementFrequency(word);
                        });
                });
                }
                for(auto& t : threads){
                        t.join();
                }
                pairs = hm.toKeyValuePairs();
                unique_words = pairs.size();
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");
        }else if (mode == "mt_mutex") {
                std::mutex m;
                std::atomic total_words = 0;
                size_t unique_words = 0;
                std::unordered_map<std::string, int> um;
                std::vector<std::streamoff> partition=pr::partition(filename, file_size, num_threads);
                std::vector<std::thread> threads;
                for(size_t i=0;i<partition.size()-1;i++){
                       threads.emplace_back([&, i]() {
                        pr::processRange(filename, partition[i], partition[i + 1], [&](const std::string& word) {
                        
                        total_words++;
                        std::lock_guard<std::mutex> lock(m);
                        um[word]++;
                        });
                });
                }
                for(auto& t : threads){
                        t.join();
                }
                unique_words = um.size();
                pairs.reserve(unique_words);
                for (const auto& p : um) pairs.emplace_back(p);
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");
        }else if (mode == "mt_hmutex") {

                size_t total_words = 0;
                size_t unique_words = 0;
                HashMapMT<std::string, int> hm;
                std::vector<std::streamoff> partition=pr::partition(filename, file_size, num_threads);
                std::vector<std::thread> threads;
                for(size_t i=0;i<partition.size()-1;i++){
                       threads.emplace_back([&, i]() {
                        pr::processRange(filename, partition[i], partition[i + 1], [&](const std::string& word) {
                        
                        total_words++;
                        hm.incrementFrequency(word);
                        });
                });
                }
                for(auto& t : threads){
                        t.join();
                }
                pairs = hm.toKeyValuePairs();
                unique_words = pairs.size();
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");

        }else if (mode == "mt_hhashes") {

                size_t total_words = 0;
                size_t unique_words = 0;
                std::vector<HashMap<std::string,int >> hashes(num_threads);
                std::vector<std::streamoff> partition=pr::partition(filename, file_size, num_threads);
                std::vector<std::thread> threads;
                HashMap<string,int> hm;
                for(size_t i=0;i<partition.size()-1;i++){
                       threads.emplace_back([&, i]() {
                        pr::processRange(filename, partition[i], partition[i + 1], [&](const std::string& word) {
                        
                        total_words++;
                        hashes[i].incrementFrequency(word);
                        });
                });
                }
                for(auto& t : threads){
                        t.join();
                }
                for(auto& um : hashes){
                        pairs=um.toKeyValuePairs();
                        for(auto& p : pairs){
                                hm.incrementFrequency(p.first,p.second);
                        }
                }
                pairs = hm.toKeyValuePairs();
                unique_words = pairs.size();
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");
        }else if (mode == "mt_hashes") {

                size_t total_words = 0;
                size_t unique_words = 0;
                std::vector<unordered_map<string,int> > hashes(num_threads);
                std::vector<std::streamoff> partition=pr::partition(filename, file_size, num_threads);
                std::vector<std::thread> threads;
                unordered_map<string,int> um;
                for(size_t i=0;i<partition.size()-1;i++){
                       threads.emplace_back([&, i]() {
                        pr::processRange(filename, partition[i], partition[i + 1], [&](const std::string& word) {
                        
                        total_words++;
                        hashes[i][word]++;
                        });
                });
                }
                for(auto& t : threads){
                        t.join();
                }
                for(auto& m : hashes){
                        for(auto& p : m){
                                um[p.first] += p.second;
                        }
                }
                unique_words = um.size();
                pairs.reserve(unique_words);
                for (const auto& p : um) pairs.emplace_back(p);
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");
        }
        
        
        
        
        
        else if (mode == "mt_hfine") {
                std::atomic<int> total_words = 0;
                size_t unique_words = 0;
                HashMapFine<std::string, int> hm;  

                std::vector<std::streamoff> partition = pr::partition(filename, file_size, num_threads);
                std::vector<std::thread> threads;

                for (size_t i = 0; i < partition.size() - 1; i++) {
                        threads.emplace_back([&, i]() {
                                pr::processRange(filename, partition[i], partition[i + 1], [&](const std::string& word) {
                                total_words++;
                                hm.incrementFrequency(word);
                        });
                        });
                }

                for (auto& t : threads) {
                        t.join();
                }

                pairs = hm.toKeyValuePairs();
                unique_words = pairs.size();
                pr::printResults(total_words, unique_words, pairs, mode + ".freq");
        }

        
        else {
                cerr << "Unknown mode '" << mode << "'. Supported modes: freqstd, freq, freqstdf" << endl;
                return 1;
        }

        // print a single total runtime for successful runs
        auto end = steady_clock::now();
        cout << "Total runtime (wall clock) : " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

        return 0;
}

