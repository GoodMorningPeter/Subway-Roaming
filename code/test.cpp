//#pragma once
//#include<vector>
//#include<cstdio>
//#include<string>
//#include<iostream>
//#include<fstream>

//// using namespace std;

//void write(const std::vector<std::string>& circuit_names, std::string filename) {
//    int totalCost = circuit_names.size();
//    std::ofstream file(filename);
//    if (file.is_open()) {
//        file << totalCost << '\n';
//        for (const std::string& name : circuit_names) {
//            file << name << '\n';
//        }
//        file.close();
//    } else {
//        // 文件无法打开，进行错误处理
//        std::cerr << "无法打开文件: " << filename << std::endl;
//    }
//}

//int main(){
//    // vector<int> a;
//    // a.reserve(10);
//    // a.emplace_back(1);dd
//    // printf("%d\n", a.size());
//    std::vector<std::string> circuit_names = {"11", "22"};
//    write(circuit_names, "./tmp.txt");

//   return 0;
//}

