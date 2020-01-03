#include "HttpDownPub.h"

int CDownPub::m_threadCnt = 0;
pthread_mutex_t CDownPub::m_mutex = PTHREAD_MUTEX_INITIALIZER;


//------------------CDownPub---------------------------
size_t CDownPub::WriteFunc(void *ptr,size_t size,size_t nmemb,void *userdata)
{
    tData *node = (tData *) userdata;
	size_t written = 0;

	pthread_mutex_lock (&m_mutex);
	if (node->startPos + size * nmemb <= node->endPos)
	{
		fseek (node->fp, node->startPos, SEEK_SET);
		written = fwrite (ptr, size, nmemb, node->fp);
		node->startPos += written;
	}
	else
	{
		fseek (node->fp, node->startPos, SEEK_SET);
		written = fwrite (ptr, sizeof(char), node->endPos - node->startPos + 1, node->fp);
		node->startPos = node->endPos;
	}

	//cout<<"thread "<<pthread_self()<<" written "<<written<<" byte!"<<endl;
	pthread_mutex_unlock (&m_mutex);

	return written;
}

void* CDownPub::WorkThread(void *pData)
{
    tData *pNode = (tData *)pData;
    CURLcode retCode = curl_easy_perform(pNode->curl);

    /*
    if(0 != retCode)
    {
        cerr<<"Curl error code: "<<curl_easy_strerror(retCode)<<endl;

        //exit(-1);
    }*/

    curl_easy_cleanup(pNode->curl);

    pthread_mutex_lock(&m_mutex);
    m_threadCnt--;
    cout<<"thread "<<pNode->tid<<" exit!"<<endl;
    pthread_mutex_unlock(&m_mutex);

    delete pNode;
    pthread_exit(0);

    return NULL;
}



//------------------CHttpDown---------------------------
CHttpDown* CHttpDown::m_pObj = NULL;

CHttpDown::~CHttpDown()
{
    curl_global_cleanup();
}

CHttpDown* CHttpDown::GetInstance(const string& sUrl)
{
    if(NULL == m_pObj)
    {
        m_pObj = new CHttpDown(sUrl);
    }

    return m_pObj;
}

void CHttpDown::SetUrl(const string& sUrl)
{
    m_sUrl = sUrl;
}

void CHttpDown::SetFileName(const string& sFileName)
{
    m_sFileName = sFileName;
}

string CHttpDown::GetFileNamebyUrl()
{
    size_t nPos = m_sUrl.find_last_of('/');
    string sFileName;

    if((nPos + 1) == m_sUrl.size())
    {

        nPos = m_sUrl.find_last_of('/',nPos - 1);
         sFileName = m_sUrl.substr(nPos + 1);
         sFileName.pop_back();
    }
    else
    {
        sFileName = m_sUrl.substr(nPos + 1);
    }
    

    this->m_sFileName = sFileName;

    cout<<sFileName<<endl;
    return sFileName;
}

long CHttpDown::GetDownloadFileLength() const
{
    long file_len = 0;
    CURL *handle = curl_easy_init();
    CURLcode retCode;

    curl_easy_setopt(handle,CURLOPT_URL,m_sUrl.c_str());
    //curl_easy_setopt(handle,CURLOPT_VERBOSE,1L);
    curl_easy_setopt(handle,CURLOPT_FOLLOWLOCATION,1L);
    curl_easy_setopt(handle,CURLOPT_MAXREDIRS,3L);
    curl_easy_setopt(handle,CURLOPT_AUTOREFERER,1L);
    curl_easy_setopt(handle,CURLOPT_HEADER,1L);
    curl_easy_setopt(handle,CURLOPT_NOBODY,1L);
    curl_easy_setopt(handle,CURLOPT_FORBID_REUSE,1L);
    //curl_easy_setopt (handle, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/36.0.1985.143 Safari/537.36");

    //curl_easy_setopt(handle,CURLOPT_CONNECTTIMEOUT,30);
    curl_easy_setopt(handle,CURLOPT_NOSIGNAL,1L);

    retCode = curl_easy_perform(handle);
    if(CURLE_OK == retCode)
    {
        curl_easy_getinfo(handle,CURLINFO_CONTENT_LENGTH_DOWNLOAD,&file_len);
    }
    else
    {
        cerr<<"Curl ret error code: "<<curl_easy_strerror(retCode)<<endl;
        file_len = -1;
    }
    curl_easy_cleanup(handle);
    
    return file_len;
}

bool CHttpDown::DownloadFunc(int threadNum)
{
    long fileLength = GetDownloadFileLength();

    if(fileLength <= 0)
    {
        cerr<<"Get the file length error!"<<endl;

        return false;
    }

    
    if(m_sFileName.empty())
    {
        GetFileNamebyUrl();
    }

    //GetFileNamebyUrl();

    FILE *fp = fopen(m_sFileName.c_str(),"w+");
    if(!fp)
    {
        cerr<<"Open file fail!"<<endl;

        return false;
    }

    long partSize = fileLength / threadNum;

    for(int i = 0; i <= threadNum; ++i)
    {
        tData *pNode = new tData();

        if(i < threadNum)
        {
            pNode->startPos = i * partSize;
            pNode->endPos = (i + 1) * partSize - 1;
        }
        else
        {
            if(0 != (fileLength % threadNum))
            {
                pNode->startPos = i * partSize;
                pNode->endPos = fileLength - 1;
            }
            else
            {
                break;
            }
            
        }

        CURL *curl = curl_easy_init();

        pNode->curl = curl;
        pNode->fp = fp;

        char range[64] = {0};

        snprintf(range,sizeof(range),"%ld-%ld",pNode->startPos,pNode->endPos);

       curl_easy_setopt (curl, CURLOPT_URL, m_sUrl.c_str ());
       //curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
		curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, this->WriteFunc);
		curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) pNode);
		curl_easy_setopt (curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt (curl, CURLOPT_PROGRESSFUNCTION, ProgressFunc);
		curl_easy_setopt (curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt (curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
		curl_easy_setopt (curl, CURLOPT_LOW_SPEED_TIME, 5L);
		curl_easy_setopt (curl, CURLOPT_RANGE, range);

        pthread_mutex_lock(&(this->m_mutex));
        (this->m_threadCnt)++;
        pthread_mutex_unlock(&(this->m_mutex));
        int rc = pthread_create(&pNode->tid,NULL,this->WorkThread,pNode);

    }

    while(m_threadCnt > 0)
    {
        usleep(1000000L);
    }

    fclose(fp);
    cout<<"Download succeed........."<<endl;

    return true;
}

int CHttpDown::ProgressFunc(void *ptr,double totalToDownload,double nowDownloaded,double totalToUpLoad,double nowUpLoaded)
{
    int percent = 0;
	if (totalToDownload > 0)
	{
		percent = (int) (nowDownloaded / totalToDownload * 100);
	}
 
    if(percent % 20 == 0)
	    cout<<"下载进度:  "<< percent<<endl;

	return 0;
}