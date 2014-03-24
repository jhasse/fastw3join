#include "wx_pch.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "app.h"

DlgSettings *dialog = NULL;

IMPLEMENT_APP(MyApp);

boost::shared_ptr<Network> MyApp::pnet;

#include "standard.xpm"
#include "loading.xpm"

bool MyApp::OnInit()
{
    #ifdef linux
    Gtk::Main kit(0, 0);
    #endif
    app_ready = false;
    taskbar_icon = new mytaskbaricon;
    taskbar_icon->SetIcon(wxIcon(loading_xpm), wxT("Loading..."));
    try
    {
        pnet.reset(new Network);
    }
    catch(std::exception& e)
    {
        wxMessageBox(wxString(wxConvUTF8.cMB2WC(e.what()), *wxConvCurrent));
        return false;
    }
    try
    {
        pnet->ActivateDevice();
    }
    catch(std::exception& e)
    {
        wxMessageBox(wxString(wxConvUTF8.cMB2WC(e.what()), *wxConvCurrent), wxT("Error"));
        Einstellungen();
    }
    taskbar_icon->SetIcon(wxIcon(standard_xpm), wxT("FastW3Join"));
    app_ready = true;

	return true;
}

int MyApp::OnExit()
{
    taskbar_icon->RemoveIcon();
    delete taskbar_icon;
	return 0;
}

void mytaskbaricon::Ende(wxCommandEvent&)
{
    wxExit();
}

wxMenu* mytaskbaricon::CreatePopupMenu()
{
    if(!app_ready)
        return 0;

    wxMenu* conmenu = new wxMenu;
    conmenu->Append(EINSTELLUNGEN, wxT("Einstellungen"));
    conmenu->Append(ABOUT, wxT("Über FastW3Join"));
    conmenu->Append(ENDE, wxT("Ende"));
    return conmenu;
}

void MyApp::Einstellungen()
{
    dialog = new DlgSettings(pnet);
    dialog->Show();
    dialog->Centre();
    return;
}

void MyApp::OnClickEinstellungen(wxCommandEvent&)
{
    Einstellungen();
    return;
}

void MyApp::OnClickAbout(wxCommandEvent&)
{
    AboutBox* about_dialog;
    about_dialog = new AboutBox();
    about_dialog->Show();
    return;
}


DlgSettings::DlgSettings(boost::shared_ptr<Network> pNetwork):
  wxDialog(NULL, wxID_ANY, wxT("FastW3Join - Einstellungen"), wxDefaultPosition, wxSize(SETTINGS_WIDTH, SETTINGS_HEIGHT),wxDEFAULT_DIALOG_STYLE), pnet(pNetwork)
{
    (void)new wxStaticText(this,-1,wxT("Bitte wählen Sie die Netzwerkkarte aus:"),wxPoint(10,10));
    (void)new wxButton(this, wxID_EXIT, _T("Ok"), wxPoint(140, 160), wxSize(80, 30));
    liste = new wxListBox(this,listid,wxPoint(10,30),wxSize(340,120));

    assert(pnet);

    std::vector<wxString> dev_list = pnet->GetDevices();
    liste->Append(wxT("None"));
    for(std::vector<wxString>::iterator i = dev_list.begin(); i != dev_list.end(); ++i)
    {
        liste->Append(*i);
    }

    int id = pnet->GetActiveDevice();
    if(id == -1)
        liste->Select(0);
    else
        liste->Select(++id);       //None Device ist GetActiveDevice unbekannt

}

DlgSettings::~DlgSettings()
{
}

void DlgSettings::OnSelect(wxCommandEvent&)
{
    wxString tmp;
    tmp << wxT("Auswahl: ") <<liste->GetSelection();
    wxMessageBox(tmp);
}

void DlgSettings::OnExit(wxCommandEvent&)
{
    assert(pnet);
    int auswahl = liste->GetSelection();
    if(auswahl == 0)  //wenn "keine Netzwerkkarte" ausgewählt wurde (letzes Element der Liste)
    {
        std::cout << "No Interface selected" << std::endl;
        pnet->ChangeDevice(-1);
        taskbar_icon->SetIcon(wxIcon(loading_xpm),wxT("Keine Netzwerkkarte ausgewählt"));

    }
    else
    {
        --auswahl;  //None ist erstes Element der Liste und darf nicht mitgezählt werden
        if(static_cast<int>(pnet->GetActiveDevice()) != auswahl)
        {
            try
            {
                pnet->ChangeDevice(auswahl);
            }
            catch(std::exception& e)
            {
                wxMessageBox(wxString(wxConvUTF8.cMB2WC(e.what())),wxT("Fehler"));
                return;
            }
            taskbar_icon->SetIcon(wxIcon(standard_xpm),wxT("FastW3Join"));
        }
    }
    Destroy();
}

AboutBox::AboutBox():
wxDialog(NULL, wxID_ANY, wxT("Über FastW3Join"), wxDefaultPosition, wxSize(300, 200),wxDEFAULT_DIALOG_STYLE)
{

    //Programm infos
    (void) new wxStaticText(this,-1,wxT("FastW3Join"),wxPoint(20,50));
    (void) new wxStaticText(this,-1,wxT("Version: 1.1"),wxPoint(20,70));
    (void) new wxStaticText(this,-1,wxT("Autoren: Fabian Franzen, Jan Niklas Hasse"),wxPoint(20,90));
    (void) new wxStaticText(this,-1,wxT("\x00A9 2007-2010 watteimdocht.de"),wxPoint(20,110));

    //wxWidgets Infos
}
