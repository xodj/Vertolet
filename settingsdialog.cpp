#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "qdebug.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QFile config("config.xml");

    if (!QFile ("config.xml").exists())
    {
            qDebug()<<"Create config...";
            config.open(QIODevice::WriteOnly);
            QXmlStreamWriter writer(&config);
            writer.setAutoFormatting(true);
            writer.writeStartDocument();
                writer.writeStartElement("settings");
                    writer.writeStartElement("serialname");
                        writer.writeAttribute("value", QString::number(1));
                    writer.writeEndElement();
                    writer.writeStartElement("serverAddress");
                        writer.writeAttribute("value", QString::number(1));
                    writer.writeEndElement();
                    writer.writeStartElement("parity");
                        writer.writeAttribute("value", QString::number(2));
                    writer.writeEndElement();
                    writer.writeStartElement("baud");
                        writer.writeAttribute("value", QString::number(9600));
                    writer.writeEndElement();
                    writer.writeStartElement("dataBits");
                        writer.writeAttribute("value", QString::number(8));
                    writer.writeEndElement();
                    writer.writeStartElement("stopBits");
                        writer.writeAttribute("value", QString::number(1));
                    writer.writeEndElement();
                    writer.writeStartElement("responseTime");
                        writer.writeAttribute("value", QString::number(20));
                    writer.writeEndElement();
                    writer.writeStartElement("numberOfRetries");
                        writer.writeAttribute("value", QString::number(0));
                    writer.writeEndElement();
                    writer.writeStartElement("speedTime");
                        writer.writeAttribute("value", QString::number(200));
                    writer.writeEndElement();
                writer.writeEndElement();
            writer.writeEndDocument();
            config.close();
    }

    qDebug()<<"Open config...";
        config.open(QIODevice::ReadOnly | QIODevice::Text);
        QXmlStreamReader xmlReader;
        xmlReader.setDevice(&config);
        xmlReader.readNext();
        while(!xmlReader.atEnd())
        {
            if (xmlReader.name() == "serialname")
                foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                    if (attr.name().toString() == "value")
                    {
                        ui->setSerialPort->setCurrentIndex(attr.value().toString().toInt());
                        m_settings.serialname = ui->setSerialPort->currentText();
                    }
                }
            if (xmlReader.name() == "serverAddress")
                foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                    if (attr.name().toString() == "value")
                    {
                        m_settings.serverAddress = attr.value().toString().toInt();
                    }
                }
            if (xmlReader.name() == "parity")
                foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                    if (attr.name().toString() == "value")
                    {
                        ui->parityCombo->setCurrentIndex(attr.value().toString().toInt());
                        m_settings.parity = ui->parityCombo->currentIndex();
                        if (m_settings.parity > 0)
                            m_settings.parity++;
                    }
                }
            if (xmlReader.name() == "baud")
                foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                    if (attr.name().toString() == "value")
                    {
                        m_settings.baud = attr.value().toString().toInt();
                    }
                }
            if (xmlReader.name() == "dataBits")
                foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                    if (attr.name().toString() == "value")
                    {
                        m_settings.dataBits = attr.value().toString().toInt();
                    }
                }
            if (xmlReader.name() == "stopBits")
                foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                    if (attr.name().toString() == "value")
                    {
                        m_settings.stopBits = attr.value().toString().toInt();
                    }
                }
            if (xmlReader.name() == "responseTime")
                foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                    if (attr.name().toString() == "value")
                    {
                        m_settings.responseTime = attr.value().toString().toInt();
                    }
                }
            if (xmlReader.name() == "numberOfRetries")
                foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                    if (attr.name().toString() == "value")
                    {
                        m_settings.numberOfRetries = attr.value().toString().toInt();
                    }
                }
            if (xmlReader.name() == "speedTime")
                foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                    if (attr.name().toString() == "value")
                    {
                        m_settings.speedTime = attr.value().toString().toInt();
                    }
                }
            xmlReader.readNext();
        }

        ui->baudCombo->setCurrentText(QString::number(m_settings.baud));
        ui->dataBitsCombo->setCurrentText(QString::number(m_settings.dataBits));
        ui->stopBitsCombo->setCurrentText(QString::number(m_settings.stopBits));
        ui->timeoutSpinner->setValue(m_settings.responseTime);
        ui->retriesSpinner->setValue(m_settings.numberOfRetries+1);
        ui->timeoutSpeed->setValue(m_settings.speedTime);
        ui->serverAddressSpinBox->setValue(m_settings.serverAddress);

    connect(ui->applyButton, &QPushButton::clicked, [this]() {
        m_settings.serialname = ui->setSerialPort->currentText();
        m_settings.parity = ui->parityCombo->currentIndex();
        if (m_settings.parity > 0)
            m_settings.parity++;
        m_settings.baud = ui->baudCombo->currentText().toInt();
        m_settings.dataBits = ui->dataBitsCombo->currentText().toInt();
        m_settings.stopBits = ui->stopBitsCombo->currentText().toInt();
        m_settings.responseTime = ui->timeoutSpinner->value();
        m_settings.numberOfRetries = ui->retriesSpinner->value()-1;
        m_settings.serverAddress = ui->serverAddressSpinBox->value();
        m_settings.speedTime = ui->timeoutSpeed->value();

        {
                qDebug()<<"Renew config...";
                QFile config("config.xml");
                config.open(QIODevice::WriteOnly);
                QXmlStreamWriter writer(&config);
                writer.setAutoFormatting(true);
                writer.writeStartDocument();
                    writer.writeStartElement("settings");
                        writer.writeStartElement("serialname");
                            writer.writeAttribute("value", QString::number(ui->setSerialPort->currentIndex()));
                        writer.writeEndElement();
                        writer.writeStartElement("serverAddress");
                            writer.writeAttribute("value", QString::number(m_settings.serverAddress));
                        writer.writeEndElement();
                        writer.writeStartElement("parity");
                            writer.writeAttribute("value", QString::number(ui->parityCombo->currentIndex()));
                        writer.writeEndElement();
                        writer.writeStartElement("baud");
                            writer.writeAttribute("value", QString::number(m_settings.baud));
                        writer.writeEndElement();
                        writer.writeStartElement("dataBits");
                            writer.writeAttribute("value", QString::number(m_settings.dataBits));
                        writer.writeEndElement();
                        writer.writeStartElement("stopBits");
                            writer.writeAttribute("value", QString::number(m_settings.stopBits));
                        writer.writeEndElement();
                        writer.writeStartElement("responseTime");
                            writer.writeAttribute("value", QString::number(m_settings.responseTime));
                        writer.writeEndElement();
                        writer.writeStartElement("numberOfRetries");
                            writer.writeAttribute("value", QString::number(m_settings.numberOfRetries));
                        writer.writeEndElement();
                        writer.writeStartElement("speedTime");
                            writer.writeAttribute("value", QString::number(m_settings.speedTime));
                        writer.writeEndElement();
                    writer.writeEndElement();
                writer.writeEndDocument();
                config.close();
            }

        hide();
    });
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return m_settings;
}
