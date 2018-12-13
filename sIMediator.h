#ifndef IMEDIATOR_H
#define IMEDIATOR_H
struct MySocket;


class CMediator
{
public:
	CMediator() {};
	virtual ~CMediator() {};
public:
	virtual bool Open() = 0;
	virtual void Close() = 0;
	virtual bool SendData(MySocket *pSockex,char* szbuf,int nLen) = 0;
	virtual bool DealData(MySocket *pSockex,char* szbuf) = 0;
};



#endif
