#if !defined(PLUCKERVIEWERAPP_H)
#define PLUCKERVIEWERAPP_H

class PluckerViewerApp : public wxApp  
{
public:
	PluckerViewerApp();
	virtual ~PluckerViewerApp();

	virtual bool OnInit();
    int OnExit(void);

};

DECLARE_APP(PluckerViewerApp)

#endif PLUCKERVIEWERAPP_H
