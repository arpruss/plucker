#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "PluckerViewerApp.h"
#include "PluckerHtmlWindow.h"

IMPLEMENT_APP(PluckerViewerApp)

PluckerViewerApp::PluckerViewerApp()
{

}

PluckerViewerApp::~PluckerViewerApp()
{

}

bool PluckerViewerApp::OnInit()
{
    PluckerHtmlWindow* frame = new PluckerHtmlWindow(_("Plucker Viewer"), wxPoint(0, 0), wxSize(600, 600));
//    frame->LoadHtml("c:/temp/default.html");
//    frame->LoadPDB("c:/progra~1/plucker/handhe~1/pluckeruserguide.pdb");
    frame->Show(TRUE);
	SetTopWindow(frame);

    return true;
}

int PluckerViewerApp::OnExit(void)
{
    return 0;
}

