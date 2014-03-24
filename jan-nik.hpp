#ifndef JANNIK_HPP_INCLUDED
#define JANNIK_HPP_INCLUDED

#include <boost/noncopyable.hpp>
#include <boost/regex.hpp>
#include <wx/string.h>
#include <wx/thread.h>
#include <vector>
#include <pcap.h>

class MyThread : public wxThread
{
public:
    MyThread(pcap_t* handle, bpf_u_int32 net);
    virtual void* Entry();
private:
    pcap_t* handle_;
};

class Network : boost::noncopyable
{
public:
    Network();
    ~Network();
    const std::vector<wxString>& GetDevices();
    void ChangeDevice(const int index);
    void ActivateDevice();
    const size_t GetActiveDevice();
private:
    std::vector<wxString> wxDevices_;
    std::vector<std::string> devices_;
    int activeDevice_;
    MyThread* myThread_;
};

#endif // JAN-NIK_HPP_INCLUDED
