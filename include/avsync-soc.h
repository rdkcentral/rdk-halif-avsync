/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __WESTEROS_GL_AVSYNC_H__
#define __WESTEROS_GL_AVSYNC_SOC_H__

#include <stdint.h>
#include <stdbool.h>

#define WESTEROS_GL_AVSYNC

#define USE_AVSYNC_SOC

typedef struct AVSyncVideoFrame_
{
    void*       context;
    void*       sync;
    void*       videoFrame;
    void*       syncFrame;
    uint32_t    pts;
    uint32_t    duration;

} AVSyncVideoFrame;

// Frame delete callback prototype
typedef void (*AVSyncFrameFree)(AVSyncVideoFrame * frame);

/**
 * This function initializes AV Sync session and returns a pointer to an AVSync object used for audio-video synchronization.
 * The function takes four arguments:
 * 1. refreshRate: The display refresh rate in hertz.
 * 2. syncType: The type of syncronization to use. 
 * 3. sessionId: A pointer to an integer that will be set to the ID of the avsync session.
 * 4. session: A pointer to an integer that will be set to the file descriptor of the avsync session.
*/
void* avsync_soc_init( int refreshRate, int syncType, int *sessionId, int* session );

/**
 * This function is used to terminate and clean up resources associated with an AVSync object.
 * The function takes two arguments:
 * sync: A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * session: The file descriptor of the avsync session.
*/
void avsync_soc_term( void* sync, int session );

/**
 * This function is used to set the synchronization mode for an AVSync object.
 * The function takes two arguments:
 * sync: A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * type: The new mode to set. 
*/
int avsync_soc_set_mode( void* sync, int type );

/**
 * This function is used to set the frame free callback function for an AVSync object.
 *  The function takes two arguments:
 * sync: A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * freeCB: A pointer to the AVSyncFrameFree structure.
*/
void avsync_soc_free_frame_callback( void* sync, AVSyncFrameFree *freeCB );

/**
 * This function is used to push a video frame to the AVSync module for synchronization.
 * The function takes two arguments:
 * sync: A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * f: A pointer to the AVSyncVideoFrame structure that contains the frame data.
*/
bool avsync_soc_push_frame( void* sync, AVSyncVideoFrame *f );

/**
 * This function is used to pop/retrieve a video frame from the AVSync module's frame queue.
 * The function takes 'sync' as an argument: A pointer to the AVSync structure that was created by the avsync_soc_init() function.
*/
AVSyncVideoFrame* avsync_soc_pop_frame(void* sync );

/**
 * This function is used to set the playback rate of an AVSync object.
 * The function takes two arguments:
 * The first argument, sync, is a pointer to the AVSync object. 
 * The second argument, rate, is the playback rate,
*/
void avsync_soc_set_rate( void* sync, float rate );

/**
 * This function is used to pause or resume playback of an AVSync object.
 * The function takes two arguments:
 * The first argument, sync, is a pointer to the AVSync object. 
 * The second argument, pause, is a boolean value that indicates whether to pause (true) or resume (false) playback.
*/
void avsync_soc_pause( void* sync, bool pause );

/**
 * This function is used to set the interval between vertical synchronization (vsync interval) events for the AVSync module.
 * The function takes two arguments:
 * sync: A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * interval: interval of type uint32_t, representing the desired interval between vsync events.
*/
void avsync_soc_set_interval( void* sync, uint32_t interval );

/**
 * This function is used to signal the end of stream (EOS) to the AVSync module.
 * The function takes 'sync' as an argument: A pointer to the AVSync structure that was created by the avsync_soc_init() function.
*/
void avsync_soc_eos( void* sync );

#endif
