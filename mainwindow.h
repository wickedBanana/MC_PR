#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rgbled.h"
#include <QDebug>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    /*Objekt-Pointer*/
    rgbLed *led = nullptr;
    QSerialPort *serial = nullptr;
    QList<QSerialPortInfo> ports;
    char m_buffer[1024];            //Read/Write-Buffer

    bool m_statusDisplay = false;       //Zustandsvariable Zyklisches Tempraturmessen an/aus
    bool m_isConnected = false;         //Zustandsvariable Serielleschnittstelle verbunden
    bool eventFilter(QObject *watched, QEvent *event); //evenFilter für die Eingabefelder

private slots:
    /*------Prototypen-----*/
    void slider_rot_moved(int value);
    void slider_gruen_moved(int value);
    void slider_blau_moved(int value);
    void button_getTemp_pressed(bool checked);
    void button_switchDisplay_pressed(bool checked);
    void button_stepperPlus_pressed(bool checked);
    void button_stepperMinus_pressed(bool checked);
    void button_displaySend_pressed(bool checked);
    void dial_stepper_valueChanged(int value);
    void dial_servo_valueChanged(int value);
    void button_connect_pressed(bool checked);
    void button_sinusSend_pressed(bool checked);
    void spinBox_Temperatur_valueChanged(int i);
    void serial_errorHandler(QSerialPort::SerialPortError error);
    void comboBoxConnect_indexChanged(int index);
};

#endif // MAINWINDOW_H
