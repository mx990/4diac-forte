/*******************************************************************************
 * Copyright (c) 2013 fortiss GmbH, 2018 TU Vienna/ACIN
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *    Alois Zoitl
 *      - initial implementation and rework communication infrastructure
 *    Martin Melik Merkumians
 *      - fixes event chain initialisation, adds typifyAnyAdapter
 *******************************************************************************/
#ifndef ANYADAPTER_H_
#define ANYADAPTER_H_

#include "adapter.h"

/*! \brief A generic adapter that behaves similar to the ANY data type and allows all other adapters to be connected.
 *
 *  The main use of such an adapter is in service interfaces that provide generic services based on the adapter connected.
 */

class CAnyAdapter : public CAdapter{
  DECLARE_ADAPTER_TYPE(CAnyAdapter)

  public:
    CAnyAdapter(CStringDictionary::TStringId pa_anAdapterInstanceName, CResource *pa_poSrcRes, bool pa_bIsPlug);
    ~CAnyAdapter() override;

    void typifyAnyAdapter(CAdapter *pa_poPeer);

    void setParentFB(CFunctionBlock *pa_poParentFB, TForteUInt8 pa_nParentAdapterlistID) override;

    bool disconnect(CAdapterConnection *pa_poAdConn) override;

    //! Helper functions allowing to retrieve interface information from any_adpaters TODO look for Doxygen grouping syntax
    TEventID getNumEIs() const {
      return m_pstInterfaceSpec->m_nNumEIs;
    }

    TEventID getNumEOs() const {
      return m_pstInterfaceSpec->m_nNumEOs;
    }

    const TForteInt16* getEIWithIndexes() const {
      return m_pstInterfaceSpec->m_anEIWithIndexes;
    }

    const TDataIOID* getEIWiths() const {
      return m_pstInterfaceSpec->m_anEIWith;
    }

    TPortId getNumDIs() const {
      return m_pstInterfaceSpec->m_nNumDIs;
    }

    const CStringDictionary::TStringId* getDataInputNames() const {
      return m_pstInterfaceSpec->m_aunDINames;
    }

    TIEC_ANYPtr getDataInputs(){
      return getDI(0);
    }

    TPortId getNumDOs() const {
      return m_pstInterfaceSpec->m_nNumDOs;
    }

    const CStringDictionary::TStringId* getDataOutputNames() const {
      return m_pstInterfaceSpec->m_aunDONames;
    }

    TIEC_ANYPtr getDataOutputs(){
      return getDO(0);
    }

  protected:

  private:
    static const SFBInterfaceSpec scm_stFBInterfaceSpec; //! interface spec for the empty interface of an any adapter will be used for plug and socket

    //!Interface specification to be used when configured
    SFBInterfaceSpec m_stCurrentFBInterfaceSpec;

    CFunctionBlock *m_ParentFB; //!< Pointer to the parent FB
    TForteUInt8 m_nParentAdapterlistID; //!< Adapter list ID in respect to the parent FB
};

#endif /* ANYADAPTER_H_ */
