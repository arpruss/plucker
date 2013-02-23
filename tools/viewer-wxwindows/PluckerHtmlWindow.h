#if !defined(PLUCKERHTMLWINDOW_H)
#define PLUCKERHTMLWINDOW_H

#include <wx/html/htmlwin.h>
#include <wx/string.h>

class PluckerHtmlWindow : public wxFrame
{
DECLARE_CLASS(PluckerHtmlWindow)
DECLARE_EVENT_TABLE()

public:
	PluckerHtmlWindow(wxString title, wxPoint topLeft, wxSize size);
	virtual ~PluckerHtmlWindow();

    void LoadHtml(wxString filename);
    void LoadPDB(wxString filename);
    
    void OnOpen(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnNavBack(wxCommandEvent &event);
    void OnNavForward(wxCommandEvent &event);
    void OnNavHome(wxCommandEvent &event);

private:
    wxHtmlWindow* htmlWindow;
    wxString filename;
    wxString homeFile;
};

#endif 
