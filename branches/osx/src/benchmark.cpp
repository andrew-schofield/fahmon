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
 * \file benchmark.cpp
 * Creates a class to store benchmarks.
 * \author François Ingelrest
 * \author Andrew Schofield
 **/

#include "fahmon.h"
#include "benchmark.h"

Benchmark::Benchmark(ClientId clientId)
{
	mClientId    = clientId;
	mNbSamples   = 0;
	mMinDuration = MAX_FRAME_DURATION;
	mAvgDuration = 0;
	mInstantDuration = 0;
	m3FrameDuration = 0;
	mFrameDuration1 = 0;
	mFrameDuration2 = 0;
	mFrameDuration3 = 0;
}


Benchmark::~Benchmark(void)
{
}


void Benchmark::Write(DataOutputStream& out) const
{
	out.Write(&mNbSamples, sizeof(mNbSamples));
	out.Write(&mMinDuration, sizeof(mMinDuration));
	out.Write(&mAvgDuration, sizeof(mAvgDuration));
}


void Benchmark::Read(DataInputStream& in)
{
	in.Read(&mNbSamples, sizeof(mNbSamples));
	in.Read(&mMinDuration, sizeof(mMinDuration));
	in.Read(&mAvgDuration, sizeof(mAvgDuration));
}


void Benchmark::AddDuration(FrameDuration duration)
{
	wxUint32 sumOfSamples;

	// 1) Update the average duration
	if(mNbSamples < MAX_NB_SAMPLES)
	{
		// Currently, durations are stored in a wxUint16 and the number of samples in a wxByte
		// This means that the result of this computation cannot exceed the size of a wxUint32
		sumOfSamples = (wxUint32)mAvgDuration * (wxUint32)mNbSamples + (wxUint32)duration;

		// This incrementation ensures that mNbSamples won't be equal to zero in the division below
		++mNbSamples;

		// Re-compute the average value
		mAvgDuration = (FrameDuration)(sumOfSamples / (wxUint32)mNbSamples);
	}

	// 2) Calculate 3 frame rolling average
	mFrameDuration1 = mFrameDuration2;
	mFrameDuration2 = mFrameDuration3;
	mFrameDuration3 = duration;
	m3FrameDuration = duration;

	if(mFrameDuration1 != 0 && mFrameDuration2 != 0 && mFrameDuration3 != 0)
	{
		m3FrameDuration = (mFrameDuration1 + mFrameDuration2 + mFrameDuration3) / 3;
	}

	// 3) Update the minimum duration
	if(mMinDuration == 0 || duration < mMinDuration)
	{
		mMinDuration = duration;
	}

	// instantaneous frame time
	mInstantDuration = duration;
}


void Benchmark::AddEffectiveDuration(FrameDuration duration)
{
	mEffectiveDuration = duration;
}
