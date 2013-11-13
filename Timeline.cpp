/*
 * Copyright (c) 2013, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         Paint Events on Timeline
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "Timeline.h"
#include "data_info/rpl_data.h"
#include <math.h>
#include <QPainter>
#include <QStyle>

Timeline::Timeline(QWidget * parent):
QSlider(parent)
{
}


void
Timeline::paintEvent(QPaintEvent * ev)
{
    QPainter painter;
    int max = maximum();
    int min = minimum();
    int w = width();
    int h = height();
    int maxVersion = rpldata_get_wsn_last_version();
    int version = 1;
    double timestamp;
    static int offset = -12;

    if(offset < 0)
        offset = style()->pixelMetric(QStyle::PM_SliderLength);



    painter.begin(this);
    int current_position = 0;
    qreal alpha = 0;
    bool has_errors = false;

    while(version <= maxVersion) {
        timestamp = rpldata_wsn_version_get_timestamp(version);
        int position = QStyle::sliderPositionFromValue(min, max, ceil(timestamp * 100), w - offset) + offset / 2;

        if(position == current_position) {
            alpha = 64 + (255 - 64) * alpha / 255;
        } else {
            if(has_errors) {
                painter.setPen(QColor(255, 0, 0, 255));
            } else {
                painter.setPen(QColor(0, 0, 0, alpha));
            }
            painter.drawLine(current_position, 0, current_position, h);
            alpha = 64;
            has_errors = false;
            current_position = position;
        }
        has_errors = has_errors || rpldata_wsn_version_get_has_errors(version);
        if(rpldata_wsn_version_get_has_errors(version)) {
            //printf("e: %d (%f)\n", version, timestamp);
        }
        version++;
    }
    if(has_errors) {
        painter.setPen(QColor(255, 0, 0, alpha));
    } else {
        painter.setPen(QColor(0, 0, 0, alpha));
    }
    painter.drawLine(current_position, 0, current_position, h);
    painter.end();

    QSlider::paintEvent(ev);
}
