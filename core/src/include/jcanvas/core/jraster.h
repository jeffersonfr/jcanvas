/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef J_RASTER_H
#define J_RASTER_H

#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jwindow.h"
#include "jcanvas/core/jbufferedimage.h"

#include <vector>

namespace jcanvas {

class Raster {

	private:
    std::shared_ptr<Font> _font;
    jrect_t<int> _clip;
    jpoint_t<int> _size;
    uint32_t *_buffer;
    uint32_t _color;
    bool _blend_enabled;

  public:
    /**
     * \brief
     *
     */
    Raster(cairo_surface_t *surface);

    /**
     * \brief
     *
     */
    Raster(uint32_t *data, jpoint_t<int> size);

    /**
     * \brief
     *
     */
    virtual ~Raster();

    /**
     * \brief
     *
     */
    virtual void SetClip(const jrect_t<int> &rect);

    /**
     * \brief
     *
     */
    virtual jrect_t<int> GetClip();

    /**
     * \brief
     *
     */
    virtual uint32_t * GetData();

    /**
     * \brief
     *
     */
    virtual jpoint_t<int> GetSize();

    /**
     * \brief
     *
     */
    virtual void SetColor(uint32_t color);

    /**
     * \brief
     *
     */
    virtual uint32_t GetColor();

    /**
     * \brief
     *
     */
    virtual void Clear();

    /**
     * \brief
     *
     */
    virtual void SetPixel(const jpoint_t<int> &v1);

    /**
     * \brief
     *
     */
    virtual uint32_t GetPixel(const jpoint_t<int> &v1);

    /**
     * \brief
     *
     */
    virtual void ScanLine(jpoint_t<int> v1, int size);

    /**
     * \brief
     *
     */
    virtual void DrawLine(const jpoint_t<int> &p0, const jpoint_t<int> &p1);

    /**
     * \brief
     *
     */
    virtual void DrawTriangle(const jpoint_t<int> &v1, const jpoint_t<int> &v2, const jpoint_t<int> &v3);

    /**
     * \brief
     *
     */
    virtual void FillTriangle(const jpoint_t<int> &v1, const jpoint_t<int> &v2, const jpoint_t<int> &v3);

    /**
     * \brief
     *
     */
    virtual void DrawRectangle(const jrect_t<int> &rect);

    /**
     * \brief
     *
     */
    virtual void FillRectangle(const jrect_t<int> &rect);

    /**
     * \brief
     *
     */
    virtual void DrawPolygon(const jpoint_t<int> &v1, const std::vector<jpoint_t<int>> &points);

    /**
     * \brief
     *
     */
    virtual void DrawBezier(const std::vector<jpoint_t<int>> &points);

    /**
     * \brief
     *
     */
    virtual void DrawCircle(const jpoint_t<int> &v1, int size);

    /**
     * \brief
     *
     */
    virtual void FillCircle(const jpoint_t<int> &v1, int size);

    /**
     * \brief
     *
     */
    virtual void DrawEllipse(const jpoint_t<int> &v1, const jpoint_t<int> &s1);

    /**
     * \brief
     *
     */
    virtual void FillEllipse(const jpoint_t<int> &v1, const jpoint_t<int> &s1);

    /**
     * \brief
     *
     */
    virtual void DrawArc(const jpoint_t<int> &v1, const jpoint_t<int> &s1, float arc0, float arc1);

    /**
     * \brief
     *
     */
    virtual void FillArc(const jpoint_t<int> &v1, const jpoint_t<int> &s1, float arc0, float arc1);

    /**
     * \brief
     *
     */
    virtual void FillPolygon(const jpoint_t<int> &v1, const std::vector<jpoint_t<int>> &points, bool holed);

    /**
     * \brief
     *
     */
    virtual void DrawImage(std::shared_ptr<Image> image, const jpoint_t<int> &v1);

    /**
     * \brief
     *
     */
    virtual void SetBlendEnabled(bool param);

    /**
     * \brief
     *
     */
    virtual float GetBlendEnabled();

    /**
     * \brief
     *
     */
    virtual void SetFont(std::shared_ptr<Font> font);

    /**
     * \brief
     *
     */
    virtual void DrawGlyph(int glyph, const jpoint_t<int> &v1);

    /**
     * \brief
     *
     */
    virtual void DrawString(std::string text, const jpoint_t<int> &v1);

};

}

#endif
