#include "forecastframe.h"

#include <QApplication>
#include <QMainWindow>

int ForecastFrame::m_FrameCount = 0;

ForecastFrame::ForecastFrame(QWidget *widget) : QFrame(widget)
{
    m_FrameID = m_FrameCount;
    m_FrameCount++;
}

void ForecastFrame::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() == Qt::LeftButton)
    {
        emit frameClicked(m_FrameID);
    }
}

