httpServer.h/httpServer.h-Created-on-Aug-20-2014-Author-kspviswanath/  #ifndef HTTPSERVER_H_ #define HTTPSERVER_H_  #include <iostream> #include <sstream> #include <fstream>  using namespace std;  class CHtmlTemplate { public:     string sFile;     ifstream *pInStream; };  class CHttpSession { public:     CHttpServer *pServer;     unsigned long nSessId;     int nConnFd;     struct sockaddr_in cliaddr;     CHtmlTemplate *pFile;     string sRawMsg; };  class CHttpServer { public:      CHttpServer();      CHttpServer(string sIP, int nPort, string strDocPath)     {