# libavg - Media Playback Engine.
# Copyright (C) 2003-2008 Ulrich von Zadow
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Current versions can be found at www.libavg.de
#

from libavg import avg
g_Player = None

class RoundedRect(avg.PolygonNode):
    def __init__(self, size, radius, pos=(0,0), parent=None, **kwargs):
        avg.PolygonNode.__init__(self, **kwargs)
        self.__pos = avg.Point2D(pos)
        self.__size = avg.Point2D(size)
        self.__radius = radius
        self.__calcPolygon()
        if parent:
            parent.appendChild(self)
        
    def getPos(self):
        return self.__pos

    def setPos(self, pos):
        self.__pos = avg.Point2D(pos)
        self.__calcPolygon()
    polyPos = avg.PolygonNode.pos
    pos = property(getPos, setPos)
        
    def getSize(self):
        return self.__size

    def setSize(self, size):
        self.__size = avg.Point2D(size)
        self.__calcPolygon()
    size = property(getSize, setSize)
    
    def getRadius(self):
        return self.__radius

    def setRadius(self, radius):
        self.__radius = radius
        self.__calcPolygon()
    radius = property(getRadius, setRadius)

    def __calcPolygon(self):
        def calcQuarterCircle(center, r, startAngle):
            pos = []
            for i in xrange(r+1):
                angle = i*(1.57/r)+startAngle
                p = avg.Point2D(center)+avg.Point2D.fromPolar(angle, r)
                pos.append(p)
            return pos

        if self.__size < self.__radius*2:
            raise RuntimeError("RoundedRect smaller than radius allows.")
        pos = []
        r = self.__radius
        size = self.__size
        pos.extend(calcQuarterCircle(self.pos+(size.x-r,r), r, -1.57))
        pos.extend(calcQuarterCircle(self.pos+(size.x-r,size.y-r), r, 0))
        pos.extend(calcQuarterCircle(self.pos+(r,size.y-r), r, 1.57))
        pos.extend(calcQuarterCircle(self.pos+(r,r), r, 3.14))
        self.polyPos = pos