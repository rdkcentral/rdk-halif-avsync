# AV Sync Documentation

## Version History

| Date [DD/MM/YY] | Comment | Version |
| --- | --- | --- |
| 29/07/23 | First Release | 1.0.0 |


## Table of Contents

- [Description](#description)
  - [Introduction](#introduction)
  - [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Component Runtime Execution Requirements](#component-runtime-execution-requirements)
  - [Initialization and Startup](#initializatio-and-startup)
  - [Threading Model](#threading-model)
  - [Process Model](#process-model)
  - [Memory Model](#memory-model)
  - [Power Management Requirements](#power-management-requirements)
  - [Asynchronous Notification Model](#asynchronous-notification-model)
  - [Blocking calls](#blocking-calls)
  - [Internal Error Handling](#internal-error-handling)
  - [Persistence Model](#persistence-model)
- [Non-functional requirements](#non-functional-requirements)
  - [Logging and debugging requirements](#logging-and-debugging-requirements)
  - [Memory and performance requirements](#memory-and-performance-requirements)
  - [Quality Control](#quality-control)
  - [Licensing](#licensing)
  - [Build Requirements](#build-requirements)
  - [Variability Management](#variability-management)
  - [Platform or Product Customization](#platform-or-product-customization)
- [Interface API Documentation](#interface-api-documentation)
  - [Theory of operation and key concepts](#theory-of-operation-and-key-concepts)
  - [Diagrams](#diagrams)
    - [AVSync Code flow](#avsync-code-flow)
  - [Data Structures and Defines](#data-structures-and-defines)


## Description
`AV Sync` `HAL` provides an interface between the `Caller` and `SoC` for audio and video synchronization in real time. The picture below shows the interactions between `Caller`, `AV Sync` `HAL` and `SoC` `AV Sync`.

```mermaid

flowchart TD
A[Caller] <--> B[AV Sync HAL] <--> C[SoC AV Sync]

style A fill:#99CCFF,stroke:#333
style B fill:#ffa,stroke:#333
style C fill:#fcc,stroke:#333
```

### Introduction
`AV Sync` module is a software component that enables synchronization between audio and video frames in real-time media applications. This module is particularly important in media playback scenarios where the audio and video streams faces synchronization issues.

### Acronyms, Terms and Abbreviations

- `AV Sync`      - Audio Video Synchronization
- `HAL`          - Hardware Abstraction Layer
- `API`          - Application Programming Interface
- `Caller`       - Any user of the interface via the APIs
- `SoC`          - System on Chip
- `IOCTL`        - Input-Output Control


## Component Runtime Execution Requirements
These requirements ensure that the `HAL` executes correctly within the run-time environment that it will be used in. Failure to meet these requirements will likely result in undefined and unexpected behaviour.

### Initialization and Startup
`Caller` should initialize the `AV Sync` session by calling `avsync_soc_init`() before calling any other `API`.

### Threading Model
This interface is not required to be thread safe. Any `Caller` while invoking these HAL `API`s should ensure calls are made in a thread safe manner.

### Process Model
The interface is expected to support a single instantiation with a single process.

### Memory Model
The `AV Sync` `HAL` will own any memory that it creates and will also be responsible to free. The `Caller` will own the memory that it creates.

### Power Management Requirements
This interface is not required to be involved in power management.

### Asynchronous Notification Model
`avsync_soc_free_frame_callback()` function sets the callback function for freeing any allocated video frames and metadata. 

### Blocking calls
This interface is required to have no blocking calls.

### Internal Error Handling
All the `API`s must return error synchronously as a return argument. `HAL` is responsible for handling system errors (e.g. out of memory) internally.

### Persistence Model
There is no requirement for the interface to persist any setting information.


## Non-functional requirements

### Logging and debugging requirements
This interface is required to support DEBUG, INFO and ERROR messages. ERROR logs should be enabled by default. DEBUG and INFO is required to be disabled by default and enabled when needed.

### Memory and performance requirements
This interface is required to not cause excessive memory and CPU utilization. 

### Quality Control
- This interface is required to perform static analysis, our preferred tool is Coverity.
- Have a zero-warning policy with regards to compiling. All warnings are required to be treated as error.
- Copyright validation is required to be performed, e.g.: Black duck, FossID.
- Use of memory analysis tools like Valgrind are encouraged, to identify leaks/corruptions.
- `HAL` Tests will endeavour to create worst case scenarios to assist investigations.
- Improvements by any party to the testing suite are required to be fed back.

### Licensing
The `HAL` implementation is expected to released under the Apache License 2.0.

### Build Requirements
The source code must build into a shared library and must be named as libXavsync.so where X denotes the `SoC` `AV Sync` module. The build mechanism should be independent of Yocto. 

### Variability Management
Any change to the interface must be reviewed and approved by component architects and owners.

### Platform or Product Customization
The requirements for `AV Sync` module for `SoC` vendor to enable adequate customization and implementation of the `AV Sync` module.


## Interface API Documentation
`API` documentation will be provided by Doxygen which will be generated from the header files.

### Theory of operation and key concepts
The `Caller` is expected to have complete control over the life cycle of the `HAL`.

- `Caller` will be calling the `HAL` `API`s, which are responsible to call the respective `SoC` specific `AV Sync` `API`s, and that is further expected to handle the low-level synchronization related operations using IOCTL calls.

- The `HAL` `AV Sync` `API`s are: 

`avsync_soc_init`(), `avsync_soc_term`(), `avsync_soc_set_mode`(), `avsync_soc_free_frame_callback`(), `avsync_soc_push_frame`(), `avsync_soc_pop_frame`(), `avsync_soc_set_rate`(), `avsync_soc_pause`(), `avsync_soc_set_interval`(), `avsync_soc_eos`().

- Initialize the `HAL` using function `avsync_soc_init`() before making any other `API` calls. If this call fails, the `HAL` must return the respective error. The `Caller` initializes the `AV Sync` session with refresh rate and sync type to get session id, session.

- The function `avsync_soc_free_frame_callback`() registers a callback function responsible for releasing video frame
and metadata that is no longer required. 

- The push and pop operations on the video frames to `AV Sync` module will be done using `avsync_soc_push_frame`() and `avsync_soc_pop_frame`() respectively. `avsync_soc_push_frame`() API is used to push a video frame to the queue. `avsync_soc_pop_frame`() API is used to pop a video frame. If the vblank interval has been changed, then `avsync_soc_set_interval`() API is used to update the vblank interval of the `SoC` `AV Sync` module.

- The `avsync_soc_pause`() and `avsync_soc_eos`() should handle the playback conditions 'pause/resume' and 'end-of-stream' respectively. 

- The sync mode, playback rate and vsync interval can be set using `avsync_soc_set_mode`(), `avsync_soc_set_rate`() and `avsync_soc_set_interval`() respectively. `avsync_soc_set_mode`() is used to set the sync mode that determines how the `SoC` `AV Sync` module synchronizes the audio and video. Supported sync types are Video Master, Audio Master and Program Clock Reference Master. `avsync_soc_set_rate`() will be called before `avsync_soc_pause`() to check if the pause argument is false and the rate field is not equal to 1.0.

- The `AV Sync` session will be terminated using `avsync_soc_term`() `API` and should deallocate all the resources which are created by the `avsync_soc_init`().

### Diagrams

#### AV Sync Code Flow

```mermaid

sequenceDiagram

participant Caller
participant AV Sync HAL
participant SoC AV Sync 

note left of Caller:Playback started

Caller->>+AV Sync HAL:avsync_soc_init()
note over AV Sync HAL,SoC AV Sync:invoke open/create session calls
AV Sync HAL-->>SoC AV Sync:session opened
activate SoC AV Sync 
SoC AV Sync-->>AV Sync HAL:return session related data
AV Sync HAL-->>-Caller:returns pointer to the AVSync structure

rect rgb(191, 225, 255)
alt Set the callback to free the allocated video frame and metadata
Caller-)AV Sync HAL:avsync_soc_free_frame_callback()
end
end

Caller->>+AV Sync HAL:avsync_soc_set_mode()
note over AV Sync HAL,SoC AV Sync:set sync mode
AV Sync HAL->>-Caller: return 0 if the mode was successfully changed, or -1 if an error occurred.

Caller->>+AV Sync HAL:avsync_soc_push_frame()
note over AV Sync HAL,SoC AV Sync:invoke push frame call
AV Sync HAL->>-Caller:return true

AV Sync HAL--)SoC AV Sync: avsync free frame
note over AV Sync HAL,SoC AV Sync:free a video frame <br/> that is no longer needed

rect rgb(191, 225, 255)
alt if vblank interval changes
Caller->>+AV Sync HAL:avsync_soc_set_interval()
note over AV Sync HAL,SoC AV Sync:update vsync interval
AV Sync HAL-->>-Caller:return 0
end
end

Caller->>+AV Sync HAL:avsync_soc_pop_frame()
note over AV Sync HAL,SoC AV Sync:invoke pop frame call
AV Sync HAL->>-Caller:returns pointer to the retrieved video frame

rect rgb(255,250,225)
alt
note left of Caller:Playback paused

rect rgb(193, 225, 255)
alt  if !pause variable and rate variable != 1.0
Caller->>+AV Sync HAL:avsync_soc_set_rate()
note over AV Sync HAL,SoC AV Sync:set playback rate/speed
end
end
Caller->>+AV Sync HAL:avsync_soc_pause()
note over AV Sync HAL,SoC AV Sync:invoke pause call
end
end

note left of Caller:Playback EOS

Caller->>+AV Sync HAL:avsync_soc_eos()
note over AV Sync HAL,SoC AV Sync:invoke eos call
Caller->>+AV Sync HAL:avsync_soc_term()
note over AV Sync HAL,SoC AV Sync:invoke close/destroy session calls
AV Sync HAL--xSoC AV Sync:session closed
deactivate SoC AV Sync 
deactivate AV Sync HAL

```

### Data Structures and Defines
`SoC` vendors should refer to the header file present under the 'include' directory for `API` implementation: https://github.com/rdkcentral/avsync-halif/blob/rdk-dev/include/
