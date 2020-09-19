//
// Created by yuyaohui on 17-08-20.
//

#ifndef DUER_LINK_HTTP2_CONNECTION_OBSERVER_H
#define DUER_LINK_HTTP2_CONNECTION_OBSERVER_H

#include <string>

class DuerLinkHttp2ConnectionObserver{
public:
    virtual std::string NotifyReceivedData(const std::string& jsonPackageData,int iSessionID = -1) = 0;
    //virtual std::string NotifyReceivedDataFromCloud(const std::string& jsonPackageData,std::string uuid = "") = 0;

    virtual std::string notify_received_bduss(const std::string& bdussValue) = 0;
};

#endif //DUER_LINK_HTTP2_CONNECTION_OBSERVER_H
