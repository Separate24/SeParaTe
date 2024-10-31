#include<iostream>
#include<assert.h>
#include<string>
#include<fstream>
#include<map>
#include<set>
#include<sstream>

using namespace std;

void readPtsFromFile(ifstream& F, map<int, set<int>>& nodePtsMap) {
    string delimiter1 = " -> { ";
    string delimiter2 = " }";

    string line;

    // get start pos
    while(F.good()) {
        getline(F, line);
        if(line == "------") break;
    }

    while (F.good())
    {
        // Parse a single line in the form of "var -> { obj1 obj2 obj3 }"
        getline(F, line);
        if (line == "------")     break;
        size_t pos = line.find(delimiter1);
        if (pos == string::npos)    break;
        if (line.back() != '}')     break;

        // var
        int nodeId = atoi(line.substr(0, pos).c_str());

        // objs
        pos = pos + delimiter1.length();
        size_t len = line.length() - pos - delimiter2.length();
        string objs = line.substr(pos, len);

        if (!objs.empty())
        {
            // map the variable ID to its unique string pointer set
            istringstream ss(objs);
            int obj;
            while (ss.good())
            {
                ss >> obj;
                nodePtsMap[nodeId].emplace(obj);
            }
        }
    }
}

void ptsDiff(string pts_fi, string pts_fs, string output_filename) {
    string line;
    ifstream file_fi(pts_fi.c_str());
    ifstream file_fs(pts_fs.c_str());

    map<int, set<int>> nodePtsMap_fi;
    map<int, set<int>> nodePtsMap_fs;

    readPtsFromFile(file_fi, nodePtsMap_fi);
    readPtsFromFile(file_fs, nodePtsMap_fs);

    map<int, set<int>> pts_diff;
    for(auto iter = nodePtsMap_fi.begin(); iter != nodePtsMap_fi.end(); ++iter) {
        if(nodePtsMap_fs.find(iter->first) == nodePtsMap_fs.end()) {
            pts_diff.emplace(iter->first, iter->second);
        }
        else {
            for(auto pt : iter->second) {
                if(nodePtsMap_fs[iter->first].find(pt) == nodePtsMap_fs[iter->first].end()) {
                    pts_diff[iter->first].emplace(pt);
                }
            }
        }
    }

    ofstream file_output(output_filename.c_str());

    file_output << "------" << endl;
    for(auto iter = pts_diff.begin(); iter != pts_diff.end(); ++iter) {
        file_output << iter->first << " -> { ";
        for(auto pt : iter->second) {
            file_output << pt << " ";
        }
        file_output << "}" << endl;
    }
    file_output << "------" << endl;

    file_fi.close();
    file_fs.close();
    file_output.close();
}


// fi --> flow-insensitive
// fs --> flow-sensitive
int main(int argc, char** argv) {
    assert(argc == 4 && "input should be ./pts_diff <path/to/pts_fi.txt> <path/to/pts_fs.txt> <path/to/output.txt>");

    ptsDiff(argv[1], argv[2], argv[3]);

    return 0;
}