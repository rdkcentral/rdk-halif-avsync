/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2023 RDK Management
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


void* avsync_soc_init( int refreshRate, int syncType, int *sessionId, int* session );
void avsync_soc_term( void* sync, int session );
int avsync_soc_set_mode( void* sync, int type );
void avsync_soc_free_frame_callback( void* sync, AVSyncFrameFree *freeCB );
bool avsync_soc_push_frame( void* sync, AVSyncVideoFrame *f );
AVSyncVideoFrame* avsync_soc_pop_frame(void* sync );
void avsync_soc_set_rate( void* sync, float rate );
void avsync_soc_pause( void* sync, bool pause );
void avsync_soc_set_interval( void* sync, uint32_t interval );
void avsync_soc_eos( void* sync );

#endif
