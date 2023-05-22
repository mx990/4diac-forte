/*******************************************************************************
 * Copyright (c) 2006 - 2011 ACIN, Profactor GmbH
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alois Zoitl, Gerhard Ebenhofer
 *     - initial API and implementation and/or initial documentation
 *******************************************************************************/

#pragma once

#include "basicfb.h"
#include "iec61131_functions.h"
#include "forte_array_common.h"
#include "forte_array.h"
#include "forte_array_fixed.h"
#include "forte_array_variable.h"


class FORTE_E_SPLIT: public CBasicFB {
  DECLARE_FIRMWARE_FB(FORTE_E_SPLIT)

private:
  static const TEventID scm_nEventEIID = 0;
  static const TForteInt16 scm_anEIWithIndexes[];
  static const CStringDictionary::TStringId scm_anEventInputNames[];
  static const TEventID scm_nEventEO1ID = 0;
  static const TEventID scm_nEventEO2ID = 1;
  static const TForteInt16 scm_anEOWithIndexes[];
  static const CStringDictionary::TStringId scm_anEventOutputNames[];

  static const SFBInterfaceSpec scm_stFBInterfaceSpec;
  CIEC_ANY *getVarInternal(size_t) override;
  static const TForteInt16 scm_nStateSTART = 0;
  static const TForteInt16 scm_nStateState = 1;
  
  void enterStateSTART(void);
  void enterStateState(void);

  void executeEvent(TEventID pa_nEIID) override;

  void readInputData(TEventID pa_nEIID) override;
  void writeOutputData(TEventID pa_nEIID) override;

public:
  FORTE_E_SPLIT(CStringDictionary::TStringId pa_nInstanceNameId, CResource *pa_poSrcRes);

  CEventConnection conn_EO1;
  CEventConnection conn_EO2;
  CIEC_ANY *getDI(size_t) override;
  CIEC_ANY *getDO(size_t) override;
  CEventConnection *getEOConUnchecked(TPortId) override;
  CDataConnection **getDIConUnchecked(TPortId) override;
  CDataConnection *getDOConUnchecked(TPortId) override;
  void evt_EI() {
    receiveInputEvent(scm_nEventEIID, nullptr);
  }
  void operator()() {
    evt_EI();
  }
};



