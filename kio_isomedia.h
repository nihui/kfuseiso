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


// heavy based on kio_media

#ifndef _KIO_ISOMEDIA_H_
#define _KIO_ISOMEDIA_H_

#include <kio/forwardingslavebase.h>
#include <KStandardDirs>

#include "fuseisolib.h"

class IsoMediaProtocol : public KIO::ForwardingSlaveBase
{
    Q_OBJECT

    public:
        IsoMediaProtocol(const QByteArray &protocol, const QByteArray &pool,
                  const QByteArray &app);
        virtual ~IsoMediaProtocol();

        virtual bool rewriteUrl(const KUrl &url, KUrl &newUrl);

        virtual void stat(const KUrl &url);
        virtual void listDir(const KUrl &url);

    private slots:
        void slotWarning( const QString &msg );

    protected:
        void listRoot();
        void createTopLevelEntry(KIO::UDSEntry &entry);
        void createMountEntry(KIO::UDSEntry &entry);

        FMountPoint::List mtab;
        KStandardDirs sdirs;
};

#endif
