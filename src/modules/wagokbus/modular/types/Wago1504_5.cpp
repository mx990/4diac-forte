/*************************************************************************
 *** FORTE Library Element
 ***
 *** This file was generated using the 4DIAC FORTE Export Filter V1.0.x!
 ***
 *** Name: Wago1504_5
 *** Description: Service Interface Function Block Type
 *** Version: 
 ***     0.0: 2016-11-30/4DIAC-IDE - 4DIAC-Consortium - 
 *************************************************************************/

#include "Wago1504_5.h"
#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "Wago1504_5_gen.cpp"
#endif

DEFINE_FIRMWARE_FB(FORTE_Wago1504_5, g_nStringIdWago1504_5)

const CStringDictionary::TStringId FORTE_Wago1504_5::scm_anDataInputNames[] = {g_nStringIdQI, g_nStringIdDigitalOutput_1, g_nStringIdDigitalOutput_2, g_nStringIdDigitalOutput_3, g_nStringIdDigitalOutput_4, g_nStringIdDigitalOutput_5, g_nStringIdDigitalOutput_6, g_nStringIdDigitalOutput_7, g_nStringIdDigitalOutput_8, g_nStringIdDigitalOutput_9, g_nStringIdDigitalOutput_10, g_nStringIdDigitalOutput_11, g_nStringIdDigitalOutput_12, g_nStringIdDigitalOutput_13, g_nStringIdDigitalOutput_14, g_nStringIdDigitalOutput_15, g_nStringIdDigitalOutput_16};

const CStringDictionary::TStringId FORTE_Wago1504_5::scm_anDataInputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING, g_nStringIdWSTRING};

const CStringDictionary::TStringId FORTE_Wago1504_5::scm_anDataOutputNames[] = {g_nStringIdQO, g_nStringIdSTATUS};

const CStringDictionary::TStringId FORTE_Wago1504_5::scm_anDataOutputTypeIds[] = {g_nStringIdBOOL, g_nStringIdWSTRING};

const TForteInt16 FORTE_Wago1504_5::scm_anEIWithIndexes[] = {0};
const TDataIOID FORTE_Wago1504_5::scm_anEIWith[] = {1, 2, 5, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0, scmWithListDelimiter};
const CStringDictionary::TStringId FORTE_Wago1504_5::scm_anEventInputNames[] = {g_nStringIdMAP};

const TDataIOID FORTE_Wago1504_5::scm_anEOWith[] = {0, scmWithListDelimiter, 0, 1, scmWithListDelimiter};
const TForteInt16 FORTE_Wago1504_5::scm_anEOWithIndexes[] = {0, 2, -1};
const CStringDictionary::TStringId FORTE_Wago1504_5::scm_anEventOutputNames[] = {g_nStringIdMAPO, g_nStringIdIND};

const SAdapterInstanceDef FORTE_Wago1504_5::scm_astAdapterInstances[] = {
{g_nStringIdWagoBusAdapter, g_nStringIdBusAdapterOut, true },
{g_nStringIdWagoBusAdapter, g_nStringIdBusAdapterIn, false }};

const SFBInterfaceSpec FORTE_Wago1504_5::scm_stFBInterfaceSpec = {
  1,  scm_anEventInputNames,  scm_anEIWith,  scm_anEIWithIndexes,
  2,  scm_anEventOutputNames,  scm_anEOWith, scm_anEOWithIndexes,  17,  scm_anDataInputNames, scm_anDataInputTypeIds,
  2,  scm_anDataOutputNames, scm_anDataOutputTypeIds,
  2,scm_astAdapterInstances};

