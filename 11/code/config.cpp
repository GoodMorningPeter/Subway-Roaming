#pragma once
#include <vector>
#include <string>
#include <fstream> 
#include <cstdio>
#include <iostream>
#include <map>
#include "sha236.cpp"


class Config{
public:
    mySHA mySHAUtil=mySHA();
    int change_cost;
    std::string path;
    std::string fileType;
    std::string cacheFile, cacheRouteStart;
    std::string version;
    std::string timetable_file;

    bool rebuild;
    int nodesNum, linesNum;
    //  Config (std::string path = "D:\\Github\\subway\\data\\config.txt", std::string fileType = "txt") {

   Config (std::string path = "../data/config.txt", std::string fileType = "txt") {
        rebuild = false;

        this->fileType = fileType;
        // std::cout << "Config"<< std::endl;
        std::string param;
        std::ifstream infile;
        infile.open(path.c_str(), std::ios::in);
//        std::cout<<"here";

        std::string lastSHA236;
        while (!infile.eof()){
            infile >> param;
            //  std::cout << param << std::endl;
//             exit(0);
            if(param == "change_cost"){
                infile >> this->change_cost;
            }else if (param == "cacheFile"){
                infile >> cacheFile;
            }else if (param == "cacheRouteStart"){
                infile >> cacheRouteStart;
            }else if(param == "subway_map_file"){
                infile >> this->path;
            }else if(param == "lastSHA") {
                infile >> lastSHA236;
            }else if (param == "version") {
                infile >> version;
            }else if (param == "timetable_file") {
                infile >> timetable_file;
            }
        } 

        std::string thisSHA = mySHAUtil.mycalSHA236(this->path);
        if(thisSHA == lastSHA236){
            std::cout << "The same subway_info file!" << std::endl;
        }else {
            std::cout << "Different subway_info file!" << std::endl;
            myrewrite(thisSHA, path);
            rebuild = true;
        }
        // std::cout << "Config finished!!"<< std::endl;
        infile.close(); 
    }

    void myrewrite(std::string& thisSHA, std::string& filename) {
        std::ifstream inputFile(filename);
        std::vector<std::string> key, value;
        std::string line;

        if (inputFile.is_open()) {
            // 读取文件的每一行并存储到vector中
            while (!inputFile.eof()) {
                std::string st;
                inputFile >> st;
                key.push_back(st);
                if (st != "lastSHA") {
                    inputFile >> st;
                    value.push_back(st);
                }else{
                    inputFile >> st;
                    value.push_back(thisSHA);
                }
            }
            inputFile.close();
             // 将修改后的内容写回到文件
            std::ofstream outputFile(filename);
            if (outputFile.is_open()) {
                for (int i=0;i<(int)key.size(); i++) {
                    outputFile << key[i]  <<" " << value[i] << "\n";
                }
                outputFile.close();
                std::cout << "succeed" << std::endl;
            } else {
                std::cerr << "open output file failed" << std::endl;
            }
        } else {
            std::cerr << "open input file failed" << std::endl;
        }
    }

};

