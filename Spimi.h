#pragma once
#include <filesystem>
#include <map>
#include <vector>
#include <string>


using std::vector;
using std::string;
using std::map;

class spimi
{
public:
	spimi(string path);

private:
	int bookNumber = 0;
	int numberOfOutputFiles = 0;
	map<string, vector<int>>* tempDict = new map<string, vector<int>>;

	vector<std::filesystem::path> path;


	void read_files(const string& path);
	void handle_file(const string& path);
	void clear_line(string& line);
	void handle_line(string& line);

	void writeMapToFile();

	void mergeFiles();
	void addItemToFinalMap(string& line);
	int getSmallestItem(vector<string>& words);
};
