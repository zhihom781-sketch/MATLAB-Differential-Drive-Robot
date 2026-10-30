#include <wx/wx.h>
#include "MyFrame.h"

class MyApp : public wxApp {
public:
    bool OnInit() override {
        auto* f = new MyFrame();
        f->SetClientSize(650, 100);
        f->Center();
        f->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
