#ifndef VERTOLET_H
#define VERTOLET_H

#include <QMainWindow>
#include <QtSerialBus/QModbusDataUnit>
#include "QtSerialBus/QModbusRtuSerialMaster"

QT_BEGIN_NAMESPACE

class QModbusClient;
class QModbusReply;

namespace Ui {
class Vertolet;
class SettingsDialog;
}

QT_END_NAMESPACE

class SettingsDialog;

class Vertolet : public QMainWindow
{
    Q_OBJECT

public:
    Vertolet(QWidget *parent = nullptr);
    ~Vertolet();
    int rtype=4;
    SettingsDialog *m_settingsDialog;

private:
    void initActions();

private slots:
    void on_SON_clicked();
    void on_SOFF_clicked();
    void on_FOR_clicked();
    void on_REV_clicked();
    void on_FOROFF_clicked();
    void on_REVOFF_clicked();
    void on_speedChange_clicked();
    void con();
    void disc();
    void readSpeedy();
    void readSpeedReq();
    void speedresponce();
    //void readModbus(quint16 key);
    //void decodeReply();
    void writeModbus(quint16 key, quint16 value);

private:
    Ui::Vertolet *ui;

    QModbusReply *lastRequest;
    QModbusClient *modbusDevice;

};
#endif // VERTOLET_H
