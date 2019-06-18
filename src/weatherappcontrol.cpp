#include "weatherappcontrol.h"
#include <QDebug>

WeatherAppControl::WeatherAppControl() :
    nam(nullptr),
    ns(nullptr),
    ready(false),
    celsiusEnabled(true),
    nErrors(0),
    minMSBeforeNewRequest(baseMsBeforeNewRequest)
{
    delayedCityRequestTimer.setSingleShot(true);
    delayedCityRequestTimer.setInterval(1000);
    requestNewWeatherTimer.setSingleShot(false);
    requestNewWeatherTimer.setInterval(10*60*1000); // Refresh weather every 10mins
    throttle.invalidate();

    api_key = QString("ae3723984918e29156906ffa2182bf02");
}

QString WeatherAppControl::getAPIKey()
{
    return api_key;
}
