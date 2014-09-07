/*
 * testHeap.cpp
 *
 *  Created on: Mar 4, 2014
 *      Author: kspviswanath
 */

#include "cacheHeapMap.h"
#include <iostream>
#include <vector>
#include <string.h>
#include "stdlib.h" // for rand
#include <iomanip> // for setw
using namespace std;

struct testData
{
	uint32 nInt32;
	double dDouble;
	//string sString;
};

typedef struct testData TESTDATA;

int main()
{
	string sHeapFile;
	CCacheHeapMapFile theCache;
	uint32 nTotSize, nRecSize = sizeof(TESTDATA);
	vector<CCacheHeapMapRecord> vecMap;

	cout << "Enter the heapfile path" << endl;
	cin >> sHeapFile;

	cout << "Enter number of records to be cached" << endl;
	cin >> nTotSize;

	nTotSize = nTotSize * nRecSize;

	cout << "Attempt to create the cache" << endl;

	if(theCache.initHeap(sHeapFile, nTotSize, nRecSize, false) < 0)
	{
		cout << "Unable to create cache file" << endl;
		return -1;
	}

	cout << "Grow the cache [only 40. Leaving 10]" << endl;

	for(int i=0; i< 40; i++)
	{
		CCacheHeapMapRecord theRecord;

		theCache.allocateHeapRecord(theRecord);
		vecMap.push_back(theRecord);

		// Now interpret this as THEDATA
		TESTDATA *theData = (TESTDATA*)theRecord.pHeapInfo->pData;

		theData->dDouble = rand();
		theData->nInt32 = rand();
		//theData->sString.assign("Random");

	}

	cout << "Lets walk throught cache" << endl;

	pVECTHEAPINFO pVect;
	theCache.walkHeap(&pVect);

	pVECTHEAPINFO::iterator it_p_vect;

	for(it_p_vect = pVect.begin(); it_p_vect != pVect.end(); ++it_p_vect)
	{
		CHeapInfo *pInfo = *it_p_vect;
		TESTDATA *theData = reinterpret_cast<TESTDATA*>(pInfo->pData);
		cout << "+++++++++++++++++++++++++++++++++++++++++" << endl;
		cout << "+" << std::setw(20) << " ID " << std::setw(2) << "|" << std::setw(14) << pInfo->nRecordIndex << std::setw(4) << "+" << endl;
		cout << "+" << std::setw(20) << " Double" << std::setw(2) << "|" << std::setw(14) << theData->dDouble << std::setw(4) << "+" << endl;
		cout << "+" << std::setw(20) <<  "nInt32" << std::setw(2) << "|" << std::setw(14) << theData->nInt32 << std::setw(4) << "+" << endl;
		//cout << "+ sString = " << theData->sString << "+" << endl;
		cout << "+++++++++++++++++++++++++++++++++++++++++" << endl;
		cout << endl;
	}

	return 0;
}
