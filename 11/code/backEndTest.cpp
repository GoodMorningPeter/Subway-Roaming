//#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include "subway_map.cpp"

int main(int argc, char **argv) {
    system("chcp 936");
    Config* config = new Config();
    Graph* graph = new Graph(config);

   auto query_subway_line = [&] () {
       std::cout<<"input 'quit' to exit\n";
       while (1) {
           std::string searchLineName;
           std::cin >> searchLineName;
           try {
               graph->run_query_by_line_num(searchLineName);
           }catch (const std::runtime_error& e) {
               if (strcmp(e.what(), "subway line quit not found")==0) {
                   break;
               }
               std::cout << e.what() << std::endl;
           }
       }
   };

   auto random_walk = [&]() {
       if (argc <= 2) {
           puts("Please enter start station");
           system("pause");
           return;
       }
       if (config->rebuild == true) {
           puts("subway station has been changed since last time");
           puts("path rebuilding .... ");
           graph->rebuild_beltway(config);
           puts("rebuild completed");
       }
       try {
           auto ret = graph->run_query_random_walk(argv[2], config);
       }catch (const std::runtime_error& e) {
           std::cout << e.what() << std::endl;
       }
   };

   auto walk_from_a_to_b = [&]() {
       if (argc <= 3) {
           puts("Please enter start and end station");
           system("pause");
           return;
       }
       try {
           auto ret = graph->run_query_from_a_to_b(argv[2], argv[3]);
       }catch (const std::runtime_error& e) {
           std::cout << e.what() << std::endl;
       }
   };
   auto walk_from_a_to_b_v2 = [&]() {
       if (argc <= 4) {
           puts("Please enter start and end station and time");
           system("pause");
           return;
       }
       try {
            std::string time(argv[4]);
            char h[3], m[3];
            sscanf(time.c_str(), "%[^:]:%s", h, m);
            auto ret = graph->run_query_from_a_to_b_v2(argv[2], argv[3], atoi(h)*60+atoi(m));
       }catch (const std::runtime_error& e) {
           std::cout << e.what() << std::endl;
       }
   };
   if (argc == 1) {
       query_subway_line();
   }else{
       graph->build(config);
       if (strcmp(argv[1], "/a") == 0) {
           random_walk();
       }else if (strcmp(argv[1], "/b") == 0) {
           walk_from_a_to_b();
       }else if (strcmp(argv[1], "/g") == 0){
            
       }else if (strcmp(argv[1], "/b2") == 0){
            walk_from_a_to_b_v2();
       }else{
           puts("Please enter /a or /b");
           system("pause");
           return 0;
       }
   }
}

