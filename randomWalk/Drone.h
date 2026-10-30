#ifndef DRONE_H_INCLUDED
#define DRONE_H_INCLUDED


#include <vector>
#include "Map.h"

class Drone {
private:
    int    droneID;
    double x;
    double y;
    double battery;
    double speed;
    int    scanRadius;
    bool   isActive;
    bool   foundSurvivor;

public:
    Drone(int id, double oriX, double oriY, double oriSpeed);

    void updatePosition2(Map& env, const std::vector<Drone>& swarm);
    void updateBattery();
    bool detectSurvivor(double survivorX, double survivorY);
    void scanForSurvivors(Map& env);

    bool getStatus() const { return isActive; }
    std::vector<double> getCoordinates() const;
    int  getDroneID() const;
};


#endif // DRONE_H_INCLUDED
