/*  Copyright (C) 2011 Javier González <jgon@itu.dk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef QCPPIALOGIMPL_H
#define QCPPIALOGIMPL_H

#include <termios.h>

#include <qfile.h>
#include <q3cstring.h>
#include "Log.h"


#define CUTECOMM_BUFSIZE (4096)

class QListWidgetItem;
class QResizeEvent;
class Q3Process;
class QProgressDialog;
class QFileDialog;

class serialSend
{
   public:
      serialSend();
    void manualConnectTTY(QString dev, int baudrate, int dataBits, QString parity, QString stop, bool softwareHandshake, bool hardwareHandshake, bool openRead, bool openWrite, bool applyNow);      
      void manualDisconnectTTY();
      void manualSendFile(QString filename, unsigned int charDelay);            
   protected:
      bool sendByte(char c, unsigned int delay);
      void disconnectTTYRestore(bool restore);
      void setNewOptions(int baudrate, int databits, const QString& parity, const QString& stop, bool softwareHandshake, bool hardwareHandshake);

      bool m_isConnected;
      int m_fd;
      struct termios m_oldtio;
      unsigned int m_cmdBufIndex;
      char m_buf[CUTECOMM_BUFSIZE];

      QString m_outputBuffer;

      unsigned int m_hexBytes;
      char m_previousChar;
      Log * log;

};

#endif
