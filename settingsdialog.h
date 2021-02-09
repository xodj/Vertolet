#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}

QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
        QString serialname;
        int serverAddress;
        int parity;
        int baud;
        int dataBits;
        int stopBits;
        int responseTime;
        int numberOfRetries;
        int speedTime;
    };

    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    Settings settings() const;

private:
    Settings m_settings;
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
