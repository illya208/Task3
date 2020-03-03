/*
_____TASK3_____
As a developer
I want to have an application that counts blank lines, comment lines, and physical 
lines of source code written in C/C++. so that I can have some internal statistic 
of the project.
Acceptance Criteria
1.	There are should be a way to specify project root folder
2.	Only files with *.h, *.hpp, *.c, *.cpp extensions are processed from all subfolders
3.	The output should contain the total number of processed files, count of blank lines, 
count of comment lines, count of lines with code and time of execution
4.	The results should also be saved in file.
*/\



//TODO doesent work siwh /* empty line 

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include<regex>
#include <thread>
#include <future>
#include <mutex>
#include <experimental/filesystem>
/*
i was trying to use boost for parsing all *.cpp files but have a problem with no .lib file....
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

so as a solutuion explorer i used bat script which is written automaticly for inputed directory
*/

using namespace std;
namespace fs = std::experimental::filesystem;
std::mutex g_lock;

int global_count_of_line;
int global_count_of_comment_lines;
int global_count_of_blank_lines;
//physical lines of source code written in C / C++ will be = global_count_of_line-
//- global_count_of_comment_lines - global_count_of_blank_lines


class Exception : public std::exception
{
	std::string _msg;
public:
	Exception(const std::string& msg) : _msg(msg) {}

	virtual const char* what() const noexcept override
	{
		return _msg.c_str();
	}
};


void line_of_code(string file_name) {

	cout<<"________________________" << "Parsing a " << file_name << "________________________" << endl;

	std::ifstream FileInput(file_name, std::ifstream::in);

	std::string String_Of_File;
	int count = 0;
	int count_empty = 0;


	ifstream ifs(file_name);
	char temp;
	string s;
	ifs.unsetf(ios::skipws);
	while (ifs >> temp)
		s += temp;


	while (!FileInput.eof())          
	{
		std::getline(FileInput, String_Of_File);
		count++;
		if (String_Of_File.empty())                
		{
			count_empty++;            
		}
	}

	int count_comments = 0;
	regex reg_com1("[\/]{2,}[\\w]*");//->//
	regex reg_com2("/\\*([\\S\\s]+?)\\*/");//->/**/        /\\*(.*?)\\*/

	cmatch sm;

	string buf = s;
	

	while (regex_search(s.c_str(), sm, reg_com1)) {
		count_comments++;
		s = sm.suffix().str();
	}

	cmatch sm2;
	while (regex_search(s.c_str(), sm2, reg_com2)) {
		string basee = sm2[1].str();
		int local_count = 1;
		for (int i = 0; i < basee.size(); i++) {
			if (basee[i] == '\n') {
				local_count++;
			}
		}
		count_comments += local_count;
		s = sm2.suffix().str();
	}


	cout << "full_amount of lines =" << count << endl;
	cout << "count_of_empty=" << count_empty << endl;
	cout << "count of comments =" << count_comments << endl;
	//
	g_lock.lock();
	global_count_of_line += count;
	global_count_of_blank_lines += count_empty;
	global_count_of_comment_lines += count_comments;
	g_lock.unlock();
	
	cout << "________________________" << "ending parsing " << file_name << "________________________" << endl;
}



int main(int argc, char *argv[])
{
	string File_Path_test;
	cout << "input Path: ";
	cin >> File_Path_test;
	//File_Path_test="D:\\task_test";
	auto start = chrono::steady_clock::now();
	fs::path workdir = File_Path_test;
	if (!fs::exists(workdir)) {
		cout << "no such Path\n";
		system("pause");
		return -1;
	}
	vector<string> vector_files_test;
	
	for (auto& p : fs::recursive_directory_iterator(File_Path_test)) {
		if (!fs::is_directory(p) && p.path().extension() == ".cpp" || !fs::is_directory(p) && p.path().extension() == ".hpp"
			|| !fs::is_directory(p) && p.path().extension() == ".c" || !fs::is_directory(p) && p.path().extension() == ".h") {
			vector_files_test.push_back(p.path().string());
		}
	}

	for (unsigned int i = 0; i < vector_files_test.size(); i++) {
		cout<<"!"<<vector_files_test[i] << "!" << endl;
	}
	
	std::vector<std::thread> vector_threads; // i'd better use async but in task reference said std::thread
	for (std::size_t i = 0; i < vector_files_test.size(); i++) {
		//line_of_code(vector_files_test[i]);
		vector_threads.push_back(std::thread(&line_of_code, vector_files_test[i]));
	}
	for (auto & th : vector_threads)
		th.join();
	auto end = chrono::steady_clock::now();
	cout << "full_amount of lines =" << global_count_of_line << endl;
	cout << "count_of_empty=" << global_count_of_blank_lines << endl;
	cout << "count of comments =" << global_count_of_comment_lines << endl;

	std::ofstream Result_file("results.txt");
	Result_file << "Files processed: " << vector_files_test.size() <<" with time "<< chrono::duration_cast<chrono::milliseconds>(end - start).count()<<" ms" << '\n';
	for (int i = 0; i < vector_files_test.size(); i++) {
		Result_file << vector_files_test[i] << '\n';
	}
	Result_file << "Full amount of lines =" << global_count_of_line << '\n';
	Result_file << "Amount of blank lines =" << global_count_of_blank_lines << '\n';
	Result_file << "Amount of comment lines =" << global_count_of_comment_lines << '\n';
	Result_file << "Physical lines of source code written in C / C++ =" << global_count_of_line - global_count_of_comment_lines
		- global_count_of_blank_lines << '\n';
	Result_file.close();


	system("pause");
	return 0;
}