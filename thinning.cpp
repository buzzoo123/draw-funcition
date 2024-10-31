#include "thinning.h"
#include <vector>
#include <tuple>

using namespace std;

//Implement Zhang-Suen Algorithm
vector<vector<int>> createSkeleton(vector<vector<int>>& pixelMatrix)    {
    bool changesMade = true;
    vector<tuple<int,int>> markedPixels;
    while(changesMade)  {
        markedPixels = getMarkedPixelsPhaseOne(pixelMatrix);
        updatePixels(pixelMatrix, markedPixels);
        changesMade = markedPixels.size() > 0;
        markedPixels = getMarkedPixelsPhaseTwo(pixelMatrix);
        updatePixels(pixelMatrix, markedPixels);
        changesMade = changesMade || markedPixels.size() > 0;
    }
    return pixelMatrix;    
}

vector<tuple<int,int>> getMarkedPixelsPhaseOne(vector<vector<int>>& pixelMatrix)    {
    vector<tuple<int,int>> markedPixels;
    for(size_t r = 0; r < pixelMatrix.size(); ++r) {
        for(size_t c = 0; c < pixelMatrix[0].size(); ++c)  {
            if(satisfiesPhaseOneConditions(pixelMatrix, r, c)) {
                markedPixels.push_back(make_tuple(r, c));
            }
        }
    }

    return markedPixels; 
}

vector<tuple<int,int>> getMarkedPixelsPhaseTwo(vector<vector<int>>& pixelMatrix)    {
    vector<tuple<int,int>> markedPixels;
    for(size_t r = 0; r < pixelMatrix.size(); ++r) {
        for(size_t c = 0; c < pixelMatrix[0].size(); ++c)  {
            if(satisfiesPhaseTwoConditions(pixelMatrix, r, c)) {
                markedPixels.push_back(make_tuple(r, c));
            }
        }
    }

    return markedPixels; 
}

vector<int> getNeighbors(vector<vector<int>>& pixelMatrix, int row, int col) {
    vector<int> neighbors;

    // Define offsets for neighbors in clockwise order (P2 to P9)
    int rowOffsets[] = {-1, -1, 0, 1, 1, 1, 0, -1};  // Top, top-right, right, bottom-right, bottom, bottom-left, left, top-left
    int colOffsets[] = {0, 1, 1, 1, 0, -1, -1, -1};

    // Loop through each offset
    for (size_t i = 0; i < 8; ++i) {
        int neighborRow = row + rowOffsets[i];
        int neighborCol = col + colOffsets[i];
        
        if (isValidNeighbor(pixelMatrix, neighborRow, neighborCol, row, col)) {
            neighbors.push_back(pixelMatrix[neighborRow][neighborCol]);
        }
    }

    return neighbors;
}

bool isValidNeighbor(vector<vector<int>>& pixelMatrix, int r, int c, int row, int col) {
    if ((r == row && c == col) || (r < 0 || r >= static_cast<int>(pixelMatrix.size())) || (c < 0 || c >= static_cast<int>(pixelMatrix[0].size()))) {
        return false;
    }
    return true;
}


bool satisfiesPhaseOneConditions(vector<vector<int>>& pixelMatrix, int row, int col)  {
    //Pixel must be black to be deleted
    if(pixelMatrix[row][col] == 0)  {
        return false;
    }

    //Pixel must have 8 neighbors (not on a boundary)
    vector<int> neighbors = getNeighbors(pixelMatrix, row, col);
    if(neighbors.size() < 8) {
        return false;
    }

    //Must have between 2 and 6 black neighbors
    int blackCount = 0;
    for(int neighbor : neighbors)   {
        if(neighbor == 1)   {
            ++blackCount;
        }
    }
    if(blackCount < 2 || blackCount > 6)    {
        return false;
    }

    //Count transitions in a clockwise loop starting from top neighbor. Must be 1.
    int transitionCount = 0;
    for (size_t i = 0; i < neighbors.size(); ++i) {
        int curr = neighbors[i];
        int next = neighbors[(i + 1) % neighbors.size()]; // Wrap around
        if (curr == 0 && next == 1) {
            ++transitionCount;
        }
    }
    if (transitionCount != 1) {
        return false;
    }

    //At least one of P2 and P4 and P6 is white
    if(neighbors[0] != 0 && neighbors[2] != 0 && neighbors[4] != 0) {
        return false;
    }

    //At least one of P4 and P6 and P8 is white
    if(neighbors[2] != 0 && neighbors[4] != 0 && neighbors[6] != 0) {
        return false;
    }
    return true;
}

bool satisfiesPhaseTwoConditions(vector<vector<int>>& pixelMatrix, int row, int col)  {
    //Pixel must be black to be deleted
    if(pixelMatrix[row][col] == 0)  {
        return false;
    }

    //Pixel must have 8 neighbors (not on a boundary)
    vector<int> neighbors = getNeighbors(pixelMatrix, row, col);
    if(neighbors.size() < 8) {
        return false;
    }

    //Must have between 2 and 6 black neighbors
    int blackCount = 0;
    for(int neighbor : neighbors)   {
        if(neighbor == 1)   {
            ++blackCount;
        }
    }
    if(blackCount < 2 || blackCount > 6)    {
        return false;
    }

    //Count transitions in a clockwise loop starting from top neighbor. Must be 1.
    int transitionCount = 0;
    for (size_t i = 0; i < neighbors.size(); ++i) {
        int curr = neighbors[i];
        int next = neighbors[(i + 1) % neighbors.size()];
        if (curr == 0 && next == 1) {
            ++transitionCount;
        }
    }

    if (transitionCount != 1) {
        return false;
    }

    //At least one of P2 and P4 and P8 is white
    if(neighbors[0] != 0 && neighbors[2] != 0 && neighbors[6] != 0) {
        return false;
    }

    //At least one of P2 and P6 and P8 is white
    if(neighbors[0] != 0 && neighbors[4] != 0 && neighbors[6] != 0) {
        return false;
    }
    
    return true;
}

//Update pixels based on markedPixels
void updatePixels(vector<vector<int>>& pixelMatrix, vector<tuple<int,int>>& markedPixels) {
    for(tuple<int,int> coord : markedPixels)    {
        pixelMatrix[get<0>(coord)][get<1>(coord)] = 0;
    }
}