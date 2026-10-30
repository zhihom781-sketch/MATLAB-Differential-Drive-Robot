#include "Drone.h"
#include <cmath>
#include <cstdlib>
#include <format>

Drone::Drone(int id, double oriX, double oriY, double oriSpeed)
: droneID(id), x(oriX), y(oriY), battery(100.0), speed(oriSpeed),
  scanRadius(1), isActive(true), foundSurvivor(false)
{}

int Drone::getDroneID() const { return droneID; }
std::vector<double> Drone::getCoordinates() const { return {x, y}; }

void Drone::updatePosition2(Map& env, const std::vector<Drone>& swarm){
    if (!isActive || battery <= 0.0) return;

    double bestX = x, bestY = y;

    // Repulsion from other drones
    double repX = 0.0, repY = 0.0;
    for (const Drone& other : swarm){
        if (other.droneID == this->droneID) continue;
        double dx = x - other.x, dy = y - other.y;
        double distSq = dx*dx + dy*dy + 0.01;
        repX += dx / distSq; repY += dy / distSq;
    }
    double repLen = std::sqrt(repX*repX + repY*repY);
    if (repLen > 0.0) { repX /= repLen; repY /= repLen; }

    // Biased random walk
    for (int i = 0; i < 5; i++){
        double angleRad = (rand() % 360) * 3.14159265358979323846 / 180.0;
        double tx = x + speed * std::cos(angleRad) + 0.5 * repX;
        double ty = y + speed * std::sin(angleRad) + 0.5 * repY;

        if (tx < 0.0) tx = 0.0;
        if (ty < 0.0) ty = 0.0;
        if (tx >= env.getWidth())  tx = env.getWidth()  - 1.0;
        if (ty >= env.getHeight()) ty = env.getHeight() - 1.0;

        int cellVal = env.getCell(tx, ty);
        if (cellVal == 1) continue;                   // obstacle
        if (cellVal == 2) env.rescueSurvivorAt((int)tx, (int)ty);  // rescue immediately

        if (cellVal == 0) { bestX = tx; bestY = ty; break; }       // prefer unvisited
        bestX = tx; bestY = ty;
    }

    if (bestX != x || bestY != y) {
        env.setCell(bestX, bestY, 3);
        updateBattery();
    }
    x = bestX; y = bestY;
}

void Drone::updateBattery(){
    if (!isActive) return;
    battery -= 0.1 * speed;
    if (battery <= 0.0) {
        battery = 0.0;
        std::cout<<std::format("Drone {} battery is drained\n", droneID);
        isActive = false; }
}

bool Drone::detectSurvivor(double sx, double sy){
    if (!isActive) return false;
    double dx = x - sx, dy = y - sy;
    double dist = std::sqrt(dx*dx + dy*dy);
    return (dist <= scanRadius);
}

void Drone::scanForSurvivors(Map& env){
    int cx = (int)x, cy = (int)y;
    for (int dx = -scanRadius; dx <= scanRadius; ++dx){
        int nx = cx + dx;
        if (nx >= env.getWidth() || nx < 0) continue;
        for (int dy = -scanRadius; dy <= scanRadius; ++dy){
            int ny = cy + dy;
            if (ny >= env.getHeight() || ny < 0) continue;
            if (env.isSurvivor(nx, ny)) env.rescueSurvivorAt(nx, ny);
        }
    }
}
