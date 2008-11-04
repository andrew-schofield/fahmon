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
 * \file benchmark.h
 * Creates a class to store benchmarks.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#ifndef _BENCHMARK_H
#define _BENCHMARK_H


#include "fahmonTypes.h"
#include "dataInputStream.h"
#include "dataOutputStream.h"


// We use a byte to store the number of samples
// This can be increase later if needed, but 255 samples should be enough to get a good average value
#define MAX_NB_SAMPLES 255


/**
 * A benchmark class.
 * Stores information about the duration of the current frame,
 * last 3 frames, average frame time and effective frame time.
 **/
class Benchmark
{
protected:
	wxByte        mNbSamples;         /**< Number of samples used for the computation of the average duration */
	ClientId      mClientId;          /**< Id used to uniquely identify a client */
	FrameDuration mMinDuration;       /**< Minimum frame duration for a client/project combination */
	FrameDuration mAvgDuration;       /**< Average frame duration for a client/project combination */
	FrameDuration mInstantDuration;   /**< Last frame duration for a client/project combination */
	FrameDuration m3FrameDuration;    /**< Rolling 3 frame average duration for a client/project combination */
	FrameDuration mFrameDuration1;    /**< Rolling 3 frame duration (1) for a client/project combination */
	FrameDuration mFrameDuration2;    /**< Rolling 3 frame duration (2) for a client/project combination */
	FrameDuration mFrameDuration3;    /**< Rolling 3 frame duration (3) for a client/project combination */
	FrameDuration mEffectiveDuration; /**< Effective duration for a client/project combination */


public:
	/**
	 * Constructor.
	 * Constructs a benchmark class for a particular client.
	 * @param clientId Id of client to create benchmark for.
	 **/
	Benchmark(ClientId clientId);

	/**
	 * Destructor
	 **/
	~Benchmark(void);

	/**
	 * Write benchmark.
	 * Writes the benchmark data into the given data stream.
	 * @param out The output data stream.
	 **/
	void Write(DataOutputStream& out) const;

	/**
	 * Read benchmark.
	 * Reads benchmark data from a given data stream.
	 * @param in The input data stream.
	 **/
	void Read(DataInputStream& in);

	/**
	 * Add a frame duration to the benchmark class.
	 * Updates the average, minimum and R3F durations.
	 * @param duration The frame duration to be added.
	 **/
	void AddDuration(FrameDuration duration);

	/**
	 * Update the effective duration.
	 * @param duration The effective frame duration to be added.
	 **/
	void AddEffectiveDuration(FrameDuration duration);

	// --- Getters
	/**
	 * @return The ClientId for the current benchmark.
	 **/
	ClientId      GetClientId(void)          const {return mClientId;}
	/**
	 * @return The minimum frame duration for the current benchmark.
	 **/
	FrameDuration GetMinDuration(void)       const {return mMinDuration;}
	/**
	 * @return The average frame duration for the current benchmark.
	 **/
	FrameDuration GetAvgDuration(void)       const {return mAvgDuration;}
	/**
	 * @return The last frame duration for the current benchmark.
	 **/
	FrameDuration GetInstantDuration(void)   const {return mInstantDuration;}
	/**
	 * @return The rolling 3 frame average duration for the current benchmark.
	 **/
	FrameDuration Get3FrameDuration(void)    const {return m3FrameDuration;}
	/**
	 * @return The effective frame duration for the current benchmark.
	 **/
	FrameDuration GetEffectiveDuration(void) const {return mEffectiveDuration;};
};


#endif /* _BENCHMARK_H */
