#include "Spimi.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include<algorithm>
#include <regex>
#include "psapi.h"
using namespace std::chrono;

using namespace std;
using std::filesystem::recursive_directory_iterator;

long int totalWordCount = 0;
int uniqueWords = 0;

spimi::spimi(string path)
{
	auto start = chrono::high_resolution_clock::now();

	
	read_files(path);

	cout << "Start Merging" << endl;
	writeMapToFile();

	mergeFiles();

	
	auto stop = chrono::high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	
	ofstream MyFile("Info.txt");
	MyFile << "Execution time: " << duration.count()/1000000 << endl;
	MyFile << "Size: 6.6GB" << endl;
	MyFile << "Unique words: " << uniqueWords << endl;
	MyFile << "Total words: " << totalWordCount << endl;
}

void spimi::read_files(const string& path)
{
	for (const auto& file : recursive_directory_iterator(path))
	{
		if(std::filesystem::is_directory(file)) continue;

		handle_file(file.path().string());
	}
}

void spimi::handle_file(const string& path)
{
	std::fstream myFile;
	string line;
	
	myFile.open(path);
	bookNumber++;

	while (std::getline(myFile, line))
	{
		handle_line(line);
	}
	cout << path << endl;
	cout << bookNumber << endl;
	myFile.close();
}


MEMORYSTATUSEX statex;
void spimi::handle_line(string& line)
{
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;


	if(virtualMemUsedByMe > 1800000000)
	{
		writeMapToFile();
	}
	

	vector<string> results;

	clear_line(line);

	istringstream iss(line);
	results.clear();
	copy((istream_iterator<string>(iss)), std::istream_iterator<string>(), std::back_inserter(results));

	for (auto it = results.begin(); it != results.end(); ++it)
	{
		if (tempDict->find(*it) == tempDict->end())
		{
			vector temp{ bookNumber };
			tempDict->insert({*it, temp});

		}
		else
		{
			auto elem = tempDict->find(*it);
			if (std::find(elem->second.begin(), elem->second.end(), bookNumber) == elem->second.end())
			{
				elem->second.push_back(bookNumber);
			}
		}
		totalWordCount++;
	}
}

void spimi::writeMapToFile()
{
	cout << "Write" << endl;
	string name = std::to_string(numberOfOutputFiles);
	name += ".txt";

	path.push_back(name);
	
	ofstream MyFile(name, fstream::app);
	std::ostringstream oss;
	for (auto it = tempDict->cbegin(); it != tempDict->cend(); ++it)
	{
		MyFile << it->first;
		MyFile << " ";
		for(auto p = it->second.cbegin(); p != it->second.cend(); ++p)
		{
			MyFile << *p << ",";
		}
		MyFile << "\n";
	}

	numberOfOutputFiles++;
	cout << numberOfOutputFiles << endl;
	delete tempDict;
	tempDict = new map<string, vector<int>>;

	//if(numberOfOutputFiles > 3)
	//{
	//	cout << "Start Merging" << endl;
	//	mergeFiles();
	//}
}

void spimi::mergeFiles()
{
	cout << "Merge" << endl;
	vector<shared_ptr<fstream>> files;
	vector<string> words(numberOfOutputFiles, "*");

	
	for(auto it = path.cbegin(); it != path.cend(); ++it)
	{
		shared_ptr<fstream> f(new fstream);

		f->open(*it);
		files.push_back(f);
	}

	
	bool readFromFile = true;
	numberOfOutputFiles = -228322;


	while(readFromFile)
	{
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;


		if(virtualMemUsedByMe > 1800000000)
		{
			writeMapToFile();
		}

		for(int i = 0; i < words.size(); i++)
		{
			if(words[i] == "*")
			{
				// string line = "";
				getline(*files[i], words[i]);				
				//words[i] = line;	
			}
		}

		int smallest = getSmallestItem(words);
		addItemToFinalMap(words[smallest]);
		words[smallest] = "*";

		for (int i = 0; i < files.size(); i++)
		{
			string l;
			if(!getline(*files[i],l))
			{
				readFromFile = false;
			}
			else
			{
				readFromFile = true;
			}
		}
		
	}
	
		writeMapToFile();
}

int spimi::getSmallestItem(vector<string>& words)
{
	vector<string> temp = words;

	sort(temp.begin(), temp.end());

	for(int i = 0; i < words.size(); ++i)
	{
		if(words[i] == temp[0])
		{
			return i;
		}
	}
	return NULL;
}


void spimi::addItemToFinalMap(string& line)
{
	vector<string> results;

	clear_line(line);

	istringstream iss(line);
	results.clear();
	copy((istream_iterator<string>(iss)), std::istream_iterator<string>(), std::back_inserter(results));

	if (tempDict->find(results[0]) == tempDict->end())
	{
		vector<int> temp;
		for(int i = 1; i < results.size(); i++)
		{
			temp.push_back(stoi(results[i]));
		}
		tempDict->insert({results[0], temp});

	}
	else
	{
		auto elem = tempDict->find(results[0]);
		for(int i = 1; i < results.size(); i++)
		{
			if (std::find(elem->second.begin(), elem->second.end(), stoi(results[i])) == elem->second.end())
			{
				elem->second.push_back(stoi(results[i]));
			}
		}
	}

	uniqueWords++;
}




const std::regex symb("[^\\w\\s]");
string result;
void spimi::clear_line(string& line)
{
	transform(line.begin(), line.end(), line.begin(), ::tolower);

	result.clear();

	regex_replace(std::back_inserter(result), line.begin(), line.end(), symb, " ");

	line = result;
}


