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

#ifndef EMULATOR_WINDOW_H
#define EMULATOR_WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QMainWindow>
#include <QSerialPort>
#include "sm_image.h"
#include "sm_animator.h"
#include "sm_hw_keyboard.h"
#include "sm_hw_bt.h"
#include "sm_font.h"
#include "sm_desktop.h"
#include "sm_display.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QLabel;
class QSpinBox;
class QPushButton;
QT_END_NAMESPACE
class RenderArea;

class EmulatorWindow : public QWidget
{
    Q_OBJECT

public:
    ~EmulatorWindow();
    void keyPressEvent(QKeyEvent * pEvent);
    void keyReleaseEvent(QKeyEvent * event);

    void onKeyDown(SmHwButtons key);
    void onKeyUp(SmHwButtons key);

    void setPortName(QString port);
    void sendPacket(const char * data, uint8_t size);

    EmulatorWindow(EmulatorWindow const&) = delete;
    void operator=(EmulatorWindow const&) = delete;
    static EmulatorWindow * getInstance()
    {
        static EmulatorWindow  instance; // Guaranteed to be destroyed.
                                 // Instantiated on first use.
        return &instance;
    }
private slots:
    void onTimerEvent(void);
    void onTimerMsEvent(void);
    void onTimer1sEvent(void);
    void onSerialData(void);

private:
    EmulatorWindow();
    RenderArea *renderArea;
    SmDesktop * desktop;
    SmImage * pImage;
    SmAnimator * pAnimator;
    SmFont * smallFont;
    SmDisplay * display;
    QLabel * lbl1;
    QLabel * lbl2;
    QLabel * lbl3;
    QLabel * lbl4;
    QSerialPort * serialPort;

    friend class SmHalSysTimer;
    friend class SmHwBt;
};

#endif // EMULATOR_WINDOW_H
