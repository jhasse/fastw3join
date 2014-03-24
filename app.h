#pragma once
#ifndef APP_H
#define APP_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <iostream>
#include <vector>

#include "wx/taskbar.h"
#include "wx/clipbrd.h"
#include "wx/listbox.h"
#include "wx/stattext.h"
#include <boost/shared_ptr.hpp>

#ifdef linux
#include <gtkmm.h>
#endif

#include "jan-nik.hpp"

//Größe des Einstellungsfensters definieren
#ifdef linux
const int SETTINGS_HEIGHT = 200;
const int SETTINGS_WIDTH = 360;
#else
const int SETTINGS_HEIGHT = 230;
const int SETTINGS_WIDTH = 360;
#endif  //linux

enum {
    EINSTELLUNGEN = 100,
    ENDE,
    ABOUT
};

const int listid = wxNewId();

class mytaskbaricon: public wxTaskBarIcon
{
    public:
        wxMenu* CreatePopupMenu();
        void Ende(wxCommandEvent&);

    DECLARE_EVENT_TABLE()
};

bool app_ready;

class MyApp : public wxApp
{
	public:
		virtual bool OnInit();
		virtual int OnExit();
		void OnClickEinstellungen(wxCommandEvent&);
		void OnClickAbout(wxCommandEvent&);
		void Einstellungen();
    private:
		static boost::shared_ptr<Network> pnet;
};

class DlgSettings:public wxDialog
{
    public:
        DlgSettings(boost::shared_ptr<Network> pNetwork);
        ~DlgSettings();
        void OnExit(wxCommandEvent&);
        void OnSelect(wxCommandEvent&);
    private:
        boost::shared_ptr<Network> pnet;
        wxListBox *liste;
    DECLARE_EVENT_TABLE()
};

class AboutBox:public wxDialog
{
    public:
        AboutBox();
};

mytaskbaricon* taskbar_icon;


BEGIN_EVENT_TABLE(DlgSettings, wxDialog)
    EVT_BUTTON(wxID_EXIT, DlgSettings::OnExit)
//    EVT_LISTBOX(listid, DlgSettings::OnSelect)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(mytaskbaricon, wxTaskBarIcon)
    EVT_MENU(ENDE, mytaskbaricon::Ende)
    EVT_MENU(EINSTELLUNGEN, MyApp::OnClickEinstellungen)
    EVT_MENU(ABOUT, MyApp::OnClickAbout)
END_EVENT_TABLE()

#endif // APP_H
