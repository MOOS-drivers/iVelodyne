/************************************************************/
/*    NAME: Mohamed Saad Ibn Seddik                                              */
/*    ORGN: MOOS-Drivers                                    */
/*    FILE: Velodyne.cpp                                        */
/*    DATE: 2016/08/18                                      */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "Velodyne.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Velodyne::Velodyne():
  m_publish_raw(false)
{
  m_udpDataSocket = new UDPConnectionServer(2368);
  m_acqThread = new AcquisitionThread<DataPacket>(*m_udpDataSocket);
}

//---------------------------------------------------------
// Destructor

Velodyne::~Velodyne()
{
  m_acqThread->interrupt();
  delete m_acqThread;
  delete m_udpDataSocket;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Velodyne::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSInstrument::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key != "APPCAST_REQ") // handled by AppCastingMOOSInstrument
      reportRunWarning("Unhandled Mail: " + key);
  }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Velodyne::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Velodyne::Iterate()
{
  AppCastingMOOSInstrument::Iterate();

  if (m_publish_raw)
    if (!m_acqThread->getBuffer().isEmpty()) {
      Notify("IVELODYNE_RAW",static_cast<void *>(m_acqThread->getBuffer().dequeue().get()),
                    m_acqThread->getBuffer().getSize(),MOOSLocalTime());
    }

  AppCastingMOOSInstrument::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Velodyne::OnStartUp()
{
  AppCastingMOOSInstrument::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "PUBLISH_RAW") {
      if (toupper(value) == "TRUE")
        m_publish_raw=true;
      else
        m_publish_raw=false;
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  registerVariables();
  m_acqThread->start();
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void Velodyne::registerVariables()
{
  AppCastingMOOSInstrument::RegisterVariables();
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool Velodyne::buildReport()
{
  m_msgs << "iVelodyne \n";
  m_msgs << "============================================ \n";

  ACTable actab(2);
  actab << "Time | Buffer Size";
  actab.addHeaderLines();
  uint buffer_size = m_acqThread->getBuffer().getSize();
  actab << MOOSLocalTime() << buffer_size;
  m_msgs << actab.getFormattedString();

  return(true);
}
