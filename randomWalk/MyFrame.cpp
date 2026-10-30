#include "MyFrame.h"
#include <algorithm>
#include <sstream>
#include <format>

MyFrame::MyFrame()
: wxFrame(nullptr, wxID_ANY, "Drone Simulation (wxWidgets + gnuplot)")
{
    // ---- Layout (top panel with controls, bottom status bar) ----
    auto* panel = new wxPanel(this);
    auto* root  = new wxBoxSizer(wxVERTICAL);

    // Row 1: Drones slider + Survivors slider
    auto* row1  = new wxBoxSizer(wxHORIZONTAL);
    sldDrones_    = new wxSlider(panel, wxID_ANY, 5, 1, 20, wxDefaultPosition, wxSize(250, -1), wxSL_VALUE_LABEL);
    sldSurvivors_ = new wxSlider(panel, wxID_ANY, 10, 1, 200, wxDefaultPosition, wxSize(250, -1), wxSL_VALUE_LABEL);
    row1->Add(new wxStaticText(panel, wxID_ANY, "Drones:"), 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 6);
    row1->Add(sldDrones_, 0, wxRIGHT, 16);
    row1->Add(new wxStaticText(panel, wxID_ANY, "Survivors:"), 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 6);
    row1->Add(sldSurvivors_, 0, wxRIGHT, 16);


    // Row 2: Buttons
    auto* row2  = new wxBoxSizer(wxHORIZONTAL);
    btnSimulate_ = new wxButton(panel, wxID_ANY, "SIMULATE");
    btnEnd_      = new wxButton(panel, wxID_ANY, "END");
    row2->Add(btnSimulate_, 0, wxRIGHT, 12);
    row2->Add(btnEnd_, 0, wxRIGHT, 12);

    root->Add(row1, 0, wxALL, 8);
    root->Add(row2, 0, wxLEFT|wxRIGHT|wxBOTTOM, 8);
    panel->SetSizerAndFit(root);

    CreateStatusBar();

    // ---- Bind UI ----
    sldDrones_->Bind(wxEVT_SLIDER,     &MyFrame::OnDroneSlider, this);
    sldSurvivors_->Bind(wxEVT_SLIDER,  &MyFrame::OnSurvivorSlider, this);
    btnSimulate_->Bind(wxEVT_BUTTON,   &MyFrame::OnSimulate, this);
    btnEnd_->Bind(wxEVT_BUTTON,        &MyFrame::OnEnd, this);
    Bind(wxEVT_TIMER,                  &MyFrame::OnSimTick, this, simTimer_.GetId());
    Bind(wxEVT_CLOSE_WINDOW,           &MyFrame::OnClose, this);

    // ---- Start gnuplot ----
    GpOpen();
    // show default empty grid (so 'replot' won't complain)
    GpSetupRanges(desiredMapW_, desiredMapH_);
    GpReplot();

    SetStatusText("Set parameters and press SIMULATE.");
}

MyFrame::~MyFrame() {
    StopSimulation();
    GpClose();
}

// ---------- UI handlers ----------
void MyFrame::OnDroneSlider(wxCommandEvent& e) {
    desiredDrones_ = e.GetInt();
    SetStatusText(wxString::Format("Drones = %d", desiredDrones_));
}

void MyFrame::OnSurvivorSlider(wxCommandEvent& e) {
    desiredSurvivors_ = e.GetInt();
    SetStatusText(wxString::Format("Survivors = %d", desiredSurvivors_));
}

void MyFrame::OnSimulate(wxCommandEvent&) {
    // Parse width/height
    long w=50, h=50;
    desiredMapW_ = (int)w;
    desiredMapH_ = (int)h;

    // (Re)create world with requested survivor count; pick obstacles ~= 8% of cells (clamped)
    int approxCells = std::max(1, desiredMapW_ * desiredMapH_);
    int obstacles   = std::min( std::max( (int)(approxCells * 0.08), 1), 10 );  // tweakable

    // Reset model
    world_ = Map(/*numObstacles*/obstacles, /*numSurvivors*/desiredSurvivors_);
    swarm_.clear();
    prevPos_.clear();
    drawnSurvivors_.clear();
    nextArrowId_ = 30000;
    timestep_ = 0;

    // Reset gnuplot canvas to new size & clear all old points/arrows
    GpClearAll();
    GpSetupRanges(desiredMapW_, desiredMapH_);

    // Draw static obstacles
    for (auto [ox,oy] : world_.getObstaclesList()) {
        GpSetCircle(ObstacleId(ox,oy), ox, oy, 0.6, "red", /*transparent*/false);
    }
    // Draw initial survivors
    for (auto [sx,sy] : world_.getSurvivorsList()) {
        int sid = SurvivorId(sx,sy);
        drawnSurvivors_.insert(sid);
        GpSetCircle(sid, sx, sy, 0.5, "blue", /*transparent*/false);
    }

    // Create drones at map center
    double cx = std::max(0, desiredMapW_-1) / 2.0;
    double cy = std::max(0, desiredMapH_-1) / 2.0;
    for (int i = 0; i < desiredDrones_; ++i) {
        swarm_.emplace_back(ID_DRONE_BASE + i, cx, cy, /*speed*/3);
        auto p = swarm_.back().getCoordinates();
        GpSetCircle(ID_DRONE_BASE + i, p[0], p[1], 0.5, "black", /*transparent*/false);
        prevPos_[ID_DRONE_BASE + i] = {p[0], p[1]};
    }

    GpReplot();

    running_ = true;
    simTimer_.Start(100, wxTIMER_CONTINUOUS); // 10 Hz; adjust if needed
    SetStatusText("Simulation running…");
}

void MyFrame::OnEnd(wxCommandEvent&) {
    Close(true);
}

void MyFrame::OnClose(wxCloseEvent& e) {
    StopSimulation();
    GpClose();
    e.Skip();
}

// ---------- Simulation ----------
void MyFrame::StartSimulation() {
    if (!running_) {
        simTimer_.Start(100, wxTIMER_CONTINUOUS);
        running_ = true;
    }
}
void MyFrame::StopSimulation() {
    if (simTimer_.IsRunning()) simTimer_.Stop();
    running_ = false;
}

void MyFrame::OnSimTick(wxTimerEvent&) {
    if (!running_) return;

    // Step drones
    for (auto& d : swarm_) {
        int id = d.getDroneID();
        auto prev = prevPos_[id];

        d.updatePosition2(world_, swarm_);
        d.scanForSurvivors(world_);

        auto p = d.getCoordinates();
        // Move drone marker
        GpSetCircle(id, p[0], p[1], 0.5, "black", /*transparent*/false);
        GpSetCircle(id+100, p[0], p[1], 2, "green", true);
        // Trajectory segment from prev -> current
        GpSetArrow(nextArrowId_++, prev.first, prev.second, p[0], p[1], "gray", 1.0);
        prevPos_[id] = {p[0], p[1]};
    }

    // Remove inactive drones safely
    for (auto it = swarm_.begin(); it != swarm_.end(); ) {
        if (!it->getStatus()) {
            GpUnsetObject(it->getDroneID());
            prevPos_.erase(it->getDroneID());
            std::cout<<std::format("Drones left : {}\n", swarm_.size());
            it = swarm_.erase(it);
        } else {
            ++it;
        }
    }

    // Sync survivors (remove rescued; draw new if any were added)
    std::unordered_set<int> current;
    current.reserve((size_t)std::max(1, world_.getSurvivorCount()*2));
    for (auto [x,y] : world_.getSurvivorsList()) {
        int sid = SurvivorId(x,y);
        current.insert(sid);
        if (!drawnSurvivors_.count(sid)) {
            GpSetCircle(sid, x, y, 0.5, "blue", /*transparent*/false);
        }
    }
    for (int sid : drawnSurvivors_) {
        if (!current.count(sid)) {
            std::cout<<std::format("Rescued survivor after {} timesteps\n\n",timestep_);
            GpUnsetObject(sid);
        }
    }
    drawnSurvivors_.swap(current);

    // Repaint gnuplot once per tick
    GpReplot();

    // End conditions
    if (swarm_.empty()) {
        StopSimulation();
        SetStatusText("All drones died…");
        std::cout<<"All drones have died...\n";
        std::cout<<"Time steps taken: "<<timestep_<<std::endl;
        return;
    }
    if (world_.getSurvivorCount() <= 0) {
        StopSimulation();
        SetStatusText("All survivors found!");
        std::cout<<"All survivors have been found!\n";
        std::cout<<"Time steps taken: "<<timestep_<<std::endl;
        return;
    }

    ++timestep_;
    if ((timestep_ % 10) == 0) {
        SetStatusText(wxString::Format("timestep=%d  survivors=%d", timestep_, world_.getSurvivorCount()));
    }
}

// ---------- gnuplot helpers ----------
void MyFrame::GpOpen() {

    gp_ = _popen("gnuplot", "w");

    if (!gp_) {
        wxLogError("Failed to start gnuplot");
        return;
    }
    fprintf(gp_, "set term wxt noraise\n");   // you can switch to 'qt' or 'windows' if preferred
    fprintf(gp_, "set size ratio -1\n");
    fprintf(gp_, "set grid xtics ytics\n");
    fprintf(gp_, "plot NaN notitle\n");       // base plot so 'replot' has something to show
    fflush(gp_);
}

void MyFrame::GpClose() {
    if (!gp_) return;

    _pclose(gp_);

    gp_ = nullptr;
}

void MyFrame::GpClearAll() {
    if (!gp_) return;
    // Clear all objects/arrows
    fprintf(gp_, "unset object\n");
    fprintf(gp_, "unset arrow\n");
    fprintf(gp_, "plot NaN notitle\n"); // reset base plot
    fflush(gp_);
}
//===================================================================================================================
void MyFrame::GpSetupRanges(int w, int h) {
    if (!gp_) return;
    fprintf(gp_, "set xrange [0:%d]\n", std::max(0, w-1));
    fprintf(gp_, "set yrange [0:%d]\n", std::max(0, h-1));
    fprintf(gp_, "plot NaN notitle\n");  // ensure a previous plot exists
    fflush(gp_);
}

void MyFrame::GpSetCircle(int id, double x, double y, double radius,
                          const char* color, bool transparent)
{
    if (!gp_) return;
    if (!transparent) {
        fprintf(gp_,
            "set object %d circle at %g,%g radius char %g fillcolor rgb '%s' fillstyle solid\n",
            id, x, y, radius, color);
    } else {
        fprintf(gp_,
            "set object %d circle at %g,%g radius char %g fillcolor rgb '%s' fillstyle transparent solid 0.3\n",
            id, x, y, radius, color);
    }
}

void MyFrame::GpUnsetObject(int id){
    if (!gp_) return;
    fprintf(gp_, "unset object %d\n", id);
}

void MyFrame::GpSetArrow(int id, double x1, double y1, double x2, double y2,
                         const char* color, double lw)
{
    if (!gp_) return;
    fprintf(gp_,
        "set arrow %d from %g,%g to %g,%g nohead lc rgb '%s' lw %g\n",
        id, x1, y1, x2, y2, color, lw);
}

void MyFrame::GpReplot(){
    if (!gp_) return;
    // guard: make sure a previous plot exists then replot
    fprintf(gp_, "plot NaN notitle\n");
    fprintf(gp_, "replot\n");
    fflush(gp_);
}
