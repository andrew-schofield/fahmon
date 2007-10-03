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

#ifndef _BENCHMARKSMANAGER_H
#define _BENCHMARKSMANAGER_H


#include "client.h"
#include "project.h"
#include "wx/thread.h"
#include "benchmark.h"
#include "wx/hashmap.h"
#include "fahmonTypes.h"
#include "workUnitFrame.h"


/**
* Clients are identified by their location, because the latter has less chances to change when compared to the 'nickname'
* To save memory, each benchmark is associated with an integer-based identifier rather than the whole location string
* We thus need to map locations to the integer-based identifier, so we use two hashmaps which must be kept synchronized:
*  - ClientIdHashMap       : Location -> ClientId
*  - ClientLocationHashMap : ClientId -> Location
*
* Another hashmap is used to map a client identifier to a benchmark:
*  - BenchmarkHashMap : ClientId -> Benchmark*
*
* Benchmarks are stored using the corresponding project identifier, so another hashmap is needed:
*  - BenchmarksListHashMap : ProjectId -> BenchmarkHashMap*
*
* Ex: A client ask for the benchmark of a given project:
*      - The integer identifier of the client is retrieved thanks to ClientIdHashMap
*      - The list of benchmarks for the given project is retrieved thanks to BenchmarksListHashMap
*      - The retrieved BenchmarkHashMap is searched for a benchmark for the client identifier
**/
WX_DECLARE_STRING_HASH_MAP(ClientId, ClientIdHashMap);
WX_DECLARE_HASH_MAP(ClientId, wxString, wxIntegerHash, wxIntegerEqual, ClientLocationHashMap);
WX_DECLARE_HASH_MAP(ClientId, Benchmark*, wxIntegerHash, wxIntegerEqual, BenchmarkHashMap);
WX_DECLARE_HASH_MAP(ProjectId, BenchmarkHashMap*, wxIntegerHash, wxIntegerEqual, BenchmarksListHashMap);


// This class stores benchmarks for couples {client,project}
// It's a singleton
class BenchmarksManager
{
protected:
	static wxMutex            mMutexBenchmarksAccess;
	static BenchmarksManager* mInstance;

	ClientId              mNextAvailableClientId;       // Used to distribute integer identifiers to clients
	ClientIdHashMap       mClientLocationToClientId;
	ClientLocationHashMap mClientIdToClientLocation;
	BenchmarksListHashMap mProjectIdToBenchmarks;

	BenchmarksManager(void);
	~BenchmarksManager(void);

	void Save(void);
	void Load(void);


public:
	// Singleton pattern
	static void CreateInstance(void);
	static void DestroyInstance(void);
	static BenchmarksManager* GetInstance(void);

	void Add(ProjectId projectId, const Client* client, const WorkUnitFrame* frame);

	// --- Getters
	wxString          GetClientLocationFromClientId(ClientId clientId);
	ProjectId*        GetBenchmarkedProjects(wxUint32 &nbProjects);
	const Benchmark** GetBenchmarksList(ProjectId projectId, wxUint32 &nbBenchmarks);
	const Benchmark*  GetBenchmark(ProjectId projectId, const Client* client);
};


#endif /* _BENCHMARKSMANAGER_H */
