/*
 * cacheHeapMap.cpp
 *
 *  Created on: Mar 3, 2014
 *      Author: kspviswanath
 */

#include "cacheHeapMap.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <stdio.h>

int32 CCacheHeapMapFile::initHeap(string sHeapFile, uint32 nSize, uint32 nRecSize, bool bRestore=false)
{
	int32 nRet = HEAP_NOK;

	this->sMapFile = sHeapFile;
	this->nHeapTotSize = nSize;
	this->nRecordSize = nRecSize;

	// Tot Items = Total Heap / space for each item
	this->nRecordItems = (this->nHeapTotSize / this->nRecordSize);

	// Unused space = Total Heap - Used Space
	this->nSizeLeftOver = (this->nHeapTotSize - (this->nRecordItems * this->nRecordSize));
	if(!this->nSizeLeftOver)
	{
		// Good Configuration
		this->bPerfectAligned = true;
	}

	// Now proceed to create a file and MMAP it.
	this->nMapFileFID = open(this->sMapFile.c_str(), O_RDWR);

	// error check
	if(this->nMapFileFID < 0)
		return nRet;

	if(!bRestore)
	{
		// Grow it to the specified size
		ftruncate(this->nMapFileFID, this->nHeapTotSize);
	}

	// Attach to this process
	this->pMapFile = (byte*)mmap(0, this->nHeapTotSize, PROT_READ|PROT_WRITE, MAP_SHARED, this->nMapFileFID, 0);
	if(this->pMapFile == MAP_FAILED)
	{
		char sz[256];
		perror(sz);
		cout << "MMAP failed due to" << sz << endl;
		close(this->nMapFileFID);
		remove(this->sMapFile.c_str());
		return nRet;
	}

	// Now proceed to create the HeapStore
	pthread_mutex_init(&this->lockFreeIdxes, NULL);
	pthread_mutex_init(&this->lockHeapRecords, NULL);

	// First populate all indexes of the custom heap to free indexes
	pthread_mutex_lock(&this->lockFreeIdxes);
	{
		uint32 nRecId;
		for(nRecId = 1; nRecId <= this->nRecordItems; nRecId++)
		{
			this->qVectFreeIdxes.push_back(nRecId);
		}
		this->nLastStoredIndex = 0;
	}
	pthread_mutex_unlock(&this->lockFreeIdxes);

	// Populate Heap and build a internal DS.
	pthread_mutex_lock(&this->lockHeapRecords);
	{
		nRet = HEAP_OK;
		byte* nHeapCurr = this->pMapFile;
		byte* nHeapEnd = nHeapCurr + this->nHeapTotSize;
		uint32 nHeapHopSize = this->nRecordSize;
		uint32 nRecId = 0;
		string sFree = "free";

		while(nHeapCurr < nHeapEnd)
		{
			if((nHeapCurr+nHeapHopSize) > nHeapEnd)
			{
				nRet = HEAP_NOK;
				break;
			}
			CHeapInfo *ptemp = new CHeapInfo();
			nRecId++;
			ptemp->nRecordIndex = nRecId;
			ptemp->pData = nHeapCurr;

			nHeapCurr+=nHeapHopSize;
			this->mapHeapRecords.insert(make_pair(nRecId, ptemp));

			// check for free
			char szData[10];
			memcpy(szData, ptemp->pData, sFree.length());
			string sData(szData, sFree.length());
			if(sFree == sData)
			{

			}
		}
	}
	pthread_mutex_unlock(&this->lockHeapRecords);

	return nRet;
}

void CCacheHeapMapFile::destroyHeap()
{
	// Time to go home :-)
	pthread_mutex_lock(&this->lockFreeIdxes);
	this->qVectFreeIdxes.clear();
	pthread_mutex_unlock(&this->lockFreeIdxes);

	// Get rid of custom heap
	pthread_mutex_lock(&this->lockHeapRecords);
	{
		map<uint32, CHeapInfo*>::iterator it;
		for(it = this->mapHeapRecords.begin(); it != this->mapHeapRecords.end(); ++it)
		{
			delete it->second;
		}

		this->mapHeapRecords.clear();
	}
	pthread_mutex_unlock(&this->lockHeapRecords);

	// undo the map
	munmap(this->pMapFile, this->nHeapTotSize);
	remove(this->sMapFile.c_str());
}

bool CCacheHeapMapFile::allocateHeapRecord(CCacheHeapMapRecord& hRecord)
{
	bool bMemEmpty = false;
	uint32 nRecId;
	map<uint32, CHeapInfo*>::iterator it;

	hRecord.pMap = this;

	pthread_mutex_lock(&this->lockFreeIdxes);
	bMemEmpty = this->qVectFreeIdxes.empty();
	if(!bMemEmpty)
	{
		nRecId = this->qVectFreeIdxes.front();
		this->qVectFreeIdxes.pop_front();
	}
	pthread_mutex_unlock(&this->lockFreeIdxes);

	if(bMemEmpty)
	{
		// No memory as of now :-(
		hRecord.pHeapInfo = NULL;
		return false;
	}

	pthread_mutex_lock(&this->lockHeapRecords);
	it = this->mapHeapRecords.find(nRecId);
	pthread_mutex_unlock(&this->lockHeapRecords);

	if(it == this->mapHeapRecords.end())
	{
		// No useful info
		hRecord.pHeapInfo = NULL;
		return false;
	}

	hRecord.pHeapInfo = it->second;
	return true;
}

bool CCacheHeapMapFile::giveupHeapRecord(CCacheHeapMapRecord& hRecord)
{
	map<uint32, CHeapInfo*>::iterator it;

	pthread_mutex_lock(&this->lockHeapRecords);
	it = this->mapHeapRecords.find(hRecord.pHeapInfo->nRecordIndex);
	if(it != this->mapHeapRecords.end())
	{
		string sFree="free";
		memset(it->second->pData, 0, this->nRecordSize);
		memcpy(it->second->pData, const_cast<char*>(sFree.c_str()), sFree.length());
	}
	pthread_mutex_unlock(&this->lockHeapRecords);

	if(it == this->mapHeapRecords.end())
	{
		// Wrong data. Silently discard
		// Todo : Bring in more communication here
		return false;
	}

	pthread_mutex_lock(&this->lockFreeIdxes);
	this->qVectFreeIdxes.push_back(hRecord.pHeapInfo->nRecordIndex);
	pthread_mutex_unlock(&this->lockFreeIdxes);

	return true;
}

void CCacheHeapMapFile::walkHeap(pVECTHEAPINFO *pVectHeapInfo)
{
	if(pVectHeapInfo)
	{
		map<uint32, CHeapInfo*>::iterator it;
		pthread_mutex_lock(&this->lockHeapRecords);

		for(it = this->mapHeapRecords.begin(); it != this->mapHeapRecords.end(); ++it)
		{
			pVectHeapInfo->push_back(it->second);
		}
	}
}

bool CCacheHeapMapFile::isHeapConfigured()
{
	if(this->pMapFile == MAP_FAILED || this->sMapFile.empty() || this->qVectFreeIdxes.empty() || this->mapHeapRecords.empty())
		return false;
	else
		return true;
}
