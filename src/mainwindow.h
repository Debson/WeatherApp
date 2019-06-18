/*
 * Author: Michal Debski
 * Date: 18/06/2019
 * Description: Class manages Main Window content and is a medium between GUI and the logic from WeatherApp class
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QPixmap>
#include <QList>
#include <QMouseEvent>
#include <QPushButton>

#include "weatherapp.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    ///
    /// \brief Initial setup of application
    ///
    void setup();

public slots:
    ///
    /// \brief Update forecast labels including their individual pages on signal from WeatherApp object
    /// \param list -- List of WeatherData objects that provide necessary information to update labels
    ///
    void updateForecastLabels(QList<WeatherData*> &list);

    ///
    /// \brief Update today's weather(only on main page card)
    /// \param w -- Current weather WeatherData object
    ///
    void updateCurrentWeatherLabels(WeatherData* w);

    ///
    /// \brief Update city name label(only when city is valid)
    /// \param cityName
    ///
    void updateCityLabel(const QString &cityName);

    ///
    /// \brief Load page card with given index
    /// \param pageCount -- page index in stacked widget
    ///
    void loadPage(int pageCount = 4);

    ///
    /// \brief Change page card to Main page as soon as we have all weather data
    ///
    void weatherAppReady();

    ///
    /// \brief Invalid input of city name(display error message)
    ///
    void invalidInput();

    ///
    /// \brief Connection problem, switch to "Reconnecting" page card
    ///
    void networkError();

signals:


private:
    WeatherApp *weatherApp;
    Ui::MainWindow *ui;
    QPushButton *buttons[9];

    ///
    /// \brief Get Wind direction(east, west, north, south)
    /// \param deg
    /// \return
    double getWindDirection(double deg);

    ///
    /// \brief Get resource asset icon nanem basing on weather code
    /// \param code
    /// \return
    QString getWeatherIconName(QString code);

private slots:
    void on_lineEdit_returnPressed();
};

#endif // MAINWINDOW_H
