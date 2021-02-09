#include "vertolet.h"
#include "ui_vertolet.h"

#include "settingsdialog.h"

#include "QDebug"
#include "QObject"
//#include <QStatusBar>
#include <QTimer>

enum ModbusConnection {Serial};

Vertolet::Vertolet(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Vertolet)
    , lastRequest(nullptr)
    , modbusDevice(nullptr)
{
    ui->setupUi(this);

    m_settingsDialog = new SettingsDialog(this);

    ui->SON->setEnabled(false);
    ui->SOFF->setEnabled(false);
    ui->FOR->setEnabled(false);
    ui->REV->setEnabled(false);
    ui->FOROFF->setEnabled(false);
    ui->REVOFF->setEnabled(false);
    ui->actionCon->setEnabled(true);
    ui->actionDisc->setEnabled(false);
    ui->actionSetcom->setEnabled(true);
    ui->speed->setEnabled(false);
    ui->speedChange->setEnabled(false);
    connect(ui->actionSetcom, &QAction::triggered, m_settingsDialog, &QDialog::show);
    connect(ui->actionCon, &QAction::triggered, this, &Vertolet::con);
    connect(ui->actionDisc, &QAction::triggered, this, &Vertolet::disc);
    //Connect Type
    modbusDevice = new QModbusRtuSerialMaster(this);
    //Autoconnect
    if (ui->actionAutoconnect->isChecked()) con();
    //Speed respnse to statusbar
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(speedresponce()));
    timer->start(m_settingsDialog->settings().speedTime);
}

Vertolet::~Vertolet()
{

    if (modbusDevice->state() >= QModbusDevice::ConnectedState)
    {
        modbusDevice->disconnectDevice();
    }
    delete modbusDevice;

    delete ui;
}

void Vertolet::on_SON_clicked()
{
    ui->SON->setEnabled(false);
    ui->SOFF->setEnabled(true);
    ui->FOR->setEnabled(true);
    ui->REV->setEnabled(true);
    ui->FOROFF->setEnabled(false);
    ui->REVOFF->setEnabled(false);
    writeModbus(4131,1);

}

void Vertolet::on_SOFF_clicked()
{
    ui->SON->setEnabled(true);
    ui->SOFF->setEnabled(false);
    ui->FOR->setEnabled(false);
    ui->REV->setEnabled(false);
    ui->FOROFF->setEnabled(false);
    ui->REVOFF->setEnabled(false);
    writeModbus(4131,0);
}

void Vertolet::on_FOR_clicked()
{
    ui->SON->setEnabled(false);
    ui->SOFF->setEnabled(false);
    ui->FOR->setEnabled(false);
    ui->REV->setEnabled(false);
    ui->FOROFF->setEnabled(true);
    ui->REVOFF->setEnabled(false);
    writeModbus(4132,1);
}

void Vertolet::on_REV_clicked()
{
    ui->SON->setEnabled(false);
    ui->SOFF->setEnabled(false);
    ui->FOR->setEnabled(false);
    ui->REV->setEnabled(false);
    ui->FOROFF->setEnabled(false);
    ui->REVOFF->setEnabled(true);
    writeModbus(4133,1);
}

void Vertolet::on_FOROFF_clicked()
{
    ui->SON->setEnabled(false);
    ui->SOFF->setEnabled(true);
    ui->FOR->setEnabled(true);
    ui->REV->setEnabled(true);
    ui->FOROFF->setEnabled(false);
    ui->REVOFF->setEnabled(false);
    writeModbus(4132,0);
}

void Vertolet::on_REVOFF_clicked()
{
    ui->SON->setEnabled(false);
    ui->SOFF->setEnabled(true);
    ui->FOR->setEnabled(true);
    ui->REV->setEnabled(true);
    ui->FOROFF->setEnabled(false);
    ui->REVOFF->setEnabled(false);
    writeModbus(4133,0);
}

void Vertolet::on_speedChange_clicked()
{
    writeModbus(305,ui->speed->value());
}

void Vertolet::con()
{
    modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
        m_settingsDialog->settings().serialname);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
        m_settingsDialog->settings().parity);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
        m_settingsDialog->settings().baud);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
        m_settingsDialog->settings().dataBits);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
        m_settingsDialog->settings().stopBits);
    modbusDevice->setTimeout(m_settingsDialog->settings().responseTime);
    modbusDevice->setNumberOfRetries(m_settingsDialog->settings().numberOfRetries);
    modbusDevice->connectDevice();
    if (modbusDevice->state() == QModbusDevice::ConnectedState)
    {
        ui->actionCon->setEnabled(false);
        ui->actionDisc->setEnabled(true);
        ui->actionSetcom->setEnabled(false);
        ui->SON->setEnabled(true);
        ui->SOFF->setEnabled(false);
        ui->FOR->setEnabled(false);
        ui->REV->setEnabled(false);
        ui->FOROFF->setEnabled(false);
        ui->REVOFF->setEnabled(false);
        ui->speed->setEnabled(true);
        ui->speedChange->setEnabled(true);
        QModbusDataUnit speedRead(QModbusDataUnit::RegisterType(4), 305, m_settingsDialog->settings().serverAddress);
        if (auto *speedy = modbusDevice->sendReadRequest(speedRead, m_settingsDialog->settings().serverAddress))
        {
            if(speedy->isFinished() == false)
            {
                connect(speedy, &QModbusReply::finished, this, &Vertolet::readSpeedy);
            }
            else
            {
                delete speedy;
            }
        }
    }
}

void Vertolet::readSpeedy()
{
    auto speedy = qobject_cast<QModbusReply *>(sender());
    if(speedy->error() == QModbusDevice::NoError) ui->speed->setValue(speedy->result().value(0));
    speedy->deleteLater();
}

void Vertolet::readSpeedReq()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if(reply->error() == QModbusDevice::NoError)
    {
        if (reply->result().value(0) >= 6000) ui->statusBar->showMessage("Текущая скорость: "+QString::number(reply->result().value(0)-65535)+" об./мин.");
        else ui->statusBar->showMessage("Текущая скорость: "+QString::number(reply->result().value(0))+" об./мин.");
    }
    else
    {
        ui->statusBar->showMessage("Что-то пошло не так :'(");
        ui->statusBar->currentMessage();
    }
    reply->deleteLater();
}

void Vertolet::disc()
{
    if (modbusDevice) modbusDevice->disconnectDevice();
    ui->SON->setEnabled(false);
    ui->SOFF->setEnabled(false);
    ui->FOR->setEnabled(false);
    ui->REV->setEnabled(false);
    ui->FOROFF->setEnabled(false);
    ui->REVOFF->setEnabled(false);
    ui->actionCon->setEnabled(true);
    ui->actionDisc->setEnabled(false);
    ui->actionSetcom->setEnabled(true);
    ui->speed->setEnabled(false);
    ui->speedChange->setEnabled(false);
}

void Vertolet::speedresponce()
{
    if (modbusDevice->state() != QModbusDevice::ConnectedState)
    {
        ui->statusBar->showMessage("Нет соединения :(");
    }
    else
    {
    QModbusDataUnit sprespDU(QModbusDataUnit::RegisterType(4), 2054, m_settingsDialog->settings().serverAddress);
    if (auto *sprespR = modbusDevice->sendReadRequest(sprespDU, m_settingsDialog->settings().serverAddress))
    {
        if(sprespR->isFinished() == false)
        {
            connect(sprespR, &QModbusReply::finished, this, &Vertolet::readSpeedReq);
        }
        else
        {
            delete sprespR;
        }
    }
    }
}

/*void Vertolet::readModbus(quint16 key)
{
    qDebug()<<"readModbus_"+QString::number(key)+"...";
    if (modbusDevice->state() != QModbusDevice::ConnectedState)
    {
        qDebug()<<"Нет соединения!";
    }
    else
    {
    QModbusDataUnit readRequest(QModbusDataUnit::RegisterType(rtype), key, m_settingsDialog->settings().serverAddress);
    if (auto *readReply = modbusDevice->sendReadRequest(readRequest, m_settingsDialog->settings().serverAddress))
    {
        if(readReply->isFinished() == false)
        {
            connect(readReply, &QModbusReply::finished, this, &Vertolet::decodeReply);
        }
        else
        {
            qDebug()<<"Ответ не полон!";
            delete readReply;
        }
    }
    else
    {
        qDebug()<<"Ответа нет!";
    }
    }
}*/

void Vertolet::writeModbus(quint16 key, quint16 value)
{
    qDebug()<<"writeModbus_"+QString::number(key)+"_"+QString::number(value);
    QModbusDataUnit writeRequest(QModbusDataUnit::RegisterType(rtype), key, m_settingsDialog->settings().serverAddress);
    writeRequest.setValue(0, value);
    modbusDevice->sendWriteRequest(writeRequest, m_settingsDialog->settings().serverAddress);
    /*if (QModbusReply *writeReply = modbusDevice->sendWriteRequest(writeRequest, m_settingsDialog->settings().serverAddress))
    {
        if(writeReply->isFinished() == false)
        {

            connect(writeReply, &QModbusReply::finished, this, &Vertolet::decodeReply);
        }
        else
        {
            qDebug()<<"Ответ не полон!";
            delete writeReply;
        }
    }
    else
    {
        qDebug()<<"Ответа нет!";
    }*/
}

/*void Vertolet::decodeReply()
{
    auto *reply = qobject_cast<QModbusReply *>(sender());
    if(reply->error() == QModbusDevice::NoError)
    {
        qDebug()<<"Ответ: "+QString::number(reply->result().value(0));
        reply->deleteLater();
    }
    else
    qDebug()<<"Ошибка обработки ответа!";
    reply->deleteLater();
}*/
