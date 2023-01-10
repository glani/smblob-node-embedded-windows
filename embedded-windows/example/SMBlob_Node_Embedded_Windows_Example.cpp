// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"


// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/artprov.h"
#include "wx/bookctrl.h"
#include "wx/sysopt.h"
#include "wx/wupdlock.h"

#include "wx/display.h"
#include <iostream>

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "./example.xpm.h"

#endif

#include "SMBlobNodeEmbeddedWindows.h"


class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    wxTextCtrl *textLog;

    MyFrame(const wxString &title);

    virtual ~MyFrame() {
        if (embeddedWindows) {
            SMBlob::EmbeddedWindows::Release(*embeddedWindows);
        }
    }

    void OnButtonInitialize(wxCommandEvent &);

    void OnButtonDestroy(wxCommandEvent &);

    void OnListBoxDoubleClick(wxCommandEvent &event);

    void OnQuit(wxCommandEvent &event);

    void OnAbout(wxCommandEvent &event);

private:
DECLARE_EVENT_TABLE()
    std::shared_ptr<SMBlob::EmbeddedWindows::SMBlobApp> embeddedWindows;
};

// Declare some IDs. These are arbitrary.
const int BOOKCTRL = 100;
const int BUTTON_INITIALIZE = 101;
const int BUTTON_DESTROY = 102;
const int TEXTBOX_LOG = 104;
const int FILE_QUIT = wxID_EXIT;
const int HELP_ABOUT = wxID_ABOUT;

// Attach the event handlers. Put this after MyFrame declaration.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
                EVT_BUTTON(BUTTON_INITIALIZE, MyFrame::OnButtonInitialize)
                EVT_BUTTON(BUTTON_DESTROY, MyFrame::OnButtonDestroy)
                EVT_MENU(FILE_QUIT, MyFrame::OnQuit)
                EVT_MENU(HELP_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
    MyFrame *frame = new MyFrame(
            _T("My wxWidgets Demo"));
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString &title)
        : wxFrame(NULL, wxID_ANY, title) {
    SetIcon(wxICON(example));
    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(HELP_ABOUT, _T("&About..."),
                     _T("Show about dialog"));
    fileMenu->Append(FILE_QUIT, _T("E&xit"),
                     _T("Quit this program"));
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));
    SetMenuBar(menuBar);
    CreateStatusBar(2);
    SetStatusText(_T("So far so good."), 0);
    SetStatusText(_T("Welcome."), 1);

    auto book = new wxBookCtrl(this, BOOKCTRL);
    wxPanel *panel = new wxPanel(book);
    new wxButton(panel, BUTTON_INITIALIZE,
                 _T("&Initialize"), wxPoint(50, 30), wxSize(100, 30));
    new wxButton(panel, BUTTON_DESTROY,
                 _T("&Destroy"), wxPoint(50, 80), wxSize(100, 30));
    book->AddPage(panel, _T("Initialize Embedded Application"), true);

    panel = new wxPanel(book);
    wxBoxSizer *mysizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(mysizer);
    textLog = new wxTextCtrl(panel, TEXTBOX_LOG, _T("Log\n"),
                             wxPoint(0, 250), wxSize(100, 50), wxTE_MULTILINE);
    mysizer->Add(textLog, 1, wxEXPAND | wxALL, 5);
    book->AddPage(panel, _T("Debug"), false);

    const SMBlob::EmbeddedWindows::SMBlobApp &args = SMBlob::EmbeddedWindows::Init();
    this->embeddedWindows = std::make_shared<SMBlob::EmbeddedWindows::SMBlobApp>(args);
}

void MyFrame::OnQuit(wxCommandEvent & WXUNUSED(event)) {
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent & WXUNUSED(event)) {
    wxString msg;
    msg.Printf(_T("About.\n")
               _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About My Program"),
                 wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnButtonInitialize(wxCommandEvent & WXUNUSED(event)) {
    wxMessageBox("Click1", "Click",
                 wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnButtonDestroy(wxCommandEvent & WXUNUSED(event)) {
    wxMessageBox("Click2", "Click",
                 wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnListBoxDoubleClick(wxCommandEvent &event) {
    *textLog << "ListBox double click string is: \n";
    *textLog << event.GetString();
    *textLog << "\n";
}