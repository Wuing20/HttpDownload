#ifndef __HTTP_DOWN_PUB_H__
#define __HTTP_DOWN_PUB_H__

#include <iostream>
#include <curl/curl.h>
#include <string>
#include <unistd.h>
#include <pthread.h>

using namespace std;

struct tData
{
    FILE *fp;
    long startPos;
    long endPos;
    CURL* curl;
    pthread_t tid;
};

class CDownPub
{
public:
    virtual bool DownloadFunc(int nPart) = 0;
    virtual void SetFileName(const string& sFileName) = 0;

protected:
    static size_t WriteFunc(void *ptr,size_t size,size_t nmemb,void *userdata);
    static void *WorkThread(void *pData);

    static int m_threadCnt ;
    static pthread_mutex_t m_mutex;
};

class CHttpDown : public CDownPub
{
public:
    virtual ~CHttpDown();

    static CHttpDown* GetInstance(const string& sUrl);

    void SetUrl(const string& sUrl);
    virtual void SetFileName(const string& sFileName);
    string GetFileNamebyUrl();
    long GetDownloadFileLength() const;

    virtual bool DownloadFunc(int nPart);

protected:
   static int ProgressFunc(void *ptr,double totalToDownload,double nowDownloaded,double totalToUpLoad,double nowUpLoaded);

private:
    CHttpDown() = delete;
    CHttpDown(CHttpDown& ) = delete;

    CHttpDown(const string& sUrl)
    : m_sUrl(sUrl),m_sFileName("")
    {
        curl_global_init(CURL_GLOBAL_ALL);
    }

private:
    static CHttpDown* m_pObj;
    string m_sUrl;
    string m_sFileName;
};

#endif