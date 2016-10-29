/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderarea.h"

#include <QPainter>

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    canvas = nullptr;

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

RenderArea::~RenderArea()
{
    if (canvas != nullptr)
        delete canvas;
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(128*1, 64*1);
}

QSize RenderArea::sizeHint() const
{
    return QSize(128*1, 64*1);
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    if (canvas == nullptr) return;

    QPainter painter(this);

    painter.save();
    painter.translate(0, 0);
    painter.scale((qreal)width()/(qreal)128,qreal(height())/(qreal)64);

    static unsigned char buffer[128*64];

    // This conversion is only for the vertical packing, 1BPP
    for (uint32_t x = 0; x < 128; ++x)
    {
        for (uint32_t y = 0; y < 64; ++y)
        {
            if (canvas->getPix(x, y))
                buffer[ y * 128 + x ] = 0xFF;
            else
                buffer[ y * 128 + x ] = 0x00;
        }
    }

    pixmap.convertFromImage(QImage(
                                    buffer,
                                    128,
                                    64,
                                    QImage::Format_Grayscale8
                                  )
    );
    painter.drawPixmap(0, 0, pixmap);

    painter.restore();
}
