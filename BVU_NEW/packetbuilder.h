#ifndef PACKETBUILDER_H
#define PACKETBUILDER_H

#include <QByteArray>
#include <QString>

class PacketBuilder
{
public:
    static QByteArray createLevel1Packet(uint8_t flag, const QByteArray &data);
    static uint8_t calculateCRC8(const QByteArray &data);
    // Для одиночного байта
    static QByteArray createLevel1Packet(uint8_t flag, uint8_t data);
};

#endif // PACKETBUILDER_H
