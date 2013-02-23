
//typedef unsigned char boolean;
//#include "explode.h"
extern "C" int explode(char* directory, char* document_path, unsigned char verbose);

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include <wx/image.h>
#include <wx/filedlg.h>
#include <wx/xrc/xmlres.h>

#include "PluckerHtmlWindow.h"

IMPLEMENT_CLASS(PluckerHtmlWindow, wxFrame)
BEGIN_EVENT_TABLE(PluckerHtmlWindow, wxFrame)
    EVT_MENU(XRCID("file_menu_open"), PluckerHtmlWindow::OnOpen)
    EVT_MENU(XRCID("file_menu_exit"), PluckerHtmlWindow::OnExit)
    EVT_MENU(XRCID("navigate_back"), PluckerHtmlWindow::OnNavBack)
    EVT_MENU(XRCID("navigate_home"), PluckerHtmlWindow::OnNavHome)
    EVT_MENU(XRCID("navigate_foward"), PluckerHtmlWindow::OnNavForward)
END_EVENT_TABLE()

PluckerHtmlWindow::PluckerHtmlWindow(wxString title, wxPoint topLeft, wxSize size) : 
    wxFrame(NULL, -1, title, topLeft, size, wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
    wxInitAllImageHandlers();
    wxXmlResource::Get()->InitAllHandlers(); 
    wxXmlResource::Get()->Load("resource/main_menubar.xrc");        
    wxXmlResource::Get()->Load("resource/main_toolbar.xrc");        
    SetMenuBar(wxXmlResource::Get()->LoadMenuBar("main_menubar"));
    SetToolBar(wxXmlResource::Get()->LoadToolBar(this, "main_toolbar"));

    htmlWindow = new wxHtmlWindow(this);
}

PluckerHtmlWindow::~PluckerHtmlWindow()
{
}

void PluckerHtmlWindow::LoadPDB(wxString filename)
{
    if (explode((char*) ::wxGetCwd().c_str(), (char*) filename.c_str(), 0) != 0) {
        htmlWindow->SetPage("<html><body>There was an error exploding the file</body></html>");
        homeFile = "";
    }
    else {
        LoadHtml(wxGetCwd() + "/default.html");
    }
}

void PluckerHtmlWindow::LoadHtml(wxString filename)
{
    homeFile = filename;
    htmlWindow->LoadPage(filename);
}

void PluckerHtmlWindow::OnOpen(wxCommandEvent &event)
{
    wxFileDialog fileDialog(this);
    fileDialog.SetWildcard("PDB files (*.pdb)|*.pdb|Html files (*.htm, *.html)|*.htm;*.html");
    if (fileDialog.ShowModal() == wxID_OK) {
        wxString extension = fileDialog.GetFilename().Right(4);
        if (extension.CmpNoCase(".pdb") == 0)
            LoadPDB(fileDialog.GetPath());
        else
            LoadHtml(fileDialog.GetPath());
    }
}

void PluckerHtmlWindow::OnExit(wxCommandEvent &event)
{
    Close();
}

void PluckerHtmlWindow::OnNavBack(wxCommandEvent &event)
{
    htmlWindow->HistoryBack();
}

void PluckerHtmlWindow::OnNavForward(wxCommandEvent &event)
{
    htmlWindow->HistoryForward();
}

void PluckerHtmlWindow::OnNavHome(wxCommandEvent &event)
{
    if (homeFile != "") 
        LoadHtml(homeFile);
    else
        htmlWindow->SetPage("<html><body>Home page was invalid</body></html>");
}

