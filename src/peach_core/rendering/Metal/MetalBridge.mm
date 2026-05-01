/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#ifdef PEACH_RENDERER_METAL

#import <QuartzCore/CAMetalLayer.h>

extern "C" void PEACH_AssignMetalDeviceToLayer(void* fp_Layer, void* fp_Device)
{
    CAMetalLayer* f_MetalLayer = (__bridge CAMetalLayer*)fp_Layer;
    f_MetalLayer.device = (__bridge id<MTLDevice>)fp_Device;
    f_MetalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
}

extern "C" void* PEACH_GetNextDrawable(void* fp_Layer)
{
    CAMetalLayer* f_Layer = (__bridge CAMetalLayer*)fp_Layer;
    return (__bridge void*)[f_Layer nextDrawable];
}

#endif