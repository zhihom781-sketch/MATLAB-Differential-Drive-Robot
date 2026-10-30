#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <set>
#include <vector>
#include <utility>
#include <iostream>

class Map {
private:
    long long unsigned int width;
    long long unsigned int height;
    std::vector<std::vector<int>> grid; // 0 empty, 1 obstacle, 2 survivor, 3 visited
    std::set<std::pair<int,int>> obstacles;
    std::set<std::pair<int,int>> survivors;

    void generateObstacles(int numObstacles);
    void generateSurvivors(int numSurvivors);

public:
    Map(int numObstacles, int numSurvivors);

    bool   isOccupied(int x, int y);
    bool   isSurvivor(int x, int y);
    int    getCell(double x, double y);
    int    getSurvivorCount();
    double getWidth();
    double getHeight();
    void   setCell(double x, double y, int value);
    void   rescueSurvivorAt(int x, int y);
    void   updateSurvivorCount(int target);

    // For plotting
    std::vector<std::pair<int,int>> getSurvivorsList() const;
    std::vector<std::pair<int,int>> getObstaclesList() const;
};


#endif // MAP_H_INCLUDED
