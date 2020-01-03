#ifndef __DOWNLOAD_FACTORY_H__
#define __DOWNLOAD_FACTORY_H__

#include "HttpDownPub.h"

class CDownloadFactory
{
public:
    CDownPub* CreateObj(const string& sUrl)
    {
        if(string::npos != sUrl.find("http"))
        {
            return CHttpDown::GetInstance(sUrl);
        }
        else
        {
            cerr<<"Not support protocol!"<<endl;

            exit(-1);
        }
        
    }
};


#endif