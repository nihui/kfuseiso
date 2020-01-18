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

#include "fuseisolib.h"

#include <linux/limits.h>
#include <stdio.h>
#include <fcntl.h>
#include <mntent.h>
#include <errno.h>

#include <QFile>
#include <QDir>
#include <QFileInfo>

#include <K3Process>//NOTE: port me when neccessary

#include <KUser>
#include <KUrl>
#include <KMessageBox>
#include <KLocale>
#include <KStandardDirs>
#include <kio/job.h>
#include <kio/jobclasses.h>

static const QString mtab_file = ".mtab.fuseiso";

FMountPoint::List FMountPoint::currentMountPoints() {
    FMountPoint::List result;

    int rc;

    KUser user;
    QString mtab_path = user.homeDir();
    mtab_path += "/";
    mtab_path += mtab_file;

    int fd = open(mtab_path.toAscii(), O_RDWR | O_CREAT, 0644);
    if(fd < 0) {
        fprintf(stderr, "Can`t open mtab file %s: %s\n", qPrintable(mtab_path), strerror(errno));//NOTE: port
        return result;
    }
    rc = lockf(fd, F_LOCK, 0);
    if(rc != 0) {
        perror("Can`t lock mtab");
        return result;
    }
    FILE* mtab = setmntent(mtab_path.toAscii(), "r");
    if(!mtab) {
        perror("Can`t open mtab");
        return result;
    }

    struct mntent* ent;
    while((ent = getmntent(mtab)) != NULL) {
        FMountPoint *mp = new FMountPoint();

        mp->m_mountedFrom = QFile::decodeName(ent->mnt_fsname);
        mp->m_mountPoint = QFile::decodeName(ent->mnt_dir);
        mp->m_mountType = QFile::decodeName(ent->mnt_type);

        result.append( KSharedPtr<FMountPoint>(mp) );//NOTE: port
    }

    endmntent(mtab);
    rc = lockf(fd, F_ULOCK, 0);
    if(rc != 0) {
        perror("Can`t unlock mtab");
        return result;
    }
    close(fd);

    return result;
}

FMountPoint FMountPoint::fromUrl(KUrl url) {
    FMountPoint mp;

    FMountPoint::List mtab = FMountPoint::currentMountPoints();
    FMountPoint::List::iterator it;

    if(url.protocol() == "isomedia") {

        for(it = mtab.begin(); it != mtab.end(); ++it) {
            KUrl to((*it)->mountPoint());
            QString fname = "/" + to.fileName();
            if(url.path().startsWith(fname)) {
//                 mp = **it;//NOTE: port
//                 break;
                return **it;//NOTE: port
            }
        }

    }
    else if(url.isLocalFile()) {
        QFileInfo fi(url.path());

        while(fi.exists() && fi.isReadable() && fi.isSymLink()) {
            QString fn = fi.readLink();
            if(fn==QString()) {//NOTE: port
                return mp; // empty
            }
            fi = QFileInfo(fn);
        }

        if(!fi.exists() || !fi.isReadable()) {
            return mp;
        }

        if(fi.isDir()) {

            for(it = mtab.begin(); it != mtab.end(); ++it) {
                if(url.path() == (*it)->mountPoint()) {
//                     mp = **it;//NOTE: port
//                     break;
                    return **it;//NOTE: port
                }
            }

        }
        else {
            for(it = mtab.begin(); it != mtab.end(); ++it) {
                if(url.path() == (*it)->mountedFrom()) {
//                     mp = **it;//NOTE: port
//                     break;
                    return **it;
                }
            }
        }
    }

    return mp;
}

FuseisoLib::FuseisoLib(KUrl media_dir_) {
    if(media_dir_.isEmpty()) {
        KUser user;
        media_dir = user.homeDir();
        media_dir.addPath("media");
    }
    else {
        media_dir = media_dir_;
    }
}

bool FuseisoLib::mount(KUrl url, KUrl& mount_point, QString& errstr) {
    if(!userInstall()) {
        return false;
    }

    if(mount_point.isEmpty()) {
        mount_point = FuseisoLib::suggestMountPoint(url);
    }

    K3Process proc;

    proc << "fuseiso";
    proc << "-p" << url.path() << mount_point.path();

    connect( &proc, SIGNAL( receivedStdout( K3Process *, char *, int ) ),
        this, SLOT( onReceivedStdout( K3Process *, char *, int ) ) );
    connect( &proc, SIGNAL( receivedStderr( K3Process *, char *, int ) ),
        this, SLOT( onReceivedStdout( K3Process *, char *, int ) ) );
    output = "";

    // block konqueror while fuseiso is not come to daemon state
    // may be this is not an excellent idea, but helps open
    // mount point when it is really mounted
    proc.start(K3Process::Block, K3Process::AllOutput);

    if(proc.normalExit() && proc.exitStatus() == 0) {

//         KDirNotify_stub notifier("*", "*");
//         notifier.FilesAdded(KUrl("isomedia:/"));
//         notifier.FilesAdded(KUrl("system:/isomedia/"));//TODO: port it to dbus
        return true;
    }
    else {
        errstr = output;
        return false;
    }
}

bool FuseisoLib::umount(KUrl url, QString &errstr) {

    K3Process proc;

    proc << "fusermount";
    proc << url.path() << "-u";

    connect( &proc, SIGNAL( receivedStdout( K3Process *, char *, int ) ),
        this, SLOT( onReceivedStdout( K3Process *, char *, int ) ) );
    connect( &proc, SIGNAL( receivedStderr( K3Process *, char *, int ) ),
        this, SLOT( onReceivedStdout( K3Process *, char *, int ) ) );
    output = "";

    // block konqueror while fuseiso is not come to daemon state
    // may be this is not an excellent idea, but helps open
    // mount point when it is really mounted
    proc.start(K3Process::Block, K3Process::AllOutput);

    if(proc.normalExit() && proc.exitStatus() == 0) {

//         KDirNotify_stub notifier("*", "*");
//         notifier.FilesRemoved(KUrl("isomedia:/" + url.fileName()));
//         notifier.FilesRemoved(KUrl("system:/isomedia/" + url.fileName()));//TODO: port it to dbus

        return true;
    }
    else {
        errstr = output;
        return false;
    }
}

bool FuseisoLib::userInstall() {

    QDir media(media_dir.path());
    if(!media.exists()) {
        if(media.mkdir(media_dir.path()/*, true*/)) {//NOTE: port
            KStandardDirs sdirs;
            KIO::file_copy(sdirs.findResource("data", "kfuseiso/media.directory"),
                KUrl(media_dir.path() + "/.directory"), -1/*, false, false, false*/);//NOTE: port
            KMessageBox::information(0,
                QString(i18n("Directory %1 created to hold image mount points")).arg(media_dir.path()));

            return true;
        }
        else {
            KMessageBox::error(0,
                QString(i18n("Can`t create directory %1. Image file will not be mounted")).arg(media_dir.path()));
            return false;
        }
    }

    return true;
}

KUrl FuseisoLib::suggestMountPoint(KUrl url) {

    KUrl mount_point(media_dir);

    QDir media(media_dir.path());
    int i = 0;
    while(1) {
        QString mount_dir = url.fileName();
        if(i) {
            mount_dir += QString("[%1]").arg(i);
        }
        if(!media.exists(mount_dir)) {
            mount_point.addPath(mount_dir);
            break;
        }
        else if(i > 100) {
            // something goes wrong
            mount_point.addPath("NULL");
            return mount_point;
        }
        i++;
    }

    return mount_point;
}

bool FuseisoLib::isMounted(KUrl url, KUrl& mount_point) {

    bool mounted = false;

    mtab = FMountPoint::currentMountPoints();
    for(FMountPoint::List::iterator mp = mtab.begin(); mp != mtab.end(); ++mp) {
        QString to = (*mp)->mountPoint();
        QString what = (*mp)->mountedFrom();
        if(urlcmp(url.path(), what/*, true, true*/)) {//NOTE: port
            mount_point = KUrl(to);
            mounted = isReallyMounted(mount_point, true);
            break;
        }
    }

    return mounted;
}

KUrl FuseisoLib::findMountPoint(KUrl url) {

    KUrl mount_point;

    mtab = FMountPoint::currentMountPoints();
    for(FMountPoint::List::iterator mp = mtab.begin(); mp != mtab.end(); ++mp) {
        KUrl to((*mp)->mountPoint());
        if(to.fileName() == url.fileName()) {
            mount_point = to;
            break;
        }
    }

    return mount_point;
}

bool FuseisoLib::isReallyMounted(KUrl mount_point, bool cleanup) {
    bool mounted = false;

    KMountPoint::List mtab = KMountPoint::currentMountPoints();
    for(KMountPoint::List::iterator mp = mtab.begin(); mp != mtab.end(); ++mp) {
        QString to = (*mp)->mountPoint();
        QString what = (*mp)->mountedFrom();
        if(urlcmp(mount_point.path(), to/*, true, true*/) && what == "fuseiso") {//NOTE:port
            mounted = true;
            break;
        }
    }

    if(mounted || !cleanup) {
        return mounted;
    }
    else if(!mounted && cleanup) {
        KUser user;
        QString mtab_path = user.homeDir();
        mtab_path += "/";
        mtab_path += mtab_file;

        int rc;

        char new_mtab_path[PATH_MAX];
        int fd = open(mtab_path.toAscii(), O_RDWR | O_CREAT, 0644);
        if(fd < 0) {
            perror("Can`t open mtab");
            return false;
        }
        rc = lockf(fd, F_LOCK, 0);
        if(rc != 0) {
            perror("Can`t lock mtab");
            return false;
        }
        strncpy(new_mtab_path, mtab_path.toAscii(), PATH_MAX - 16);
        new_mtab_path[PATH_MAX - 1] = 0;
        strcat(new_mtab_path, ".new");
        FILE* mtab = setmntent(qPrintable(mtab_path), "r");//NOTE: port
        if(!mtab) {
            perror("Can`t open mtab");
            return false;
        }
        FILE* new_mtab = setmntent(new_mtab_path, "a+");
        if(!new_mtab) {
            perror("Can`t open new mtab");
            return false;
        }
        struct mntent* ent;
        while((ent = getmntent(mtab)) != NULL) {
            if(strcmp(ent->mnt_dir, mount_point.path().toAscii()) == 0 &&
                strcmp(ent->mnt_type, "fuseiso") == 0) {
                // skip
            }
            else {
                rc = addmntent(new_mtab, ent);
                if(rc != 0) {
                    perror("Can`t add mtab entry");
                    return false;
                }
            }
        }
        endmntent(mtab);
        endmntent(new_mtab);
        rc = rename(new_mtab_path, mtab_path.toAscii());
        if(rc != 0) {
            perror("Can`t rewrite mtab");
            return false;
        }
        rc = lockf(fd, F_ULOCK, 0);
        if(rc != 0) {
            perror("Can`t unlock mtab");
            return false;
        }
        close(fd);

        rc = rmdir(mount_point.path().toAscii());
        if(rc != 0) {
            perror("Can`t delete mount point");
        }

        return false;

    }
    else {

        return mounted;

    }
}

void FuseisoLib::onReceivedStdout(K3Process *proc, char *buf, int len) {
    Q_UNUSED( proc );
    output += QString::fromLocal8Bit( buf, len );
}

#include "fuseisolib.moc"
