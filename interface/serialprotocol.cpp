#include "serialprotocol.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

SerialProtocol::SerialProtocol(QString portName, qint32 baudRate) {
    // Ouverture du port serie
    serial_ = new QSerialPort();
    serial_->setPortName(portName);
    serial_->setBaudRate(baudRate);
    serial_->open(QIODevice::ReadWrite);
    // Connection des signaux
    connectSignals();
}

SerialProtocol::~SerialProtocol(){
 delete serial_;
}

void SerialProtocol::connectSignals(){
    connect(serial_, SIGNAL(readyRead()), this, SLOT(readReceivedMsg()));
}

void SerialProtocol::sendMessage(QString msg) {
    // Fonction d'ecriture sur le port serie
    if (serial_->isOpen()) {
        QByteArray data = msg.toUtf8();  // NOTE : Il faut avoir un \n pour que le Arduino parse correctement
        data.append("\n");
        serial_->write(data);
    }
}

void SerialProtocol::readReceivedMsg(){
    // Fonction de lecture du port serie
    receiveBuffer_.append(serial_->readAll());
    qsizetype newlineIndex;

    while((newlineIndex = receiveBuffer_.indexOf('\n')) >= 0)
    {
        QByteArray line = receiveBuffer_.left(newlineIndex);
        receiveBuffer_.remove(0, newlineIndex+1);

        // Le println du arduino ajoute normalement "\r\n"
        line = line.trimmed();

        if(!line.isEmpty())
        {
            emit newMessage(QString::fromUtf8(line));
        }
    }
}
