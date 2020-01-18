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

#include <stdlib.h>

#include <QFileInfo>

#include <KDebug>
#include <KLocale>
#include <KApplication>
// #include <dcopclient.h>//NOTE: port it to dbus
#include <KCmdLineArgs>

#include "kio_isomedia.h"

#include "fuseisolib.h"

extern "C" int KDE_EXPORT kdemain( int argc, char **argv )
{
    // KApplication is necessary to use other ioslaves
    putenv(strdup("SESSION_MANAGER="));
    KCmdLineArgs::init( argc, argv, "kio_isomedia", "", KLocalizedString(), "" );

    KCmdLineOptions options;
    options.add( "+protocol", ki18n( "Protocol name" ) );
    options.add( "+pool", ki18n( "Socket name" ) );
    options.add( "+app", ki18n( "Socket name" ) );
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication app;/*( false, false );*///NOTE: port
    // We want to be anonymous even if we use DCOP
//     app.dcopClient()->attach();//NOTE: port it to dbus

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    IsoMediaProtocol slave( args->arg(0).toLocal8Bit(), args->arg(1).toLocal8Bit(), args->arg(2).toLocal8Bit() );//NOTE: port
    slave.dispatchLoop();
    return 0;
}

IsoMediaProtocol::IsoMediaProtocol(const QByteArray &protocol,
                             const QByteArray &pool, const QByteArray &app)
    : ForwardingSlaveBase(protocol, pool, app), mtab(), sdirs()
{
}

IsoMediaProtocol::~IsoMediaProtocol()
{
}

void IsoMediaProtocol::createTopLevelEntry(KIO::UDSEntry &entry) {
    entry.clear();
    entry.insert( KIO::UDSEntry::UDS_URL, "isomedia:/" );
    entry.insert( KIO::UDSEntry::UDS_NAME, "." );
    entry.insert( KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR );
    entry.insert( KIO::UDSEntry::UDS_ACCESS, 0555 );
    entry.insert( KIO::UDSEntry::UDS_MIME_TYPE, "inode/directory" );
    entry.insert( KIO::UDSEntry::UDS_ICON_NAME, "media-optical" );
}

void IsoMediaProtocol::createMountEntry(KIO::UDSEntry &entry) {
    entry.clear();
    entry.insert( KIO::UDSEntry::UDS_URL, QString( "file://%1" ).arg( sdirs.findResource( "data", "kfuseiso/mount.desktop" ) )  );
    // kdDebug() << "IsoMediaProtocol::createMountEntry: mount.desktop: " << sdirs.findResource("data", "kfuseiso/mount.desktop") << endl;
    entry.insert( KIO::UDSEntry::UDS_NAME, i18n( "Mount ISO image file" ) );
    entry.insert( KIO::UDSEntry::UDS_FILE_TYPE, S_IFREG );
    entry.insert( KIO::UDSEntry::UDS_ACCESS, 0444 );
    entry.insert( KIO::UDSEntry::UDS_MIME_TYPE, "application/x-desktop" );
    entry.insert( KIO::UDSEntry::UDS_ICON_NAME, "list-add" );
    entry.insert( KIO::UDSEntry::UDS_SIZE, QFileInfo( sdirs.findResource( "data", "kfuseiso/mount.desktop" ) ).size() );
}

bool IsoMediaProtocol::rewriteUrl(const KUrl &url, KUrl &newUrl)
{
    if(url.path() == "/mount.desktop" || url.path() == "isomedia:/mount.desktop") {
        newUrl = QString("file://%1").arg(sdirs.findResource("data", "kfuseiso/mount.desktop"));
        return true;
    }

    if(mtab.empty()) {
        mtab = FMountPoint::currentMountPoints();
//         kdDebug() << "read " << mtab.size() << " mtab entries" << endl;
    }
    FMountPoint::List::iterator it;
    for(it = mtab.begin(); it != mtab.end(); ++it) {
        KUrl mp((*it)->mountPoint());
        QString fname = "/" + mp.fileName();

//         kdDebug() << "trying " << fname << " for " << url.path() << endl;

        if(url.path().startsWith(fname)) {
            newUrl = mp.directory(/*true, true*/ KUrl::AppendTrailingSlash );//NOTE: port
            newUrl.addPath(url.path());
            newUrl.setProtocol("file");

//             kdDebug() << "IsoMediaProtocol::rewriteURL: " << url << " " << newUrl << endl << flush;
            return true;
        }
    }

    error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());

    kdDebug() << "IsoMediaProtocol::rewriteUrl: NOT FOUND! " << url << endl << flush;

//     kdDebug() << "IsoMediaProtocol::rewriteURL: " << url << " " << newUrl << endl;

    return false;
}

void IsoMediaProtocol::stat(const KUrl &url)
{
    // kdDebug() << "IsoMediaProtocol::stat: " << url << endl;

    QString path = url.path();
    if(path.startsWith("isomedia:")) {
        path = path.mid(9);
    }
    if(path.isEmpty() || path == "/") {
        // The root is "virtual" - it's not a single physical directory
        KIO::UDSEntry entry;
        createTopLevelEntry(entry);
        statEntry(entry);
        finished();
        return;
    }

    // more clarification needed for stat() mechanics; disabled for now
    if(path.indexOf('/', 1) == -1 and false) {
        KIO::UDSEntry entry;

        if(path.endsWith("mount.desktop")) {

            createMountEntry(entry);
            statEntry(entry);
            finished();
            return;

        }
        else {
            if(mtab.empty()) {
                mtab = FMountPoint::currentMountPoints();
            }
            FMountPoint::List::iterator it;
            for(it = mtab.begin(); it != mtab.end(); ++it) {
                KUrl fname((*it)->mountPoint());

                if(fname.fileName() == url.fileName()) {
                    entry.clear();

                    QString furl = "isomedia:/" + fname.fileName();
                    QString label = KIO::encodeFileName(fname.fileName());
                    entry.insert( KIO::UDSEntry::UDS_URL, furl );
                    entry.insert( KIO::UDSEntry::UDS_NAME, label );
                    entry.insert( KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR );
                    entry.insert( KIO::UDSEntry::UDS_MIME_TYPE, "inode/x-iso-image-mounted" );
                    entry.insert( KIO::UDSEntry::UDS_GUESSED_MIME_TYPE, "inode/directory" );
                    entry.insert( KIO::UDSEntry::UDS_ICON_NAME, "application-x-cd-image" );
                    entry.insert( KIO::UDSEntry::UDS_LOCAL_PATH, fname.path() );

                    statEntry(entry);
                    finished();
                    return;
                }
            }

            error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
        }
    }
    else {
        ForwardingSlaveBase::stat(url);
    }
}

void IsoMediaProtocol::listDir(const KUrl &url)
{
    // kdDebug() << "IsoMediaProtocol::listDir: " << url << endl;

    if(url.path().length() <= 1) {
        listRoot();
        return;
    }

    ForwardingSlaveBase::listDir(url);
}

void IsoMediaProtocol::listRoot()
{
    KIO::UDSEntry entry;

    KIO::UDSEntryList media_entries;

    createTopLevelEntry(entry);
    listEntry(entry, false);

    // to be used in stat() and rewriteURL()
    mtab = FMountPoint::currentMountPoints();
    FMountPoint::List::iterator it;
    for(it = mtab.begin(); it != mtab.end(); ++it) {
        entry.clear();

        KUrl fname((*it)->mountPoint());
        QString url = "isomedia:/" + fname.fileName();
        QString label = KIO::encodeFileName(fname.fileName());
        entry.insert( KIO::UDSEntry::UDS_URL, url );
        entry.insert( KIO::UDSEntry::UDS_NAME, label );
        entry.insert( KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR );
        entry.insert( KIO::UDSEntry::UDS_MIME_TYPE, "inode/x-iso-image-mounted" );
        entry.insert( KIO::UDSEntry::UDS_GUESSED_MIME_TYPE, "inode/directory" );
        entry.insert( KIO::UDSEntry::UDS_ICON_NAME, "application-x-cd-image" );
        entry.insert( KIO::UDSEntry::UDS_LOCAL_PATH, fname.path() );

        media_entries.append(entry);
    }

    createMountEntry(entry);
    listEntry(entry, false);

    totalSize(media_entries.count());

    QList<KIO::UDSEntry>::iterator it1 = media_entries.begin();//NOTE: port
    QList<KIO::UDSEntry>::iterator end = media_entries.end();//NOTE: port

    for(; it1 != end; ++it1) {
        listEntry(*it1, false);
    }

    entry.clear();
    listEntry(entry, true);

    finished();
}

void IsoMediaProtocol::slotWarning( const QString &msg )
{
    warning( msg );
}

#include "kio_isomedia.moc"
