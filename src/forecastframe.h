/*
 * Author: Michal Debski
 * Date: 18/06/2019
 * Description: Class that provides button functionality to QFrame
*/

#ifndef FORECASTFRAME_H
#define FORECASTFRAME_H

#include <QFrame>
#include <QMouseEvent>

class ForecastFrame : public QFrame
{
    Q_OBJECT
public:
    ForecastFrame(QWidget *widget = nullptr);
signals:
    void frameClicked(int);

protected:
    virtual void mousePressEvent(QMouseEvent *e);

private:
    int m_FrameID;
    static int m_FrameCount;
};

#endif // FORECASTFRAME_H
