#include "jan-nik.hpp"

#include <fstream>
#include <pcap.h>
#include <stdio.h>
#include <iostream>
#include <boost/regex.hpp>
#include <wx/wx.h>
#include <wx/string.h>
#include <wx/clipbrd.h>
#include <wx/wxprec.h>
#include <exception>
#ifdef linux
#include <gtkmm.h>
#endif

#ifdef linux
const std::string basepath = "/usr/share/fastw3join/"; // regex.txt
const std::string basepath2 = "/etc/fastw3join/"; // device.txt
#else
const std::string basepath = "";
const std::string basepath2 = "";
#endif

using namespace boost;

std::vector<regex> expressions_;

const std::vector<wxString>& Network::GetDevices()
{
    return wxDevices_;
}

const size_t Network::GetActiveDevice()
{
    return activeDevice_;
}

MyThread::MyThread(pcap_t* handle, bpf_u_int32 net) : handle_(handle)
{

    struct bpf_program fp;		/* The compiled filter */
    char filter_exp[] = "src host europe.battle.net || asia.battle.net || uswest.battle.net || useast.battle.net";	/* The filter expression */

    /* Compile and apply the filter */

    std::cout << "Parsing filter..." << std::endl;
    if (pcap_compile(handle_, &fp, filter_exp, 0, net) == -1)
    {
        throw std::runtime_error(std::string("Couldn't parse filter ") + filter_exp + ": " + pcap_geterr(handle));
    }
    if (pcap_setfilter(handle_, &fp) == -1)
    {
        throw std::runtime_error(std::string("Couldn't install filter ") + filter_exp + ": " + pcap_geterr(handle));
    }
}

Network::Network() : activeDevice_(0), myThread_(0)
{
    std::ifstream fin((basepath + "regex.txt").c_str());

    if (!fin)
    {
        throw std::runtime_error(std::string("Could not find " + basepath + "regex.txt!"));
    }
    do
    {
        std::string temp;
        std::getline(fin, temp);
        if (!temp.empty())
        {
            expressions_.push_back(regex(temp));
        }
    }
    while (fin);

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;
    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        throw std::runtime_error(errbuf);
    }
    if (!alldevs)
    {
        throw std::runtime_error("No devices found! (Try to run as root)");
    }
    devices_.clear();
    for (pcap_if_t* d = alldevs; d; d = d->next)
    {
        std::string temp(d->name);
        devices_.push_back(temp);
        if (d->description)
        {
#ifdef linux
            temp += " (";
            temp += d->description;
            temp += ")";
#else
            temp = d->description;
#endif
        }
        wxDevices_.push_back(wxString(wxConvUTF8.cMB2WC(temp.c_str()), *wxConvCurrent));
    }

    // Set Active Device
    const char* const dev = pcap_lookupdev(errbuf);

    std::ifstream deviceTxt((basepath2 + "device.txt").c_str());

    std::string savedDevice;
    if(deviceTxt)
    {
        std::getline(deviceTxt, savedDevice);
    }
#ifdef linux
    else
    {
        system("mkdir /etc/fastw3join");
    }
#endif
    bool alreadySetForAny = false; // any hat Vorang vorm Standard-Device
    for (size_t i = 0; i < devices_.size(); ++i)
    {
        if(!alreadySetForAny &&
            dev &&
            devices_[i] == dev)
        {
            activeDevice_ = i;
        }

        if(devices_[i] == "any")
        {
            activeDevice_ = i;
        }

        if(deviceTxt && devices_[i] == savedDevice) // Falls ein Device vom letzten Mal gespeichert wurde...
        {
            activeDevice_ = i;
            break; // ... hat es vor allen anderen Vorrang
        }
    }
}

void Network::ActivateDevice()
{
    if(activeDevice_ == -1)
        return;
    pcap_t *handle;			/* Session handle */
    std::vector<char> dev(devices_[activeDevice_].begin(), devices_[activeDevice_].end());
    dev.push_back(0); // Nicht vergessen!!!
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
    bpf_u_int32 net;		/* Our IP */
    bpf_u_int32 mask;		/* Our netmask */

    std::cout << "Opening device " << &dev[0] << "..." << std::endl;
    /* Find the properties for the device */
    if (pcap_lookupnet(&dev[0], &net, &mask, errbuf) == -1)
    {
        throw std::runtime_error(std::string("Couldn't get netmask for device ") + &dev[0] + ": " + errbuf);
    }
    /* Open the session in promiscuous mode */
    handle = pcap_open_live(&dev[0], BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL)
    {
        throw std::runtime_error(std::string("Couldn't open device ") + &dev[0] + ": " + errbuf);
    }

    std::cout << "Starting thread..." << std::endl;

    myThread_ = new MyThread(handle, net);
    myThread_->Create();
    myThread_->Run();
}

Network::~Network()
{
    std::ofstream fout((basepath2 + "device.txt").c_str());
    fout << devices_[activeDevice_];
}

bool invalid(char c)
{
    return c == '\0' || c == '\n';
}

#ifdef linux
class ClipboardEntry
{
public:
    ClipboardEntry(std::string data) : data_(data)
    {
    }
    void on_get(Gtk::SelectionData& selection_data, guint)
    {
        selection_data.set("STRING", data_);
    }
    void on_clear()
    {
        delete this;
    }
private:
    std::string data_;
};
#endif

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    std::string x((const char*)packet, (size_t)header->len);
    x = std::string(x.begin(), std::remove_if(x.begin(), x.end(), invalid));
    cmatch what;
    for (std::vector<regex>::const_iterator it = expressions_.begin(); it != expressions_.end(); ++it)
    {
        if (regex_match(x.c_str(), what, *it))
        {
            const std::string temp = what[2];
            std::cout << temp;

			HGLOBAL hText = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, temp.size() + 1);
			char* pText = (char*)GlobalLock(hText);
			strcpy(pText, temp.c_str());
			GlobalUnlock(hText);

			OpenClipboard(0);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hText);
			CloseClipboard();
            break;
        }
    }
    std::cout << "." << std::flush;
}

void* MyThread::Entry()
{
    std::cout << "Waiting for packets..." << std::endl;
    while (!TestDestroy())
    {
        pcap_dispatch(handle_, 1000, got_packet, 0);
    }
    std::cout << "Exiting Thread..." << std::endl;
    pcap_close(handle_);
    return 0;
}

void Network::ChangeDevice(const int index)
{
    std::cout << "Sending close message to thread..." << std::endl;
    if(activeDevice_ != -1)
        myThread_->Delete();
    activeDevice_ = index;
    if(index != -1)
        ActivateDevice();
}
