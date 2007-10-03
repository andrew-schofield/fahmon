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
 
#ifndef _BENCHMARK_H
#define _BENCHMARK_H


#include "fahmonTypes.h"
#include "dataInputStream.h"
#include "dataOutputStream.h"


// We use a byte to store the number of samples
// This can be increase later if needed, but 255 samples should be enough to get a good average value
#define MAX_NB_SAMPLES 255


/**
 * Store information about the duration of a frame
**/
class Benchmark
{
protected:
    wxByte        mNbSamples;        // Number of samples used for the computation of the average duration
    ClientId      mClientId;
    FrameDuration mMinDuration;
    FrameDuration mAvgDuration;


public:
     Benchmark(ClientId clientId);
    ~Benchmark(void);

    void Write(DataOutputStream& out) const;
    void Read(DataInputStream& in);

    void AddDuration(FrameDuration duration);

    // --- Getters
    ClientId      GetClientId(void)    const {return mClientId;}
    FrameDuration GetMinDuration(void) const {return mMinDuration;}
    FrameDuration GetAvgDuration(void) const {return mAvgDuration;}
};


#endif /* _BENCHMARK_H */
