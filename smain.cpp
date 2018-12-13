#include <iostream>
#include "TCPMediator.h"
using namespace std;




int main()
{



	CTCPMediator *p =new  CTCPMediator;
	p->Open();
	cout<<"server is running"<<endl;
	
	//p->Open();
	while(1)
	{
		::Sleep(10000);
	}
	system("pause");
	return 0;
}

