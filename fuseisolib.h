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

// this file was borrowed from linux kernel
// originally it has name /usr/src/linux/include/linux/iso_fs.h
// unfortunately there was no copyright header on it,
// so i`m assume it have copyrighted under terms of
// GNU GENERAL PUBLIC LICENSE version 2
// as a whole linux kernel
// copy of this license included into fuseiso
// distribution in file COPYING


#ifndef _ISOFS_FS_H
#define _ISOFS_FS_H

/*
 * The isofs filesystem constants/structures
 */

/* This part borrowed from the bsd386 isofs */
#define ISODCL(from, to) (to - from + 1)

struct iso_volume_descriptor {
    char type[ISODCL(1,1)]; /* 711 */
    char id[ISODCL(2,6)];
    char version[ISODCL(7,7)];
    char data[ISODCL(8,2048)];
};

/* volume descriptor types */
#define ISO_VD_PRIMARY 1
#define ISO_VD_SUPPLEMENTARY 2
#define ISO_VD_END 255

#define ISO_STANDARD_ID "CD001"

struct iso_primary_descriptor {
    char type           [ISODCL (  1,   1)]; /* 711 */
    char id             [ISODCL (  2,   6)];
    char version            [ISODCL (  7,   7)]; /* 711 */
    char unused1            [ISODCL (  8,   8)];
    char system_id          [ISODCL (  9,  40)]; /* achars */
    char volume_id          [ISODCL ( 41,  72)]; /* dchars */
    char unused2            [ISODCL ( 73,  80)];
    char volume_space_size      [ISODCL ( 81,  88)]; /* 733 */
    char unused3            [ISODCL ( 89, 120)];
    char volume_set_size        [ISODCL (121, 124)]; /* 723 */
    char volume_sequence_number [ISODCL (125, 128)]; /* 723 */
    char logical_block_size     [ISODCL (129, 132)]; /* 723 */
    char path_table_size        [ISODCL (133, 140)]; /* 733 */
    char type_l_path_table      [ISODCL (141, 144)]; /* 731 */
    char opt_type_l_path_table  [ISODCL (145, 148)]; /* 731 */
    char type_m_path_table      [ISODCL (149, 152)]; /* 732 */
    char opt_type_m_path_table  [ISODCL (153, 156)]; /* 732 */
    char root_directory_record  [ISODCL (157, 190)]; /* 9.1 */
    char volume_set_id      [ISODCL (191, 318)]; /* dchars */
    char publisher_id       [ISODCL (319, 446)]; /* achars */
    char preparer_id        [ISODCL (447, 574)]; /* achars */
    char application_id     [ISODCL (575, 702)]; /* achars */
    char copyright_file_id      [ISODCL (703, 739)]; /* 7.5 dchars */
    char abstract_file_id       [ISODCL (740, 776)]; /* 7.5 dchars */
    char bibliographic_file_id  [ISODCL (777, 813)]; /* 7.5 dchars */
    char creation_date      [ISODCL (814, 830)]; /* 8.4.26.1 */
    char modification_date      [ISODCL (831, 847)]; /* 8.4.26.1 */
    char expiration_date        [ISODCL (848, 864)]; /* 8.4.26.1 */
    char effective_date     [ISODCL (865, 881)]; /* 8.4.26.1 */
    char file_structure_version [ISODCL (882, 882)]; /* 711 */
    char unused4            [ISODCL (883, 883)];
    char application_data       [ISODCL (884, 1395)];
    char unused5            [ISODCL (1396, 2048)];
};

/* Almost the same as the primary descriptor but two fields are specified */
struct iso_supplementary_descriptor {
    char type           [ISODCL (  1,   1)]; /* 711 */
    char id             [ISODCL (  2,   6)];
    char version            [ISODCL (  7,   7)]; /* 711 */
    char flags          [ISODCL (  8,   8)]; /* 853 */
    char system_id          [ISODCL (  9,  40)]; /* achars */
    char volume_id          [ISODCL ( 41,  72)]; /* dchars */
    char unused2            [ISODCL ( 73,  80)];
    char volume_space_size      [ISODCL ( 81,  88)]; /* 733 */
    char escape         [ISODCL ( 89, 120)]; /* 856 */
    char volume_set_size        [ISODCL (121, 124)]; /* 723 */
    char volume_sequence_number [ISODCL (125, 128)]; /* 723 */
    char logical_block_size     [ISODCL (129, 132)]; /* 723 */
    char path_table_size        [ISODCL (133, 140)]; /* 733 */
    char type_l_path_table      [ISODCL (141, 144)]; /* 731 */
    char opt_type_l_path_table  [ISODCL (145, 148)]; /* 731 */
    char type_m_path_table      [ISODCL (149, 152)]; /* 732 */
    char opt_type_m_path_table  [ISODCL (153, 156)]; /* 732 */
    char root_directory_record  [ISODCL (157, 190)]; /* 9.1 */
    char volume_set_id      [ISODCL (191, 318)]; /* dchars */
    char publisher_id       [ISODCL (319, 446)]; /* achars */
    char preparer_id        [ISODCL (447, 574)]; /* achars */
    char application_id     [ISODCL (575, 702)]; /* achars */
    char copyright_file_id      [ISODCL (703, 739)]; /* 7.5 dchars */
    char abstract_file_id       [ISODCL (740, 776)]; /* 7.5 dchars */
    char bibliographic_file_id  [ISODCL (777, 813)]; /* 7.5 dchars */
    char creation_date      [ISODCL (814, 830)]; /* 8.4.26.1 */
    char modification_date      [ISODCL (831, 847)]; /* 8.4.26.1 */
    char expiration_date        [ISODCL (848, 864)]; /* 8.4.26.1 */
    char effective_date     [ISODCL (865, 881)]; /* 8.4.26.1 */
    char file_structure_version [ISODCL (882, 882)]; /* 711 */
    char unused4            [ISODCL (883, 883)];
    char application_data       [ISODCL (884, 1395)];
    char unused5            [ISODCL (1396, 2048)];
};


#define HS_STANDARD_ID "CDROM"

struct  hs_volume_descriptor {
    char foo            [ISODCL (  1,   8)]; /* 733 */
    char type           [ISODCL (  9,   9)]; /* 711 */
    char id             [ISODCL ( 10,  14)];
    char version            [ISODCL ( 15,  15)]; /* 711 */
    char data[ISODCL(16,2048)];
};


struct hs_primary_descriptor {
    char foo            [ISODCL (  1,   8)]; /* 733 */
    char type           [ISODCL (  9,   9)]; /* 711 */
    char id             [ISODCL ( 10,  14)];
    char version            [ISODCL ( 15,  15)]; /* 711 */
    char unused1            [ISODCL ( 16,  16)]; /* 711 */
    char system_id          [ISODCL ( 17,  48)]; /* achars */
    char volume_id          [ISODCL ( 49,  80)]; /* dchars */
    char unused2            [ISODCL ( 81,  88)]; /* 733 */
    char volume_space_size      [ISODCL ( 89,  96)]; /* 733 */
    char unused3            [ISODCL ( 97, 128)]; /* 733 */
    char volume_set_size        [ISODCL (129, 132)]; /* 723 */
    char volume_sequence_number [ISODCL (133, 136)]; /* 723 */
    char logical_block_size     [ISODCL (137, 140)]; /* 723 */
    char path_table_size        [ISODCL (141, 148)]; /* 733 */
    char type_l_path_table      [ISODCL (149, 152)]; /* 731 */
    char unused4            [ISODCL (153, 180)]; /* 733 */
    char root_directory_record  [ISODCL (181, 214)]; /* 9.1 */
};

#endif // _ISOFS_FS_H

// borrowed from fuseiso
#ifndef _ISOFS_H
#define _ISOFS_H

// borrowed from linux kernel isofs code

/* Number conversion inlines, named after the section in ISO 9660
   they correspond to. */

#include <byteswap.h>

static inline int isonum_711(unsigned char *p)
{
    return *(unsigned char *)p;
}
static inline unsigned int isonum_721(char *p)
{
#if defined(WORDS_BIGENDIAN)
    return *(unsigned short *)p;
#else
    return bswap_16(*(unsigned short *)p);
#endif
}
static inline unsigned int isonum_723(char *p)
{
    /* Ignore bigendian datum due to broken mastering programs */
#if defined(WORDS_BIGENDIAN)
    return bswap_16(*(unsigned short *)p);
#else
    return *(unsigned short *)p;
#endif
}
static inline unsigned int isonum_731(char *p)
{
#if defined(WORDS_BIGENDIAN)
    return bswap_32(*(unsigned int *)p);
#else
    return *(unsigned int *)p;
#endif
}
static inline unsigned int isonum_733(char *p)
{
    /* Ignore bigendian datum due to broken mastering programs */
#if defined(WORDS_BIGENDIAN)
    return bswap_32(*(unsigned int *)p);
#else
    return *(unsigned int *)p;
#endif
}

#endif // _ISOFS_H

#include <QObject>
#include <kmountpoint.h>
#include <KUrl>

class FMountPoint : public KShared {

    public:

        typedef KSharedPtr<FMountPoint> Ptr;
        typedef QList<Ptr> List;

        FMountPoint() {}

        static FMountPoint::List currentMountPoints();
        static FMountPoint fromUrl(KUrl url);

        QString mountedFrom() const { return m_mountedFrom; }
        QString mountPoint() const { return m_mountPoint; }
        QString mountType() const { return m_mountType; }

    protected:

        QString m_mountedFrom;
        QString m_mountPoint;
        QString m_mountType;

};

class K3Process;

class FuseisoLib : public QObject {
    Q_OBJECT

    public:

        FuseisoLib(KUrl media_dir_ = KUrl());

        bool mount(KUrl url, KUrl& mount_point, QString& errstr);
        bool umount(KUrl url, QString& errstr);
        bool userInstall();
        KUrl suggestMountPoint(KUrl url);
        bool isMounted(KUrl url, KUrl& mount_point);
        bool isReallyMounted(KUrl mount_point, bool cleanup);
        KUrl findMountPoint(KUrl url);

    protected:

        KUrl media_dir;
        FMountPoint::List mtab;

        QString output;

    protected slots:

        void onReceivedStdout(K3Process *proc, char *buf, int len);

};
