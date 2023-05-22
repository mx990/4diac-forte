/*******************************************************************************
 * Copyright (c) 2017 - 2018 fortiss GmbH
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Johannes Messmer - initial API and implementation and/or initial documentation
 *   Jose Cabral - Cleaning of namespaces
 *******************************************************************************/

#include "io_master_multi.h"

using namespace forte::core::io;

const char * const IOConfigFBMultiMaster::scmFailedToInitSlaves = "Failed to initialize slaves. Check if the configuration matches the hardware setup.";

TMasterList IOConfigFBMultiMaster::mInstances;
TForteUInt16 IOConfigFBMultiMaster::mInstancesIncrement = 0;

IOConfigFBMultiMaster::IOConfigFBMultiMaster(CResource *paSrcRes, const SFBInterfaceSpec *paInterfaceSpec, const CStringDictionary::TStringId paInstanceNameId,
    TForteByte *paFBConnData, TForteByte *paFBVarsData) :
    IOConfigFBController(paSrcRes, paInterfaceSpec, paInstanceNameId, paFBConnData, paFBVarsData) {
  mId = mInstancesIncrement++;
  mInstances.pushBack(this);
}

IOConfigFBMultiMaster* IOConfigFBMultiMaster::getMasterById(TForteUInt16 paId) {
  TMasterList::Iterator itEnd = mInstances.end();
  int i = 0;
  for(TMasterList::Iterator it = mInstances.begin(); it != itEnd; ++it, i++) {
    if(paId == i && *it != nullptr) {
      return *it;
    }
  }
  return nullptr;
}

void IOConfigFBMultiMaster::onStartup() {
  if(nullptr == BusAdapterOut().getPeer()) {
    return IOConfigFBController::onStartup();
  }

  BusAdapterOut().MasterId() = CIEC_UINT(mId);
  BusAdapterOut().Index() = CIEC_UINT(0);
  BusAdapterOut().QI() = CIEC_BOOL(true);
  sendAdapterEvent(scmBusAdapterAdpNum, IOConfigFBMultiAdapter::scmEventINITID);
}

void IOConfigFBMultiMaster::onStop() {
  if(nullptr == BusAdapterOut().getPeer()) {
    return IOConfigFBController::onStop();
  }

  BusAdapterOut().QI() = CIEC_BOOL(false);
  sendAdapterEvent(scmBusAdapterAdpNum, IOConfigFBMultiAdapter::scmEventINITID);
}

void IOConfigFBMultiMaster::executeEvent(TEventID paEIID) {
  IOConfigFBController::executeEvent(paEIID);

  if(BusAdapterOut().INITO() == paEIID) {
    QO() = BusAdapterOut().QO();

    if(BusAdapterOut().QI() == true) {
      if(BusAdapterOut().QO() == true) {
        IOConfigFBController::onStartup();
      } else {
        started(scmFailedToInitSlaves);
      }
    } else {
      IOConfigFBController::onStop();
    }
  }
}

