//在大数据的情况下统计一个日志文件中出现次数最多的IP

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <vector>
#include <random>
#include <utility>
#include <future>
#include <thread>
#include <memory>
#include <chrono>
#include <algorithm>
using namespace std;

map<long,unsigned> table;
vector<map<long,unsigned>> list;

long GetHashVal(long ip)
{
	return ip % 512;
}

long GetIntIP(const string& arg)
{
	string ip = arg;
	long res = 0;
	int len = ip.length();
	if(!ip.empty())
	{
		for(int i = 3;i >= 0;i--)
		{
			int pos = ip.find_first_of(".");
			string tmp = ip.substr(0,pos);
			ip = ip.substr(ip.find_first_of(".") + 1,len - pos);
			res += static_cast<long>(std::stoi(tmp)*std::pow(256,i));
		}
	}
	return res;
}

void GenerateData(string filename,long count)
{
	ofstream file("data.txt");
	default_random_engine engine;
	uniform_int_distribution<unsigned> u(0,255);
	for(int i = 0;i < count;i++)
		file << "192.168." + to_string(u(engine)) + "." + to_string(u(engine)) << std::endl;
	file.close();
}

pair<long,unsigned> DealFile(ifstream& out)
{
	map<long,unsigned> record;
	string s;
	while(getline(out,s))
	{
		if(s.empty())
			break;
		long num = GetIntIP(s);
		record[num]++;
		//this_thread::sleep_for(chrono::seconds(3));
	}
	//cout << "one time " << endl;
	pair<long,unsigned> init = {0,0};
	for(auto& p : record)
		if(p.second > init.second)
			init = p;
	//cout << init.second << endl;
	return init;
}

int main(int argc,char** argv)
{
	long count = stol(argv[1])/1000;
	int i = 1000;
	future<void> res = async(GenerateData,"data.txt",stol(argv[1]));
	res.wait();
	cout << "创建文件完成" << endl;
	ifstream in("data.txt");
	string line;
	while(i--)
	{
		ofstream out(to_string(i) + ".txt",ofstream::out|ofstream::app);
		for(int n = count;n > 0;n--)
		{
			in >> line;
			//long ipInt = GetIntIP(line);
			out << line << endl;
		}
		cout << "完成第" << i << "个文件" << endl;
		//ofstream out(to_string(hash) + ".txt",ofstream::out|ofstream::app);
	}
	vector<pair<long,unsigned>> resSet;
	ifstream streams[1000];
	for(int i = 0;i < 1000;i++)
	{
		cout << "处理第" << i << "个文件" << endl;
		streams[i].open(to_string(i) + ".txt");
		//stream.back().open(to_string(i) + ".txt");
		//cout << "1" << endl;
		//resSet.emplace_back(async(DealFile,ref(streams[i])));
		resSet.push_back(DealFile(streams[i]));
		//this_thread::sleep_for(chrono::seconds(1));
	}
	cout << "开始排序" << endl;
	sort(resSet.begin(),resSet.end(),
		[](const pair<long,unsigned>& lhs,const pair<long,unsigned>& rhs)
		{ return lhs.second < rhs.second;});
	cout << "ip: " << resSet.back().first << " count: " << resSet.back().second  << endl;
	return 0;
}