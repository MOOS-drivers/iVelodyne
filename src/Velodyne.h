/************************************************************/
/*    NAME: Mohamed Saad Ibn Seddik                                              */
/*    ORGN: MOOS-Drivers                                    */
/*    FILE: Velodyne.h                                          */
/*    DATE: 2016/08/18                                      */
/************************************************************/

#ifndef Velodyne_HEADER
#define Velodyne_HEADER

#include "sensor/AcquisitionThread.h"
#include "sensor/DataPacket.h"

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSInstrument.h"

#define VEL_MAX_DIST 100.
#define VEL_MIN_DIST .5

class Velodyne : public AppCastingMOOSInstrument
{
 public:
  Velodyne();
  ~Velodyne();

 protected: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSInstrument function to overload
  bool buildReport();

 protected:
  void registerVariables();

 private: // Configuration variables
  bool m_publish_raw;
  double m_max_distance, m_min_distance;
  bool setMaxDistance(double);
  bool setMinDistance(double);

  bool readPacket(std::shared_ptr<DataPacket>&);


 private: // State variables
  UDPConnectionServer *m_udpDataSocket;
  AcquisitionThread<DataPacket> *m_acqThread;
};

#endif
