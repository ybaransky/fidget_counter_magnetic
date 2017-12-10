#ifndef WIFI_H
#define WIFI_H

class Wifi {
    public:
        Wifi(const char* ssid, const char* password) : _ssid(ssid), _password(password) {}
        void    start();
        void    process();
    private:
        void    mdnsStart();
        bool    captivePortal();

        const char* _ssid;   
        const char* _password;   
};


#endif
