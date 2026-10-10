/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

pragma ComponentBehavior: Bound

import QtQuick

import Muse.Ui
import Muse.UiComponents
import Muse.Dock

import MuseScore.Project

DockPage {
    id: root

    property string section: "scores"
    property string subSection: ""

    property var window: null

    objectName: "Home"
    uri: "musescore://home"

    onSetParamsRequested: function(params) {
        if (Boolean(params["section"])) {
            setCurrentCentral(params["section"])

            if (Boolean(params["subSection"])) {
                subSection = params["subSection"]
            }
        }
    }

    onSectionChanged: {
        Qt.callLater(root.setCurrentCentral, section)
    }

    function setCurrentCentral(name) {
        if (section === name || !Boolean(name)) {
            return
        }

        section = name

        switch (name) {
        case "scores": root.central = scoresComp; break
        case "extensions": root.central = extensionsComp; break
        }
    }

    panels: [
        DockPanel {
            id: menuPanel

            objectName: "homeMenu"

            readonly property int maxFixedWidth: 260
            readonly property int minFixedWidth: 76
            readonly property bool iconsOnly: root.window
                                                ? root.window.width < (root.window.minimumWidth + maxFixedWidth - minFixedWidth)
                                                : false
            readonly property int currentFixedWidth: iconsOnly ? minFixedWidth : maxFixedWidth

            width: currentFixedWidth
            minimumWidth: currentFixedWidth
            maximumWidth: currentFixedWidth

            floatable: false
            closable: false

            HomeMenu {
                currentPageName: root.section
                iconsOnly: menuPanel.iconsOnly

                onSelected: function(name) {
                    root.setCurrentCentral(name)
                }
            }
        }
    ]

    central: scoresComp

    Component {
        id: scoresComp

        ScoresPage {}
    }

    Component {
        id: extensionsComp

        PluginsPage {}
    }

}
