/*
 * Author: Michal Debski
 * Date: 18/06/2019
 * Description: Class that gathers weather data on a specific day
*/

#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QObject>

struct WeatherHourly
{
    QString m_Hour;
    QString m_Temp;
    QString m_WeatherDescription;
};

class WeatherData : public QObject
{
    Q_OBJECT
public:
    explicit WeatherData(QObject *parent = nullptr);
    WeatherData(const WeatherData &other);

    QString getDate() const;
    QString getMonth() const;
    QString getWeatherIconSymbol() const;
    QString getWeatherDescription() const;
    QString getTempDay() const;
    QString getTempMin() const;
    QString getTempMax() const;
    QString getPoP() const;
    QString getPressure() const;
    QString getHumidity() const;
    QString getWindSpeed() const;
    double getWindDegree() const;
    QList<WeatherHourly*> *getWeatherHourly();


    void setDate(const QString &value);
    void setMonth(const QString &value);
    void setWeatherIcon(const QString &value);
    void setWeatherDescription(const QString &value);
    void setTemperatureDay(const QString &value);
    void setTemperatureMin(const QString &value);
    void setTemperatureMax(const QString &value);
    void setPoP(const QString &value);
    void setPressure(const QString &value);
    void setHumidity(const QString &value);
    void setWindSpeed(const QString &value);
    void setWindDegree(const double value);
    void setWeatherHourly(const QList<WeatherHourly*> value);

signals:
    void dataChanged();

public slots:

private:
    QString m_Date;
    QString m_Month;
    QString m_WeatherIcon;
    QString m_WeatherDesc;
    QString m_TemperatureDay;
    QString m_TemperatureMin;
    QString m_TemperatureMax;
    QString m_PoP;
    QString m_Pressure;
    QString m_Humidity;
    QString m_WindSpeed;
    double m_WindDegree;
    QList<WeatherHourly*> m_WeatherHourly;

};

#endif // WEATHERDATA_H
