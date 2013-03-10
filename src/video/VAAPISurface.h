//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2011 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//
#ifndef _VAAPISurface_H_
#define _VAAPISurface_H_


#include "../avgconfigwrapper.h"
#include "../base/GLMHelper.h"
#include "../graphics/Bitmap.h"

#include "WrapFFMpeg.h"

#include <libavcodec/vaapi.h>
#include <va/va.h>

namespace avg {

class VAAPIDecoder;

class VAAPISurface
{
public:
    VAAPISurface(VASurfaceID surfaceID, VAAPIDecoder* pDecoder);

    VASurfaceID getSurfaceID() const;
    void incRef();
    void decRef();
    bool isUsed() const;
    void getYUVBmps(BitmapPtr pBmpY, BitmapPtr pBmpU, BitmapPtr pBmpV);
    void getRGBBmp(BitmapPtr pBmp);

private:
    void splitInterleaved(BitmapPtr pBmpU, BitmapPtr pBmpV, BitmapPtr pSrcBmp);

    VASurfaceID m_SurfaceID;
    int m_RefCount;
    IntPoint m_Size;
    VAImage* m_pImage; // Shared between all surfaces in a decoder.
};

}
#endif
