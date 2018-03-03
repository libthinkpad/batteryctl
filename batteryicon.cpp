/*
 * Copyright (c) 2017 Ognjen Galić
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/

#include "batteryicon.h"

#include <QPainter>

BatteryIcon::BatteryIcon(QWidget *parent = NULL) : QWidget(parent)
{
    this->m_percentage = 0;
}

void BatteryIcon::setPercentage(int percentage)
{
    this->m_percentage = percentage;
    this->repaint();
}

int BatteryIcon::percentage() const
{
    return this->m_percentage;
}

void BatteryIcon::paintEvent(QPaintEvent *event)
{
    (void)event;

    QPainter painter(this);

    int width = this->width() - 8;
    int height = this->height();
    int margin = 0;

    QLinearGradient background(0, 0, 0, height / 2);
    background.setColorAt(0, QColor("#6c6c6c"));
    background.setColorAt(1, QColor("#000000"));

    QBrush brush(background);
    painter.setBrush(brush);

    painter.drawRect(margin, margin, width - (2 * margin), height - (2 * margin));

    QLinearGradient fill(0, 0, 0, height / 0.5);

    if (m_percentage < 20) {
        fill.setColorAt(0, QColor("#f0ad6d"));
        fill.setColorAt(1, QColor("#cb7c00"));
    } else {
        fill.setColorAt(0, QColor("#5afe5a"));
        fill.setColorAt(1, QColor("#044f04"));
    }

    brush = QBrush(fill);
    painter.setBrush(brush);

    int fillMargin = margin + 3;
    painter.drawRect(fillMargin, fillMargin, (width -
                     (2 * fillMargin)) * (m_percentage / 100.0f), height - (2 * fillMargin) - 1);

    brush = QBrush();
    brush.setColor(QColor("#000000"));
    brush.setStyle(Qt::SolidPattern);

    painter.setBrush(brush);
    painter.drawRect(width, height / 3, width + 10, height / 3);

    QPen pen;
    pen.setColor(QColor("#FFFFFF"));
    painter.setPen(pen);
    painter.setBrush(brush);

    painter.setFont(QFont("arial", height / 3));
    painter.drawText(0, 0, width, height, Qt::AlignCenter,
                     QString::number(m_percentage) + QString("%")); // I love c++

}
