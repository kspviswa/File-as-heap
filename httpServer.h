/*
 * httpServer.h
 *
 *  Created on: Aug 20, 2014
 *      Author: kspviswanath
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

class CHtmlTemplate
{
public:
        string sFile;
        ifstream *pInStream;
};

class CHttpSession
{
public:
        CHttpServer *pServer;
        unsigned long nSessId;
        int nConnFd;
        struct sockaddr_in cliaddr;
        CHtmlTemplate *pFile;
        string sRawMsg;
};

class CHttpServer
{
public:

        CHttpServer();

        CHttpServer(string sIP, int nPort, string strDocPath)
        {
                CHttpServer();
                initServer(sIP, nPort, strDocPath);
        }

        CHttpServer(string sIniFile)
        {
                CHttpServer();
                loadConfigFromIni(sIniFile);
        }


        int initServer(string sIP, int nPort, string strDocPath);
        int loadConfigFromIni(string sIniFile);
        void loadDocuments();

        void startService();

        int parseDocRoot(string sPath);

        int endService(unsigned long nSecWait);

        void setMaxConnections(unsigned long nMax)
        {
                nMaxAcceptConnections = nMax;
        }

        unsigned long getMaxConnections()
        {
                return nMaxAcceptConnections;
        }

        string GetServerIPChars()
        {
                return sIP;
        }

        bool InsertHttpSession(CHttpSession *pSession)
        {
                if(pSession && vectHttpSessions.size() < nMaxAcceptConnections)
                {
                        vectHttpSessions.push_back(pSession);
                        pSession->nSessId = pSession && vectHttpSessions.size();
                        return true;
                }

                return false;
        }

private:
        string sIP;
        string sDocRoot;
        string sIniFile;
        unsigned long nPort;
        unsigned long nMaxAcceptConnections;
        bool bThreaded;

public:
        vector<CHttpSession*> vectHttpSessions;
        map<string, CHtmlTemplate*> mapDoc;
};

#endif /* HTTPSERVER_H_ */
