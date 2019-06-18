/*
 * Author: Michal Debski
 * Date: 18/06/2019
 * Description: Class that provides low level functionality to WeatherApp class
*/

#ifndef WEATHERAPPCONTROL_H
#define WEATHERAPPCONTROL_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkSession>
#include <QList>
#include <QTimer>
#include <QElapsedTimer>

#include "weatherdata.h"

class WeatherAppControl
{
public:
    static const int baseMsBeforeNewRequest = 1000;
    QString city;
    QNetworkAccessManager *nam;
    QNetworkSession *ns;
    WeatherData current;
    QList<WeatherData*> forecast;
    bool ready;
    bool celsiusEnabled;
    int nErrors;

    QElapsedTimer throttle;
    int minMSBeforeNewRequest;
    QTimer delayedCityRequestTimer;
    QTimer requestNewWeatherTimer;

    WeatherAppControl();

    QString getAPIKey();

private:
    QString api_key;
};

#endif // WEATHERAPPCONTROL_H
