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

#include <QVBoxLayout>
#include <QLabel>

#include <KApplication>
#include <KLocale>
#include <KUrlRequester>
#include <kglobal.h>
#include <KMessageBox>
#include <KLineEdit>
#include <KRun>
#include <KCmdLineArgs>
// #include <dcopclient.h>//NOTE: port it to dbus
// #include <kiconloader.h>//NOTE: port

#include "fuseisolib.h"

#include "kfuseisomount.h"

// heavily based on KUrlRequesterDlg
// why the hell everything in kde classes declared private!?

FuseisoMountDlg::FuseisoMountDlg( QWidget* parent ) :
        KDialog(parent)
{
    QWidget* mainwidget = new QWidget( this );
    QVBoxLayout * topLayout = new QVBoxLayout( mainwidget );
    setMainWidget( mainwidget );

    QLabel * label = new QLabel( i18n( "ISO image filename" ) );
    topLayout->addWidget( label );

    urlRequester_ = new KUrlRequester( KUrl()/*, plainPage(), "urlRequester"*/ );//NOTE: port
    urlRequester_->setMinimumWidth( urlRequester_->sizeHint().width() * 3 );
    urlRequester_->setMode( KFile::File | KFile::ExistingOnly | KFile::LocalOnly );
    urlRequester_->setFilter( "*.iso *.ISO *.bin *.BIN *.nrg *.NRG *.img *.IMG *.mdf *.MDF|" + i18n( "All supported files" ) + "\n*|" + i18n( "All files" ) );
    topLayout->addWidget( urlRequester_ );

    urlRequester_->setFocus();
    openBrowser = new QCheckBox( i18n( "Open mounted image in new window" ) );
    openBrowser->setChecked( true );
    topLayout->addWidget( openBrowser );

    setCaption( i18n( "Mount ISO image" ) );
    setWindowIcon( KIcon( "application-x-cd-image" ) );
    enableButtonOk( false );

    connect( urlRequester_->lineEdit(), SIGNAL(textChanged(const QString&)),
             SLOT(slotTextChanged(const QString&)) );
}

void FuseisoMountDlg::slotTextChanged(const QString & text)
{
    enableButtonOk( !text.trimmed().isEmpty() );
}

KUrl FuseisoMountDlg::selectedURL() const
{
    if ( result() == QDialog::Accepted )
        return KUrl( urlRequester_->url() );
    else
        return KUrl();
}

bool FuseisoMountDlg::needOpenBrowser() const
{
    return openBrowser->isChecked();
}

int main( int argc, char **argv )
{
    KCmdLineArgs::init(argc, argv, "kfuseisomount",
        /*PACKAGE*/"", ki18n("Utility to mount ISO image"),
        /*VERSION*/"");//NOTE: port

    KCmdLineArgs::addStdCmdLineOptions();//NOTE: port
    KApplication app;
//     app.dcopClient()->attach();//NOTE: port it to dbus

//     KConfig config("kfuseisomount", KConfig::SimpleConfig);//NOTE: port

//     KFileDialog::setStartDir(/*KUrl("::x-iso-image")*/);//NOTE: port
    FuseisoMountDlg dlg;

    dlg.exec();
    KUrl url = dlg.selectedURL();

    if(url.isValid() && url.isLocalFile()) {
        KUrl mount_point;
        FuseisoLib flib;
        QString errstr;
        if(flib.isMounted(url, mount_point)) {
            if (dlg.needOpenBrowser()) {
                KRun::runUrl(mount_point, "inode/directory", 0 );//NOTE: port
            }
        }
        else {
            if(flib.mount(url, mount_point, errstr)) {
                if(dlg.needOpenBrowser()) {
                    KRun::runUrl(mount_point, "inode/directory", 0 );//NOTE: port
                }
            }
            else {
                KMessageBox::error(0,
                    QString(i18n("Error while mounting image:\n%1")).arg(errstr));
            }
        }
    }

//     config.writeEntry("openBrowser", obb->isChecked());//NOTE: port it
//     config.sync();

    return 0;
}

#include "kfuseisomount.moc"
