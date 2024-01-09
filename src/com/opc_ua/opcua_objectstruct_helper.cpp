/*******************************************************************************
 * Copyright (c) 2023 Primetals Technologies Austria GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *    Markus Meingast - initial implementation
 *******************************************************************************/

#include "opcua_objectstruct_helper.h"
#include "opcua_layer.h"
#include "struct_action_info.h"
#include "../../core/cominfra/basecommfb.h"
#include "opcua_local_handler.h"

using namespace forte::com_infra;

const UA_UInt16 COPC_UA_ObjectStruct_Helper::smOpcuaNamespaceIndex = 2;

const std::string COPC_UA_ObjectStruct_Helper::smStructTypesBrowsePath = "/Types/0:ObjectTypes/0:BaseObjectType/2:";

const std::string COPC_UA_ObjectStruct_Helper::smMemberNamespaceIndex = "/2:";

char COPC_UA_ObjectStruct_Helper::smEmptyLocale[] = "";

COPC_UA_ObjectStruct_Helper::COPC_UA_ObjectStruct_Helper(COPC_UA_Layer &paLayer, COPC_UA_HandlerAbstract *paHandler):
  mLayer(paLayer), mHandler(paHandler) {
}

COPC_UA_ObjectStruct_Helper::~COPC_UA_ObjectStruct_Helper() {
  for(char* name: mStructTypeNames) {
    delete[] name;
  }
  mHandler = nullptr;
}

void COPC_UA_ObjectStruct_Helper::uninitializeStruct() {
  for(std::shared_ptr<CActionInfo> actionInfo : mStructMemberActionInfos) {
      mHandler->uninitializeAction(*actionInfo);
    }
    if(mCreateNodeActionInfo) {
      mHandler->uninitializeAction(*mCreateNodeActionInfo);
  }
}

bool COPC_UA_ObjectStruct_Helper::checkStructTypeConnection(bool paIsPublisher) {
  std::string browsePath(getStructBrowsePath(smStructTypesBrowsePath, paIsPublisher));
  if(isOPCUAObjectPresent(browsePath)) {
    return true;
  }

  CIEC_ANY** apoRDs = paIsPublisher ? mLayer.getCommFB()->getSDs() : mLayer.getCommFB()->getRDs();
  CIEC_STRUCT& structType = static_cast<CIEC_STRUCT&>(apoRDs[0]->unwrap());
  if(createOPCUAStructType(getStructTypeName(paIsPublisher), structType)) {
    return true;
  }
  DEVLOG_ERROR("[OPC UA OBJECT STRUCT HELPER]: Could not create OPC UA Struct Type at FB %s\n", mLayer.getCommFB()->getInstanceName());
  return false;
}

bool COPC_UA_ObjectStruct_Helper::createOPCUAStructType(const std::string &paStructTypeName, CIEC_STRUCT &paStructType) {
  COPC_UA_Local_Handler* localHandler = static_cast<COPC_UA_Local_Handler*>(mHandler);
  if(!localHandler) {
    DEVLOG_ERROR("[OPC UA OBJECT STRUCT HELPER]: Failed to get LocalHandler because LocalHandler is null!\n");
    return false;
  }
  UA_Server *server = localHandler->getUAServer();
  UA_NodeId typeNodeId;
  if(!defineOPCUAStructTypeNode(server, typeNodeId, paStructTypeName)) return false;
  const CStringDictionary::TStringId* structMemberNames = paStructType.elementNames();
  for(size_t i = 0; i < paStructType.getStructSize(); i++) {
    CIEC_ANY* structMember = paStructType.getMember(i);
    if(!addOPCUAStructTypeComponent(server, typeNodeId, structMember, CStringDictionary::getInstance().get(structMemberNames[i]))) {
      return false;
    } 
  }
  return true;
}

bool COPC_UA_ObjectStruct_Helper::defineOPCUAStructTypeNode(UA_Server *paServer, UA_NodeId &paNodeId, const std::string &paStructTypeName) {
  char* structTypeName = new char[paStructTypeName.length() +1];
  strncpy(structTypeName, paStructTypeName.c_str(), paStructTypeName.length());
  structTypeName[paStructTypeName.length()] = '\0';
  mStructTypeNames.push_back(structTypeName);
  paNodeId = UA_NODEID_NUMERIC(smOpcuaNamespaceIndex, 0);
  UA_ObjectTypeAttributes oAttr = UA_ObjectTypeAttributes_default;
  oAttr.displayName = UA_LOCALIZEDTEXT(smEmptyLocale, structTypeName);
  UA_StatusCode status = UA_Server_addObjectTypeNode(paServer, paNodeId,
    UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
    UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
    UA_QUALIFIEDNAME(smOpcuaNamespaceIndex, structTypeName), oAttr,
    nullptr, &paNodeId);
  
  if (status != UA_STATUSCODE_GOOD) {
    DEVLOG_ERROR("[OPC UA OBJECT STRUCT HELPER]: Failed to create OPC UA Struct Type Node for Type %s, Status Code: %s\n", paStructTypeName.c_str(), UA_StatusCode_name(status));
    return false;
  }
  return true;
}

bool COPC_UA_ObjectStruct_Helper::addOPCUAStructTypeComponent(UA_Server *paServer, UA_NodeId &paParentNodeId, CIEC_ANY *paStructMember, const std::string &paStructMemberName) {
  char* memberName = new char[paStructMemberName.length() +1];
  strncpy(memberName, paStructMemberName.c_str(), paStructMemberName.length());
  memberName[paStructMemberName.length()] = '\0';
  mStructTypeNames.push_back(memberName);
  UA_VariableAttributes vAttr = UA_VariableAttributes_default;
    vAttr.displayName = UA_LOCALIZEDTEXT(smEmptyLocale, memberName);
    vAttr.valueRank = UA_VALUERANK_SCALAR;
    vAttr.minimumSamplingInterval = 0.000000;
    vAttr.userAccessLevel = 1;
    vAttr.accessLevel = 3;
    vAttr.dataType = COPC_UA_Helper::getOPCUATypeFromAny(*paStructMember)->typeId;//UA_NODEID_NUMERIC(0, 1); // 1 is Boolean

    UA_NodeId memberNodeId = UA_NODEID_NUMERIC(smOpcuaNamespaceIndex, 0);
    UA_StatusCode status = UA_Server_addVariableNode(paServer, UA_NODEID_NULL, paParentNodeId,
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
      UA_QUALIFIEDNAME(smOpcuaNamespaceIndex, memberName),
      UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), vAttr, NULL, &memberNodeId);
  if(status != UA_STATUSCODE_GOOD) {
    DEVLOG_ERROR("[OPC UA OBJECT STRUCT HELPER]: Failed to add Member to OPC UA Struct Type Node for Member %s, Status Code: %s\n", paStructMemberName.c_str(), UA_StatusCode_name(status));
    return false;
  }
  status = UA_Server_addReference(paServer, memberNodeId,
      UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
      UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
  if(status != UA_STATUSCODE_GOOD) {
    DEVLOG_ERROR("[OPC UA OBJECT STRUCT HELPER]: Failed to add OPC UA reference to Struct Member %s, Status Code: %s\n", paStructMemberName.c_str(), UA_StatusCode_name(status));
    return false;
  }
  return true;
}

forte::com_infra::EComResponse COPC_UA_ObjectStruct_Helper::createObjectNode(CActionInfo& paActionInfo, bool paIsPublisher) {
  EComResponse response = e_InitTerminated;
  std::string browsePath = (*paActionInfo.getNodePairInfo().begin())->mBrowsePath;
  if(!isOPCUAObjectPresent(browsePath)) {
    mCreateNodeActionInfo = getCreateObjectActionInfo(paActionInfo, browsePath, paIsPublisher);
    if( (UA_STATUSCODE_GOOD != mHandler->initializeAction(*mCreateNodeActionInfo)) || (UA_STATUSCODE_GOOD != mHandler->executeAction(*mCreateNodeActionInfo)) ) {
      return response;
    }
  }
  return initializeMemberAction(paActionInfo, browsePath, paIsPublisher);
}

CIEC_ANY const *COPC_UA_ObjectStruct_Helper::getStructMember(CActionInfo &paActionInfo, bool paIsSD) {
  CIEC_ANY** apoDataPorts = paIsSD ? mLayer.getCommFB()->getSDs() : mLayer.getCommFB()->getRDs();
  CIEC_STRUCT& structType = static_cast<CIEC_STRUCT&>(apoDataPorts[0]->unwrap());
  const std::string actionBrowsePath = (*paActionInfo.getNodePairInfo().begin())->mBrowsePath;

  for(size_t i = 0; i < mStructMemberActionInfos.size(); i++) {
    std::shared_ptr<CActionInfo> actionInfo = mStructMemberActionInfos[i];
    std::string browsePath = (*actionInfo->getNodePairInfo().begin())->mBrowsePath;
    if(browsePath == actionBrowsePath) {
      return structType.getMember(i);
    }
  }
  return nullptr;
}

forte::com_infra::EComResponse COPC_UA_ObjectStruct_Helper::executeStructAction() {
  for(std::shared_ptr<CActionInfo> actionInfo : mStructMemberActionInfos) {
    if(UA_STATUSCODE_GOOD != mHandler->executeAction(*actionInfo)) {
      return e_ProcessDataDataTypeError;
    }
  }
  return e_ProcessDataOk;
}

int COPC_UA_ObjectStruct_Helper::getRDBufferIndexFromNodeId(const UA_NodeId *paNodeId) {
  for(size_t i = 0; i < mStructMemberActionInfos.size(); i++) {
    std::shared_ptr<CActionInfo> actionInfo = mStructMemberActionInfos[i];
    UA_NodeId *nodeId = (*actionInfo->getNodePairInfo().begin())->mNodeId;
    if(UA_NodeId_equal(nodeId, paNodeId)) {
      return (int)i;
    }
  }
  return -1;
}

void COPC_UA_ObjectStruct_Helper::setMemberValues(CIEC_ANY** paRDs, CIEC_ANY **paRDBuffer) {
  // TODO implement layer to handle more than 1 struct
  CIEC_STRUCT& structType = static_cast<CIEC_STRUCT&>(paRDs[0]->unwrap());
  for(size_t i = 0; i < structType.getStructSize(); i++) {
    structType.getMember(i)->setValue(*paRDBuffer[i]);
  }
}

CIEC_ANY **COPC_UA_ObjectStruct_Helper::initializeRDBuffer() {
  // TODO implement layer to handle more than 1 struct
  CIEC_ANY** rds = mLayer.getCommFB()->getRDs();
  CIEC_STRUCT& structType = static_cast<CIEC_STRUCT&>(rds[0]->unwrap());
  const size_t structSize = structType.getStructSize();
  CIEC_ANY **RDBuffer = new CIEC_ANY*[structSize];
  for(size_t i = 0; i < structSize; i++) {
    RDBuffer[i] = structType.getMember(i)->clone(nullptr);
  }
  return RDBuffer;
}

void COPC_UA_ObjectStruct_Helper::deleteRDBufferEntries(forte::com_infra::CBaseCommFB &paCommFB, CIEC_ANY **paRDBuffer) {
  if(paCommFB.getComServiceType() == e_Subscriber) {
    CIEC_ANY** rds = paCommFB.getRDs();
    CIEC_STRUCT& structType = static_cast<CIEC_STRUCT&>(rds[0]->unwrap());
    for(size_t i = 0; i < structType.getStructSize(); i++) {
      delete paRDBuffer[i];
    }
  }
}

bool COPC_UA_ObjectStruct_Helper::isStructType(const COPC_UA_Layer &paLayer, bool paIsPublisher) {
  // TODO implement layer to handle more than 1 struct
  // For the concept, only the first SD/RD port needs to be a struct 
  TPortId nrOfPorts = paIsPublisher ? paLayer.getCommFB()->getNumSD() : paLayer.getCommFB()->getNumRD();
  CIEC_ANY** apoDataPorts = paIsPublisher ? paLayer.getCommFB()->getSDs() : paLayer.getCommFB()->getRDs();
  return nrOfPorts > 0 && CIEC_ANY::e_STRUCT == apoDataPorts[0]->unwrap().getDataTypeID();
}

std::shared_ptr<CActionInfo> COPC_UA_ObjectStruct_Helper::getCreateObjectActionInfo(CActionInfo& paActionInfo, std::string &paBrowsePath, bool paIsPublisher) {
  // TODO implement layer to handle more than 1 struct
  std::shared_ptr<CActionInfo> actionInfo = std::make_shared<CActionInfo>(mLayer, CActionInfo::UA_ActionType::eCreateObject, paActionInfo.getEndpoint());
  std::string typeBrowsePath(getStructBrowsePath(smStructTypesBrowsePath, paIsPublisher));
  CSinglyLinkedList<CActionInfo::CNodePairInfo*>& nodePairs = actionInfo->getNodePairInfo();
  nodePairs.pushBack(new CActionInfo::CNodePairInfo(nullptr, typeBrowsePath));
  nodePairs.pushBack(new CActionInfo::CNodePairInfo(nullptr, paBrowsePath));
  return actionInfo;
}

forte::com_infra::EComResponse COPC_UA_ObjectStruct_Helper::initializeMemberAction(CActionInfo& paActionInfo, std::string &paBrowsePath, bool paIsPublisher) {
  // TODO implement layer to handle more than 1 struct
  COPC_UA_Local_Handler* localHandler = static_cast<COPC_UA_Local_Handler*>(mHandler);
  if(!localHandler) {
    DEVLOG_ERROR("[OPC UA OBJECT STRUCT HELPER]: Failed to get LocalHandler because LocalHandler is null!\n");
    return e_InitTerminated;
  }
  CIEC_ANY** apoDataPorts = paIsPublisher ? mLayer.getCommFB()->getSDs() : mLayer.getCommFB()->getRDs();
  CIEC_STRUCT& structType = static_cast<CIEC_STRUCT&>(apoDataPorts[0]->unwrap());
  const CStringDictionary::TStringId* structMemberNames = structType.elementNames();
  
  for(size_t i = 0; i < structType.getStructSize(); i++) {
    std::string memberBrowsePath(getStructMemberBrowsePath(paBrowsePath, structMemberNames[i]));

    std::shared_ptr<CActionInfo> actionInfo = std::make_shared<CStructMemberActionInfo>(*this, mLayer, paActionInfo.getAction(), paActionInfo.getEndpoint());
    CIEC_ANY* memberVariable = structType.getMember(i);
    actionInfo->getNodePairInfo().pushBack(new CActionInfo::CNodePairInfo(nullptr, memberBrowsePath));
    if(UA_STATUSCODE_GOOD != localHandler->initializeActionForObjectStruct(actionInfo, *memberVariable)) {
      DEVLOG_ERROR("[OPC UA OBJECT STRUCT HELPER]: Error occured in FB %s while initializing Struct member %s\n", mLayer.getCommFB()->getInstanceName(),
      CStringDictionary::getInstance().get(structMemberNames[i]));
      return e_InitTerminated;
    }
    mStructMemberActionInfos.push_back(std::move(actionInfo));
  }
  return e_InitOk;
}

bool COPC_UA_ObjectStruct_Helper::isOPCUAObjectPresent(std::string &paBrowsePath) {
  COPC_UA_Local_Handler* localHandler = static_cast<COPC_UA_Local_Handler*>(mHandler);
  if(localHandler) {
    // CActionInfo::CNodePairInfo* nodePair = new CActionInfo::CNodePairInfo(nullptr, paBrowsePath);
    CActionInfo::CNodePairInfo nodePair(nullptr, paBrowsePath);
    bool retVal = localHandler->isOPCUAObjectPresent(nodePair);
    if(nodePair.mNodeId) delete nodePair.mNodeId;
    return retVal;
  } else {
    DEVLOG_ERROR("[OPC UA OBJECT STRUCT HELPER]: Failed to get LocalHandler because LocalHandler is null!\n");
  }
  return false;
}

std::string COPC_UA_ObjectStruct_Helper::getStructBrowsePath(const std::string &paPathPrefix, bool paIsPublisher) {
  std::string structTypeName(getStructTypeName(paIsPublisher));
  if(!structTypeName.empty()) {
	  return paPathPrefix + structTypeName;
  }
  return std::string();
}

std::string COPC_UA_ObjectStruct_Helper::getStructMemberBrowsePath(std::string &paBrowsePathPrefix, const CStringDictionary::TStringId structMemberNameId) {
  return paBrowsePathPrefix + smMemberNamespaceIndex + CStringDictionary::getInstance().get(structMemberNameId);
}

std::string COPC_UA_ObjectStruct_Helper::getStructTypeName(bool paIsPublisher) {
  CIEC_ANY* type = paIsPublisher ? mLayer.getCommFB()->getSDs()[0] : mLayer.getCommFB()->getRDs()[0];
  return (type != nullptr) ? std::string(CStringDictionary::getInstance().get(type->unwrap().getTypeNameID())) : std::string();
}