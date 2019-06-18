/*
 * Author: Michal Debski
 * Date: 18/06/2019
 * Description: Class that requests data directly from Open Weather App and processes it
*/

#ifndef WEATHERAPP_H
#define WEATHERAPP_H

#include <QObject>
#include <QNetworkReply>
#include <QtQml/QQmlListProperty>

#include "weatherappcontrol.h"
#include "weatherdata.h"


#define WEATHER_FORECAST1_PAGE 0
#define WEATHER_FORECAST2_PAGE 1
#define WEATHER_FORECAST3_PAGE 2
#define WEATHER_FORECAST4_PAGE 3
#define WEATHER_MAIN_PAGE 4
#define WEATHER_LOADING_PAGE 5
#define WEATHER_RECONNECTING_PAGE 6


class WeatherApp : public QObject
{
    Q_OBJECT

   public:
    explicit WeatherApp(QObject *parent = nullptr);
    ~WeatherApp();

    void setCity(const QString &value);

    ///
    /// \brief Basing on parameters take approperiate actions regarding to a query error
    /// \param tryAgain
    /// \param errorCode
    ///
    void hadError(bool tryAgain, int errorCode);

    WeatherData *getCurrentWeather() const;

signals:
    void appReady();
    void forecastChanged(QList<WeatherData*> &fc);
    void currentWeatherChanged(WeatherData* w);
    void cityChanged(const QString &cityName);
    void invalidInput();
    void networkError();

public slots:
    Q_INVOKABLE void refreshWeather();

private slots:
    ///
    /// \brief Request and process current weather request
    /// \param networkReply
    ///
    void handleCurrentWeatherNetworkData(QNetworkReply *networkReply);

    ///
    /// \brief Request and process current weather in next 9 hours
    /// \param networkReply
    ///
    void handleCurrentHourlyWeatherNetworkData(QNetworkReply *networkReply);

    ///
    /// \brief Request and process weather for next 4 days
    /// \param networkReply
    ///
    void handleForecastNetworkData(QNetworkReply *networkReply);

    void queryCity();

private:
    QList<WeatherData*> list;
    QString tempToStr(const double temp);
    QString toCamelCase(const QString &value);
    WeatherAppControl *app;

};

#endif // WEATHERAPP_H
