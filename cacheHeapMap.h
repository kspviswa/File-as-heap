/*
 * cacheHeapMap.h
 *
 *  Created on: Mar 3, 2014
 *      Author: kspviswanath
 */

#ifndef CACHEHEAPMAP_H_
#define CACHEHEAPMAP_H_

#include <iostream>
#include <pthread.h>
#include <vector>
#include <deque>
#include <map>

using namespace std;

#define uint32 unsigned long
#define int32 signed long
#define byte unsigned char

#define HEAP_OK 0
#define HEAP_NOK -1

class CCacheHeapMapRecord;
class CHeapInfo;
typedef vector<CHeapInfo*> pVECTHEAPINFO;
class CCacheHeapMapFile
{
public :

	int32 initHeap(string sHeapFile, uint32 nSize, uint32 nRecSize, bool bRestore);
	void destroyHeap();
	bool allocateHeapRecord(CCacheHeapMapRecord& hRecord);
	bool giveupHeapRecord(CCacheHeapMapRecord& hRecord);
	bool isHeapConfigured();
	void walkHeap(pVECTHEAPINFO *pVectHeapInfo);

public:
	uint32 nRecordSize;
	uint32 nHeapTotSize;
	uint32 nRecordItems;
	uint32 nSizeLeftOver;
	bool bPerfectAligned;
	uint32 nLastStoredIndex;

	deque<uint32> qVectFreeIdxes;
	pthread_mutex_t lockFreeIdxes;

	map<uint32, CHeapInfo*> mapHeapRecords;
	pthread_mutex_t lockHeapRecords;

	string sMapFile;
	uint32 nMapFileFID;
	byte* pMapFile;

};

/**
 * Class CCacheHeapMapRecord - Class that represents a Heap record on UserSpace
 */
class CCacheHeapMapRecord
{
public:
	CCacheHeapMapFile* pMap;
	CHeapInfo* pHeapInfo;
};



/**
 * Class CHeapInfo - Class that represents information about a Heap record on MapHeap
 * Contains raw data.
 */

class CHeapInfo
{
public :
	uint32 nRecordIndex;
	byte *pData;
};

/*
 * Future Enhancements
 *
 * class CHeapStats; ==> Class to inform about the statistics of Heap
 */

#endif /* CACHEHEAPMAP_H_ */
