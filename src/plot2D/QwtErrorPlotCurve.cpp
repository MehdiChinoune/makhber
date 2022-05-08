/***************************************************************************
    File                 : QwtErrorPlotCurve.cpp
    Project              : Makhber
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Error bars curve

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#include "QwtErrorPlotCurve.h"

#include "plot2D/QwtBarCurve.h"
#include "aspects/column/Column.h"

#include <qwt_painter.h>
#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_interval_symbol.h>

#include <QPainter>
#include <QLocale>

QwtErrorPlotCurve::QwtErrorPlotCurve(Qt::Orientation orientation, Table *t, const QString &name)
    : DataCurve(t, QString(), name), d_master_curve(nullptr)
{
    cap = 10;
    type = orientation;
    setType(Graph::ErrorBars);
}

QwtErrorPlotCurve::QwtErrorPlotCurve(Table *t, const QString &name)
    : DataCurve(t, QString(), name), d_master_curve(nullptr)
{
    cap = 10;
    type = Qt::Vertical;
    setType(Graph::ErrorBars);
}

void QwtErrorPlotCurve::copy(const QwtErrorPlotCurve *e)
{
    cap = e->cap;
    type = e->type;
    setPen(e->pen());
    err = e->err;
}

void QwtErrorPlotCurve::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
                             [[maybe_unused]] const QRectF &canvasRect) const
{
    if (!painter || dataSize() == 0)
        return;

    painter->save();
    painter->setPen(pen());
    drawErrorBars(painter, xMap, yMap, 0, dataSize() - 1);
    painter->restore();
}

void QwtErrorPlotCurve::drawErrorBars(QPainter *painter, const QwtScaleMap &xMap,
                                      const QwtScaleMap &yMap, int from, int to) const
{
    double d_xOffset = 0.0;
    double d_yOffset = 0.0;
    if (d_master_curve->type() == Graph::VerticalBars)
        d_xOffset = (dynamic_cast<QwtBarCurve *>(d_master_curve))->dataOffset();
    else if (d_master_curve->type() == Graph::HorizontalBars)
        d_yOffset = (dynamic_cast<QwtBarCurve *>(d_master_curve))->dataOffset();

    QwtIntervalSymbol errorBar(QwtIntervalSymbol::Bar);
    errorBar.setWidth(cap);
    for (int i = from; i <= to; i++) {
        const int xi = xMap.transform(sample(i).x() + d_xOffset);
        const int yi = yMap.transform(sample(i).y() + d_yOffset);

        if (type == Qt::Vertical) {
            int y_plus = yMap.transform(sample(i).y() + err[i]);
            int y_minus = yMap.transform(sample(i).y() - err[i]);
            errorBar.draw(painter, type, QPointF(xi, y_minus), QPointF(xi, y_plus));

            /*bool y_minus_is_finite = true;
            if (yMap.transformation()->type() == QwtScaleTransformation::Log10 && err[i] >= y(i))
            { y_minus = yMap.transform(qMin(yMap.s1(), yMap.s2())); y_minus_is_finite = false;
            }*/
        } else if (type == Qt::Horizontal) {
            int x_plus = xMap.transform(sample(i).x() + err[i]);
            int x_minus = xMap.transform(sample(i).x() - err[i]);
            errorBar.draw(painter, type, QPointF(x_minus, yi), QPointF(x_plus, yi));

            /*bool x_minus_is_finite = true;
            if (xMap.transformation()->type() == QwtScaleTransformation::Log10 && err[i] >= x(i))
            { x_minus = xMap.transform(qMin(xMap.s1(), xMap.s2())); x_minus_is_finite = false;
            }*/
        }
    }
}

double QwtErrorPlotCurve::errorValue(int i)
{
    if (i >= 0 && i < static_cast<int>(dataSize()))
        return err[i];
    else
        return 0.0;
}

bool QwtErrorPlotCurve::xErrors()
{
    bool x = false;
    if (type == Qt::Horizontal)
        x = true;

    return x;
}

void QwtErrorPlotCurve::setXErrors(bool yes)
{
    if (yes)
        type = Qt::Horizontal;
    else
        type = Qt::Vertical;
}

void QwtErrorPlotCurve::setWidth(int w)
{
    QPen p = pen();
    p.setWidth(w);
    setPen(p);
}

void QwtErrorPlotCurve::setColor(const QColor &c)
{
    QPen p = pen();
    p.setColor(c);
    setPen(p);
}

QRectF QwtErrorPlotCurve::boundingRect() const
{
    QRectF rect = QwtPlotCurve::boundingRect();

    int size = static_cast<int>(dataSize());

    QVector<double> X(size), Y(size), min(size), max(size);
    for (int i = 0; i < size; i++) {
        X[i] = sample(i).x();
        Y[i] = sample(i).y();
        if (type == Qt::Vertical) {
            min[i] = sample(i).y() - err[i];
            max[i] = sample(i).y() + err[i];
        } else {
            min[i] = sample(i).x() - err[i];
            max[i] = sample(i).x() + err[i];
        }
    }

    /* QwtArrayData *erMin = nullptr, *erMax = nullptr;
    if (type == Vertical) {
        erMin = new QwtArrayData(X, min);
        erMax = new QwtArrayData(X, max);
    } else {
        erMin = new QwtArrayData(min, Y);
        erMax = new QwtArrayData(max, Y);
    }

    QRectF minrect = erMin->boundingRect();
    QRectF maxrect = erMax->boundingRect();

    rect.setTop(qMin(minrect.top(), maxrect.top()));
    rect.setBottom(qMax(minrect.bottom(), maxrect.bottom()));
    rect.setLeft(qMin(minrect.left(), maxrect.left()));
    rect.setRight(qMax(minrect.right(), maxrect.right()));

    delete erMin;
    delete erMax;*/

    return rect;
}

void QwtErrorPlotCurve::setMasterCurve(DataCurve *c)
{
    if (!c || d_master_curve == c)
        return;

    d_master_curve = c;
    setAxes(c->xAxis(), c->yAxis());
    d_start_row = c->startRow();
    d_end_row = c->endRow();
    c->addErrorBars(this);

    loadData();
}

bool QwtErrorPlotCurve::loadData()
{
    if (!d_master_curve)
        return false;
    Table *mt = d_master_curve->table();
    if (!mt)
        return false;
    Column *x = mt->column(d_master_curve->xColumnName());
    Column *y = mt->column(d_master_curve->title().text());

    Column *err = d_table->column(title().text());

    if (!x || !y || !err) {
        remove();
        return false;
    }

    QList<QVector<double>> data = convertData(
            d_master_curve->type() == Graph::HorizontalBars ? (QList<Column *>() << y << x << err)
                                                            : (QList<Column *>() << x << y << err),
            QList<int>() << xAxis() << yAxis() << (type == Qt::Horizontal ? xAxis() : yAxis()));

    if (data.isEmpty() || data[0].size() == 0) {
        remove();
        return false;
    }

    setSamples(data[0].data(), data[1].data(), data[0].size());
    setErrors(data[2]);

    return true;
}

QString QwtErrorPlotCurve::plotAssociation()
{
    if (!d_master_curve)
        return QString();

    QString base = d_master_curve->xColumnName() + "(X)," + d_master_curve->title().text() + "(Y),"
            + title().text();
    if (type == Qt::Horizontal)
        return base + "(xErr)";
    else
        return base + "(yErr)";
}

bool QwtErrorPlotCurve::updateData(Table *t, const QString &colName)
{
    if (d_table != t || colName != title().text())
        return false;

    loadData();
    return true;
}
