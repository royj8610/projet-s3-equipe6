#include "encoder.h"

//-----------------------------------------
//             Constructeurs
//-----------------------------------------
PendulumEncoder::PendulumEncoder(uint8_t chipSelectPin)
    : _sensor(chipSelectPin)
{
}

//-----------------------------------------
//                Public
//-----------------------------------------
bool PendulumEncoder::init()
{
    _initialized = _sensor.initSPI();
    return _initialized;
}

double PendulumEncoder::readAngle()
{
    if (!_initialized)
    {
        return NAN;
    }
    float angle = _sensor.readAngleDegree(true);
    if (angle <= 180)
    {
        return -angle;
    }
    else
    {
        return -(angle - 360);
    }
}

double PendulumEncoder::readRelativeAngle()
{
    // Une fois ZPOS configuré, le capteur retourne déjà
    // une position relative au zéro matériel.
    return readAngle();
}

bool PendulumEncoder::setZero()
{
    if (!_initialized)
    {
        return false;
    }

    // Lecture de la position actuelle sur 14 bits.
    const uint16_t currentPosition = _sensor.readAngleRaw(true);

    return writeZeroPosition(currentPosition);
}

bool PendulumEncoder::resetZero()
{
    return writeZeroPosition(0);
}

//-----------------------------------------
//                 Private
//-----------------------------------------
bool PendulumEncoder::writeZeroPosition(uint16_t zeroPosition)
{
    if (!_initialized)
    {
        return false;
    }

    // Le AS5047P utilise une valeur de position sur 14 bits.
    zeroPosition &= 0x3FFF;

    AS5047P_Types::ZPOSM_t zposm;
    AS5047P_Types::ZPOSL_t zposl;

    // Bits 13 à 6.
    zposm.data.values.ZPOSM = (zeroPosition >> 6) & 0xFF;

    // Bits 5 à 0.
    zposl.data.values.ZPOSL = zeroPosition & 0x3F;

    /*
     * Préserve les deux bits de configuration MAGL/MAGH déjà présents
     * dans ZPOSL.
     */
    const AS5047P_Types::ZPOSL_t currentZposl =
        _sensor.read_ZPOSL();

    zposl.data.values.comp_l_error_en =
        currentZposl.data.values.comp_l_error_en;

    zposl.data.values.comp_h_error_en =
        currentZposl.data.values.comp_h_error_en;

    AS5047P_Types::ERROR_t error;

    const bool msbWritten = _sensor.write_ZPOSM(
        &zposm,
        &error,
        true,
        false);

    if (!msbWritten || !error.noError())
    {
        return false;
    }

    const bool lsbWritten = _sensor.write_ZPOSL(
        &zposl,
        &error,
        true,
        false);

    return lsbWritten && error.noError();
}
