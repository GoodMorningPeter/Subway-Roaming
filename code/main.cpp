//#pragma once
#include "mainwindow.h"
#include <QApplication>
#include <QThread>
#include <iostream>
#include <cstring>

int main(int argc, char **argv) {
    system("chcp 936");
//    Config* config = new Config();
//    cout<<"gr!!"<<endl;
//    Graph* graph = new Graph(config);
//    cout<<"graph"<<endl;


//    auto query_subway_line = [&] () {
//        std::cout<<"输入地铁线路以查询，输入quit退出";
//        while (1) {
//            std::string searchLineName;
//            std::cin >> searchLineName;
//            try {
//                graph->run_query_by_line_num(searchLineName);
//            }catch (const std::runtime_error& e) {
//                if (strcmp(e.what(), "subway line quit not found")==0) {
//                    break;
//                }
//                std::cout << e.what() << std::endl;
//            }
//        }
//    };

//    auto random_walk = [&]() {
//        if (argc <= 2) {
//            puts("Please enter start station");
//            system("pause");
//            return 0;
//        }
//        if (config->rebuild == true) {
//            puts("subway station has been changed since last time");
//            puts("path rebuilding .... ");
//            graph->rebuild_beltway(config);
//            puts("rebuild completed");
//        }
//        try {
//            graph->run_query_random_walk(argv[2], config);
//        }catch (const std::runtime_error& e) {
//            std::cout << e.what() << std::endl;
//        }
//    };

//    auto walk_from_a_to_b = [&]() {
//        if (argc <= 3) {
//            puts("Please enter start and end station");
//            system("pause");
//            return 0;
//        }
//        try {
//            graph->run_query_from_a_to_b(argv[2], argv[3]);
//        }catch (const std::runtime_error& e) {
//            std::cout << e.what() << std::endl;
//        }
//    };

//    if (argc == 1) {
//        query_subway_line();
//    }else{
//        graph->build(config);
//        if (strcmp(argv[1], "/a") == 0) {
//            random_walk();
//        }else if (strcmp(argv[1], "/b") == 0) {
//            walk_from_a_to_b();
//        }else if (strcmp(argv[1], "/g") == 0){
            
//        }else{
//            puts("Please enter /a or /b");
//            system("pause");
//            return 0;
//        }
//    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

