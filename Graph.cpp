#include "Graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <tuple>
#include <queue>
#include <stack>
#include <algorithm>
using namespace std;


vector<string> stringvector;
int numnodes;
int numedges;
vector<tuple<string, string, double>> tuples;
vector<Node*> nodevector;

void editNode(Node* n, int index, bool start) {
    if(start) {
        n->edges.push_back(make_pair(get<1>(tuples[index]),get<2>(tuples[index])));
    }
    else {
        n->edges.push_back(make_pair(get<0>(tuples[index]),get<2>(tuples[index])));
    }
}

Graph::Graph(const char* const & edgelist_csv_fn) {
    numnodes = 0;
    numedges = 0;

    // parse file and store tuples in tuples vector
    ifstream instream(edgelist_csv_fn);
    string nextline;                     
        while(getline(instream, nextline)) {  
        istringstream ss(nextline);      
        string start, end, weight;
        getline(ss, start, ',');     
        getline(ss, end, ',');    
        getline(ss, weight, '\n');
        double w = stod(weight);   
        tuples.push_back(make_tuple(start, end, w));
    }
    //end of reading file
    instream.close(); 
    numedges = tuples.size();

    for(int i=0; i<numedges; i++) {
        bool nodeexists = false;
        for(int j=0; j<stringvector.size(); j++) {
            if(get<0>(tuples[i]) == stringvector[j] ) {
                nodeexists = true;
                editNode(nodevector[j], i, true);
            }
        }
        // if node doesn't exist already, create new node
        if(!nodeexists) {
            string symbol = get<0>(tuples[i]);
            Node* n = new Node(symbol);
            stringvector.push_back(symbol);
            nodevector.push_back(n);
            editNode(n, i, true);
        }
        nodeexists = false;

        for(int j=0; j<stringvector.size(); j++) {
            if(get<1>(tuples[i]) == stringvector[j] ) {
                nodeexists = true;
                editNode(nodevector[j], i, false);
            }
        }

        if(!nodeexists) {
            string symbol = get<1>(tuples[i]);
            Node* n = new Node(symbol);
            stringvector.push_back(symbol);
            nodevector.push_back(n);
            editNode(n, i, false);
        }
        
    }
    numnodes = nodevector.size();
}

Graph::~Graph() {
    stringvector.clear();
    tuples.clear();
    for(int i=0; i<nodevector.size(); i++) {
        nodevector[i]->edges.clear();
        delete nodevector[i];
    }
}

unsigned int Graph::num_nodes() {
    return numnodes;
}

vector<string> Graph::nodes() {
    return stringvector;
}

unsigned int Graph::num_edges() {
    return numedges;
}

unsigned int Graph::num_neighbors(string const & node_label) {
    for(int i=0; i<numnodes; i++) {
        if(node_label == stringvector[i]) {
            Node* n = nodevector[i];
            return n->edges.size();
        }
    }
}

double Graph::edge_weight(string const & u_label, string const & v_label) {
    Node* n;
    for(int i=0; i<numnodes; i++) {
        if(u_label == stringvector[i]) {
            n = nodevector[i];
            break;
        }
    }
    for(int j=0; j<n->edges.size(); j++) {
        pair<string, double> p = n->edges[j];
        if(v_label == get<0>(p)) {
            return get<1>(p);
        }
    }
    return -1;
}

vector<string> Graph::neighbors(string const & node_label) {
    vector<string> neighbornodes;
    Node* n;
    for(int i=0; i<numnodes; i++) {
        if(node_label == stringvector[i]) {
            n = nodevector[i];
        }
    }
    for(int j=0; j<n->edges.size(); j++) {
        pair<string, double> p = n->edges[j];
        neighbornodes.push_back(get<0>(p));
    }
    return neighbornodes;
}

int BFS(Node* n, string end) {
    queue<pair<int, Node*>> q;
    q.push(make_pair(0,n));
    n->length = 0;
    while(!q.empty()) {
        pair<int, Node*> p = q.front();
        q.pop();
        int length = get<0>(p);
        Node* temp = get<1>(p);
        temp->visited = true;
        if(temp->symbol == end) {
            return length;
        }
        for(int i=0; i<temp->edges.size(); i++) {
            pair<string, double> e = temp->edges[i];
            string symbol = get<0>(e);
            for(int i=0; i<nodevector.size(); i++) {
                if(nodevector[i]->symbol == symbol && nodevector[i]->length == -1) {
                    if(!nodevector[i]->visited) {
                        nodevector[i]->length = length + 1;
                        q.push(make_pair(length + 1, nodevector[i]));
                        break;
                    }
                }
            }
        }
    }
    return -1;
}

vector<string> Graph::shortest_path_unweighted(string const & start_label, string const & end_label) {
    vector<string> unweighted;
    if(start_label == end_label) {
        unweighted.push_back(start_label);
        return unweighted;
    }
    Node* s;
    for(int i=0; i<nodevector.size(); i++) {
        nodevector[i]->visited = false;
        nodevector[i]->length = -1;
        if(start_label == nodevector[i]->symbol) {
            s = nodevector[i];
        }
    }
    int distance = BFS(s, end_label);
    if(distance == -1) {
        return unweighted;
    }
    Node* next;
    Node* prev;
    for(int i=0; i<nodevector.size(); i++) {
        if(nodevector[i]->symbol == end_label) {
            next = nodevector[i];
            break;
        }
    }
    unweighted.push_back(end_label);
    distance--;
    while(distance >= 0) {
        for(int i=0; i<next->edges.size(); i++) {
            pair<string, double> p = next->edges[i];
            for(int j=0; j<nodevector.size(); j++) {
                if(get<0>(p) == nodevector[j]->symbol) {
                    prev = nodevector[j];
                    break;
                }
            }
            if(prev->length == distance) {
                unweighted.push_back(prev->symbol);
                next = prev;
                break;
            }
        }
        distance--;
    }
    reverse(unweighted.begin(), unweighted.end());
    return unweighted;
}

Node* getNode(string symbol) {
    for(int i=0; i<nodevector.size(); i++) {
        if(nodevector[i]->symbol == symbol) {
            return nodevector[i];
        }
    }
}

void dijkstra(Node* start) {
    priority_queue<pair<double, Node*>, vector<pair<double, Node*>>, pairComp> pq;
    pq.push(make_pair(0, start));
    start->distance = 0;

    while(!pq.empty()) {
        pair<double, Node*> p = pq.top();
        pq.pop();
        Node* temp = get<1>(p);
        double dist = get<0>(p);
        if(!temp->visited) {
            temp->visited = true;
            for(int i=0; i<temp->edges.size(); i++) {
                pair<string, double> p = temp->edges[i];
                Node* edge = getNode(get<0>(p));
                double edgel = get<1>(p);
                double c = dist + edgel;
                if(c < edge->distance) {
                    edge->prev = temp;
                    edge->distance = c;
                    pq.push(make_pair(c, edge));
                }
            }
        }
    }
}

vector<tuple<string,string,double>> Graph::shortest_path_weighted(string const & start_label, string const & end_label) {
    vector<tuple<string,string,double>> weighted;
    if(start_label == end_label) {
        weighted.push_back(make_tuple(start_label, end_label, -1));
        return weighted;
    }
    Node* s = getNode(start_label);

    for(int i=0; i<nodevector.size(); i++) {
        nodevector[i]->distance = 100000000;
        nodevector[i]->prev = nullptr;
        nodevector[i]->visited = false;
    } 
    
    dijkstra(s);
    Node* next = getNode(end_label);
    if(next->prev == nullptr) {
       return weighted;
    }

    Node* previous = next->prev;
    while(next->prev != nullptr) {
        for(int i=0; i<tuples.size(); i++) {
            tuple<string, string, double> t = tuples[i];
            if(get<0>(t) == previous->symbol && get<1>(t) == next->symbol) {
                weighted.push_back(t);
                break;
            }
            else if(get<1>(t) == previous->symbol && get<0>(t) == next->symbol) {
                tuple<string, string, double> r = make_tuple(get<1>(t), get<0>(t), get<2>(t));
                weighted.push_back(r);
                break;
            }
        }
        next = next->prev;
        previous = previous->prev;
    }
    reverse(weighted.begin(), weighted.end());
    return weighted;
}

vector<string> connBFS(Node* n, double limit) {
    vector<string> set;
    queue<Node*> q;
    q.push(n);
    set.push_back(n->symbol);
    while(!q.empty()) {
        Node* temp = q.front();
        q.pop();
        temp->visited = true;
        for(int i=0; i<temp->edges.size(); i++) {
            pair<string, double> p = temp->edges[i];
            double length = get<1>(p);
            string symbol = get<0>(p);
            if(length <= limit) {
                for(int j=0; j<nodevector.size(); j++) {
                    if(nodevector[j]->symbol == symbol) {
                            if(!nodevector[j]->visited) {
                                set.push_back(symbol);
                                q.push(nodevector[j]);
                                break;
                            }
                    }
                }
            }
        }
    }
    return set;
}

vector<vector<string>> Graph::connected_components(double const & threshold) {
    for(int i=0; i<nodevector.size(); i++) {
        nodevector[i]->visited = false;
    }

    vector<vector<string>> connected;
    for(int i=0; i<nodevector.size(); i++) {
        if(!nodevector[i]->visited) {
            vector<string> set = connBFS(nodevector[i], threshold);
            connected.push_back(set);
        }
    }

    return connected;
}



Node* find(string symbol, vector<int> & parents) {
    Node* n;
    int index;
    for(int i=0; i<nodevector.size(); i++) {
        if(symbol == nodevector[i]->symbol) {
            index = i;
            n = nodevector[i];
        }
    }
    if(parents[index] == -1) {
        return n;
    }
    while(parents[index] != -1) {
        index = parents[index];
    }
    n = nodevector[index];
    return n;
}

void unionF(string s, string e, vector<int> & parents) {
    Node* sp = find(s, parents);
    Node* ep = find(e, parents);
    int sindex, eindex;
    if(sp == ep) {
        return;
    }
    for(int i=0; i<nodevector.size(); i++) {
        if(sp == nodevector[i]) {
            sindex = i;
        }
        if(ep == nodevector[i]) {
            eindex = i;
        }
    }
    
    parents[sindex] = eindex;

}

double Graph::smallest_connecting_threshold(string const & start_label, string const & end_label) {
    if(start_label == end_label) {
        return 0;
    }
    priority_queue<tuple<string, string, double>, vector<tuple<string, string, double>>, tupleComp> pq;
    for(int i=0; i<tuples.size(); i++) {
        pq.push(tuples[i]);
    }
    vector<int> parents;
    for(int i=0; i<nodevector.size(); i++) {
        parents.push_back(-1);
    }
    while(!pq.empty()) {
        tuple<string, string, double> t = pq.top();
        pq.pop();
        string start = get<0>(t);
        string end = get<1>(t);
        double edge = get<2>(t);
        
        unionF(start, end, parents);
        if(find(start_label, parents) == find(end_label, parents)) {
            while(!pq.empty()) {
                pq.pop();
            }
            return edge;
        } 
    }

    return -1;

}
