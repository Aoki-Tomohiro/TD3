#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>

struct Node {
    int x, y;
    //gがスタートからの移動距離,hがゴールまでの推定距離(ヒューリスティックコスト)
    int g, h;
    //親ノード
    Node* parent;

    Node(int x_, int y_, int g_, int h_, Node* parent_) :
        x(x_), y(y_), g(g_), h(h_), parent(parent_) {}

    int f() const {
        //スタートからの距離とゴールまでの推定距離から求める
        return g + h;
    }
};
class Astar
{
public:
    bool operator()(const Node* n1, const Node* n2) const {
        return n1->f() > n2->f();
    }
};

bool isValid(int x, int y, int width, int height);

int calcH(int x, int y, int targetX, int targetY);
//経路を求める関数                                            スタートからの距離　スタート地点X、Y　　　　　　目的地X、Y
std::vector<Node*> findPath(const std::vector<std::vector<int>>& grid, int startX, int startY, int targetX, int targetY);