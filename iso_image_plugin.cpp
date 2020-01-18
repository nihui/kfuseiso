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

// heavily based on sources of ark_plugin from kdeaddons package

#include <stdio.h>
#include <fcntl.h>

#include <QDir>

#include <KStandardDirs>
#include <KAction>
#include <KMenu>
#include <KGenericFactory>
#include <KMessageBox>
#include <kmountpoint.h>
// #include <kmdcodec.h>//NOTE: port it to dbus
#include <KRun>
#include <konq_popupmenu.h>
#include <konq_popupmenuinformation.h>

#include "fuseisolib.h"

#include "iso_image_plugin.h"

typedef KGenericFactory<IsoImageMenu, KonqPopupMenu> IsoImageMenuFactory;
K_EXPORT_COMPONENT_FACTORY( libiso_image_plugin, IsoImageMenuFactory("iso_image_plugin") )

IsoImageMenu::IsoImageMenu( KonqPopupMenu* popupmenu, const QStringList& /* list */ )
                : KonqPopupMenuPlugin(popupmenu), flib()
{
    KGlobal::locale()->insertCatalog( "iso_image_plugin" );
    m_popupmenu = popupmenu;
    m_parentwidget = popupmenu->parentWidget();
}

IsoImageMenu::~IsoImageMenu()
{
}

void IsoImageMenu::setup( KActionCollection* actionCollection, const KonqPopupMenuInformation& popupMenuInfo, QMenu* menu )
{
    if ( KStandardDirs::findExe( "fuseiso" ).isNull()
        || KStandardDirs::findExe( "fusermount" ).isNull() ) {
        return;
    }

    KFileItemList itemList = popupMenuInfo.items();
    if ( itemList.count() != 1 ) {
        return;
    }

    KUrl file_url = itemList.first().url();

    kdDebug() << " there at all with " << file_url.path() << endl;

    if ( itemList.first().mimetype() == "inode/x-iso-image-mounted" ) {
        KUrl mount_point_url = flib.findMountPoint( file_url );
        if ( mount_point_url.isEmpty() ) {
            return;
        }

        mount_point = mount_point_url.path();

        KAction* action = new KAction( i18n( "Umount image" ), this );
        action->setIcon( KIcon( "application-x-cd-image" ) );
        actionCollection->addAction( "iso_image_umount_menu", action );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotUmount() ) );
        menu->addSeparator();
        menu->addAction( action );
        menu->addSeparator();
    }
    else if ( file_url.isLocalFile() ) { // local file to mount/umount/browse
        image_file = file_url.path();

        if ( image_file.isEmpty() ) {
            return;
        }

        QString label;
        if ( !checkImage( image_file, label ) ) {
            return;
        }

        KUrl mount_point_url;
        bool mounted = flib.isMounted( file_url, mount_point_url );
        if ( mounted ) {
            mount_point = mount_point_url.path();
        }
        else {
            mount_point_url = flib.suggestMountPoint( file_url );
            mount_point = mount_point_url.path();
        }

        if ( mounted ) {
            KAction* action = new KAction( i18n( "Browse..." ), this );
            action->setIcon( KIcon( "application-x-cd-image" ) );
            actionCollection->addAction( "iso_image_browse_menu", action );
            connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotBrowse() ) );
            menu->addSeparator();
            menu->addAction( action );
            KAction* action2 = new KAction( i18n( "Umount image" ), this );
            action2->setIcon( KIcon( "application-x-cd-image" ) );
            actionCollection->addAction( "iso_image_umount_menu", action2 );
            connect( action2, SIGNAL( triggered( bool ) ), this, SLOT( slotUmount() ) );
            menu->addAction( action2 );
            menu->addSeparator();
        }
        else {
            KAction* action = new KAction( i18n( "Mount image" ), this );
            action->setIcon( KIcon( "application-x-cd-image" ) );
            actionCollection->addAction( "iso_image_mount_menu", action );
            connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotMount() ) );
            menu->addSeparator();
            menu->addAction( action );
            KAction* action2 = new KAction( i18n( "Mount image and browse" ), this );
            action2->setIcon( KIcon( "application-x-cd-image" ) );
            actionCollection->addAction( "iso_image_mount_browse_menu", action2 );
            connect( action2, SIGNAL( triggered( bool ) ), this, SLOT( slotMountAndBrowse() ) );
            menu->addAction( action2 );
            menu->addSeparator();
        }
    }
}

void IsoImageMenu::slotMount() {
    KUrl mount_point_url(mount_point);
    QString errstr;
    if(!flib.mount(KUrl(image_file), mount_point_url, errstr)) {
        KMessageBox::error(0,
            QString(i18n("Error while mounting image:\n%1")).arg(errstr));
    }
}

void IsoImageMenu::slotMountAndBrowse() {
    KUrl mount_point_url(mount_point);
    QString errstr;
    if(flib.mount(KUrl(image_file), mount_point_url, errstr)) {
        slotBrowse();
    }
    else {
        KMessageBox::error(0,
            QString(i18n("Error while mounting image:\n%1")).arg(errstr));
    }
}

void IsoImageMenu::slotUmount() {
    QString errstr;
    if(!flib.umount(KUrl(mount_point), errstr)) {
        KMessageBox::error(0,
            QString(i18n("Error while unmounting image:\n%1")).arg(errstr));
    }
}

void IsoImageMenu::slotBrowse() {
    KUrl mp_url( mount_point );
    //new KRun( KUrl( mp_url ), 0, 0, true );//TODO: memory leak here?
    KRun::runUrl( mp_url, "inode/directory", 0 );
}

/// @todo move to fuseisolib, make use of kde file access functions
bool IsoImageMenu::checkImage(const QString fname, QString& label) {

    int vd_num = 0;
    int fd;
    struct iso_volume_descriptor vd;

    memset(&vd, 0, sizeof(struct iso_volume_descriptor));

    // defaults for iso
    int block_size = 2048;
    int block_offset = 0;
    int file_offset = 0;

    fd = open(fname.toAscii(), O_RDONLY | O_LARGEFILE);
    if(fd < 0) {
        return false;
    }

    enum {
        IDOFF_ISO_2048 = 2048 * 16,
        IDOFF_MODE1_2352 = 2352 * 16 + 16,
        IDOFF_MODE2_2352_RAW = 2352 * 16,
        IDOFF_MODE2_2352 = 2352 * 16 + 24,
        IDOFF_MODE2_2336 = 2336 * 16 + 16,
        IDOFF_NRG = 2048 * 16 + 307200,
    };
    int iso_offsets[] = {IDOFF_ISO_2048, IDOFF_MODE2_2336, IDOFF_MODE2_2352_RAW, IDOFF_NRG};
    // try to find CD001 identifier
    int i;
    for(i = 0; i < 4; i++) {
        if(lseek(fd, iso_offsets[i], SEEK_SET) == -1) {
            kdWarning() << "can`t lseek() to next possible data start position; is it really supported file?";
            goto out;
        }
        ssize_t size = read(fd, &vd, sizeof(struct iso_volume_descriptor));
        if(size != sizeof(struct iso_volume_descriptor)) {
            kdWarning() << "only " << size << " bytes read from position " << iso_offsets[i] <<
                ", " << sizeof(struct iso_volume_descriptor) << " required; is it really supported file?";
            goto out;
        }
        if(strncmp("CD001", vd.id, 5) == 0) {
            // found CD001!
            // fill context with information about block size and block offsets
            switch(iso_offsets[i]) {
                case IDOFF_ISO_2048:
                    // normal iso file
                    // use defaults
                    break;
                case IDOFF_MODE2_2352_RAW:
                    block_size = 2352;
                    break;
                case IDOFF_MODE2_2336:
                    block_size = 2336;
                    block_offset = 16;
                    break;
                case IDOFF_NRG:
                    file_offset = 307200;
                    break;
                default:
                    break;
            }
            break;
        }
        else if(strncmp("CD001", vd.id + 16, 5) == 0) {
            block_size = 2352;
            block_offset = 16;
            break;
        }
        else if(strncmp("CD001", vd.id + 24, 5) == 0) {
            block_size = 2352;
            block_offset = 24;
            break;
        }
    }

    while(1) {
        if(lseek(fd, block_size * (16 + vd_num) + block_offset + file_offset, SEEK_SET) == -1) {
            kdWarning() << "can`t lseek() to next volume descriptor";
            goto out;
        }
        ssize_t size = read(fd, &vd, sizeof(struct iso_volume_descriptor));
        if(size != sizeof(struct iso_volume_descriptor)) {
            kdWarning() << "only " << size << " bytes read from volume descriptor " << vd_num <<
                ", " << sizeof(struct iso_volume_descriptor) << " required";
            goto out;
        }

        int vd_type = isonum_711((unsigned char *)vd.type);

        if(strncmp("CD001", vd.id, 5) != 0) {
            if(vd_num > 16) {
                // no more trying
                kdWarning() << "wrong standard identifier in volume descriptor " << vd_num << ", exiting..";
                goto out;
            }
            else {
                // try to continue
                kdWarning() << "wrong standard identifier in volume descriptor " << vd_num << ", skipping..";
            }
        }
        else {
            struct iso_primary_descriptor* pd = (struct iso_primary_descriptor*) &vd;
            switch(vd_type) {
                case ISO_VD_PRIMARY:
                    // that is all we need

                    label = QString::fromAscii(pd->volume_id, 32).trimmed();

                    close(fd);
                    return true;

                    break;

                case ISO_VD_SUPPLEMENTARY:
                    // not interesting
                    break;

                case 0:
                    // boot record, not intresting..
                    break;

                case ISO_VD_END:
                    goto out;
                    break;

                default:
                    kdWarning() << "unsupported volume descriptor type " << vd_type << ", vd_num " << vd_num;
                    break;
            }
        }

        vd_num += 1;
    }

out:

    close(fd);

    return false;
}

#include "iso_image_plugin.moc"
