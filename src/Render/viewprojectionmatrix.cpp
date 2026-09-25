//============================================================================
//  0x6F526A40 - the current view-projection matrix, sixteen floats into the
//  caller's buffer.
//
//  Both locals are declared identity - the 4x3 with ones at 0, 4 and 8 and
//  the 4x4 with ones at 0, 5, 10 and 15, which is exactly the pattern of the
//  seven `fst`s the shipped prologue makes off one `fld1` and one `fldz` -
//  and both are then overwritten wholesale by the device.  The declaration
//  is still load-bearing: without it this build has nothing to emit and the
//  twenty-eight stores go missing.
//============================================================================
#include "renderdevice.h"
#include "fmatrix.h"

void __fastcall GetViewProjectionMatrix(float* outMatrix)
{
    FMATRIX43 view = { { 1.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f,
                         0.0f, 0.0f, 1.0f,
                         0.0f, 0.0f, 0.0f } };
    float proj[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f, 0.0f,
                       0.0f, 0.0f, 1.0f, 0.0f,
                       0.0f, 0.0f, 0.0f, 1.0f };

    g_renderDevice->GetViewMatrix(&view);
    g_renderDevice->GetProjectionMatrix((FMATRIX44*)proj);

    //  One expression, because the shipped stream pushes `proj` *before* it
    //  builds the widened view: that is the right-to-left evaluation of
    //  MultiplyMatrix44's own argument list, and splitting the temporary out
    //  into a named local puts the two in the other order.
    float result[16];
    *(FMATRIX44*)outMatrix = *(const FMATRIX44*)
        MultiplyMatrix44(result, FMATRIX44(view).m, proj);
}
