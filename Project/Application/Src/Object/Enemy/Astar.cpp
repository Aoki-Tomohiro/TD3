#include "Astar.h"

bool isValid(int x, int y, int width, int height) {
    //進行先がマップ内かどうか
    return (x >= 0 && x < width && y >= 0 && y < height);
}

int calcH(int x, int y, int targetX, int targetY) {
    // マンハッタン距離を使ったヒューリスティック関数
    return std::abs(x - targetX) + std::abs(y - targetY);
}

std::vector<Node*> findPath(const std::vector<std::vector<int>>& map, int startX, int startY, int targetX, int targetY) {
    //マップの幅
    int width = (int)map[0].size();
    int height = (int)map.size();
    //対象のノードに訪れたかのフラグ
    std::vector<std::vector<bool>> visited(width, std::vector<bool>(height, false));
    std::priority_queue<Node*, std::vector<Node*>, Astar> openList;
    //スタート位置
    Node* startNode = new Node(startX, startY, 0, calcH(startX, startY, targetX, targetY), nullptr);
    //オープンリストの最後にstartNodeの値を追加する
    openList.push(startNode);

    while (!openList.empty()/*openListの要素数が0じゃないとき*/) {
        //currentにopenlistの先頭の値を入れる
        Node* current = openList.top();
        //先頭の値を削除する
        openList.pop();

        if (current->x == targetX && current->y == targetY) {
            // 目的地に到達した場合、パスを構築して返す
            std::vector<Node*> path;
            while (current != nullptr) {
                //パスの最後にcurrentPathを追加する
                path.push_back(current);
                current = current->parent;
            }
            //pathの値を反転して返す
            std::reverse(path.begin(), path.end());
      
            return path;
        }



        // 上下左右の移動
        int dx[] = { 0, 0, 1, -1 };
        int dy[] = { 1, -1, 0, 0 };
        //iが1の時は下,2の時は上,3の時は左,4の時は右を調べる
        for (int i = 0; i < 4; i++) {
            int nextX = current->x + dx[i];
            int nextY = current->y + dy[i];
            /*
            if (isValid(nextX, nextY, width, height) && visited[nextX][nextY] <= 10/*対象のマスが移動可能で且つまだそのノードを調べていないとき) {
                if (map[nextY][nextX] != 9 && map[nextY][nextX] != 10/*何もない地面ならば) {
                    //スタートからの距離にそのマス分のコストをプラスする
                    int g = current->g + 1;
                    //ゴールまでの予想距離を計算する
                    int h = calcH(nextX, nextY, targetX, targetY);
                    //次に調べるノードを新たに作成する
                    Node* nextNode = new Node(nextX, nextY, g, h, current);
                    //パスの最後にnextNodeを追加する
                    openList.push(nextNode);
                    //すでにそのノードを訪れた
                    visited[nextX][nextY] = true;
                }
               
            }*/

            
            if (isValid(nextX, nextY, width, height)) {
               
                if (map[nextY][nextX] != 9 &&map[nextY][nextX] != 10/*何もない地面ならば*/) {
                    //スタートからの距離にそのマス分のコストをプラスする
                    int g = current->g + 1;
                    //ゴールまでの予想距離を計算する
                    int h = calcH(nextX, nextY, targetX, targetY);
                    //次に調べるノードを新たに作成する
                    Node* nextNode = new Node(nextX, nextY, g, h, current);
                    //パスの最後にnextNodeを追加する
                    openList.push(nextNode);
                    //すでにそのノードを訪れた
                    visited[nextX][nextY] = true;
                }
            }
        }
    }

    return std::vector<Node*>();
}