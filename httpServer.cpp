/*
 * httpServer.cpp
 *
 *  Created on: Aug 20, 2014
 *      Author: kspviswanath
 */

#include "httpServer.h"

CHttpServer::CHttpServer()
{
        bThreaded = 0;
        sIP = "127.0.0.1";
        sDocRoot = ".";
        sIniFile = "./httpserver.ini";
        nPort = 6060;
        nMaxAcceptConnections = 1;
        bThreaded = false;
}

int CHttpServer::initServer(string sIP, int nPort, string strDocPath)
{
        this->sIP = sIP;
        this->nPort = nPort;
        this->sDocRoot = strDocPath;

        this->mapDoc.clear();
        this->vectHttpSessions.clear();

        parseDocRoot(this->sDocRoot);

        return 0;
}
