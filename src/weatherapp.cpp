#include "weatherapp.h"

#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QLoggingCategory>
#include <QUrlQuery>

#define ZERO_KELIVN 273.15


WeatherApp::WeatherApp(QObject *parent) : QObject(parent), app(new WeatherAppControl)
{
    connect(&app->delayedCityRequestTimer, SIGNAL(timeout()), this, SLOT(queryCity()));
    connect(&app->requestNewWeatherTimer, SIGNAL(timeout()), this, SLOT(refreshWeather()));
    app->requestNewWeatherTimer.start();

    app->nam = new QNetworkAccessManager(this);

    QNetworkConfigurationManager ncm;
    app->ns = new QNetworkSession(ncm.defaultConfiguration(), this);

    //if(app->ns->isOpen())
    app->city = QString("Dublin");

    app->ns->open();
}

WeatherApp::~WeatherApp()
{
    app->ns->close();

    delete app;
}

void WeatherApp::queryCity()
{
    if(app->throttle.isValid() && app->throttle.elapsed() < app->minMSBeforeNewRequest)
    {
        if(!app->delayedCityRequestTimer.isActive())
            app->delayedCityRequestTimer.start();
        return;
    }

    app->throttle.start();
    app->minMSBeforeNewRequest = (app->nErrors + 1) * app->baseMsBeforeNewRequest;

    refreshWeather();
}

void WeatherApp::hadError(bool tryAgain, int errorCode)
{
    app->throttle.start();
    if(app->nErrors < 10)
    {
        app->nErrors++;
    }
    else
    {
        qDebug() << "Reconnecting...";
        emit networkError();
    }
    app->minMSBeforeNewRequest = (app->nErrors + 1) * app->baseMsBeforeNewRequest;
    if(tryAgain == true && errorCode != QNetworkReply::ContentNotFoundError)
    {
        app->delayedCityRequestTimer.start();
    }
    else if(errorCode == QNetworkReply::ContentNotFoundError)
    {
        qDebug("Invalid input...");
        emit invalidInput();
    }
}

void WeatherApp::refreshWeather()
{
    if(app->city.isEmpty())
    {
        qDebug("no city selcted");
        return;
    }

    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;

    query.addQueryItem("q", app->city);
    query.addQueryItem("mode", "json");
    query.addQueryItem("appid", app->getAPIKey());
    url.setQuery(query);

    QNetworkReply * rep = app->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() { handleCurrentWeatherNetworkData(rep); });

}

void WeatherApp::handleCurrentWeatherNetworkData(QNetworkReply *networkReply)
{
    if(!networkReply)
    {
        hadError(true, networkReply->error());
        return;
    }

    foreach(WeatherData *inf, app->forecast)
        delete inf;
    app->forecast.clear();

    foreach(WeatherHourly *inf, *(app->current.getWeatherHourly()))
        delete inf;
    app->current.getWeatherHourly()->clear();

    if(!networkReply->error())
    {
        app->nErrors = 0;
        // City name is a valid, update city label text
        emit cityChanged(app->city);

        // Extract current weather info
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        QJsonObject obj = document.object();
        QJsonObject subtree;
        QJsonObject tempObj;

        QJsonValue val = obj.value(QStringLiteral("weather"));
        QJsonArray weatherArray = val.toArray();
        val = weatherArray.at(0);
        tempObj = val.toObject();
        val = tempObj.value("description");
        app->current.setWeatherDescription(toCamelCase(val.toString()));
        subtree = obj.value(QStringLiteral("main")).toObject();
        val = subtree.value(QStringLiteral("temp"));
        app->current.setTemperatureDay(tempToStr(val.toDouble()));
        subtree = obj.value(QStringLiteral("wind")).toObject();
        val = subtree.value(QStringLiteral("speed"));
        app->current.setWindSpeed(QString().setNum(val.toDouble() * 3.6, 'f', 1));
        val = subtree.value(QStringLiteral("deg"));
        app->current.setWindDegree(val.toDouble());


        QUrl url("http://api.openweathermap.org/data/2.5/forecast");
        QUrlQuery query;

        query.addQueryItem("q", app->city);
        query.addQueryItem("mode", "json");
        query.addQueryItem("cnt", "3");
        query.addQueryItem("appid", app->getAPIKey());
        url.setQuery(query);

        QNetworkReply * rep = app->nam->get(QNetworkRequest(url));
        connect(rep, &QNetworkReply::finished, this, [this, rep]() { handleCurrentHourlyWeatherNetworkData(rep); });
    }
    else
    {
        hadError(true, networkReply->error());
    }

    networkReply->deleteLater();

}

void WeatherApp::handleCurrentHourlyWeatherNetworkData(QNetworkReply *networkReply)
{
    if(!networkReply)
        return;

    if(!networkReply->error())
    {
        // Extract weather info for next 9hrs
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
        QJsonObject obj = document.object();
        QJsonValue val = obj.value(QStringLiteral("list"));
        QJsonArray listArr = val.toArray();

        QList<WeatherHourly*> forecastEntries;
        for(int i = 0; i < listArr.count(); i++)
        {
            WeatherHourly *entry = new WeatherHourly;
            QJsonObject subtree = listArr.at(i).toObject();
            QJsonObject tempObj;
            obj = subtree.value(QStringLiteral("main")).toObject();
            val = obj.value(QStringLiteral("temp"));
            entry->m_Temp = tempToStr(val.toDouble());

            val = subtree.value(QStringLiteral("dt"));
            QDateTime dt = QDateTime::fromMSecsSinceEpoch(qint64(val.toDouble()) * 1000);
            entry->m_Hour = dt.time().toString(QStringLiteral("h:mm A"));

            val = subtree.value(QStringLiteral("weather"));
            QJsonArray weatherArray = val.toArray();
            val = weatherArray.at(0);
            tempObj = val.toObject();
            val = tempObj.value("description");
            entry->m_WeatherDescription = toCamelCase(val.toString());

            forecastEntries.append(entry);
        }
        app->current.setWeatherHourly(forecastEntries);
    }
    else
    {

    }
    networkReply->deleteLater();

    emit currentWeatherChanged(&app->current);

    // Forecast (for other days)

    QUrl url("http://api.openweathermap.org/data/2.5/forecast/daily");
    QUrlQuery query;

    query.addQueryItem("q", app->city);
    query.addQueryItem("mode", "json");
    query.addQueryItem("cnt", "5");
    query.addQueryItem("appid", app->getAPIKey());
    url.setQuery(query);

    QNetworkReply * rep = app->nam->get(QNetworkRequest(url));
    connect(rep, &QNetworkReply::finished, this, [this, rep]() { handleForecastNetworkData(rep); });
}

void WeatherApp::handleForecastNetworkData(QNetworkReply *networkReply)
{
    if(!networkReply)
        return;

    if(!networkReply->error())
    {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());

        QJsonObject jo;
        QJsonValue jv;
        QJsonObject root = document.object();
        jv = root.value(QStringLiteral("list"));
        if(!jv.isArray())
            qWarning() << "Invalid forecast object!";
        QJsonArray ja = jv.toArray();

        // Need to retrieve 4 other days. The first one is TODAY
        for(int i = 1; i < ja.count(); i++)
        {
            WeatherData *forecastEntry = new WeatherData();

            QJsonObject subtree = ja.at(i).toObject();
            jo = subtree.value(QStringLiteral("temp")).toObject();

            // get day temp
            jv = jo.value(QStringLiteral("day"));
            forecastEntry->setTemperatureDay(tempToStr(jv.toDouble()));

            // get lowest temp
            jv = jo.value(QStringLiteral("min"));
            forecastEntry->setTemperatureMin(tempToStr(jv.toDouble()));

            // get highest temp
            jv = jo.value(QStringLiteral("max"));
            forecastEntry->setTemperatureMax(tempToStr(jv.toDouble()));

            // get date
            jv = subtree.value(QStringLiteral("dt"));
            QDateTime dt = QDateTime::fromMSecsSinceEpoch(qint64(jv.toDouble()) * 1000);
            forecastEntry->setDate(dt.date().toString(QStringLiteral("dddd dd")));

            // get month
            forecastEntry->setMonth(dt.date().toString(QStringLiteral("MMMM")));

            // get icon
            QJsonArray weatherArray = subtree.value(QStringLiteral("weather")).toArray();
            jo = weatherArray.at(0).toObject();
            forecastEntry->setWeatherIcon(jo.value(QStringLiteral("icon")).toString());

            // get pressure
            jv = subtree.value(QStringLiteral("pressure"));
            forecastEntry->setPressure(QString::number(int(jv.toDouble())));

            // get humidity
            jv = subtree.value(QStringLiteral("humidity"));
            forecastEntry->setHumidity(QString::number(jv.toInt()));

            // get description
            jv = jo.value(QStringLiteral("description"));
            forecastEntry->setWeatherDescription(toCamelCase(jv.toString()));

            // get wind speed
            jv = subtree.value(QStringLiteral("speed"));
            forecastEntry->setWindSpeed(QString().setNum(jv.toDouble() * 3.6, 'f', 1));

            // get wind dir
            jv = subtree.value(QStringLiteral("deg"));
            forecastEntry->setWindDegree(jv.toDouble());

            app->forecast.append(forecastEntry);
        }

        if(!(app->ready))
        {
            app->ready = true;
            emit appReady();
        }

        emit forecastChanged(app->forecast);
    }

    networkReply->deleteLater();
}

QString WeatherApp::tempToStr(double temp)
{
    return QString::number(qRound(temp - ZERO_KELIVN)) + QString("\u00b0");
}

QString WeatherApp::toCamelCase(const QString &value)
{
    QStringList parts = value.split(' ', QString::SkipEmptyParts);
    for (int i = 0; i < parts.size(); i++)
        parts[i].replace(0, 1, parts[i][0].toUpper());

    return parts.join(" ");
}

WeatherData *WeatherApp::getCurrentWeather() const
{
    return &(app->current);
}

void WeatherApp::setCity(const QString &value)
{
    app->city = toCamelCase(value);
    refreshWeather();
}


















