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

#include "serialSendLib.h"
#include "Log.h"

#include <iostream>
using namespace std;

#include <sstream>

#include <qfile.h>
#include <q3cstring.h>


#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>

std::stringstream msg(std::ios::out);


void millisleep(int ms)
{
   if (ms>0)
   {
      struct timeval tv;
      tv.tv_sec=0;
      //tv.tv_usec=ms*1000;
      tv.tv_usec=ms;
      select(0, 0, 0, 0, &tv);
   }
}



/*************************** CONSTRUCTOR ***************************/
serialSend::serialSend()
{
    //manualDisconnectTTY();    

    //Initialize log
    log = new Log("/home/javigon/work/XBeeWSC");
    log->mesg(" Starting new connection...");

}


/*******************************************************************/
/*manualSendFile > We send a plain file over a serial tunnel that  */
/*that we have previously configured. PUBLIC
/*arg1 - filename:  Name of the file containing serial data        */
/*arg2 - charDelay: Amount of microseconds we wait between bytes   */
/*                                                                 */
/*This function contains some code taken from cutecom              */
/*******************************************************************/

void  serialSend::manualSendFile(QString filename, unsigned int charDelay)
{
    //QString filename="../../data_WQM2.txt";
    //unsigned int charDelay=200;
    //Logging Variables
    log->mesg(" Opening file to be sent...");
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        //QMessageBox::information(this, tr("Opening file failed"), tr("Could not open file %1").arg(filename));
        msg << "Opening file " << filename.toStdString() << " failed";
        log->mesg(msg.str());
        return;
    }
    QByteArray data=file.readAll();
    //delete m_progress;
    //m_progress=new QProgressDialog(tr("Sending file..."), tr("Cancel"), 0, 100, this);
    //m_progress->setMinimumDuration(100);

    unsigned int step=data.size()/100;
    if (step<1)
    {
        step=1;
    }
    
    msg << " Starting sending file. File size: " << data.size();
    log->mesg(msg.str());
    msg.str("");
    
    int i;
    for (i=0; i<data.size(); i++)
    {
       // if ((i%step)==0)
       // {
       //     m_progress->setValue(i/step);
       //     qApp->processEvents();
       // }

        sendByte(data.data()[i], charDelay);

        if ((data.data()[i]=='\n') || (data.data()[i]=='\r')) //wait twice as long after bytes which might be line ends (helps some uCs)
        {
            millisleep(charDelay);
        }
        //         if (!sendByte(data.data()[i]))
        if (0)
        {
            //QMessageBox::information(this, tr("Comm error"), tr("Sending failed (%1/%2").arg(i).arg(data.count()));
            break;
        }
    }
    msg << " File sent. Sent " << i << " out of "<< data.size() << " bytes.";
    log->mesg(msg.str());
    msg.str("");
    //delete m_progress;
    //m_progress=0;
}

/*******************************************************************/
/*manualConnectTTY > Function that allows us to connect a serial   */
/*device. We have access to all kind of parameters in order to     */
/*properly configure linux's UART. PUBLIC                          */
/*arg1 - dev:       Device we want to connect to                   */
/*arg2 - baudrate:  Baudrate at which we aim to connect            */
/*arg3 - dataBits:  Number of bits of data in each byte            */
/*arg4 - parity:    "none", "even" or "odd"                        */
/*arg5 - stop:      Number of stop bits. 1 or 2                    */
/*arg6 - softwareHandshake:   Software Handshake. "true" or "false"*/
/*arg7 - hardwareHandshake:   Hardware handshake. "true" or "false"*/
/*arg8 - openRead:  Open file to read. "true" or "false"           */
/*arg9 - openWrite  Open file to write. "true" or "false"          */
/*arg10 - applyNow  Apply configuration straight away. "true/fase" */
/*                                                                 */
/*This function contains some code taken from cutecom              */
/*******************************************************************/

void serialSend::manualConnectTTY(QString dev, int baudrate, int dataBits, QString parity, QString stop, bool softwareHandshake, bool hardwareHandshake, bool openRead, bool openWrite, bool applyNow)
{
/*  QString dev="/dev/ttyACM0";
    int baudrate=115200;
    int dataBits=8;
    QString parity="none";
    QString stop="1";
    bool softwareHandshake=false;
    bool hardwareHandshake=false;
    bool openRead=true;
    bool openWrite=true;
    bool applyNow=true;
*/
    
    msg << " Opening new connection: \n";
    msg << "    Device: " << dev.toStdString() << "\n";
    msg << "    Baudrate: " << baudrate << "\n";
    msg << "    Data Bits: " << dataBits << "\n";
    msg << "    Parity: " << parity.toStdString() << "\n"; 
    msg << "    Stop Bits: " << stop.toStdString() << "\n";
    if (softwareHandshake)
        msg << "    Software Handshake: True\n";
    else 
        msg << "    Software Handshake: False\n";
    
    if (hardwareHandshake)
        msg << "    Hardware Handshake: True\n";
    else
        msg << "    Hardware Handshake: False\n";
    
    if (openRead)
        msg << "    Open for Reading: True\n";
    else
        msg << "    Open for Reading: False\n";

    if (openWrite)
        msg << "    Open for Writing: True\n";
    else
        msg << "    Open for Writing: False\n";
    log->mesg(msg.str());
    msg.str("");
    
    int flags=0;
    if (openRead && openWrite)
    {
        flags=O_RDWR;
    }
    else if (!openRead && openWrite)
    {
        flags=O_WRONLY;
    }
    else if (openRead && !openWrite)
    {
        flags=O_RDONLY;
    }
    else
    {
        //QMessageBox::information(this, tr("Error"), tr("Opening the device neither for reading nor writing doesn't seem to make much sense ;-)"));
        return;
    }
    
    m_fd=open(dev.toLatin1(), flags | O_NDELAY);
    if (m_fd<0)
    {
        std::cout << " Opening device: " << dev.toStdString() << " failed" << std::endl;
        msg << " Opening device: " << dev.toStdString() << " failed";
        log->mesg(msg.str());
        msg.str("");
        m_fd=-1;
        //QMessageBox::information(this, tr("Error"), tr("Could not open %1").arg(dev));
        return;
    }
    
    // flushing is to be done after opening. This prevents first read and write to be spam'ish.
    tcflush(m_fd, TCIOFLUSH);
    
    if (applyNow)
    {
        int n = fcntl(m_fd, F_GETFL, 0);
        fcntl(m_fd, F_SETFL, n & ~O_NDELAY);
        
        if (tcgetattr(m_fd, &m_oldtio)!=0)
        {
            std::cerr<<"tcgetattr() 2 failed"<<std::endl;
            log->mesg("tcgetattr() 2 failed");

        }
        
        setNewOptions(baudrate, dataBits, parity, stop, softwareHandshake, hardwareHandshake);
    }
    
    m_previousChar = '\0';
    m_hexBytes=0;
}

/*******************************************************************/
/*manualDisconnetTTY > Finish a serial connection we had           */
/*established before. PUBLIC                                       */
/*                                                                 */
/*This function contains some code taken from cutecom              */
/*******************************************************************/

void serialSend::manualDisconnectTTY()
{
    disconnectTTYRestore(true);
    log->mesg(" Serial Connection. Disconnected \n####################################################### \n");


}

/*******************************************************************/
/*disconnectTTYRestore > We close the serial connection. PRIVATE   */
/*arg1 - restoreSettings: "true" or "false"                        */
/*                                                                 */
/*This function contains some code taken from cutecom              */
/*******************************************************************/

void serialSend::disconnectTTYRestore(bool restoreSettings)
{
    m_outputBuffer="";
    
    //   std::cerr<<"closing "<<m_fd<<std::endl;
    if (m_fd!=-1)
    {
        if (restoreSettings)
        {
            tcsetattr(m_fd, TCSANOW, &m_oldtio);
        }
        ::close(m_fd);
    }
    m_fd=-1;
        
    m_isConnected=false;
}


/*******************************************************************/
/*sendByte > We send one byte over linux's UART. PRIVATE           */
/*arg1 - c:     The character we want to send (char)               */
/*arg2 - delay: Delay we want to introduce between sent bytes      */
/*                                                                 */
/*This function contains some code taken from cutecom              */
/*******************************************************************/

bool serialSend::sendByte(char c, unsigned int delay)
{
    if (m_fd==-1)
    {
        return false;
    }
    int res=::write(m_fd, &c, 1);
    //   std::cerr<<"wrote "<<(unsigned int)(c)<<std::endl;
    if (res<1)
    {
        msg <<"write returned "<<res<<" errno: "<<errno;
        log->mesg(msg.str());
        msg.str("");

        std::cerr<<"write returned "<<res<<" errno: "<<errno<<std::endl;
        perror("write\n");
        return false;
    }
    millisleep(delay);
    return true;
}

/*******************************************************************/
/*setNewOptions > We configure linux's UART. PRIVATE               */
/*arg1 - baudrate:  Baudrate at which we aim to connect            */
/*arg2 - dataBits:  Number of bits of data in each byte            */
/*arg3 - parity:    "none", "even" or "odd"                        */
/*arg4 - stop:      Number of stop bits. 1 or 2                    */
/*arg5 - softwareHandshake:   Software Handshake. "true" or "false"*/
/*arg6 - hardwareHandshake:   Hardware handshake. "true" or "false"*/
/*                                                                 */
/*This function features some code from minicom 2.0.0,             */
/*src/sysdep1.c and from cutecom                                   */
/*******************************************************************/

void serialSend::setNewOptions(int baudrate, int databits, const QString& parity, const QString& stop, bool softwareHandshake, bool hardwareHandshake)
{
    struct termios newtio;
    //   memset(&newtio, 0, sizeof(newtio));
    if (tcgetattr(m_fd, &newtio)!=0)
    {
        std::cerr<<"tcgetattr() 3 failed"<<std::endl;
        log->mesg("tcgetattr() 3 failed");
    }
    
    speed_t _baud=0;
    switch (baudrate)
    {
#ifdef B0
        case      0: _baud=B0;     break;
#endif
            
#ifdef B50
        case     50: _baud=B50;    break;
#endif
#ifdef B75
        case     75: _baud=B75;    break;
#endif
#ifdef B110
        case    110: _baud=B110;   break;
#endif
#ifdef B134
        case    134: _baud=B134;   break;
#endif
#ifdef B150
        case    150: _baud=B150;   break;
#endif
#ifdef B200
        case    200: _baud=B200;   break;
#endif
#ifdef B300
        case    300: _baud=B300;   break;
#endif
#ifdef B600
        case    600: _baud=B600;   break;
#endif
#ifdef B1200
        case   1200: _baud=B1200;  break;
#endif
#ifdef B1800
        case   1800: _baud=B1800;  break;
#endif
#ifdef B2400
        case   2400: _baud=B2400;  break;
#endif
#ifdef B4800
        case   4800: _baud=B4800;  break;
#endif
#ifdef B7200
        case   7200: _baud=B7200;  break;
#endif
#ifdef B9600
        case   9600: _baud=B9600;  break;
#endif
#ifdef B14400
        case  14400: _baud=B14400; break;
#endif
#ifdef B19200
        case  19200: _baud=B19200; break;
#endif
#ifdef B28800
        case  28800: _baud=B28800; break;
#endif
#ifdef B38400
        case  38400: _baud=B38400; break;
#endif
#ifdef B57600
        case  57600: _baud=B57600; break;
#endif
#ifdef B76800
        case  76800: _baud=B76800; break;
#endif
#ifdef B115200
        case 115200: _baud=B115200; break;
#endif
#ifdef B128000
        case 128000: _baud=B128000; break;
#endif
#ifdef B230400
        case 230400: _baud=B230400; break;
#endif
#ifdef B460800
        case 460800: _baud=B460800; break;
#endif
#ifdef B576000
        case 576000: _baud=B576000; break;
#endif
#ifdef B921600
        case 921600: _baud=B921600; break;
#endif
        default:
            //   case 256000:
            //      _baud=B256000;
            break;
    }
    cfsetospeed(&newtio, (speed_t)_baud);
    cfsetispeed(&newtio, (speed_t)_baud);
    
    /* We generate mark and space parity ourself. */
    if (databits == 7 && (parity=="Mark" || parity == "Space"))
    {
        databits = 8;
    }
    switch (databits)
    {
        case 5:
            newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS5;
            break;
        case 6:
            newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS6;
            break;
        case 7:
            newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS7;
            break;
        case 8:
        default:
            newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;
            break;
    }
    newtio.c_cflag |= CLOCAL | CREAD;
    
    //parity
    newtio.c_cflag &= ~(PARENB | PARODD);
    if (parity == "Even")
    {
        newtio.c_cflag |= PARENB;
    }
    else if (parity== "Odd")
    {
        newtio.c_cflag |= (PARENB | PARODD);
    }
    
    //hardware handshake
    /*   if (hardwareHandshake)
     newtio.c_cflag |= CRTSCTS;
     else
     newtio.c_cflag &= ~CRTSCTS;*/
    newtio.c_cflag &= ~CRTSCTS;
    
    //stopbits
    if (stop=="2")
    {
        newtio.c_cflag |= CSTOPB;
    }
    else
    {
        newtio.c_cflag &= ~CSTOPB;
    }
    
    //   newtio.c_iflag=IGNPAR | IGNBRK;
    newtio.c_iflag=IGNBRK;
    //   newtio.c_iflag=IGNPAR;
    
    //software handshake
    if (softwareHandshake)
    {
        newtio.c_iflag |= IXON | IXOFF;
    }
    else
    {
        newtio.c_iflag &= ~(IXON|IXOFF|IXANY);
    }
    
    newtio.c_lflag=0;
    newtio.c_oflag=0;
    
    newtio.c_cc[VTIME]=1;
    newtio.c_cc[VMIN]=60;
    
    //   tcflush(m_fd, TCIFLUSH);
    if (tcsetattr(m_fd, TCSANOW, &newtio)!=0)
    {
        std::cerr<<"tcsetattr() 1 failed"<<std::endl;
        log->mesg("tcsetattr() 1 failed");
    }
    
    int mcs=0;
    ioctl(m_fd, TIOCMGET, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(m_fd, TIOCMSET, &mcs);
    
    if (tcgetattr(m_fd, &newtio)!=0)
    {
        std::cerr<<"tcgetattr() 4 failed"<<std::endl;
        log->mesg("tcgetattr() 4 failed");

    }
    
    //hardware handshake
    if (hardwareHandshake)
    {
        newtio.c_cflag |= CRTSCTS;
    }
    else
    {
        newtio.c_cflag &= ~CRTSCTS;
    }
    /*  if (on)
     newtio.c_cflag |= CRTSCTS;
     else
     newtio.c_cflag &= ~CRTSCTS;*/
    if (tcsetattr(m_fd, TCSANOW, &newtio)!=0)
    {
        std::cerr<<"tcsetattr() 2 failed"<<std::endl;
        log->mesg("tcsetattr() 2 failed");

    }
    
}

