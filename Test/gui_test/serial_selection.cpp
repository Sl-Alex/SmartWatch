#include "serial_selection.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QGridLayout>

#include <emulator_window.h>
#include <QDesktopWidget>

#include <QtSerialPort/QSerialPortInfo>

QT_USE_NAMESPACE

SerialSelection::SerialSelection(QWidget *parent)
    : QDialog(parent)
    , serialPortLabel(new QLabel(tr("Serial port:")))
    , serialPortComboBox(new QComboBox())
    , runButton(new QPushButton(tr("Start")))
{
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        serialPortComboBox->addItem(info.portName());

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(serialPortLabel, 0, 0);
    mainLayout->addWidget(serialPortComboBox, 0, 1);
    mainLayout->addWidget(runButton, 0, 2, 2, 1);
    setLayout(mainLayout);

    setWindowTitle(tr("Select port"));
    serialPortComboBox->setFocus();

    connect(runButton, &QAbstractButton::clicked, this, &SerialSelection::onRunButton);

    mainWindow = EmulatorWindow::getInstance();
}

void SerialSelection::onRunButton(void)
{
    QDesktopWidget wid;

    int screenWidth = wid.screen()->width();
    int screenHeight = wid.screen()->height();

    int width = 128;
    int height = 64;

    mainWindow->setPortName(serialPortComboBox->currentText());
    mainWindow->setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);
    mainWindow->show();
    close();
}
