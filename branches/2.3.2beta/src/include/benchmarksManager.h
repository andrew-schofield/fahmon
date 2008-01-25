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
 * \file benchmarksManager
 * Manages stored benchmarks.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _BENCHMARKSMANAGER_H
#define _BENCHMARKSMANAGER_H


#include "client.h"
#include "project.h"
#include "wx/thread.h"
#include "benchmark.h"
#include "wx/hashmap.h"
#include "fahmonTypes.h"
#include "workUnitFrame.h"


WX_DECLARE_STRING_HASH_MAP(ClientId, ClientIdHashMap); /**< ClientIdHashMap : Location -> ClientId */
WX_DECLARE_HASH_MAP(ClientId, wxString, wxIntegerHash, wxIntegerEqual, ClientLocationHashMap); /**< ClientLocationHashMap : ClientId -> Location */
WX_DECLARE_HASH_MAP(ClientId, Benchmark*, wxIntegerHash, wxIntegerEqual, BenchmarkHashMap); /**< BenchmarkHashMap : ClientId -> Benchmark* */
WX_DECLARE_HASH_MAP(ProjectId, BenchmarkHashMap*, wxIntegerHash, wxIntegerEqual, BenchmarksListHashMap); /**< BenchmarksListHashMap : ProjectId -> BenchmarkHashMap* */


/**
 * This class stores benchmarks for couples {client,project}.
 * This class can only be instantiated once.
 * Clients are identified by their location, because the latter has less chances to change when compared to the 'nickname'
 * To save memory, each benchmark is associated with an integer-based identifier rather than the whole location string,
 * thus locations are mapped to the integer-based identifier using two synchronized hashmaps.
 * \note Ex: A client asks for the benchmark of a given project:
 *     - The integer identifier of the client is retrieved thanks to ClientIdHashMap
 *     - The list of benchmarks for the given project is retrieved thanks to BenchmarksListHashMap
 *     - The retrieved BenchmarkHashMap is searched for a benchmark for the client identifier
 **/
class BenchmarksManager
{
protected:
	static wxMutex            mMutexBenchmarksAccess; /**< Locks access to this class */
	static BenchmarksManager* mInstance; /**< The single instance of the Benchmarks Manager */

	ClientId              mNextAvailableClientId; /**< Used to distribute integer identifiers to clients */
	ClientIdHashMap       mClientLocationToClientId; /**< Hashmap to map location to client ID */
	ClientLocationHashMap mClientIdToClientLocation; /**< Hasmap to map client ID location */
	BenchmarksListHashMap mProjectIdToBenchmarks; /**< Hashmap to map project ID to benchmarks */

	/**
	 * Constructor.
	 * Contructs the BenchmarksManager class.
	 **/
	BenchmarksManager(void);

	/**
	 * Destructor.
	 **/
	~BenchmarksManager(void);

	/**
	 * Save the benchmarks to the disk.
	 **/
	void Save(void);

	/**
	 * Load the benchmarks from the disk
	 **/
	void Load(void);


public:
	// Singleton pattern
	/**
	 * Create the instance of BenchmarksManager.
	 **/
	static void CreateInstance(void);

	/**
	 * Destroy the single instance of the BenchmarksManager.
	 * Only if it exists.
	 **/
	static void DestroyInstance(void);

	/**
	 * Retrieve the instance of BenchmarksManager.
	 * If an instance doesn't already exist, create it.
	 * @return An instance of class BenchmarksManager.
	 **/
	static BenchmarksManager* GetInstance(void);

	/**
	 * Add a new benchmark.
	 * @param projectId The project identifier to add.
	 * @param client The client identifier for the project to add.
	 * @param frame The actual frame time data to add to the benchmarks database.
	 **/
	void Add(ProjectId projectId, const Client* client, const WorkUnitFrame* frame);

	// --- Getters
	/**
	 * Convert an internal ClientId to the corresponding client location.
	 * @param clientId The input client identifier.
	 * @return The client location as a wxString.
	 **/
	wxString          GetClientLocationFromClientId(ClientId clientId);

	/**
	 * Get the projects which have benchmarks associated with them.
	 * Fill the parameter nbProjects and return an array containing the list of ProjectId for which a benchmark is registered
	 * Return NULL if there is no benchmarks at all
	 * In any case, nbBenchmarks is filled with the correct value (thus 0 when NULL is returned)
	 * The array is sorted in ascending order
	 *
	 * The array, if not NULL, must be deleted[] by the caller when needed!
	 * @param &nbProjects Project count.
	 * @return An array of benchmarked projects.
	 **/
	ProjectId*        GetBenchmarkedProjects(wxUint32 &nbProjects);

	/**
	 * Get the list of benchmarks.
	 * Fill the parameter nbBenchmarks and return the list of known benchmarks for the given project
	 * If there is no benchmark at all, return NULL
	 * In any case, nbBenchmarks is filled with the correct value (thus 0 when NULL is returned)
	 *
	 * The array, if not NULL, must be deleted[] by the caller when needed!
	 * @param projectId Project to select in the returned list
	 * @param &nbBenchmarks The benchmarks count.
	 * @return An array of benchmarks.
	 **/
	const Benchmark** GetBenchmarksList(ProjectId projectId, wxUint32 &nbBenchmarks);

	/**
	 * Return the known benchmark for the given couple {project, client}, NULL if none exists.
	 * @param projectId The input project identifier
	 * @param client The input client
	 * @return Benchmark data.
	 **/
	const Benchmark*  GetBenchmark(ProjectId projectId, const Client* client);
};


#endif /* _BENCHMARKSMANAGER_H */
