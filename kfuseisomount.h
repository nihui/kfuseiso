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

#pragma once

#include <QCheckBox>

#include <KUrlRequester>
#include <KDialog>

// heavily based on KUrlRequesterDlg
// why the hell everything in kde classes declared private!?

class FuseisoMountDlg : public KDialog
{
    Q_OBJECT

    public:
        FuseisoMountDlg( QWidget* parent = 0 );
        ~FuseisoMountDlg() {}

        KUrl selectedURL() const;
        bool needOpenBrowser() const;

    private slots:
        void slotTextChanged(const QString &);

    private:
        KUrlRequester *urlRequester_;
        QCheckBox *openBrowser;
};
