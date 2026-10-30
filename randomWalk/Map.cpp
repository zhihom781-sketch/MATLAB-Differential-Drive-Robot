#include "Map.h"
#include <random>
#include <format>

Map::Map(int numObstacles, int numSurvivors)
 : width{50}, height{50}, grid(height, std::vector<int>(width, 0)), obstacles{}, survivors{}
{
    generateObstacles(numObstacles);
    generateSurvivors(numSurvivors);
}


void Map::generateObstacles(int numObstacles) {
    std::random_device rd; std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distx(0, (int)width-1);
    std::uniform_int_distribution<int> disty(0, (int)height-1);

    int placed = 0;
    while (placed < numObstacles) {
        int x = distx(gen), y = disty(gen);
        if (grid[(size_t)y][(size_t)x] == 0) {
            obstacles.insert({x,y});
            grid[(size_t)y][(size_t)x] = 1;
            placed++;
        }
    }
}

void Map::generateSurvivors(int numSurvivors) {
    std::random_device rd; std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distx(0, (int)width-1);
    std::uniform_int_distribution<int> disty(0, (int)height-1);

    int placed = 0;
    while (placed < numSurvivors) {
        int x = distx(gen), y = disty(gen);
        if (grid[(size_t)y][(size_t)x] == 0) {
            survivors.insert({x,y});
            grid[(size_t)y][(size_t)x] = 2;
            placed++;
        }
    }
}

bool Map::isOccupied(int x, int y) {
    return obstacles.count({x,y}) > 0 || survivors.count({x,y}) > 0;
}

bool Map::isSurvivor(int x, int y) {
    return grid[(size_t)y][(size_t)x] == 2;
}

void Map::rescueSurvivorAt(int x, int y) {
    if (survivors.erase({x, y}) > 0) {
        grid[(size_t)y][(size_t)x] = 0;
        std::cout <<std::format("Rescued survivor at {}, {}\n", x, y);
        std::cout<<std::format("Survivor count : {}\n\n", survivors.size());
    }
}

int Map::getCell(double x, double y) {
    int gx = (int)x, gy = (int)y;
    int w = (int)width, h = (int)height;
    if (gx < 0) gx = 0;
    if (gx >= w) gx = w-1;
    if (gy < 0) gy = 0;
    if (gy >= h) gy = h-1;
    return grid[(size_t)gy][(size_t)gx];
}

int    Map::getSurvivorCount() { return (int)survivors.size(); }
double Map::getWidth()  { return (double)width; }
double Map::getHeight() { return (double)height; }

void Map::setCell(double x, double y, int value) {
    int gx = (int)x, gy = (int)y;
    int w = (int)width, h = (int)height;
    if (gx < 0) gx = 0;
    if (gx >= w) gx = w-1;
    if (gy < 0) gy = 0;
    if (gy >= h) gy = h-1;
    grid[(size_t)gy][(size_t)gx] = value;
}

void Map::updateSurvivorCount(int target) {
    int current = (int)survivors.size();
    if (target > current) {
        generateSurvivors(target - current);
    } else if (target < current) {
        int toRemove = current - target;
        auto it = survivors.begin();
        while (toRemove > 0 && !survivors.empty()) {
            it = survivors.erase(it);
            --toRemove;
            if (it == survivors.end()) it = survivors.begin();
        }
    }
}

std::vector<std::pair<int,int>> Map::getSurvivorsList() const {
    return {survivors.begin(), survivors.end()};
}
std::vector<std::pair<int,int>> Map::getObstaclesList() const {
    return {obstacles.begin(), obstacles.end()};
}
