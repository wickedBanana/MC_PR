#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <QByteArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    led = new rgbLed();
    serial = new QSerialPort();

    connect(ui->horizontalSliderRot, SIGNAL(valueChanged(int)), this, SLOT(slider_rot_moved(int)));
    connect(ui->horizontalSliderGreun, SIGNAL(valueChanged(int)), this, SLOT(slider_gruen_moved(int)));
    connect(ui->horizontalSliderBlau, SIGNAL(valueChanged(int)), this, SLOT(slider_blau_moved(int)));
    connect(ui->pushButtonTemp, SIGNAL(clicked(bool)), this, SLOT(button_getTemp_pressed(bool)));
    connect(ui->buttonSwitchDisplay, SIGNAL(clicked(bool)), this, SLOT(button_switchDisplay_pressed(bool)));
    connect(ui->pushButtonStepperPlus, SIGNAL(clicked(bool)), this, SLOT(button_stepperPlus_pressed(bool)));
    connect(ui->pushButtonStepperMinus, SIGNAL(clicked(bool)), this, SLOT(button_stepperMinus_pressed(bool)));
    connect(ui->dialStepper, SIGNAL(valueChanged(int)), this, SLOT(dial_stepper_valueChanged(int)));
    connect(ui->dialServo, SIGNAL(valueChanged(int)), this, SLOT(dial_servo_valueChanged(int)));
    connect(ui->pushButtonConnect, SIGNAL(clicked(bool)), this, SLOT(button_connect_pressed(bool)));
    connect(ui->pushButtonSinusSend, SIGNAL(clicked(bool)), this, SLOT(button_sinusSend_pressed(bool)));
    connect(ui->pushButtonDisplaySend, SIGNAL(clicked(bool)), this, SLOT(button_displaySend_pressed(bool)));
    connect(ui->spinBoxTempratur, SIGNAL(valueChanged(int)), this, SLOT(spinBox_Temperatur_valueChanged(int)));
    connect(serial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(serial_errorHandler(QSerialPort::SerialPortError)));
    connect(ui->comboBoxConnect, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxConnect_indexChanged(int)));

    /*Event Filter für Textboxen installieren*/
    ui->lineEditDisplayPosition->installEventFilter(this);
    ui->lineEditDisplayNumber->installEventFilter(this);
    ui->lineEditDisplayB1->installEventFilter(this);
    ui->lineEditDisplayB2->installEventFilter(this);
    ui->lineEditDisplayB3->installEventFilter(this);
    ui->lineEditDisplayB4->installEventFilter(this);
    ui->lineEditSinusValue->installEventFilter(this);
    ui->lineEditSinusPos->installEventFilter(this);

    ui->labelError->setText("");

    ports = QSerialPortInfo::availablePorts();                                      //Serielle Ports scannen
    if(ports.size() >0){
        for(int i = 0; i < ports.size(); i++){                                      //comboBox mit den verfügbaren Ports füllen
            ui->comboBoxConnect->addItem(ports[i].portName(), QVariant(i));
        }

        /*  QString text;                                                            //Infobox mit Portinfos füllen

        int index = ui->comboBoxConnect->currentData().toInt();
        ui->textBrowserConnect->clear();
        text = ports[index].portName(); +" "+ ports[index].description() + "<br>";
        ui->textBrowserConnect->setText(text); */
    }
}


MainWindow::~MainWindow()
{
    delete ui;
    delete led;
    delete serial;

}

/*Slider für die RGB-LED*/
void MainWindow::slider_rot_moved(int value){
    led->red =  value;                                                              //Postion des SLiders in entspechender Variable des LED Objekts speichern
    if(m_isConnected){
        int n = sprintf(m_buffer,"r%dg%db%d\r",led->red, led->green, led->blue);
        serial->write(m_buffer, n);
    }
}

/*Slider für die RGB-LED*/
void MainWindow::slider_gruen_moved(int value){
    led->green = value;
    if(m_isConnected){
        int n = sprintf(m_buffer,"r%dg%db%d\r",led->red, led->green, led->blue);
        serial->write(m_buffer, n);
    }
}

/*Slider für die RGB-LED*/
void MainWindow::slider_blau_moved(int value){
    led->blue =  value;
    if(m_isConnected){
        int n = sprintf(m_buffer,"r%dg%db%d\r",led->red, led->green, led->blue);
        serial->write(m_buffer, n);
    }
}

/*Temperatur lesen*/
void MainWindow::button_getTemp_pressed(bool checked){
    if(m_isConnected){
        serial->clear(QSerialPort::Direction::Output);
        serial->write("tr\r");
        serial->waitForReadyRead();                                             //auf Antwort warten
        QByteArray Data;
        bool found = false;
        for(int i = 0; i < 5; i++){                                            //Tasten Echo filtern(wenn Tastenecho auf dem stm32 entfernt wird, kann die die for Schleife entfernt werden)
                Data  = serial->readLine();                                   //Zeile lesen
                foreach(QChar c, Data){
                    if(c == '='){
                       found = true;
                       break;
                    }
                }
                if(found) break;
        }
        QString tempString = QString(Data);
        QString dataString;

        foreach(QChar c, tempString){
            if(c.isDigit() || c == '.' || c == ','){
                dataString.append(c);
            }
        }
        dataString.truncate(dataString.size()-3);                                       //nicht verwendete Nachkommastellen abschneiden
        ui->lcdNumberTemp->display(dataString);                                                 //Temperatur anzeigen
        ui->lcdNumberTemp->show();
    }
}

/*Tempeatur Messung an/ausscchalten */
void MainWindow::button_switchDisplay_pressed(bool checked){
    if(m_isConnected){
        if(m_statusDisplay){
            strcpy(m_buffer, "ta0\r");
            ui->buttonSwitchDisplay->setText("Aus");
        }
        else {
            strcpy(m_buffer, "ta1\r");
            ui->buttonSwitchDisplay->setText("An");
        }
        serial->write(m_buffer, 4);
        m_statusDisplay = !m_statusDisplay;
    }
}


/*Schrittmotor rechts/links drehen*/
void MainWindow::button_stepperMinus_pressed(bool checked){
    if(m_isConnected){
        serial->write("-\r");
    }
}

/*Schrittmotor rechts/links drehen*/
void MainWindow::button_stepperPlus_pressed(bool checked){
    if(m_isConnected){
        serial->write("+\r");
    }
}

/*Postion des Drehknopfes senden*/
void MainWindow::dial_stepper_valueChanged(int value){
    if(m_isConnected){
        ui->labelStepper->setNum(value);
        int n = sprintf(m_buffer,"m%d\r",value);
        serial->write(m_buffer, n);
    }
}


/*Serielle Schnittstelle initialisieren*/
void MainWindow::button_connect_pressed(bool checked){
    if(!m_isConnected && ports.size() > 0){                          //Mit Uart verbinden
        serial->setPortName(ports[ ui->comboBoxConnect->currentData().toInt()].portName());     //Gewählter Port in der Combobox ermiiteln
        serial->setBaudRate(QSerialPort::Baud9600);                 //Parameter setzen
        serial->setDataBits(QSerialPort::Data8);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setParity(QSerialPort::Parity::NoParity);
        serial->setReadBufferSize(400);
        if(serial->open(QIODevice::ReadWrite)){                      //initialisieren erfolgreich
            m_isConnected = true;
            ui->pushButtonConnect->setText("Trennen");
            for(int i = 0; i<3; i++){                               //Workaround
                serial->write("tr\r");
                serial->readLine();
            }
        }
    }
    else {                                                          //Serielle Schnittstelle trennen
        serial->close();
        m_isConnected = false;
        ui->pushButtonConnect->setText("Verbinden");
    }
}

/*Neue Position des Drehrades des Servos senden*/
void MainWindow::dial_servo_valueChanged(int value){
    if(m_isConnected){
        ui->labelServo->setNum(value);
        int n = sprintf(m_buffer,"s%d\r",value);
        serial->write(m_buffer, n);
    }
}

/*DAC Sinus*/
void MainWindow::button_sinusSend_pressed(bool checked){
    if(m_isConnected){
        QString tempStringPos =  ui->lineEditSinusPos->text();
        QString tempStringValue =  ui->lineEditSinusValue->text();
        int tempIntValue = tempStringValue.toInt();
        int tempIntPos = tempStringPos.toInt();
        if((tempIntPos>= 0 && tempIntPos< 32) && (tempIntValue>= 0 && tempIntValue< 4096)){
            int n = sprintf(m_buffer,"p%dw%d\r",tempIntPos, tempIntValue);
            serial->write(m_buffer, n);
        }
    }
}

/*Auflösung der Temperaturmessung ändern*/
void MainWindow::spinBox_Temperatur_valueChanged(int i){
    if(m_isConnected){
        int n = sprintf(m_buffer,"ts%d\r",ui->spinBoxTempratur->value());
        serial->write(m_buffer,n);
    }
}

/*7-Segment Display ansteuern*/
void MainWindow::button_displaySend_pressed(bool checked){
    if(m_isConnected){
        if(ui->radioButtonDisplayNumber->isChecked()){                          //Nummer und Kommastelle anzeigen
            QString tempStringPos =  ui->lineEditDisplayPosition->text();
            QString tempStringNumber =  ui->lineEditDisplayNumber->text();
            int tempIntValue = tempStringNumber.toInt();
            int tempIntPos = tempStringPos.toInt();
            int n = sprintf(m_buffer,"dz%d,%d\r",tempIntValue, tempIntPos);
            serial->write(m_buffer,n);
        }
        else if(ui->radioButtonDisplayPatern->isChecked()){                     //Bitmuster anzeigen
            QLineEdit *tempLine[] = {ui->lineEditDisplayB1, ui->lineEditDisplayB2,ui->lineEditDisplayB3,ui->lineEditDisplayB4};
            QString tempString[4];
            int tempInt[4];
            bool ok;
            for(int i = 0; i < 4; i++){
                tempString[i] = tempLine[i]->text();
                tempInt[i] = tempString[i].toInt(&ok,16);
            }
            int n = sprintf(m_buffer,"db%x,%x,%x,%x\r",tempInt[0], tempInt[1], tempInt[2], tempInt[3]);
            serial->write(m_buffer,n);
        }
    }
}

/*Event Filter Methode für Textboxen(Automatische Auswahl des Textes bei Mausclick oder focus in)*/
bool MainWindow::eventFilter(QObject *watched, QEvent *event){
    if(watched == ui->lineEditDisplayB1 && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)){
        ui->lineEditDisplayB1->selectAll();
        return true;
    }
    else if(watched == ui->lineEditDisplayB2 && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)){
        ui->lineEditDisplayB2->selectAll();
        return true;
    }
    else if(watched == ui->lineEditDisplayB3 && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)){
        ui->lineEditDisplayB3->selectAll();
        return true;
    }
    else if(watched == ui->lineEditDisplayB4 && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)){
        ui->lineEditDisplayB4->selectAll();
        return true;
    }
    else if(watched == ui->lineEditDisplayNumber && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)){
        ui->lineEditDisplayNumber->selectAll();
        return true;
    }
    else if(watched == ui->lineEditDisplayPosition && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)){
        ui->lineEditDisplayPosition->selectAll();
        return true;
    }
    else if(watched == ui->lineEditSinusPos && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)){
        ui->lineEditSinusPos->selectAll();
        return true;
    }
    else if(watched == ui->lineEditSinusValue && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)){
        ui->lineEditSinusValue->selectAll();
        return true;
    }
    return false;
}

/*Error Handler Serial Port*/
void MainWindow::serial_errorHandler(QSerialPort::SerialPortError error){
    qDebug() << error;
    if(error == QSerialPort::NoError){  //Kein Fehler->Text label löschen
        ui->labelError->setText("");
    }
    else{
        ui->labelError->setText("Fehler: " + serial->errorString());    //Fehlerstring im labelError ausgeben
    }
}

void MainWindow::comboBoxConnect_indexChanged(int index){                               //Slot-Methode zur Anzeige der Portinformationen in einer Infobox(es wurden keine Information gefunden(Win10, ubuntu, rasbian))
    /*    QString text;

    ui->textBrowserConnect->clear();
    text = ports[index].portName(); +" "+ ports[index].description() + "<br>";
    ui->textBrowserConnect->setText(text);*/
}
