/*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Library General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/**
 * \file benchmarksManager.cpp
 * Manages stored benchmarks.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "benchmarksManager.h"

#include "tools.h"
#include "pathManager.h"
#include "clientsManager.h"
#include "messagesManager.h"
#include "dataInputStream.h"
#include "dataOutputStream.h"


// The single instance of BenchmarksManager accross the application
BenchmarksManager* BenchmarksManager::mInstance = NULL;

// Protects from multiple access from different threads
// This way, a benchamrk can't be accessed when something is being modified by another thread
wxMutex BenchmarksManager::mMutexBenchmarksAccess;


/**
* Perform a quick sort on the given array.
* @param array[] Array to sort
* @param lowerBound Lowest item in array to sort by
* @param upperBound Highest item in array to sort by
**/
void SortArrayOfProjectId(ProjectId array[], wxInt32 lowerBound, wxInt32 upperBound)
{
	wxInt32  lo;
	wxInt32  hi;
	ProjectId pivot;
	ProjectId tmp;

	lo    = lowerBound;
	hi    = upperBound;
	pivot = array[(lowerBound + upperBound) / 2];

	while(lo <= hi)
	{
		while(lo < upperBound && array[lo] < pivot)
		{
			++lo;
		}

		while(hi > lowerBound && array[hi] > pivot)
		{
			--hi;
		}

		if(lo <= hi)
		{
			tmp       = array[lo];
			array[lo] = array[hi];
			array[hi] = tmp;
			++lo;
			--hi;
		}
	}

	if(hi > lowerBound)
	{
		SortArrayOfProjectId(array, lowerBound, hi);
	}

	if(lo < upperBound)
	{
		SortArrayOfProjectId(array, lo, upperBound);
	}
}


BenchmarksManager::BenchmarksManager(void)
{
	mNextAvailableClientId = 0;
}


BenchmarksManager::~BenchmarksManager(void)
{
}


void BenchmarksManager::CreateInstance(void)
{
	wxASSERT(mInstance == NULL);

	mInstance = new BenchmarksManager();
	mInstance->Load();
}


void BenchmarksManager::DestroyInstance(void)
{
	wxASSERT(mInstance != NULL);

	mInstance->Save();

	delete mInstance;
	mInstance = NULL;
}


BenchmarksManager* BenchmarksManager::GetInstance(void)
{
	wxASSERT(mInstance != NULL);

	return mInstance;
}


void BenchmarksManager::Save(void)
{
	DataOutputStream                 out(PathManager::GetCfgPath() + wxT(FMC_FILE_BENCHMARKS));
	BenchmarkHashMap                *listOfBenchmarks;
	ClientIdHashMap::iterator        iteratorClientId;
	BenchmarkHashMap::iterator       iteratorBenchmark;
	BenchmarksListHashMap::iterator  iteratorBenchmarksList;


	if(out.Ok() == false)
	{
		Tools::ErrorMsgBox(wxString::Format(_("Could not open file <%s> for writing!"), (PathManager::GetCfgPath() + wxT(FMC_FILE_BENCHMARKS)).c_str()));
		return;
	}

	// First, write all the 'registered' clients
	out.WriteUint(mClientLocationToClientId.size());
	for(iteratorClientId=mClientLocationToClientId.begin(); iteratorClientId!=mClientLocationToClientId.end(); ++iteratorClientId)
	{
		out.WriteString(iteratorClientId->first);
		out.Write(&iteratorClientId->second, sizeof(ClientId));
	}

	// Then for all projects, and all clients, the associated benchmark
	out.WriteUint(mProjectIdToBenchmarks.size());
	for(iteratorBenchmarksList=mProjectIdToBenchmarks.begin(); iteratorBenchmarksList!=mProjectIdToBenchmarks.end(); ++iteratorBenchmarksList)
	{
		out.Write(&iteratorBenchmarksList->first, sizeof(ProjectId));
		listOfBenchmarks = iteratorBenchmarksList->second;

		out.WriteUint(listOfBenchmarks->size());
		for(iteratorBenchmark=listOfBenchmarks->begin(); iteratorBenchmark!=listOfBenchmarks->end(); ++iteratorBenchmark)
		{
			out.Write(&iteratorBenchmark->first, sizeof(ClientId));
			iteratorBenchmark->second->Write(out);
		}
	}
}


void BenchmarksManager::Load(void)
{
	wxUint32          i;
	wxUint32          j;
	wxUint32          nbProjects;
	wxUint32          nbBenchmarks;
	wxUint32          nbRegisteredClients;
	wxString          clientLocation;
	ClientId          clientId;
	ProjectId         projectId;
	Benchmark        *benchmark;
	DataInputStream   in(PathManager::GetCfgPath() + wxT(FMC_FILE_BENCHMARKS));
	BenchmarkHashMap *listOfBenchmarks;

	// Could the file be opened ?
	if(in.Ok() == false)
	{
		return;
	}

	// The registered clients
	in.ReadUint(nbRegisteredClients);
	for(i=0; i<nbRegisteredClients; ++i)
	{
		// Read a client
		in.ReadString(clientLocation);
		in.Read(&clientId, sizeof(clientId));

		// And insert it in the hashmaps
		mClientLocationToClientId[clientLocation] = clientId;
		mClientIdToClientLocation[clientId]       = clientLocation;

		// Update the next available new client identifier if needed
		if(clientId >= mNextAvailableClientId)
		{
			if(clientId < MAX_CLIENT_ID)
			{
				mNextAvailableClientId = clientId + 1;
			}
			else
			{
				mNextAvailableClientId = MAX_CLIENT_ID;
			}
		}
	}

	// The known projects and each benchmark
	in.ReadUint(nbProjects);
	for(i=0; i<nbProjects; ++i)
	{
		listOfBenchmarks = new BenchmarkHashMap();

		in.Read(&projectId, sizeof(projectId));
		in.ReadUint(nbBenchmarks);

		for(j=0; j<nbBenchmarks; ++j)
		{
			in.Read(&clientId, sizeof(clientId));
			benchmark = new Benchmark(clientId);
			benchmark->Read(in);

			(*listOfBenchmarks)[clientId] = benchmark;
		}

		mProjectIdToBenchmarks[projectId] = listOfBenchmarks;
	}
}


const Benchmark* BenchmarksManager::GetBenchmark(ProjectId projectId, const Client* client)
{
	wxMutexLocker mutexLocker(mMutexBenchmarksAccess);  // Lock access to this method

	ClientId                         clientId;
	BenchmarkHashMap                *listOfBenchmarks;
	ClientIdHashMap::iterator        iteratorClientId;
	BenchmarkHashMap::iterator       iteratorBenchmark;
	BenchmarksListHashMap::iterator  iteratorBenchmarksList;

	// Do we have some benchmarks for the given project?
	iteratorBenchmarksList = mProjectIdToBenchmarks.find(projectId);
	if(iteratorBenchmarksList == mProjectIdToBenchmarks.end())
	{
		return NULL;
	}
	listOfBenchmarks = iteratorBenchmarksList->second;

	// Find the identifier associated with this client
	iteratorClientId = mClientLocationToClientId.find(client->GetLocation());
	if(iteratorClientId == mClientLocationToClientId.end()){
		return NULL;
	}
	clientId = iteratorClientId->second;

	// There is at least one known benchmark, try to see if we have one for the given client
	iteratorBenchmark = listOfBenchmarks->find(clientId);
	if(iteratorBenchmark == listOfBenchmarks->end()){
		return NULL;
	}

	return (const Benchmark*)iteratorBenchmark->second;
}


void BenchmarksManager::Add(ProjectId projectId, const Client* client, const WorkUnitFrame* frame)
{
	wxMutexLocker mutexLocker(mMutexBenchmarksAccess);  // Lock access to this method

	ClientId                         clientId;
	Benchmark                       *benchmark;
	BenchmarkHashMap                *listOfBenchmarks;
	ClientIdHashMap::iterator        iteratorClientId;
	BenchmarkHashMap::iterator       iteratorBenchmark;
	BenchmarksListHashMap::iterator  iteratorBenchmarksList;

	// --- Retrieve the identifier associated to the given client, create a new one if needed
	iteratorClientId = mClientLocationToClientId.find(client->GetLocation());
	if(iteratorClientId != mClientLocationToClientId.end())
	{
		clientId = iteratorClientId->second;
	}
	else
	{
		// Stop before the limit
		if(mNextAvailableClientId == MAX_CLIENT_ID)
		{
			_LogMsgError(_("(Benchmarks) The maximum number of known clients has been reached!"));
			return;
		}

		// Assign a new identifier
		clientId                                         = mNextAvailableClientId;
		mClientLocationToClientId[client->GetLocation()] = clientId;
		mClientIdToClientLocation[clientId]              = client->GetLocation();

		++mNextAvailableClientId;
	}

	// --- Try to retrieve the already known benchmarks for the considered project and client, if any
	iteratorBenchmarksList = mProjectIdToBenchmarks.find(projectId);
	if(iteratorBenchmarksList != mProjectIdToBenchmarks.end())
	{
		listOfBenchmarks  = iteratorBenchmarksList->second;
		iteratorBenchmark = listOfBenchmarks->find(clientId);
		if(iteratorBenchmark != listOfBenchmarks->end())
		{
			benchmark = iteratorBenchmark->second;
		}
		else
		{
			benchmark = new Benchmark(clientId);
		}
	}
	else
	{
		// This hashtable stores all the benchmarks for the known clients, so it doesn't have to be too large
		listOfBenchmarks = new BenchmarkHashMap(ClientsManager::GetInstance()->GetCount()*3);
		benchmark        = new Benchmark(clientId);
	}

	// --- Add the new benchmarked frame
	benchmark->AddDuration(frame->GetDuration());
	benchmark->AddEffectiveDuration(frame->GetEffectiveDuration());

	// --- So far, so good
	// Store the modified benchmark back into the hashtable, and the hashtable itself into the main hashtable
	(*listOfBenchmarks)[clientId]     = benchmark;
	mProjectIdToBenchmarks[projectId] = listOfBenchmarks;
}


ProjectId* BenchmarksManager::GetBenchmarkedProjects(wxUint32 &nbProjects)
{
	wxMutexLocker mutexLocker(mMutexBenchmarksAccess);  // Lock access to this method

	wxUint32                         currentIndex;
	ProjectId                       *projects;
	BenchmarksListHashMap::iterator  iterator;

	// Are there some benchmarks?
	nbProjects = mProjectIdToBenchmarks.size();
	if(nbProjects == 0)
	{
		return NULL;
	}

	// Ok, we can fill an array with all the projects
	projects     = new ProjectId[nbProjects];
	currentIndex = 0;

	for(iterator = mProjectIdToBenchmarks.begin(); iterator != mProjectIdToBenchmarks.end(); ++iterator)
	{
		projects[currentIndex] = iterator->first;
		++currentIndex;
	}

	SortArrayOfProjectId(projects, 0, nbProjects-1);

	return projects;
}


const Benchmark** BenchmarksManager::GetBenchmarksList(ProjectId projectId, wxUint32 &nbBenchmarks)
{
	wxMutexLocker mutexLocker(mMutexBenchmarksAccess);  // Lock access to this method

	wxUint32                          currentBenchmark;
	const Benchmark                 **benchmarks;
	BenchmarkHashMap                 *listOfBenchmarks;
	BenchmarkHashMap::iterator        iteratorBenchmark;
	BenchmarksListHashMap::iterator   iteratorBenchmarksList;

	// TODO
	// Sort this list

	// Do we have some benchmarks for this project?
	iteratorBenchmarksList = mProjectIdToBenchmarks.find(projectId);
	if(iteratorBenchmarksList == mProjectIdToBenchmarks.end())
	{
		nbBenchmarks = 0;
		return NULL;
	}

	// Verify that there are indeed some benchmarks
	// This should not happen, but code can change in the future
	listOfBenchmarks = iteratorBenchmarksList->second;
	nbBenchmarks     = listOfBenchmarks->size();
	if(nbBenchmarks == 0)
	{
		return NULL;
	}

	// Ok, we can fill the array
	benchmarks       = new const Benchmark*[nbBenchmarks];
	currentBenchmark = 0;

	for(iteratorBenchmark = listOfBenchmarks->begin(); iteratorBenchmark != listOfBenchmarks->end(); ++iteratorBenchmark)
	{
		benchmarks[currentBenchmark] = iteratorBenchmark->second;
		++currentBenchmark;
	}

	return benchmarks;
}


wxString BenchmarksManager::GetClientLocationFromClientId(ClientId clientId)
{
	wxMutexLocker mutexLocker(mMutexBenchmarksAccess);  // Lock access to this method

	ClientLocationHashMap::iterator iterator = mClientIdToClientLocation.find(clientId);

	// This should not happen!
	if(iterator == mClientIdToClientLocation.end())
	{
		wxASSERT(false);
		return _("Unknown client");
	}

	return iterator->second;
}
