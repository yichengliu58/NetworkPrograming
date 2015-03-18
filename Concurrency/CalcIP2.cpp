#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#include <thread>
#include <cmath>
using namespace std;
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

int main()
{
	vector<pair<long,unsigned>> resSet;
	int segNum = 1024;
	long devide = pow(2,32)/1024;
	ifstream in("data.txt");
	string line;
	for(int i = 0;i < segNum;i++)
	{
		cout << i*devide << endl;
		map<long,unsigned> record;
		while(getline(in,line))
		{
			if(line.empty())
				break;
			long ipInt = GetIntIP(line);
			//this_thread::sleep_for(chrono::seconds(3));
			if((ipInt >= i*devide) && (ipInt < (i+1)*devide))
			{
				record[ipInt]++;
				cout << record[ipInt];
			}
			//cout << record[ipInt];
		}
		pair<long,unsigned> init = {0,0};
		for(auto& p : record)
			if(p.second > init.second)
				init = p;
		resSet.push_back(init);

	}
	/*sort(resSet.begin(),resSet.end(),[](const pair<long,unsigned>& lhs,
		const pair<long,unsigned>& rhs){return lhs.second < rhs.second;});*/
	for(auto& r : resSet)
		cout << r.second << endl;
	//cout << " ip: " << resSet.back().first << " count: " << resSet.back().second << endl;
	return 0;
}