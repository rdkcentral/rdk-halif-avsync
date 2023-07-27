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
#ifndef _WESTEROS_RENDER_H
#define _WESTEROS_RENDER_H

#include <vector>

#include "wayland-client.h"

/*
 * Westeros Renderer Interface
 *
 * This interface is used by the compositor to preform all necessary
 * rendering activities required for compositing surfaces.
 *
 * Modules that implement this interface must supply an "renderer_init" 
 * entry point that populates the supplied WstRenderer structure.  
 */

#define RENDERER_MODULE_INIT "renderer_init"

typedef enum _WstRenderer_format
{
   WstRenderer_format_unknown,
   WstRenderer_format_ARGB8888,
   WstRenderer_format_BGRA8888,
   WstRenderer_format_XRGB8888,
   WstRenderer_format_BGRX8888,
   WstRenderer_format_RGB565,
   WstRenderer_format_ARGB4444
} WstRenderer_format;

typedef struct _WstRect
{
   int x;
   int y;
   int width;
   int height;
} WstRect;

typedef enum _WstHints
{
   WstHints_none= 0,
   WstHints_noRotation= (1<<0),
   WstHints_holePunch= (1<<1),
   WstHints_fboTarget= (1<<2),
   WstHints_applyTransform= (1<<3),
   WstHints_animating= (1<<4),
   WstHints_hidden= (1<<5),
} WstHints;

typedef struct _WstRenderer WstRenderer;
typedef struct _WstRenderSurface WstRenderSurface;
typedef struct _WstNestedConnection WstNestedConnection;

typedef int (*WSTMethodRenderInit)( WstRenderer *renderer, int argc, char **argv);
typedef void (*WSTMethodRenderTerm)( WstRenderer *renderer );
typedef void (*WSTMethodUpdateScene)( WstRenderer *renderer );
typedef WstRenderSurface* (*WSTMethodSurfaceCreate)( WstRenderer *renderer );
typedef void (*WSTMethodSurfaceDestroy)( WstRenderer *renderer, WstRenderSurface *surf );
typedef void (*WSTMethodSurfaceCommit)( WstRenderer *renderer, WstRenderSurface *surface, struct wl_resource *resource );
typedef void (*WSTMethodSurfaceSetVisible)( WstRenderer *renderer, WstRenderSurface *surface, bool visible );
typedef bool (*WSTMethodSurfaceGetVisible)( WstRenderer *renderer, WstRenderSurface *surface, bool *visible );
typedef void (*WSTMethodSurfaceSetGeometry)( WstRenderer *renderer, WstRenderSurface *surface, int x, int y, int width, int height );
typedef void (*WSTMethodSurfaceGetGeometry)( WstRenderer *renderer, WstRenderSurface *surface, int *x, int *y, int *width, int *height );
typedef void (*WSTMethodSurfaceSetOpacity)( WstRenderer *renderer, WstRenderSurface *surface, float opacity );
typedef float (*WSTMethodSurfaceGetOpacity)( WstRenderer *renderer, WstRenderSurface *surface, float *opaticty );
typedef void (*WSTMethodSurfaceSetZOrder)( WstRenderer *renderer, WstRenderSurface *surface, float z );
typedef float (*WSTMethodSurfaceGetZOrder)( WstRenderer *renderer, WstRenderSurface *surface, float *z );
typedef void (*WSTMethodSurfaceSetCrop)( WstRenderer *renderer, WstRenderSurface *surface, float x, float y, float width, float height );
typedef void (*WSTMethodQueryDmabufFormats)( WstRenderer *renderer, int **formats, int *num_formats );
typedef void (*WSTMethodQueryDmabufModifiers)( WstRenderer *renderer, int format, uint64_t **modifiers, int *num_modifiers );
typedef void (*WSTMethodDelegateUpdateScene)( WstRenderer *renderer, std::vector<WstRect> &rects );
typedef void (*WSTMethodHolePunch)( WstRenderer *renderr, int x, int y, int width, int height );
typedef void (*WSTMethodResolutionChangeBegin)( WstRenderer *renderer );
typedef void (*WSTMethodResolutionChangeEnd)( WstRenderer *renderer );

typedef struct _WstRenderer
{
   int outputWidth;
   int outputHeight;
   void *nativeWindow;
   struct wl_display *display;
   void *renderer;
   
   WSTMethodRenderTerm renderTerm;
   WSTMethodUpdateScene updateScene;
   WSTMethodSurfaceCreate surfaceCreate;
   WSTMethodSurfaceDestroy surfaceDestroy;
   WSTMethodSurfaceCommit surfaceCommit;
   WSTMethodSurfaceSetVisible surfaceSetVisible;
   WSTMethodSurfaceGetVisible surfaceGetVisible;
   WSTMethodSurfaceSetGeometry surfaceSetGeometry;
   WSTMethodSurfaceGetGeometry surfaceGetGeometry;
   WSTMethodSurfaceSetOpacity surfaceSetOpacity;
   WSTMethodSurfaceGetOpacity surfaceGetOpacity;
   WSTMethodSurfaceSetZOrder surfaceSetZOrder;
   WSTMethodSurfaceGetZOrder surfaceGetZOrder;
   WSTMethodSurfaceSetCrop surfaceSetCrop;
   WSTMethodQueryDmabufFormats queryDmabufFormats;
   WSTMethodQueryDmabufModifiers queryDmabufModifiers;
   WSTMethodDelegateUpdateScene delegateUpdateScene;
   WSTMethodHolePunch holePunch;
   WSTMethodResolutionChangeBegin resolutionChangeBegin;
   WSTMethodResolutionChangeEnd resolutionChangeEnd;

   // For nested composition
   WstNestedConnection *nc;
   struct wl_display *displayNested;
   struct wl_surface *surfaceNested;
   
   // For embedded composition
   int outputX;
   int outputY;
   float *matrix;
   float alpha;
   bool fastHint;
   int hints;
   bool needHolePunch;
   std::vector<WstRect> rects;
} WstRenderer;


/**
 * This function creates a new WstRenderer object and takes several input parameters, including a module name, command line arguments, a Wayland display, and a nested connection object. 
 * The command line arguments are parsed to retrieve the width and height values of the output window (and a native window if it is available).
 * The function needs to allocate memory for a new WstRenderer object and sets its properties based on the input parameters, including the Wayland display and the nested connection object.
*/
WstRenderer* WstRendererCreate( const char *moduleName, int argc, char **argv, 
                                struct wl_display *display, WstNestedConnection *nc );

/**
 * This function cleans up and destroys a WstRenderer object by terminating the rendering process and freeing the allocated memory. The function takes a single input parameter, a pointer to the WstRenderer object to be destroyed.
*/
void WstRendererDestroy( WstRenderer *renderer );

/**
 * This function updates the current scene of the renderer. It takes a single input parameter, a pointer to the WstRenderer object whose scene is to be updated.
*/
void WstRendererUpdateScene( WstRenderer *renderer );

/**
 * This function needs to create a rendering surface for a given renderer. It returns a pointer to a newly created WstRenderSurface object. The function takes in a pointer to a WstRenderer object renderer as its input.
*/
WstRenderSurface* WstRendererSurfaceCreate( WstRenderer *renderer );

/**
 * This function is responsible for destroying a render surface that was previously created using the WstRendererSurfaceCreate function. 
 * This function typically needs to perform any necessary cleanup operations to free up resources associated with the surface, such as releasing graphics buffers or deleting the surface from the list of surfaces maintained by the renderer.
*/
void WstRendererSurfaceDestroy( WstRenderer *renderer, WstRenderSurface *surface );

/**
 * This function needs to commit any pending changes made to the surface, which means that the renderer will render the updated surface contents to the output display. 
 * It takes three parameters: renderer (responsible for rendering the surface), surface (represents the surface to be committed), resource (represents the client's resource for the surface). 
 * The actual implementation of this function will depend on the type of renderer being used, which is determined by the renderer parameter.
*/
void WstRendererSurfaceCommit( WstRenderer *renderer, WstRenderSurface *surface, struct wl_resource *resource );

/**
 * This function needs to set the visibility of a given WstRenderSurface object to the specified value of visible and it takes three arguments. 
*/
void WstRendererSurfaceSetVisible( WstRenderer *renderer, WstRenderSurface *surface, bool visible );

/**
 * This function needs to get the visibility status of a given surface and 'visible' will be set by the function to indicate whether the surface is visible or not. 
 * The function needs to return a boolean value that indicates whether the operation was successful or not.
*/
bool WstRendererSurfaceGetVisible( WstRenderer *renderer, WstRenderSurface *surface, bool *visible );

/**
 * This function needs to set the position and size of a rendering surface and it takes six parameters: renderer (represents the renderer to which the surface belongs), surface (rendering surface whose geometry is to be set), int x and int y (specify the position of the surface), widht and height (sets the width and height of the rendering surface).
*/
void WstRendererSurfaceSetGeometry( WstRenderer *renderer, WstRenderSurface *surface, int x, int y, int width, int height );

/**
 * This function needs to retrieve the geometry (position:x, y and size: width, height) of a rendering surface associated with a renderer. 
*/
void WstRendererSurfaceGetGeometry( WstRenderer *renderer, WstRenderSurface *surface, int *x, int *y, int *width, int *height );

/**
 * This function needs to set the opacity of a given surface in a renderer. The 'opacity' will represent the desired opacity of the surface.
*/
void WstRendererSurfaceSetOpacity( WstRenderer *renderer, WstRenderSurface *surface, float opacity );

/**
 * This function needs to get the opacity of a given WstRenderSurface where pointer 'opacity' holds the returned opacity value.
*/
float WstRendererSurfaceGetOpacity( WstRenderer *renderer, WstRenderSurface *surface, float *opacity );

/**
 * This function needs to set the desired Z order value of a given rendering surface in a WstRenderer instance.
*/
void WstRendererSurfaceSetZOrder( WstRenderer *renderer, WstRenderSurface *surface, float z );

/**
 * This function needs to get the Z order value of a given surface from a renderer where the pointer 'z' stores the returned z-order value.
*/
float WstRendererSurfaceGetZOrder( WstRenderer *renderer, WstRenderSurface *surface, float *z );

/**
 * This function needs to set the cropping region of a given rendering surface. It takes as input a pointer to the renderer, a pointer to the surface to be modified, and the coordinates and dimensions of the cropping region (x, y, width, height).
*/
void WstRendererSurfaceSetCrop( WstRenderer *renderer, WstRenderSurface *surface, float x, float y, float width, float height );

/**
 * This is a function that queries the list of supported DMA-BUF buffer formats by the renderer. 
 * It takes a pointer to an array of integers (formats) and a pointer to an integer (num_formats), which will be used to store the list of supported DMA-BUF formats and the number of formats, respectively.
*/
void WstRendererQueryDmabufFormats( WstRenderer *renderer, int **formats, int *num_formats );

/**
 * This function queries the available modifiers for a given DMABUF format from a renderer. 
 * It takes parameters: renderer (WstRenderer instance), format (integer representing the DMABUF format), modifiers (pointer to an array of uint64_t, which will be filled with the available modifiers for the given format) and num_modifiers: (pointer to an integer, which will be filled with the number of available modifiers for the given format).
*/
void WstRendererQueryDmabufModifiers( WstRenderer *renderer, int format, uint64_t **modifiers, int *num_modifiers );

/**
 * This function needs to update the scene using the provided renderer's delegate function. 
 * The function takes a pointer to a renderer and a reference to a vector of WstRect structures as arguments. 
 * The WstRect structure represents a rectangular area with integer coordinates and dimensions.
*/
void WstRendererDelegateUpdateScene( WstRenderer *renderer, std::vector<WstRect> &rects );

/**
 * This function needs to notify the renderer that a resolution change is about to occur, and it takes the pointer to WstRenderer structure as its argument. 
 * This structure contains a set of function pointers that the renderer can use to interact with the underlying system.
*/
void WstRendererResolutionChangeBegin( WstRenderer *renderer );

/**
 * This function is called when the resolution change is complete. 
*/
void WstRendererResolutionChangeEnd( WstRenderer *renderer );

#endif

