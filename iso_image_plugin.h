/***************************************************************************
 *   Copyright (C) 2006 by Dmitry Morozhnikov   *
 *   dmiceman@mail.ru   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _ISOIMAGEPLUGIN_H_
#define _ISOIMAGEPLUGIN_H_

#include <konq_popupmenuplugin.h>

#include "fuseisolib.h"

class KAction;
class KonqPopupMenuInformation;

class IsoImageMenu : public KonqPopupMenuPlugin
{
    Q_OBJECT

    public:
        IsoImageMenu( KonqPopupMenu* , const QStringList& list );
        virtual ~IsoImageMenu();
        // implement pure virtual method from KonqPopupMenuPlugin
        virtual void setup( KActionCollection* actionCollection, const KonqPopupMenuInformation& popupMenuInfo, QMenu* menu );

    private slots:
        void slotMount();
        void slotMountAndBrowse();
        void slotUmount();
        void slotBrowse();

    protected:

    private:
        QString image_file;
        QString mount_point;
        FuseisoLib flib;

        bool checkImage(const QString fname, QString& label);

        QStringList m_list;
        QWidget* m_parentwidget;
        KonqPopupMenu* m_popupmenu;

};

#endif

