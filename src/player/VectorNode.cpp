//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2008 Ulrich von Zadow
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

#include "VectorNode.h"

#include "NodeDefinition.h"
#include "SDLDisplayEngine.h"

#include "../graphics/VertexArray.h"

#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/ScopeTimer.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace boost;

namespace avg {

NodeDefinition VectorNode::createDefinition()
{
    return NodeDefinition("vector")
        .extendDefinition(Node::createDefinition())
        .addArg(Arg<string>("color", "FFFFFF", false, offsetof(VectorNode, m_sColorName)))
        .addArg(Arg<double>("strokewidth", 1, false, offsetof(VectorNode, m_StrokeWidth)));
}

VectorNode::VectorNode()
{
}

VectorNode::~VectorNode()
{
}

void VectorNode::setRenderingEngines(DisplayEngine * pDisplayEngine, 
        AudioEngine * pAudioEngine)
{
    setDrawNeeded(true);
    m_Color = colorStringToColor(m_sColorName);
    Node::setRenderingEngines(pDisplayEngine, pAudioEngine);

    m_pVertexArray = VertexArrayPtr(new VertexArray(getNumVertexes(), getNumIndexes(),
            100, 100));
    m_OldOpacity = -1;
}

void VectorNode::disconnect()
{
    m_pVertexArray = VertexArrayPtr();
    Node::disconnect();
}

static ProfilingZone PrerenderProfilingZone("VectorNode::prerender");
static ProfilingZone VAProfilingZone("VectorNode::update VA");
static ProfilingZone VASizeProfilingZone("VectorNode::resize VA");

void VectorNode::preRender()
{
    ScopeTimer Timer(PrerenderProfilingZone);
    double curOpacity = getEffectiveOpacity();

    if (m_bVASizeChanged) {
        ScopeTimer Timer(VASizeProfilingZone);
        m_pVertexArray->changeSize(getNumVertexes(), getNumIndexes());
        m_bVASizeChanged = false;
    }
    {
        ScopeTimer Timer(VAProfilingZone);
        if (m_bDrawNeeded || curOpacity != m_OldOpacity) {
            m_pVertexArray->reset();
            calcVertexes(m_pVertexArray, curOpacity);
            m_pVertexArray->update();
            m_bDrawNeeded = false;
            m_OldOpacity = curOpacity;
        }
    }
    
}

void VectorNode::maybeRender(const DRect& Rect)
{
    assert(getState() == NS_CANRENDER);
    if (getEffectiveOpacity() > 0.01) {
        if (getID() != "") {
            AVG_TRACE(Logger::BLTS, "Rendering " << getTypeStr() << 
                    " with ID " << getID());
        } else {
            AVG_TRACE(Logger::BLTS, "Rendering " << getTypeStr()); 
        }
        SDLDisplayEngine * pEngine = dynamic_cast<SDLDisplayEngine*>(getDisplayEngine());
        pEngine->enableTexture(false);
        pEngine->enableGLColorArray(true);
        render(Rect);
    }
}

static ProfilingZone RenderProfilingZone("VectorNode::render");

void VectorNode::render(const DRect& rect)
{
    ScopeTimer Timer(RenderProfilingZone);
    m_pVertexArray->draw();
}

void VectorNode::setColor(const string& sColor)
{
    if (m_sColorName != sColor) {
        m_sColorName = sColor;
        m_Color = colorStringToColor(m_sColorName);
        m_bDrawNeeded = true;
    }
}

const string& VectorNode::getColor() const
{
    return m_sColorName;
}

void VectorNode::setStrokeWidth(double width)
{
    if (width != m_StrokeWidth) {
        m_bDrawNeeded = true;
        m_StrokeWidth = width;
    }
}

double VectorNode::getStrokeWidth() const
{
    return m_StrokeWidth;
}

Pixel32 VectorNode::getColorVal() const
{
    return m_Color;
}

void VectorNode::updateLineData(VertexArrayPtr& pVertexArray,
        double opacity, const DPoint& p1, const DPoint& p2)
{
    Pixel32 color = getColorVal();
    color.setA((unsigned char)(opacity*255));

    WideLine wl(p1, p2, getStrokeWidth());
    int curVertex = pVertexArray->getCurVert();
    pVertexArray->appendPos(wl.pl0, DPoint(0,0), color);
    pVertexArray->appendPos(wl.pr0, DPoint(0,0), color);
    pVertexArray->appendPos(wl.pl1, DPoint(0,0), color);
    pVertexArray->appendPos(wl.pr1, DPoint(0,0), color);
    pVertexArray->appendQuadIndexes(curVertex+1, curVertex, curVertex+3, curVertex+2); 
}
     
void VectorNode::setDrawNeeded(bool bSizeChanged)
{
    m_bDrawNeeded = true;
    if (bSizeChanged) {
        m_bVASizeChanged = true;
    }
}

WideLine::WideLine(const DPoint& p0, const DPoint& p1, double width)
    : pt0(p0),
      pt1(p1)
{
    DPoint m = (pt1-pt0);
    m.normalize();
    DPoint w = DPoint(m.y, -m.x)*width/2;
    pl0 = p0-w;
    pr0 = p0+w;
    pl1 = p1-w;
    pr1 = p1+w;
    dir = DPoint(w.y, -w.x); 
}

std::ostream& operator<<(std::ostream& os, const WideLine& line)
{
    os << "(" << line.pt0 << "," << line.pt1 << ")";
    return os;
}


}
