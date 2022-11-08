#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

constexpr double kInf = std::numeric_limits<double>::infinity();

class DisjointSet {
    public:
        DisjointSet(int n = 0) : pNumberOfSets(n) {
            pTree.reserve(n);
            for (int i = 0; i < n; ++i)
                pTree.push_back({i, 1});
        }

        int MakeSet() {
            int newSet = pTree.size();
            pTree.push_back({newSet, 1});
            ++pNumberOfSets;
            return newSet;
        }

        int Find(int x) const {
            int parent;
            // path splitting
            while ((parent=pTree[x].parent) != x) {
#ifdef OPTIMIZE_DJS
                pTree[x].parent = pTree[parent].parent;
#endif
                x = parent;
            }
            return parent;
        }

        bool Join(int x, int y) {
            x = Find(x);
            y = Find(y);

            if (x == y)
                return false;

#ifdef OPTIMIZE_DJS
            if (pTree[x].size < pTree[y].size)
                std::swap(x, y);
#endif

            pTree[y].parent = x;
            pTree[x].size += pTree[y].size;
            --pNumberOfSets;

            return true;
        }

        int SetSize(int x) const {
            x = Find(x);
            return pTree[x].size;
        }

        int NumberOfSets() const {
            return pNumberOfSets;
        }

        size_t NumberOfItems() const {
            return pTree.size();
        }

        void PrettyPrint(std::ostream& out) const {
            std::vector<std::vector<int>> children(pTree.size());
            std::vector<int> sets;
            for (int i = 0; i < (int)pTree.size(); ++i) {
                if (pTree[i].parent == i)
                    sets.push_back(i);
                else
                    children[pTree[i].parent].push_back(i);
            }
            for (int set : sets) {
                PrettyPrint(out, children, set);
            }
        }

    private:

        struct Node {
            int parent, size;
        };

        void PrettyPrint(std::ostream& out,
                         std::vector<std::vector<int>> children,
                         int set,
                         int indent = 0) const {
            for (int count = 0; count < indent; ++count)
                out << ' ';
            out << set << '\n';
            for (int child : children[set])
                PrettyPrint(out, children, child, indent+4);
        }

#ifdef OPTIMIZE_DJS
        mutable
#endif
        std::vector<Node> pTree;

        size_t pNumberOfSets;

};


class Dendrogram {
    public:
        Dendrogram(size_t numberOfLeaves) :
            pClusters(numberOfLeaves),
            pNumberOfLeaves(numberOfLeaves)
        {
            pClusters.reserve(2*numberOfLeaves-1);
            for (size_t i = 0; i < numberOfLeaves; ++i) {
                pClusters[i].topLevelAncestor = i;
                pClusters[i].size = 1;
            }
        }

        size_t Find(size_t clusterIndex) const {
            size_t& topLevelAncestor = pClusters[clusterIndex].topLevelAncestor; 
            if (topLevelAncestor != clusterIndex)
                topLevelAncestor = Find(topLevelAncestor);
            return topLevelAncestor;
        }

        bool IsLeaf(size_t clusterIndex) const {
            return clusterIndex < pNumberOfLeaves;
        }

        bool IsTopLevel(size_t clusterIndex) const {
            return pClusters[clusterIndex].topLevelAncestor == clusterIndex;
        }

        size_t GetParent(size_t clusterIndex) const {
            return pClusters[clusterIndex].parent;
        }

        std::pair<size_t,size_t> GetChildren(size_t clusterIndex) const {
            return {pClusters[clusterIndex].leftChild, pClusters[clusterIndex].rightChild};
        }

        size_t GetClusterSize(size_t clusterIndex) const {
            return pClusters[clusterIndex].size;
        }

        std::pair<bool,size_t> Join(size_t clusterIndex1, size_t clusterIndex2) {
            clusterIndex1 = Find(clusterIndex1);
            clusterIndex2 = Find(clusterIndex2);

            if (clusterIndex1 == clusterIndex2)
                return {false, clusterIndex1};

            size_t newClusterIndex = NewCluster(clusterIndex1, clusterIndex2);
            return {true, newClusterIndex};
        }

        size_t GetNumberOfLeaves() const {
            return pNumberOfLeaves;
        }

        size_t GetNumberOfClusters() const {
            return pClusters.size();
        }

        size_t GetNumberOfTopLevelClusters() const {
            return 2*pNumberOfLeaves - pClusters.size();
        }

        std::vector<size_t> GetTopLevelClusters() const {
            std::vector<size_t> topLevelClusters;
            topLevelClusters.reserve(GetNumberOfTopLevelClusters());
            for (size_t i = 0; i < pClusters.size(); ++i) {
                if (IsTopLevel(i))
                    topLevelClusters.push_back(i);
            }
            return topLevelClusters;
        }

        void PrettyPrint(std::ostream& out) const {
            for (size_t clusterIndex : GetTopLevelClusters())
                PrettyPrint(out, clusterIndex);
            out << "Number of clusters: " << pClusters.size() << std::endl
                << "Number of top level clusters: " << GetNumberOfTopLevelClusters() << std::endl;
        }

    private:
        struct Cluster {
            mutable size_t topLevelAncestor;
            size_t parent, leftChild, rightChild, size;
        };

        void PrettyPrint(std::ostream& out, size_t clusterIndex, size_t indent=0) const {
            for (size_t i = 0; i < indent; ++i)
                out << ' ';
            out << clusterIndex;
            if (!IsTopLevel(clusterIndex))
                out << " (topLevelAncestor: " << Find(clusterIndex) << ')';
            out << ", size: " << GetClusterSize(clusterIndex) << '\n';
            if (!IsLeaf(clusterIndex)) {
                auto[leftChild,rightChild] = GetChildren(clusterIndex);
                PrettyPrint(out, leftChild, indent+2);
                PrettyPrint(out, rightChild, indent+2);
            }
        }

        size_t NewCluster(size_t leftChildIndex, size_t rightChildIndex) {
            Cluster& leftChild = pClusters[leftChildIndex];
            Cluster& rightChild = pClusters[rightChildIndex];

            size_t newClusterIndex = pClusters.size();

            pClusters.emplace_back();
            Cluster& newCluster = pClusters.back();

            newCluster.topLevelAncestor = newClusterIndex;
            newCluster.leftChild        = leftChildIndex;
            newCluster.rightChild       = rightChildIndex;
            newCluster.size             = leftChild.size + rightChild.size;
            
            leftChild.topLevelAncestor  = rightChild.parent = newClusterIndex;
            rightChild.topLevelAncestor = rightChild.parent = newClusterIndex;

            return newClusterIndex;
        }

        std::vector<Cluster> pClusters;
        size_t pNumberOfLeaves;
};

class Matrix {
    public:
        Matrix(size_t width, size_t height, double value = 0)
            : pData(width*height, value), pWidth(width), pHeight(height) {}

        double operator()(size_t i, size_t j) const {
            return pData[i*pWidth+j];
        }

        double& operator()(size_t i, size_t j) {
            return pData[i*pWidth+j];
        }

        size_t Width() const {
            return pWidth;
        }

        size_t Height() const {
            return pHeight;
        }

    private:
        std::vector<double> pData;
        size_t pWidth, pHeight;
};

typedef std::tuple<double,int,int> Edge;

std::vector<Edge> MinimumSpanningTree(const Matrix& distances) {
    int numNodes = distances.Width();

    std::vector<Edge> edges(numNodes-1, Edge(kInf,-1,-1));
    std::vector<bool> inTree(numNodes);
    std::vector<int> remainingNodes(numNodes);
    std::iota(remainingNodes.begin(), remainingNodes.end(), 0);

    int closestNodeIndex = 0;
    int closestNode = remainingNodes[closestNodeIndex];
    do {
        std::swap(remainingNodes[closestNodeIndex], remainingNodes.back());
        remainingNodes.pop_back();
        inTree[closestNode] = true;

        for (int to = 1; to < numNodes; ++to) {
            if (inTree[to] || to == closestNode)
                continue;
            if (distances(closestNode,to) < std::get<0>(edges[to-1]))
                edges[to-1] = Edge(distances(closestNode,to), closestNode, to);
        }

        closestNodeIndex = 0;
        closestNode = remainingNodes[closestNodeIndex];
        double minimumDistance = std::get<0>(edges[closestNode-1]);
        for (size_t index = 1; index < remainingNodes.size(); ++index) {
            int node = remainingNodes[index];
            double distance = std::get<0>(edges[node-1]);
            if (distance < minimumDistance) {
                closestNodeIndex = index;
                closestNode = node;
                minimumDistance = distance;
            }
        }
    } while (!remainingNodes.empty());

    return edges;
}

//std::vector<Edge> SingleLinkage(const Matrix& distances) {
//    auto tree = MinimumSpanningTree(distances);
//    std::sort(tree.begin(), tree.end());
//
//}

/*
int main() {
    std::cout << std::boolalpha;
    DisjointSet djs;
    std::string cmd;
    while (std::cin >> cmd && cmd != "Exit") {
        if (cmd == "MakeSet") {
            std::cout << djs.MakeSet() << std::endl;
        }
        else if (cmd == "Find") {
            int arg;
            std::cin >> arg;
            std::cout << djs.Find(arg) << std::endl;
        }
        else if (cmd == "Join") {
            int arg1, arg2;
            std::cin >> arg1 >> arg2;
            std::cout << djs.Join(arg1, arg2) << std::endl;
        }
        else if (cmd == "SetSize") {
            int arg;
            std::cin >> arg;
            std::cout << djs.SetSize(arg) << std::endl;
        }
        else if (cmd == "NumberOfSets") {
            std::cout << djs.NumberOfSets() << std::endl;
        }
        else if (cmd == "NumberOfItems") {
            std::cout << djs.NumberOfItems() << std::endl;
        }
        else if (cmd == "PrettyPrint") {
            djs.PrettyPrint(std::cout);
        }
        else {
            std::cout << "Unknown command " << cmd << std::endl;
        }
        std::cout << "---" << std::endl;
    }
}
*/


int main() {
    std::cout << std::boolalpha;
    Dendrogram den(10);
    den.PrettyPrint(std::cout);
    std::string cmd;
    while (std::cout << "---" << std::endl && std::cin >> cmd && cmd != "Exit") {
        if (cmd == "Find") {
            int arg;
            std::cin >> arg;
            std::cout << den.Find(arg) << std::endl;
        }
        else if (cmd == "Join") {
            int arg1, arg2;
            std::cin >> arg1 >> arg2;
            auto[success,newClusterIndex] = den.Join(arg1, arg2);
            std::cout << success << ' ' << newClusterIndex << std::endl;
        }
        den.PrettyPrint(std::cout);
    }
}

/*
typedef std::tuple<double,double> Point2d;

double distance(const Point2d& p1, const Point2d& p2) {
    using std::get;
    double dx = get<0>(p1) - get<0>(p2);
    double dy = get<1>(p1) - get<1>(p2);
    return std::sqrt(dx*dx + dy*dy);
}

Matrix ComputeDistanceMatrix(const std::vector<Point2d>& points) {
    Matrix distances(points.size(), points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            distances(i,j) = distances(j,i) = distance(points[i], points[j]);
        }
    }
    return distances;
}

int main() {
    std::vector<Point2d> points{
        {0, 0},
        {0, 1},
        {0, 4},
        {0, 5},
        {1, 0},
        {1, 5},
        {4, 0},
        {4, 5},
        {5, 0},
        {5, 1},
        {5, 4},
        {5, 5}
    };

    Matrix distances = ComputeDistanceMatrix(points);

    std::cout << distances.Width() << ' ' << distances.Height() << std::endl;

    auto mst = MinimumSpanningTree(distances);

    for (auto[distance,u,v] : mst) {
        std::cout << '(' << u << ", " << v << "): " << distance << std::endl;
    }


}
*/
