#pragma once
#include <deque>
#include <queue>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <set>
#include <random>
#include <ctime>
#include <string>
#include "utils.h"
#include "config.cpp"


class SubwayStruct {
public:
    virtual void print() = 0;
    virtual std::string getName() = 0;
    virtual int getID() = 0;
};

class Point {
public:
    int x, y;
    Point(int x=0, int y=0):x(x),y(y){}

};

class Node : public SubwayStruct {
public:
    int id;
    std::string name;
    double x,y;
    Node() : id(-1), name("NULL") {}
    Node(int id, std::string name) : id(id), name(name) {}
    Node(int id, std::string name, double x, double y) : id(id), name(name), x(x), y(y) {}
    std::string getName(){
        return this->name;
    }
    int getID() {
        return this->id;
    }
    void print(){
        std::cout << name ;
    }
    int getId(){ return id; }
};

class Line : public SubwayStruct {
public:
    int id;
    std::string name;
    std::vector<Node*> nodes;
    Line() : name("NULL") {}
    Line(std::string name, std::vector<Node*> nodes, int id) : name(name), nodes(nodes), id(id) {}
    std::string getName(){
        return this->name;
    }
    int getID() {
        return this->id;
    }
    void print(){
        for(int i=0; i<nodes.size(); i++){
            nodes[i]->print();
            std::cout<<" ";
        }
    }
    std::vector<int> getIdList() {
        std::vector<int> ret;
        for (auto pNode : nodes) {
            ret.push_back(pNode->getId());
        }
        return ret;
    }
};

class UniqueIntVector : public std::vector<int> {
public:
    UniqueIntVector(): vector(0) {}
    UniqueIntVector(int n) : vector(n) {}
    UniqueIntVector(int n, int v) : vector(n, v) {}
    int cost(const std::vector<int>& mapper) {
        int cost = 0;
        for (int i=0; i<size(); i++) {
            if (i==0 || at(i)==0 || mapper[at(i)] != mapper[at(i-1)]) cost++;
        }
        return cost;
    }
};

class Graph{
    std::vector<Node> nodes;
    std::vector<Line> lines;
    std::map<int, Node*> id2Nodes;
    const Config* const config;
    const int first_train = 4*60+54;
    const int train_minute = 1200;

    std::map<std::string, SubwayStruct*> name2subwayStructs;  
    friend class MainWindow;
    static double myrandom() { 
        static std::random_device seed;
        static std::ranlux48 engine(seed());
        static std::uniform_int_distribution<> distrib(0, 1000000);
        return (distrib(engine)%1000000)/1000000.0; 
    }

    struct v2_route {
        int node, line, time;
    };
    struct AbstGraph{
        int n, maxp, actualNodeNum;
        std::vector<std::vector<int>> changeSeqList;
        std::vector<int> x, y, w, _last, _next;
        std::vector<int> mapper, mapperLine;
        std::vector<int> starter;
        std::vector<std::map<int, int>> mapper_lineid_newid;

        void initial(int n, int totNodes) { 
            this->n=this->maxp=n;
            changeSeqList.resize(n+1);
            mapper.reserve(n+1);
            for (int i=0;i<=n;i++) mapper.emplace_back(i);
            mapperLine.resize(n+1);

            _last.resize(totNodes+1, -1);
        }
        void initial_v2(int totLines) {
            mapper_lineid_newid.resize(totLines);
        }
        void edge(int u, int v, int cost=1) {
            //Adjacency tables
            x.emplace_back(u), y.emplace_back(v), w.emplace_back(cost);
            _next.emplace_back(_last[u]);

            _last[u]=_next.size() - 1;
        }
        void allocate(const std::vector<int>& ids, int lineNum) {
            std::map<int, int> lineid2newid;
            //new node
            for (int id : ids) {
                int newid = id;
                if (changeSeqList[id].size() != 0) {//allocate a new node
                    newid = ++maxp; // map true_id to abst_id
                    mapper.emplace_back(id);
                    mapperLine.emplace_back(-1);
                }
                changeSeqList[id].emplace_back(newid);
                mapperLine[newid] = lineNum;//map node_id to LineNumber
                lineid2newid[id] = newid;
            }
            mapper_lineid_newid[lineNum] = lineid2newid;
        }
        //build edges on the node belong to new Line
        void buildRoute(int u, int v) {
            int u_ = changeSeqList[u].back();
            int v_ = changeSeqList[v].back();
            edge(u_, v_, 1);
            edge(v_, u_, 1);
        }
        //bi_mapper, sequence number start from 1 instead of 0
        int get_v2id(int id, int time) {return id+time*maxp;}
        int get_v2id_revid(int id) {return (id-1)%maxp+1;}
        int get_v2id_revt(int id) {return (id-get_v2id_revid(id))/maxp;}
        //build pink edge for v2
        void buildRoute_v2(int lineNum, int idx, int timex, int idy, int timey) {
            std::map<int, int> &mapper = mapper_lineid_newid[lineNum];
            // printf("%d %d\n", get_v2id(mapper[idx],timex), get_v2id(mapper[idy],timey));
            // printf("%d %d %d %d\n", mapper[idx],timex, mapper[idy],timey);
            assert(mapper[idx]!=0 && mapper[idy]!=0);
            edge(get_v2id(mapper[idx],timex), get_v2id(mapper[idy],timey));
        }
        //build bidirectional edge
        void insert_biedge(int u, int v, int cost, bool ext = false) {
            if (ext==true) _last.emplace_back(-1);
            edge(u, v, cost);
            edge(v, u, cost);
        }
        //Establish change(transfer) edges, details in mainblog
        void selfconnect(int change_cost) {
            for (int i=1;i<=n;i++) {
                if (changeSeqList[i].size() > 1) {
                    for (int id1 : changeSeqList[i]) {
                        for (int id2 : changeSeqList[i]) {
                            int now = id1;
                            for (int k=1;k<change_cost;k++) {
                                insert_biedge(now, ++maxp, 1, true);
                                now = maxp;
                            }
                            int last_cost = (change_cost==0)?0:1;
                            insert_biedge(now, id2, last_cost);
                        }
                    }
                }
            }
            //set virtual starting node
            starter.resize(maxp+1);
            for (int i=0;i<=maxp;i++) starter[i] = i;
            for (int i=1;i<=n;i++) {
                if (changeSeqList[i].size() > 1) {
                    _last.emplace_back(-1);
                    maxp++;
                    for (int id : changeSeqList[i]) {
                        starter[id] = maxp;
                        edge(maxp, id, 0);
                    }
                }
            }
        }
        //build all edge except pink edge
        void rebuild_v2(int max_minute, int change_cost) {
            //clear original map
            x.clear(), y.clear(), w.clear(), _next.clear(), _last.clear();
            int N = max_minute*(maxp+1)+1;
            x.reserve(N), y.reserve(N), w.reserve(N), _next.reserve(N);
            _last.resize(N, -1);
            // printf("N = %d\n", N);

            //build change station
            for (int i=1;i<=n;i++) {
                if (changeSeqList[i].size() > 1) {
                    for (int id1 : changeSeqList[i]) {
                        for (int id2 : changeSeqList[i]) {
                            for (int t=0; t+change_cost<max_minute; t++) {
                                edge(get_v2id(id1, t), get_v2id(id2, t+change_cost));
                            }
                        }
                    }
                }
            }

            //build virtual station
            for (int i=1;i<=n;i++) {
                if (changeSeqList[i].size() > 1) {
                    for (int t=0; t<max_minute; t++) {
                        for (int id : changeSeqList[i]) {
                            edge(get_v2id(starter[id], t), get_v2id(id, t));
                        }
                    }
                }
            }
            //build waiting edge
            for (int i=1;i<=n;i++) {
                for (int t=0; t+1<max_minute; t++) {
                    for (int id : changeSeqList[i]) {
                        edge(get_v2id(id, t), get_v2id(id, t+1));
                    }
                }
            }
        }
        //run 0-1 Dijkstra
        std::pair<int, std::vector<std::pair<int, int>>> run(int s, int t) {
            std::vector<int> from(maxp+1, -1), dis(maxp+1, 0);
            std::deque<int> dq;
            from[starter[s]] = 0;
            dq.emplace_back(starter[s]);
            bool complete = false;
            while (complete == false) {
                int pos = dq.front(); dq.pop_front();
                for (int j=_last[pos]; j!=-1; j=_next[j]) {
                    if (from[y[j]] == -1) {
                        from[y[j]] = pos;
                        dis[y[j]] = dis[pos] + w[j];
                        if (w[j] == 1) {
                            dq.emplace_back(y[j]);
                        }else{
                            dq.emplace_front(y[j]);
                        }
                        if (y[j] < mapper.size() && mapper[y[j]] == mapper[t]) {
                            t = y[j];
                            complete = true;
                            break;
                        }
                    }
                }
            }
            //Build an S to T path
            std::vector<int> route;
            route.emplace_back(t);
            while (t >= mapper.size() || mapper[t]!=mapper[s]) {
                t = from[t];
                route.emplace_back(t);
            }
            reverse(route.begin(), route.end());
            //delete Auxiliary Point 
            std::vector<int> route_backbone;
            for (int abst_id : route) {
                if (abst_id >= mapper.size()) continue;
                route_backbone.push_back(abst_id);
            }
            //deal wrong start and end point
            std::vector<int> std_route;
            int l = 0, r = route_backbone.size()-1;
            while (l < r && mapper[route_backbone[l]] == mapper[route_backbone[l+1]]) l++;
            while (l < r && mapper[route_backbone[r-1]] == mapper[route_backbone[r]]) r--;
            for (int i = l; i <= r; i++) {
                std_route.emplace_back(route_backbone[i]);
            }
            // printf("%d %d\n", dis[std_route.back()],dis[std_route.front()]);
            int mindis = dis[std_route.back()] - dis[std_route.front()];
            //map
            std::vector<std::pair<int, int>> ans;
            for (int abst_id : std_route) {
                ans.push_back({mapper[abst_id], mapperLine[abst_id]});
            }
            return {mindis, ans};
        }
        //cost, <<Node, Line>, time>
        std::pair<int ,std::vector<v2_route>> run_v2(int s, int t, int time) {
            //initial start point
            s = get_v2id(starter[s], time);
            std::vector<int> from(_last.size()+1, -1);
            std::queue<int> q;
            from[s] = 0;
            q.push(s);
            bool complete = false;
            //bfs, use queue
            while (!q.empty()) {
                int pos = q.front(); q.pop();
                for (int j=_last[pos]; j!=-1; j=_next[j]) {
                    if (from[y[j]] == -1) {
                        from[y[j]] = pos;
                        q.push(y[j]);
                    }
                }
            }
            //find the minimum cost target(end point)
            int time_cost = 0;
            int t_true = -1, min_time_cost = 1e5;
            for (int t_point : changeSeqList[t]) {
                while (from[get_v2id(t_point, time+time_cost)] == -1) time_cost ++;
                if (min_time_cost > time_cost) {
                    min_time_cost = time_cost;
                    t_true = t_point;
                }else if (min_time_cost == time_cost && get_v2id_revt(from[get_v2id(t_true, time_cost)]) != time_cost) {
                    //ensure end point will not change more than once
                    min_time_cost = time_cost;
                    t_true = t_point;
                }
            }
            //determined end point
            t = get_v2id(t_true, time+min_time_cost);
            //Build an S to T path
            std::vector<int> route;
            route.emplace_back(t);
            while (get_v2id_revt(t) != get_v2id_revt(s) || mapper[get_v2id_revid(t)]!=mapper[get_v2id_revid(s)]) {
                t = from[t];
                route.emplace_back(t);
            }
            reverse(route.begin(), route.end());
            //delete Auxiliary Point 
            std::vector<int> route_backbone;
            for (int abst_id : route) {
                if (get_v2id_revid(abst_id) >= mapper.size()) continue;
                route_backbone.push_back(abst_id);
            }
            //deal wrong start and end point
            std::vector<int> std_route = route_backbone;
            // printf("%d %d\n", dis[std_route.back()],dis[std_route.front()]);
            int mindis = min_time_cost;
            //map
            std::vector<v2_route> ans;
            for (int abst_id : std_route) {
                ans.push_back({mapper[get_v2id_revid(abst_id)], mapperLine[get_v2id_revid(abst_id)], get_v2id_revt(abst_id)});
            }
            return {mindis, ans};
        }
        void save_state() { actualNodeNum = maxp; }
        //Simulate annealing to find beltway
        std::pair<int, std::vector<std::pair<int,int>>> find_circuit(int start, int change_cost) {
            double a = 10, k = 5;
            //Heuristic function
            auto h = [&](int dis) {
                return a/pow(dis+1, k);
            };
            //Simulate annealing Info and basic function
            struct SimulatedAnnealing{
                double T = 1e3, q = 0.98;
                // double T = 1e3, q = 0.001;
                int Len = 50,  cur = 1;
                double T_min = 1e-2;
                double _a, _k;
                double tot = log(T_min/T)/log(q);
                bool alive() { return T > T_min; }
                void shake(double& a, double& k) {
                    _a=a, _k=k;
                    // a = a+(myrandom()-0.5);
                    k = k+(Graph::myrandom()-0.5)/3;
                }
                //get next state
                void next() { 
                    T*=(cur==Len)?q:1;
                    cur = cur%Len+1;
                }
                //judge whether need to annealing
                bool accept(double dE, double &a, double &k) { 
                    bool isAccept = (dE >= 0 || (dE < 0 && exp(dE/T) > Graph::myrandom()));
                    if (isAccept == false) a=_a, k=_k;
                    return isAccept; 
                }
                //output annealing information
                void show(int cur, double avg, int minv) {
                    double precent_cur = 100-100*(log(T_min/T)/log(q))/tot;
                    double precent_nxt = 100-100*(log(T_min/(T*q))/log(q))/tot;
                    double precent = precent_cur + (precent_nxt-precent_cur)/Len*this->cur;

                    precent = std::min(precent, 100.0);
                    std::cout << "\r" << std::setw(6) << std::fixed << std::setprecision(3)<< precent << "% ";
                    std::cout << std::setw(6) << cur << " cost ";
                    std::cout << std::setw(6) << std::fixed << std::setprecision(2) << avg << " avgcost ";
                    std::cout << std::setw(6) << minv << " mincost ";
                    std::cout<<"["<<_a<<" "<<_k<<"] ";

                    static int done = 0;
                    static clock_t t0 = clock();
                    if (done == 0) t0 = clock();
                    done = 1;
                    int difft = 1000*(clock()-t0)/CLOCKS_PER_SEC;
                    int rest = difft/(precent/100) - difft;

                    std::cout << std::setw(2) << difft/3600000 << "h:" << std::setw(2) << difft%3600000/60000 << "m:" << std::setw(2) << difft%60000/1000<< "s:" << std::setw(3) << difft%1000;
                    std::cout << " /";
                    std::cout << std::setw(2) << rest/3600000 << "h:" << std::setw(2) << rest%3600000/60000 << "m:" << std::setw(2) << rest%60000/1000<< "s:" << std::setw(3) << rest%1000;
                }
            }simulator;
            //Preprocess the shortest path between any two nodes
            std::vector<std::vector<int>> dis(actualNodeNum+1, std::vector<int>(actualNodeNum+1));
            std::vector<std::vector<int>> from(actualNodeNum+1, std::vector<int>(actualNodeNum+1));
            for (int s=1; s<=actualNodeNum; s++) {
                std::deque<int> dq;
                std::vector<int> ldis(maxp+1),lfrom(maxp+1, -1);
                lfrom[s] = 0;
                ldis[s] = 0;
                dq.emplace_back(s);
                int res = actualNodeNum - 1;
                while (res) {
                    int pos = dq.front(); dq.pop_front();
                    for (int j=_last[pos]; j!=-1; j=_next[j]) {
                        // if (s>200) std::cout<<res<<std::endl;
                        if (lfrom[y[j]] == -1) {
                            if (y[j] <= actualNodeNum) res--;
                            ldis[y[j]] = ldis[pos] + w[j];
                            if (pos <= actualNodeNum)
                                lfrom[y[j]] = pos;    
                            else
                                lfrom[y[j]] = lfrom[pos];
                            if (w[j] == 1) {
                                dq.emplace_back(y[j]);
                            }else{
                                dq.emplace_front(y[j]);
                            }
                        }
                    }
                }
                for (int i=1;i<=actualNodeNum;i++) {
                    from[s][i] = lfrom[i];
                    dis[s][i] = ldis[i];
                }
            }
            UniqueIntVector result, min_result(n*n/2);
            //Simulated Annealing
            while (simulator.alive()) {
                simulator.shake(a,k);
                //trying sequence
                UniqueIntVector attempt(1, start);
                std::set<int> unvisited;
                //find path from 'f' to 't' base on shortest path Preprocessed
                auto expend = [&](int f, int t) {
                    std::vector<int> route;
                    int dealUnvisitedNodeNum=0;
                    while (t != f) {
                        route.emplace_back(t);
                        if (unvisited.count(t) != 0) {
                            dealUnvisitedNodeNum++;
                            unvisited.erase(t);
                        }
                        t = from[f][t];
                    }  
                    reverse(route.begin(), route.end());
                    attempt.insert(attempt.end(), route.begin(), route.end());
                    return dealUnvisitedNodeNum;
                };
                //initialize node need to be visited
                for (int i=1;i<=actualNodeNum;i++) {
                    if (i != start) {
                        unvisited.insert(i);
                    }
                }
                int now_id = start, resNode = actualNodeNum-1;
                while (resNode) {
                    //node selection strategy, base on Heuristic functions 
                    double sum = 0;
                    for (int node_id : unvisited) sum += h(dis[now_id][node_id]);
                    double selector = sum * Graph::myrandom();
                    for (int node_id : unvisited) {
                        selector -= h(dis[now_id][node_id]);
                        if (selector <= 0) {
                            resNode -= expend(now_id, node_id);
                            now_id = node_id;
                            break;
                        }
                    }
                }
                expend(now_id, start);
                //annealing judgements
                if (min_result.cost(mapper) > attempt.cost(mapper)) min_result = attempt;
                int dE = result.cost(mapper) - attempt.cost(mapper);
                if (result.cost(mapper) == 0) dE = 1;
                
                if (simulator.accept(dE,a,k)) {
                    result = attempt;
                }
                //annealing
                simulator.next();
                static double avgcost = -1;
                if (avgcost < 0) avgcost = result.cost(mapper);
                else avgcost = 0.99*avgcost + 0.01*result.cost(mapper);
                simulator.show(result.cost(mapper), avgcost, min_result.cost(mapper));
            }
            //calculate result
            /*
            v 1.0
            int cost = 0;
            std::vector<int> ans;
            for (int i=0; i<min_result.size(); i++) {
                if (ans.size() == 0 || ans.back()!=mapper[min_result[i]]) {
                    ans.emplace_back(mapper[min_result[i]]);
                    cost += 1;
                }else{
                    cost += change_cost;
                }
            }
            */
           /*
           v 2.0  
           */   
            int cost = 0;
            std::vector<std::pair<int,int>> ans;
            for (int i=0; i<min_result.size(); i++) {
                if (ans.size() == 0 || ans.back().first!=mapper[min_result[i]]) {
                    ans.emplace_back((std::pair<int,int>){mapper[min_result[i]], mapperLine[min_result[i]]});
                    cost += 1;
                }else{
                    cost += change_cost;
                }
            }    
            return {cost, ans}; 
        }
    }abstGraph;

public:
    Graph (Config* config) : config(config) {
        nodes.reserve(500);
        lines.reserve(50);
        //use .txt config
        if (config->fileType == "txt") {
            //read graph info
            std::ifstream infile;
            infile.open(config->path.c_str(), std::ios::in);
            // std::cout<<config->path.c_str();
//            infile.imbue(std::locale("zh_CN.GBK"));
            char type;
            int totNode = 0;

            while (!infile.eof()){
                infile >> type;
                // std::cout << type;
                //get station 
                if (type == '&') {  
                    int id;
                    std::string name;
                    double x,y;
                    infile >> id >> name >> x >> y;
                    // std::cout<<"add point"<<id<<name<<x<<y<<std::endl;
                    nodes.push_back(Node(id, name, x, y));
                    id2Nodes[id] = &nodes.back();
                    name2subwayStructs[name] = &nodes.back();
                    
                }else if (type == '@'){ 
                    //get Line
                    int num;
                    std::string name;
                    infile >> name >> num;
                    // std::cout<<name<<std::endl;
                    // std::cout<<"add line"<<name<<num<<std::endl;
                    std::vector<Node*> lineNodes;
                    for (int i = 0; i < num; i++) {//station in Line
                        int nodeid;
                        infile >> nodeid;

                        lineNodes.push_back(id2Nodes[nodeid]);  
                    }
                    
                    lines.push_back(Line(name, lineNodes, lines.size()));
                    name2subwayStructs[name] = &lines.back();
                    totNode += num;
                }
                type = '#';
            }
            infile.close();  
            config->nodesNum = nodes.size();
            config->linesNum = lines.size();
            abstGraph.initial(config->nodesNum, totNode);
        }else{
            throw std::runtime_error("NoImplementError");
        }
    }
    //task query
    void run_query_by_line_num(std::string searchLineName){
        if (name2subwayStructs.count(searchLineName.c_str()) == 0) {
            throw std::runtime_error("subway line " + searchLineName + " not found");
        }
        //print line
        name2subwayStructs[searchLineName.c_str()]->print();
        std::cout<<std::endl;
    }

    void build(const Config* config) {
        int change_cost = config->change_cost;
        abstGraph.initial_v2(lines.size());
        for (int lineNum = 0; lineNum < lines.size(); ++lineNum) {
            auto line = lines[lineNum];
            std::vector<int> ids = line.getIdList();
            abstGraph.allocate(ids, lineNum);
            for (int i=1;i<ids.size();i++) {
                abstGraph.buildRoute(ids[i-1], ids[i]);
            }
        }
        abstGraph.save_state();
        abstGraph.selfconnect(change_cost);
        if (config->version == "2.0") {
            buildHierarchical();
        }
    }
    void buildHierarchical() {
        abstGraph.rebuild_v2(train_minute, config->change_cost);
        std::ifstream infile;
        // std::cout<<"!"<<config->timetable_file<<std::endl;
        infile.open(config->timetable_file.c_str(), std::ios::in);
        while (!infile.eof()) {
            std::string lineName; infile >> lineName;
            if (lineName.size() ==0 ) break;
            // std::cout<<lineName<<std::endl;
            int N; infile >> N;
            int lineNum = name2subwayStructs[lineName]->getID();

            int lst_time; infile >> lst_time; lst_time -= first_train;
            std::string stationName; infile >> stationName;
            int lst_stationID = name2subwayStructs[stationName]->getID();
            for (int i = 1; i < N; i++) {
                int time; infile >> time; time -= first_train;
                std::string stationName; infile >> stationName; 
                // std::cout<<lineName <<" " << lineNum<< " " <<lst_time+first_train <<" " << lst_stationID<<" "<<time+first_train <<" " << stationName <<std::endl;
                int cur_id = name2subwayStructs[stationName]->getID();
                abstGraph.buildRoute_v2(lineNum, lst_stationID, lst_time, cur_id, time);
                // std::cout<<lst_time+first_train <<" " << lst_stationID<<" "<<time+first_train <<" " << stationName <<std::endl;

                lst_time = time, lst_stationID = cur_id;
            }
        }infile.close(); 
    }
    //task query
    typedef std::vector<std::string> sv;
    std::pair<sv, sv> run_query_from_a_to_b(std::string s, std::string t){
        if (name2subwayStructs.count(s) == 0) throw std::runtime_error("station " + s + " not found");
        if (name2subwayStructs.count(t) == 0) throw std::runtime_error("station " + t + " not found");//station not found
        //call AbstGraph to get path, ans = pair<int, vector<int>>
//        puts(s.c_str());
//        puts(t.c_str());

        auto ans = abstGraph.run(((Node*)name2subwayStructs[s])->getId(), 
                                 ((Node*)name2subwayStructs[t])->getId());
        int dis=ans.first;
        auto route = ans.second;
        std::cout<< "cost is: " << dis << std::endl;
        //convert id to name, but we need to eliminate duplicates of station names
        //duplicates of station names is because of ZERO COST change 
        int lst_line = -1;
        sv routeNode, routeLine;
        for (int i=0; i<route.size();i++) {
            if (i+1 < route.size()) {
                if (route[i].first == route[i+1].first) {// duplicates
                    continue;
                }
            }
            routeNode.push_back(id2Nodes[route[i].first]->getName());
            routeLine.push_back(lines[route[i].second].getName());

            std::cout<< id2Nodes[route[i].first]->getName();
            if (route[i].second != lst_line) {
                if (lst_line == -1) {
                    std::cout<< " [take " << lines[route[i].second].getName() << "]" << std::endl;
                }else{ 
                    std::cout<< " [change " << lines[route[i].second].getName() << "]" << std::endl;
                }
                lst_line = route[i].second;
            }else{
                std::cout<< std::endl;
            }
        }
        return {routeNode, routeLine};
    }
    std::pair<std::pair<sv, sv>, sv> run_query_from_a_to_b_v2(std::string s, std::string t, int time){
        if (name2subwayStructs.count(s) == 0) throw std::runtime_error("station " + s + " not found");
        if (name2subwayStructs.count(t) == 0) throw std::runtime_error("station " + t + " not found");//station not found
        //call AbstGraph to get path, ans = pair<int, vector<int>>
        time -= first_train;
        auto ans = abstGraph.run_v2(((Node*)name2subwayStructs[s])->getId(), 
                                    ((Node*)name2subwayStructs[t])->getId(),
                                    time);
        
        int dis=ans.first;
        auto route = ans.second;
        std::cout<< "time need: " << dis/60 << " hour, " << dis%60 << " minute" << std::endl;
        //convert id to name, but we need to eliminate duplicates of station names
        //duplicates of station names is because of ZERO COST change 
        int lst_line = -1;
        sv routeNode, routeLine, routeTime;
        for (int i=0; i<route.size();i++) {
            if (i+1 < route.size()) {
                if (route[i].node == route[i+1].node && (i==0 || route[i].node == route[i-1].node)) {// duplicates
                    continue;
                }
            }
            routeNode.push_back(id2Nodes[route[i].node]->getName());
            routeLine.push_back(lines[route[i].line].getName());
            auto calculateTime = [&](int t) {
                int h = t/60, m = t%60;
                std::string sh = std::to_string(h);
                std::string sm = std::to_string(m);
                if (sh.size() < 2) sh = "0"+sh;
                if (sm.size() < 2) sm = "0"+sm;
                return  sh + ":" + sm;
            };
            std::string str_time = calculateTime(route[i].time + this->first_train);
            routeTime.push_back(str_time);
            std::cout<< id2Nodes[route[i].node]->getName() << " (at time " << str_time << ") ";
            if (route[i].line != lst_line) {
                if (lst_line == -1) {
                    std::cout<< " [take " << lines[route[i].line].getName() << "]" << std::endl;
                }else{ 
                    std::cout<< " [change " << lines[route[i].line].getName() << "]" << std::endl;
                }
                lst_line = route[i].line;
            }else{
                if (i+1 < route.size() && route[i].node == route[i+1].node) {
                    int j = i+1;
                    while (j+1 < route.size() && route[j].node == route[j+1].node) j++;
                    std::cout<< " [wait " << route[j].time - route[i].time << " minutes]" << std::endl;
                }else{
                    std::cout<< std::endl;
                }
            }
        }
        return {{routeNode, routeLine}, routeTime};
    }
    //write file
    void write(const std::vector<std::string>& circuit_names, std::string filename) {
        int totalCost = circuit_names.size();
        std::ofstream file(filename);
        if (file.is_open()) {
            file << totalCost << '\n';
            for (const std::string& name : circuit_names) {
                file << name << '\n';
            }
            file.close();
        } else {
            // ????????????��?????
            std::cerr << "????????: " << filename << std::endl;
        }
    }

    void rebuild_beltway(const Config* config) {
        //call AbstGraph to find current beltway, costANDcircuit_ids = pair<int, vector<int>>
        auto costANDcircuit_ids = abstGraph.find_circuit(
            ((Node*)name2subwayStructs[config->cacheRouteStart])->getId(),
            config->change_cost
        );
        int cost = costANDcircuit_ids.first;
        auto circuit_ids = costANDcircuit_ids.second;
        std::vector<std::string> circuit_names;
        //convert id to name
        for (auto id : circuit_ids) {
            circuit_names.emplace_back(id2Nodes[id.first]->getName() + " "
                                      +lines[id.second].getName());
        }
        std::cout<< "cost: " << cost<<std::endl;
        std::cout<< "length: " << circuit_names.size() <<std::endl;
        // puts("!");
        write(circuit_names, config->cacheFile);
    }
    //task  query
    std::pair<sv, sv> run_query_random_walk(std::string startNodeName, const Config* config){
        if (name2subwayStructs.count(startNodeName) == 0) {
            throw std::runtime_error("station " + startNodeName + " not found");
        }//station not found
        //read path from cache
        std::ifstream infile;
        infile.open((config->cacheFile).c_str(), std::ios::in);
        int num; infile >> num;

        sv routeNode,routeLine;
        routeNode.reserve(num);//path array
        routeLine.reserve(num);//path array
        for (int i=0;i<num;i++) {
            std::string name, line;
            infile >> name >> line;
            routeNode.emplace_back(name);
            routeLine.emplace_back(line);
        }
        //output path
        std::string lstLine = "NULL";
        for (int i=0;i<routeNode.size();i++) {
            // std::cout << routeNode[i] << std::endl;
            if (routeNode[i] == startNodeName) {
                for (int j=i, cnt = 0; cnt<routeNode.size(); cnt++, j=(j+1)%routeNode.size()) {
                    std::cout << routeNode[j];
                    if (lstLine == "NULL") std::cout<<" [take " << routeLine[j] <<"]";
                    else if (lstLine != routeLine[j]) std::cout<<" [change   " << routeLine[j] <<"]";
                    std::cout << std::endl;
                    lstLine = routeLine[j];
                }
                std::cout << startNodeName << std::endl;
                return {routeNode, routeLine};
            }
        }
        throw std::runtime_error("UNKNOWN ERROR");
    }

};

