#include "weatherdata.h"

WeatherData::WeatherData(QObject *parent) : QObject(parent)
{
}

WeatherData::WeatherData(const WeatherData &other) :
    QObject (nullptr),
    m_Date(other.m_Date),
    m_Month(other.m_Month),
    m_WeatherIcon(other.m_WeatherIcon),
    m_WeatherDesc(other.m_WeatherDesc),
    m_TemperatureDay(other.m_TemperatureDay),
    m_TemperatureMin(other.m_TemperatureMin),
    m_TemperatureMax(other.m_TemperatureMax),
    m_PoP(other.m_PoP),
    m_Pressure(other.m_Pressure),
    m_Humidity(other.m_Humidity),
    m_WindSpeed(other.m_WindSpeed),
    m_WindDegree(other.m_WindDegree),
    m_WeatherHourly(other.m_WeatherHourly)
{
}

QString WeatherData::getDate() const
{
    return m_Date;
}

QString WeatherData::getMonth() const
{
    return m_Month;
}

QString WeatherData::getWeatherIconSymbol() const
{
    return m_WeatherIcon;
}

QString WeatherData::getWeatherDescription() const
{
    return m_WeatherDesc;
}

QString WeatherData::getTempDay() const
{
    return m_TemperatureDay;
}

QString WeatherData::getTempMin() const
{
    return m_TemperatureMin;
}

QString WeatherData::getTempMax() const
{
    return m_TemperatureMax;
}

QString WeatherData::getPoP() const
{
    return m_PoP;
}

QString WeatherData::getPressure() const
{
    return m_Pressure;
}

QString WeatherData::getHumidity() const
{
    return m_Humidity;
}

QString WeatherData::getWindSpeed() const
{
    return m_WindSpeed;
}

double WeatherData::getWindDegree() const
{
   return m_WindDegree;
}

QList<WeatherHourly*> *WeatherData::getWeatherHourly()
{
    return &m_WeatherHourly;
}


// Setters

void WeatherData::setDate(const QString &value)
{
    m_Date = value;
    emit dataChanged();
}

void WeatherData::setMonth(const QString &value)
{
    m_Month = value;
}

void WeatherData::setWeatherIcon(const QString &value)
{
    m_WeatherIcon = value;
    emit dataChanged();
}

void WeatherData::setWeatherDescription(const QString &value)
{
    m_WeatherDesc = value;
}

void WeatherData::setTemperatureDay(const QString &value)
{
    m_TemperatureDay = value;
    emit dataChanged();
}

void WeatherData::setTemperatureMin(const QString &value)
{
    m_TemperatureMin = value;
}

void WeatherData::setTemperatureMax(const QString &value)
{
    m_TemperatureMax = value;
}

void WeatherData::setPoP(const QString &value)
{
    m_PoP = value;
}

void WeatherData::setPressure(const QString &value)
{
    m_Pressure = value;
}

void WeatherData::setHumidity(const QString &value)
{
    m_Humidity = value;
}

void WeatherData::setWindSpeed(const QString &value)
{
    m_WindSpeed = value;
}

void WeatherData::setWindDegree(const double value)
{
    m_WindDegree = value;
}

void WeatherData::setWeatherHourly(const QList<WeatherHourly*> value)
{
    m_WeatherHourly = value;
}
