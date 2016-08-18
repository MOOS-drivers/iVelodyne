/************************************************************/
/*    NAME: Mohamed Saad Ibn Seddik                                              */
/*    ORGN: MOOS-Drivers                                    */
/*    FILE: Velodyne.h                                          */
/*    DATE: 2016/08/18                                      */
/************************************************************/

#ifndef Velodyne_HEADER
#define Velodyne_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSInstrument.h"

class Velodyne : public AppCastingMOOSInstrument
{
 public:
   Velodyne();
   ~Velodyne() {};

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

 private: // State variables
};

#endif 
