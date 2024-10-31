#include <vector>
#include <tuple>

using namespace std;

vector<vector<int>> createSkeleton(vector<vector<int>>& pixelMatrix);
vector<tuple<int, int>> getMarkedPixelsPhaseOne(vector<vector<int>>& pixelMatrix);
vector<tuple<int, int>> getMarkedPixelsPhaseTwo(vector<vector<int>>& pixelMatrix);
void updatePixels(vector<vector<int>>& pixelMatrix, vector<tuple<int, int>>& markedPixels);
vector<int> getNeighbors(vector<vector<int>>& pixelMatrix, int row, int col);
bool isValidNeighbor(vector<vector<int>>& pixelMatrix, int r, int c, int row, int col);
bool satisfiesPhaseOneConditions(vector<vector<int>>& pixelMatrix, int row, int col);
bool satisfiesPhaseTwoConditions(vector<vector<int>>& pixelMatrix, int row, int col);
