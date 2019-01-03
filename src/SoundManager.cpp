// Kosmos Updater
// Copyright (C) 2018 Steven Mattera
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "SoundManager.hpp"
#include <malloc.h>
#include <mpg123.h>

#define swapbuf (curBuf = (curBuf+1)%(BUF_COUNT))

bool SoundManager::initialize() {
    mutexInit(&_mutex);
	
	_buffSize = 80256;
    for(int curBuf = 0; curBuf < BUF_COUNT; curBuf++) {
        _buffData[curBuf] = (u8 *) memalign(0x1000, _buffSize);
    }

    audoutInitialize();
    audoutStartAudioOut();

    int err = 0;
	if ((err = mpg123_init()) != MPG123_OK)
		return false;

    return true;
}

void SoundManager::dealloc() {
    mpg123_exit();
    audoutStopAudioOut();
    audoutExit();
}

void SoundManager::playSound(Sound * sound) {
    mutexLock(&_mutex);

    u32 released_count = 0;
    int toPlayCount = 0;
    int lastFill = 1;

    while (appletMainLoop() && lastFill) {
	    for (int curBuf = 0; curBuf < BUF_COUNT / 2; curBuf++) {
            lastFill = _fillBuf(sound);
			toPlayCount += lastFill > 0;
		}

        for (int curBuf = 0; curBuf < BUF_COUNT/2 && toPlayCount--; curBuf++) {
            audoutWaitPlayFinish(&_audoutReleasedBuf, &released_count, 1000000000L);
        }
    }

    while (toPlayCount--) {
        audoutWaitPlayFinish(&_audoutReleasedBuf, &released_count, 1000000000L);
    }

    mutexUnlock(&_mutex);
}

int SoundManager::_fillBuf(Sound * sound) {
    return 0;
}
