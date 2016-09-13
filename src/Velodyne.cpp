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
  setMaxDistance(VEL_MAX_DIST);
  setMinDistance(VEL_MIN_DIST);
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

  std::shared_ptr<DataPacket> packet;

  while (!m_acqThread->getBuffer().isEmpty())
    if (readPacket(packet))
      if (m_publish_raw){
        Notify("IVELODYNE_RAW",
                  static_cast<void *>(packet.get()),
                  packet.get()->mPacketSize,MOOSLocalTime());
      }

  AppCastingMOOSInstrument::PostReport();
  return(true);
}

bool Velodyne::readPacket(std::shared_ptr<DataPacket>& packet)
{
  if (m_acqThread->getBuffer().isEmpty()) {
    return(false);
  } else {
    packet = m_acqThread->getBuffer().dequeue();
  }
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
    else if (param == "MAX_DISTANCE")
      handled = setMaxDistance(atof(value.c_str()));
    else if (param == "MIN_DISTANCE")
      handled = setMinDistance(atof(value.c_str()));


    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  registerVariables();
  m_acqThread->start();
  return(true);
}

bool Velodyne::setMaxDistance(double v)
{
  m_max_distance = std::max(v, m_min_distance);
  return(true);
}

bool Velodyne::setMinDistance(double v)
{
  m_min_distance = std::min(v, m_max_distance);
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
  m_msgs << "Configuration:\n";
  m_msgs << "--------------\n";
  ACTable actab(2);
  actab << "Variable | Value";
  actab.addHeaderLines();
  actab << "Maximum Distance" << m_max_distance;
  actab << "Minimum Distance" << m_min_distance;
  m_msgs << actab.getFormattedString();

  m_msgs << endl << endl;

  actab = ACTable(2);
  actab << "Time | Buffer Size";
  actab.addHeaderLines();
  uint buffer_size = m_acqThread->getBuffer().getSize();
  actab << MOOSLocalTime() << buffer_size;
  m_msgs << actab.getFormattedString();

  return(true);
}
