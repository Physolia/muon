/*
 *   Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
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
import QtQuick.Controls 1.1

Item {
    property QtObject category: null
    property real proposedMargin: 0
    
    Image {
        id: iconItem
        source: "image://icon/"+category.icon
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            leftMargin: parent.proposedMargin
            margins: 15
        }
        width: height
    }
    
    Label {
        anchors {
            verticalCenter: parent.verticalCenter
            left: iconItem.right
            right: parent.right
            rightMargin: parent.proposedMargin
            leftMargin: 50
        }
        elide: Text.ElideRight
        font.pointSize: parent.height*0.5
        text: category.name
    }
}
