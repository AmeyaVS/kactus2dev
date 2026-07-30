#include "KactusTabBarStyle.h"

#include <QTabBar>

QRect KactusTabBarStyle::subElementRect(QStyle::SubElement element, const QStyleOption* option, const QWidget* widget) const
{
    QRect rect = QProxyStyle::subElementRect(element, option, widget);

    if (widget == nullptr)
    {
        return rect;
    }

    // Customize tab bar with left scroll button on left side of tabs, right scroll button on right side.
    // Both buttons have reserved space.
    const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, option, widget);

    switch (element)
    {
    case QStyle::SE_TabBarScrollLeftButton:
        return QRect(0, 0, buttonWidth, widget->height() - 1);
    case QStyle::SE_TabBarScrollRightButton:
        return QRect(widget->width() - buttonWidth, 0, buttonWidth, widget->height() - 1);
    default:
        return rect;
    }

    return rect;
}