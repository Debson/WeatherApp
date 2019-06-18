#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>
#include <QPainter>
#include <QPixmap>
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), weatherApp(new WeatherApp)
{
    ui->setupUi(this);
    this->setWindowTitle("WeatherApp");

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();
    int wHeight = this->geometry().height();
    int wWidth = this->geometry().width();
    this->setGeometry((width - wWidth) / 2, (height - wHeight) / 2, this->geometry().width(), this->geometry().height());
    this->setFixedSize(wWidth, wHeight);



    // Dynamicaly create buttons on every forecat page that will go back to the Main page card
    for(int i = 0; i < 4; i++)
    {
        QWidget *page = this->findChild<QWidget*>("forecast" + QString::number(i + 1) + "Page");
        buttons[i] = new QPushButton(page);
        buttons[i]->setText("");
        buttons[i]->setGeometry(QRect(QPoint(20, 20), QSize(30, 30)));

        buttons[i]->setStyleSheet("border-image:url(:resources/icons/back.svg); "
                                  "background-color:transparent;");

        connect(buttons[i], SIGNAL(clicked()), this, SLOT(loadPage()));

    }

    // Create connection with WeatherApp object that will invoke THIS class functions on specific signal
    connect(weatherApp, SIGNAL(forecastChanged(QList<WeatherData*>&)), this, SLOT(updateForecastLabels(QList<WeatherData*>&)));
    connect(weatherApp, SIGNAL(currentWeatherChanged(WeatherData*)), this, SLOT(updateCurrentWeatherLabels(WeatherData*)));
    connect(weatherApp, SIGNAL(cityChanged(const QString&)), this, SLOT(updateCityLabel(const QString&)));
    connect(weatherApp, SIGNAL(appReady()), this, SLOT(weatherAppReady()));
    connect(weatherApp, SIGNAL(invalidInput()), this, SLOT(invalidInput()));
    connect(weatherApp, SIGNAL(networkError()), this, SLOT(networkError()));


    // Create connection between THIS class and every forecast individual page
    for(int i = 0; i < 4; i++)
    {
        ForecastFrame * frame = this->findChild<ForecastFrame*>("forecast" + QString::number(i + 1));
        connect(frame, SIGNAL(frameClicked(int)), this, SLOT(loadPage(int)));
    }

    setup();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete weatherApp;
}

void MainWindow::setup()
{
    // Defauly location
    weatherApp->setCity("Dublin");

    // Disable invalid input text
    ui->wrongInput->setVisible(false);

    // Start the app from loading page
    ui->stackedWidget->setCurrentIndex(WEATHER_LOADING_PAGE);
}

void MainWindow::updateForecastLabels(QList<WeatherData*> &list)
{
    ui->wrongInput->setVisible(false);
    ui->stackedWidget->setCurrentIndex(WEATHER_MAIN_PAGE);

    for(int i = 0; i < list.count(); i++)
    {
        // Main card
        QLabel *label = this->findChild<QLabel*>(QStringLiteral("day") + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getDate());
           label = nullptr;
        }

        label = this->findChild<QLabel*>(QStringLiteral("temp") + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getTempDay());
           label = nullptr;
        }

        label = this->findChild<QLabel*>(QStringLiteral("description") + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getWeatherDescription());
           label = nullptr;
        }

        label = this->findChild<QLabel*>(QStringLiteral("pressure") + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getPressure() + " hPa");
           label = nullptr;
        }

        label = this->findChild<QLabel*>(QStringLiteral("low") + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getTempMin() + " Low");
           label = nullptr;
        }

        // Forecast page cards
        label = this->findChild<QLabel*>(QStringLiteral("cityPage") + QString::number(i + 1));
        if(label)
        {
           label->setText(ui->city->text());
           label = nullptr;
        }

        label = this->findChild<QLabel*>(QStringLiteral("datePage") + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getDate());
           label = nullptr;
        }

        label = this->findChild<QLabel*>("descriptionPage" + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getWeatherDescription());
           label = nullptr;
        }

        label = this->findChild<QLabel*>("humidityPage" + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getHumidity() + "% Humidity");
           label = nullptr;
        }

        label = this->findChild<QLabel*>("monthPage" + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getMonth());
           label = nullptr;
        }

        label = this->findChild<QLabel*>("pressurePage" + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getPressure() + " hPa");
           label = nullptr;
        }

        label = this->findChild<QLabel*>("tempHighPage" + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getTempMax() + " High");
           label = nullptr;
        }

        label = this->findChild<QLabel*>("tempLowPage" + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getTempMin() + " Low");
           label = nullptr;
        }

        label = this->findChild<QLabel*>("tempPage" + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getTempDay());
           label = nullptr;
        }

        label = this->findChild<QLabel*>("windSpeedPage" + QString::number(i + 1));
        if(label)
        {
           label->setText(list.at(i)->getWindSpeed() + " km/h");
           label = nullptr;
        }

        label = this->findChild<QLabel*>("windIconPage" + QString::number(i + 1));
        if(label)
        {
            double deg = getWindDirection(list.at(i)->getWindDegree());
            QPixmap pix(":resources/icons/wind_deg.svg");
            pix = pix.transformed(QTransform()
                                  .translate(ui->windIcon->x(), ui->windIcon->y())
                                  .rotate(deg)
                                  .translate(-ui->windIcon->x(), -ui->windIcon->y()));
            pix = pix.scaled(label->size(), Qt::KeepAspectRatio);
            label->setPixmap(pix);
            label = nullptr;
        }

        label = this->findChild<QLabel*>("weatherIconPage" + QString::number(i + 1));
        if(label)
        {
            int size = 96;
            QString iconName = getWeatherIconName(list.at(i)->getWeatherIconSymbol());
            QPixmap pix = QIcon(":resources/icons/" + iconName).pixmap(QSize(size, size));
            pix = pix.transformed(QTransform().translate(ui->windIcon->x() - size, ui->windIcon->y()));
            pix = pix.scaled(label->size(), Qt::KeepAspectRatio);
            label->setPixmap(pix);
            label = nullptr;
        }

    }
}

void MainWindow::updateCurrentWeatherLabels(WeatherData *weather)
{
    ui->temp->setText(weather->getTempDay());
    ui->description->setText(weather->getWeatherDescription());
    ui->windSpeed->setText(weather->getWindSpeed() + QString(" km/h"));

    double deg = getWindDirection(weather->getWindDegree());
    QPixmap pix(":resources/icons/wind_deg.svg");
    pix = pix.transformed(QTransform()
                          .translate(ui->windIcon->x(), ui->windIcon->y())
                          .rotate(deg)
                          .translate(-ui->windIcon->x(), -ui->windIcon->y()));

    ui->windIcon->setPixmap(pix);

    int cnt = 1;
    for(auto & i : *weather->getWeatherHourly())
    {
        QLabel *label = this->findChild<QLabel*>("hour" + QString::number(cnt));
        if(label)
        {
            label->setText(i->m_Hour);
            label = nullptr;
        }

        label = this->findChild<QLabel*>("tempHour" + QString::number(cnt));
        if(label)
        {
            label->setText(i->m_Temp);
            label = nullptr;
        }


        cnt++;
    }
}

void MainWindow::updateCityLabel(const QString &cityName)
{
    ui->city->setText(cityName);
}

void MainWindow::invalidInput()
{
    ui->wrongInput->setVisible(true);
}

void MainWindow::networkError()
{
    ui->stackedWidget->setCurrentIndex(WEATHER_RECONNECTING_PAGE);
}

void MainWindow::loadPage(int frameCount)
{
    //qDebug("FRAME " + QString::number(frameCount).toUtf8());
    ui->stackedWidget->setCurrentIndex(frameCount);
}

void MainWindow::on_lineEdit_returnPressed()
{
    //qDebug("Text changed");
    weatherApp->setCity(ui->lineEdit->text());
}

void MainWindow::weatherAppReady()
{
    ui->stackedWidget->setCurrentIndex(4);
}

double MainWindow::getWindDirection(double deg)
{
    double d = 0.0;
    if((deg >= 0 && deg < 45) || (deg > 315 && deg <= 360))
        d = 0;
    else if(deg >= 45 && deg < 135)
        d = 90;
    else if(deg >= 135 && deg < 225)
        d = 180;
    else if(deg >= 225 && deg <= 315)
        d = 270;

    return d;
}

QString MainWindow::getWeatherIconName(QString code)
{
    QString weatherName;
    int codeNum = code.mid(0, code.count() - 1).toInt();

    //qDebug() << code;
    switch(codeNum)
    {
    case 1:{
        weatherName = "weather_sunny.svg";
        break;
    }
    case 2:{
        weatherName = "weather_few_clouds.svg";
        break;
    }
    case 3:{
        weatherName = "weather_cloudy.svg";
        break;
    }
    case 4:{
        weatherName = "weather_cloudy.svg";
        break;
    }
    case 9:{
        weatherName = "weather_shower_rain.svg";
        break;
    }
    case 10:{
        weatherName = "weather_rain.svg";
        break;
    }
    case 11:{
        weatherName = "weather_thunderstorm.svg";
        break;
    }
    case 13:{
        weatherName = "weather_snow.svg";
        break;
    }
    case 50:{
        weatherName = "weather_mist.svg";
        break;
    }
    default: {
        weatherName = "weather_sunny.svg";
        break;
    }
    }

    return weatherName;
}
