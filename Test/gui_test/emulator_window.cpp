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

#include <QGridLayout>
#include <QTimer>
#include "renderarea.h"
#include "emulator_window.h"
#include "sm_hw_storage.h"
#include "sm_canvas.h"
#include "sm_hal_rtc.h"

#define LABEL_STYLE_DEFAULT "QLabel { background-color : #C0C0C0; color : black; }"
#define LABEL_STYLE_PRESSED "QLabel { background-color : #303030; color : white; }"

EmulatorWindow::EmulatorWindow()
{
    serialPort = nullptr;
    SmHalSysTimer::initSubscribersPool(10);
    SmHwStorage::getInstance()->init();

    renderArea = new RenderArea;
    display = SmDisplay::getInstance();
    display->init(128,64);
    display->getCanvas()->clear();
    renderArea->setCanvas(display->getCanvas());
    desktop = SmDesktop::getInstance();
    desktop->init();

    lbl1 = new QLabel();
    lbl1->setText("A");
    lbl2 = new QLabel();
    lbl2->setText("Z");
    lbl3 = new QLabel();
    lbl3->setText("M");
    lbl4 = new QLabel();
    lbl4->setText("K");

    lbl1->setMaximumWidth(40);
    lbl1->setMinimumWidth(40);
    lbl1->setMaximumHeight(500);
    lbl1->setMinimumHeight(32);

    lbl2->setMaximumWidth(40);
    lbl2->setMinimumWidth(40);
    lbl2->setMaximumHeight(500);
    lbl2->setMinimumHeight(32);

    lbl3->setMaximumWidth(40);
    lbl3->setMinimumWidth(40);
    lbl3->setMaximumHeight(500);
    lbl3->setMinimumHeight(32);

    lbl4->setMaximumWidth(40);
    lbl4->setMinimumWidth(40);
    lbl4->setMaximumHeight(500);
    lbl4->setMinimumHeight(32);

    lbl1->setAlignment(Qt::AlignCenter);
    lbl2->setAlignment(Qt::AlignCenter);
    lbl3->setAlignment(Qt::AlignCenter);
    lbl4->setAlignment(Qt::AlignCenter);

    lbl1->setStyleSheet(LABEL_STYLE_DEFAULT);
    lbl2->setStyleSheet(LABEL_STYLE_DEFAULT);
    lbl3->setStyleSheet(LABEL_STYLE_DEFAULT);
    lbl4->setStyleSheet(LABEL_STYLE_DEFAULT);

    QGridLayout *mainLayout = new QGridLayout();

    mainLayout->addWidget(lbl1,0,0);
    mainLayout->addWidget(lbl2,1,0);
    mainLayout->addWidget(renderArea, 0, 1, 2, 1);
    mainLayout->addWidget(lbl4,0,2);
    mainLayout->addWidget(lbl3,1,2);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setLayout(mainLayout);

    setWindowTitle(tr("SmartWatch GUI test"));

    QTimer * pTimer = new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
    pTimer->start(20);

    QTimer * pTimerMs = new QTimer(this);
    connect(pTimerMs, SIGNAL(timeout()), this, SLOT(onTimerMsEvent()));
    pTimerMs->start(1);

    QTimer * pTimer1s = new QTimer(this);
    pTimer1s->setTimerType(Qt::PreciseTimer);
    connect(pTimer1s, SIGNAL(timeout()), this, SLOT(onTimer1sEvent()));
    pTimer1s->start(1000);
}

EmulatorWindow::~EmulatorWindow()
{
    if (serialPort != nullptr)
    {
        serialPort->close();
        delete serialPort;
    }
}

void EmulatorWindow::onTimerEvent(void)
{
    renderArea->update();
}

void EmulatorWindow::keyPressEvent(QKeyEvent *pEvent)
{
    if (Qt::Key_A == pEvent->key())
    {
        lbl1->setStyleSheet(LABEL_STYLE_PRESSED);
        SmHwKeyboard::getInstance()->simulatedState |= 0x01;
    }
    if (Qt::Key_Z == pEvent->key())
    {
        lbl2->setStyleSheet(LABEL_STYLE_PRESSED);
        SmHwKeyboard::getInstance()->simulatedState |= 0x02;
    }
    if (Qt::Key_M == pEvent->key())
    {
        lbl3->setStyleSheet(LABEL_STYLE_PRESSED);
        SmHwKeyboard::getInstance()->simulatedState |= 0x04;
    }
    if (Qt::Key_K == pEvent->key())
    {
        lbl4->setStyleSheet(LABEL_STYLE_PRESSED);
        SmHwKeyboard::getInstance()->simulatedState |= 0x08;
    }

    QWidget::keyPressEvent(pEvent);
}

void EmulatorWindow::keyReleaseEvent(QKeyEvent *pEvent)
{
    if (Qt::Key_A == pEvent->key())
    {
        lbl1->setStyleSheet(LABEL_STYLE_DEFAULT);
        SmHwKeyboard::getInstance()->simulatedState &=~0x01;
    }
    if (Qt::Key_Z == pEvent->key())
    {
        lbl2->setStyleSheet(LABEL_STYLE_DEFAULT);
        SmHwKeyboard::getInstance()->simulatedState &=~0x02;
    }
    if (Qt::Key_M == pEvent->key())
    {
        lbl3->setStyleSheet(LABEL_STYLE_DEFAULT);
        SmHwKeyboard::getInstance()->simulatedState &=~0x04;
    }
    if (Qt::Key_K == pEvent->key())
    {
        lbl4->setStyleSheet(LABEL_STYLE_DEFAULT);
        SmHwKeyboard::getInstance()->simulatedState &=~0x08;
    }
    if (Qt::Key_Q == pEvent->key())
    {
        SmNotification * pNotification = desktop->getNotification();

        static int cnt = 0;
        if (cnt < 9) cnt++;
        SmText header;       //  S    M    S
        SmText text;         //  I    n    c    o    m    i    n    g         m    e    s    s    a    g    e         0

        static uint16_t s_hdr_data[] = {0x33,0x2D,0x33};
        static uint16_t s_txt_data[] = {0x29,0x4E,0x43,0x4F,0x4D,0x49,0x4E,0x47,0x00,0x4D,0x45,0x53,0x53,0x41,0x47,0x45,0x00,0x10};

        header.length = sizeof(s_hdr_data)/sizeof(uint16_t);
        text.length = sizeof(s_txt_data)/sizeof(uint16_t);

        uint16_t * pHeader = new uint16_t[header.length];
        uint16_t * pText = new uint16_t[text.length];

        memcpy(pHeader, s_hdr_data, header.length * sizeof(uint16_t));
        memcpy(pText, s_txt_data, text.length * sizeof(uint16_t));

        header.pText = pHeader;
        text.pText = pText;
        // header.pText = s_hdr_data;
        // text.pText = s_txt_data;
        text.pText[text.length - 1] = 0x10 + cnt;
        if (pNotification)
            pNotification->addNotification(header, text);
        else
            desktop->showNotification(header, text);
    }

    QWidget::keyReleaseEvent(pEvent);
}

void EmulatorWindow::setPortName(QString port)
{
    if (serialPort == nullptr)
    {
        serialPort = new QSerialPort();
    }
    else
    {
        serialPort->close();
    }
    serialPort->setPortName(port);
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    serialPort->open(QSerialPort::ReadWrite);
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(onSerialData()));
}

void EmulatorWindow::onSerialData(void)
{
    static QByteArray dataPacket;
    static uint32_t timestamp = SmHalSysTimer::getTimeStamp();
    uint32_t newTimestamp = SmHalSysTimer::getTimeStamp();
    if (newTimestamp - timestamp < 100)
    {
        dataPacket += serialPort->readAll();
    }
    else
    {
        dataPacket = serialPort->readAll();
    }
    if (dataPacket.length() >= 20)
    {
        // 20 bytes packet is received
        SmHwBt::getInstance()->injectPacket(dataPacket.data(), dataPacket.count());
        dataPacket.clear();
    }
    timestamp = newTimestamp;
}

void EmulatorWindow::sendPacket(const char *data, uint8_t size)
{
    serialPort->write(data, size);
}

void SysTick_Handler(void);
void EmulatorWindow::onTimerMsEvent(void)
{
    SysTick_Handler();
    SmHalSysTimer::processEvents();
    SmHwBt::getInstance()->update();
    if (SmHwBt::getInstance()->isNotification())
    {
        SmNotification * pNotification = desktop->getNotification();

        if (pNotification)
            pNotification->addNotification(SmHwBt::getInstance()->getHeader(), SmHwBt::getInstance()->getText());
        else
            desktop->showNotification(SmHwBt::getInstance()->getHeader(), SmHwBt::getInstance()->getText());

        SmHwBt::getInstance()->clearNotification();
    }
}

void EmulatorWindow::onTimer1sEvent(void)
{
    SmHalRtc::getInstance()->incrementCounter();
}
