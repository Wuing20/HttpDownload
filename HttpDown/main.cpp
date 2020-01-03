#include "DownloadFactory.h"
#include <stdlib.h>

//#define URL "http://www.faw.com.cn/"
//#define URL "http://www.chinaz.com/tags/HTTPwangzhan.shtml"
//#define URL "http://www.zgycrc.com/"
//#define URL "http://www.baidu.com/"
//#define URL "http://jingyan.baidu.com/article/7e440953135d2d2fc0e2efcf.html"
//#define URL "http://139.209.90.30/ws.cdn.baidupcs.com/file/03f85133cb241c57cc17f5baf66b9820?xcode=e38bc1881ff679abc91893c2710fc81ac7f100988f506460837047dfb5e85c39&fid=335809860-250528-571147930138020&time=1410622082&sign=FDTAXER-DCb740ccc5511e5e8fedcff06b081203-cJtuWcK6QQghdq9RC%2F%2F4eJQ39gU%3D&to=cb&fm=Nin,B,U,nc&sta_dx=105&sta_cs=89&sta_ft=mp4&sta_ct=3&newver=1&newfm=1&flow_ver=3&expires=8h&rt=pr&r=588575374&mlogid=1816906087&vuk=335809860&vbdid=140272377&fn=opclass.com-%E4%BA%92%E8%81%94%E7%BD%91%E6%97%B6%E4%BB%A3%E7%AC%AC2%E9%9B%86%EF%BC%9A%E6%B5%AA%E6%BD%AE.mp4&wshc_tag=0&wsts_tag=54146282&wsid_tag=72f52ad0&wsiphost=ipdbm"
//#define URL "http://www.sina.com.cn"
//#define URL "http://ardownload.adobe.com/pub/adobe/reader/win/11.x/11.0.01/en_US/AdbeRdr11001_en_US.exe"
//#define URL "http://www.dlpgg.com/"
//#define URL "http://www.ynjstjgc.com/ynjzyWeb/xxgk/inijgchome.action"
//#define URL "http://one.951368.com:801/xiaobai_v1980.exe"
//#define URL "http://mirrors.cqu.edu.cn/CentOS/7.7.1908/isos/x86_64/CentOS-7-x86_64-DVD-1908.iso"
#define URL "http://mirrors.cqu.edu.cn/CentOS/7.7.1908/isos/x86_64/CentOS-7-x86_64-Everything-1908.iso"

int main(int argc,char* argv[])
{
	/*
	if(argc != 3)
	{
		cerr<<"Parament error! Please Usage <nPart> <Url>"<<endl;

		return -1;
	}

	long nPart = strtol(argv[1],NULL,10);
	string sUrl = argv[2];*/

	long nPart = 10;
	//string sUrl = "http://www.chinaz.com/tags/HTTPwangzhan.shtml";
	string sUrl = URL;

	CDownloadFactory *FactoryObj = new CDownloadFactory;
	CDownPub *pObj = FactoryObj->CreateObj(sUrl);
	//CHttpDown* pObj = FactoryObj->CreateObj(sUrl);

	//pObj->SetFileName("inijgchome.html");
	pObj->DownloadFunc(nPart);


	return 0;
}
