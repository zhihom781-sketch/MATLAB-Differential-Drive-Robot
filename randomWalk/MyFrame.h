#ifndef MYFRAME_H_INCLUDED
#define MYFRAME_H_INCLUDED

#include <wx/wx.h>
#include <wx/timer.h>
#include <wx/slider.h>
#include <cstdio>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "Map.h"
#include "Drone.h"

class MyFrame : public wxFrame {
public:
    MyFrame();
    ~MyFrame();

private:
    // ----  UI  ----
    wxSlider*   sldDrones_     = nullptr;
    wxSlider*   sldSurvivors_  = nullptr;
    wxButton*   btnSimulate_   = nullptr;
    wxButton*   btnEnd_        = nullptr;

    void OnDroneSlider(wxCommandEvent&);
    void OnSurvivorSlider(wxCommandEvent&);
    void OnSimulate(wxCommandEvent&);
    void OnEnd(wxCommandEvent&);
    void OnClose(wxCloseEvent&);

    // ---- Simulation model ----
    Map world_{10, 10};
    std::vector<Drone> swarm_;
    int  timestep_ = 0;
    bool running_  = false;

    // survivor draw cache
    std::unordered_set<int> drawnSurvivors_;

    // trajectory support
    std::unordered_map<int, std::pair<double,double>> prevPos_;
    int nextArrowId_ = 30000;

    // desired parameters from UI
    int desiredDrones_    = 5;
    int desiredSurvivors_ = 10;

    // Map dimensions now come from internal defaults (change here if you like)
    int desiredMapW_      = 50;   // <- change default map width here
    int desiredMapH_      = 50;   // <- change default map height here

    // ---- Timer ----
    wxTimer simTimer_{this};
    void StartSimulation();
    void StopSimulation();
    void OnSimTick(wxTimerEvent&);

    // ---- gnuplot helpers ----
    FILE* gp_ = nullptr;
    void   GpOpen();
    void   GpClose();
    void   GpClearAll();                 // unset all objects/arrows, base plot again
    void   GpSetupRanges(int w, int h);  // sets range + base plot
    void   GpSetCircle(int id, double x, double y, double radius, const char* color, bool transparent=false);
    void   GpUnsetObject(int id);
    void   GpSetArrow(int id, double x1, double y1, double x2, double y2, const char* color="gray", double lw=1.0);
    void   GpReplot();

    // stable IDs
    static constexpr int ID_DRONE_BASE = 900;     // + index
    static constexpr int ID_SURV_BASE  = 10000;   // + x*100 + y
    static constexpr int ID_OBS_BASE   = 20000;   // + x*100 + y

    static int SurvivorId(int x, int y) { return ID_SURV_BASE + x*100 + y; }
    static int ObstacleId(int x, int y) { return ID_OBS_BASE + x*100 + y; }
};

#endif // MYFRAME_H_INCLUDED
