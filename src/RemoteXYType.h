#ifndef RemoteXYType_h
#define RemoteXYType_h

#include "RemoteXYGuiData.h"

#define UNUSED(x) (void)(x)

class RemoteXYType {

  public:
  CRemoteXYGuiData * guiData;

  public:     
  virtual void handler () {};

  virtual void init (CRemoteXYGuiData * _guiData) {
    guiData = _guiData;
  };


};






#endif // RemoteXYType_h