//-----------------------------------------------------------------------------
// File: KactusTabBarStyle.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 30.07.2026
//
// Description:
// Overrides the default style for tab bars (used e.g. in QTabWidget)
//-----------------------------------------------------------------------------

#pragma once

#include <QProxyStyle>

class KactusTabBarStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    /*!
     *  Override placement of sub-elements.
     *
     *    @param[in] element        The element whose area to override
     *    @param[in] option         The style option, which contains information for drawing.
     *    @param[in] widget         Widget being drawn.
     *
     *    @return The modified rect for the sub element.
     */
    QRect subElementRect(QStyle::SubElement element, const QStyleOption* option, const QWidget* widget) const override;
};
