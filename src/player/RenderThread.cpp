
//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2014 Ulrich von Zadow
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

#include "RenderThread.h"

#include "Canvas.h"
#include "Window.h"

#include "../base/ProfilingZoneID.h"
#include "../base/StringHelper.h"

#ifdef __linux__
#include "../graphics/GLXContext.h"
#endif
#include "../graphics/MCFBO.h"

using namespace std;

namespace avg {

RenderThread::RenderThread(CQueue& cmdQueue, int idx)
    : WorkerThread<RenderThread>("Render "+toString(idx), cmdQueue,
            Logger::category::PROFILE)
{
}

RenderThread::~RenderThread()
{
}

void RenderThread::render(Canvas* pCanvas, Window* pWindow, MCFBOPtr pFBO,
        IntRect viewport)
{
    pCanvas->renderWindow(pWindow, pFBO, viewport);
#ifdef __linux__
    GLXContext* pContext = dynamic_cast<GLXContext*>(pWindow->getGLContext());
    pContext->deactivate();
#endif
    pCanvas->onRenderDone();
}

bool RenderThread::work()
{
    waitForCommand();
    return true;
}

}

