/*
 *   Copyright (C) 2012-2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Window 2.2

Flickable {
    property int columnCount: app.isCompact ? 1 : Math.max(Math.floor(app.actualWidth/minCellWidth), 1)
    property real cellWidth: (app.actualWidth-(columnCount-1)*dataFlow.spacing)/columnCount
    property int minCellWidth: 130
    property alias header: headerLoader.sourceComponent
    property alias footer: footerLoader.sourceComponent
    property alias delegate: dataRepeater.delegate
    property alias model: dataRepeater.model
    contentHeight: conts.height
    
    Column {
        id: conts
        width: app.actualWidth
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        Loader {
            id: headerLoader
            width: parent.width
        }
        
        Flow {
            id: dataFlow
            width: parent.width
            spacing: 10
            Repeater {
                id: dataRepeater
            }
        }
        
        Loader {
            id: footerLoader
            width: parent.width
        }
    }
}
