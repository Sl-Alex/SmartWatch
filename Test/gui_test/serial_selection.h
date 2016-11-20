#ifndef SERIAL_SELECTION_H
#define SERIAL_SELECTION_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <emulator_window.h>

class SerialSelection : public QDialog
{
    Q_OBJECT

public:
    SerialSelection(QWidget *parent = 0);
    ~SerialSelection();

private slots:
    void onRunButton(void);

private:
    void setControlsEnabled(bool enable);

private:
    QLabel *serialPortLabel;
    QComboBox *serialPortComboBox;
    QPushButton *runButton;
    EmulatorWindow *mainWindow;
};

#endif // SERIAL_SELECTION_H
