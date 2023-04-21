/*******************************************************************************
 * Copyright (c) 2018, fortiss GmbH
 *               2018 Johannes Kepler University
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Jose Cabral - initial tests
 *   Alois Zoitl - migrated fb tests to boost test infrastructure
 *******************************************************************************/
#include "../../core/fbtests/fbtestfixture.h"

#ifdef FORTE_ENABLE_GENERATED_SOURCE_CPP
#include "GET_STRUCT_VALUE_tester_gen.cpp"
#endif

#ifdef LAYOUT_REFACTOR //TODO Reenable after struct refactor is complete
class CIEC_GET_STRUCT_VALUE_Struct_test1 : public CIEC_STRUCT {
  DECLARE_FIRMWARE_DATATYPE(GET_STRUCT_VALUE_Struct_test1)

    /* Struct:
     *   Val1 : String[3]
     *   Val2 : INT
     */

  public:
    CIEC_GET_STRUCT_VALUE_Struct_test1() :
        CIEC_STRUCT(g_nStringIdGET_STRUCT_VALUE_Struct_test1, 2, scm_unElementTypes, scm_unElementNames, e_APPLICATION + e_CONSTRUCTED + 1) {
    }

    virtual ~CIEC_GET_STRUCT_VALUE_Struct_test1() = default;

    static const unsigned int sizeOfFirstArray = 2;

  private:
    static const CStringDictionary::TStringId scm_unElementTypes[];
    static const CStringDictionary::TStringId scm_unElementNames[];
};

const CStringDictionary::TStringId CIEC_GET_STRUCT_VALUE_Struct_test1::scm_unElementTypes[] = { g_nStringIdARRAY, sizeOfFirstArray, g_nStringIdSTRING,
  g_nStringIdINT };
const CStringDictionary::TStringId CIEC_GET_STRUCT_VALUE_Struct_test1::scm_unElementNames[] = { g_nStringIdVal1, g_nStringIdVal2 };

DEFINE_FIRMWARE_DATATYPE(GET_STRUCT_VALUE_Struct_test1, g_nStringIdGET_STRUCT_VALUE_Struct_test1)

class CIEC_GET_STRUCT_VALUE_Struct_test2 : public CIEC_STRUCT {
  DECLARE_FIRMWARE_DATATYPE(GET_STRUCT_VALUE_Struct_test2)

    /* Struct:
     *   Val1 : INT
     *   Val2 : CIEC_GET_STRUCT_VALUE_Struct_test1
     */

  public:
    CIEC_GET_STRUCT_VALUE_Struct_test2() :
        CIEC_STRUCT(g_nStringIdGET_STRUCT_VALUE_Struct_test2, 2, scm_unElementTypes, scm_unElementNames, e_APPLICATION + e_CONSTRUCTED + 1) {
    }

    virtual ~CIEC_GET_STRUCT_VALUE_Struct_test2() = default;

  private:
    static const CStringDictionary::TStringId scm_unElementTypes[];
    static const CStringDictionary::TStringId scm_unElementNames[];
};

const CStringDictionary::TStringId CIEC_GET_STRUCT_VALUE_Struct_test2::scm_unElementTypes[] = { g_nStringIdINT, g_nStringIdGET_STRUCT_VALUE_Struct_test1 };
const CStringDictionary::TStringId CIEC_GET_STRUCT_VALUE_Struct_test2::scm_unElementNames[] = { g_nStringIdVal1, g_nStringIdVal2 };

DEFINE_FIRMWARE_DATATYPE(GET_STRUCT_VALUE_Struct_test2, g_nStringIdGET_STRUCT_VALUE_Struct_test2)



struct GET_STRUCT_VALUE_GenericTestFixture : public CFBTestFixtureBase{

    GET_STRUCT_VALUE_GenericTestFixture(CIEC_ANY *paIN_STRUCT, CIEC_ANY *paOUT) :
        CFBTestFixtureBase(g_nStringIdGET_STRUCT_VALUE){
      setInputData({paIN_STRUCT, &mMember});
      setOutputData({&mQO, paOUT});
    }

    CIEC_STRING mMember;
    CIEC_BOOL mQO;
};

struct GET_STRUCT_VALUE_Main_TestFixture : public GET_STRUCT_VALUE_GenericTestFixture{

    GET_STRUCT_VALUE_Main_TestFixture() :
        GET_STRUCT_VALUE_GenericTestFixture(&mIn_struct, &mOut){
      mIn_struct.fromString("(Val1:=1,Val2:=(Val1:=['strin1','string2',''], Val2:=2))");
      CFBTestFixtureBase::setup();
    }

    CIEC_GET_STRUCT_VALUE_Struct_test2 mIn_struct;
    CIEC_INT mOut;
};

BOOST_FIXTURE_TEST_SUITE( GET_STRUCT_VALUE_MainTests, GET_STRUCT_VALUE_Main_TestFixture)

  BOOST_AUTO_TEST_CASE(firstLevel) {
    mMember = CIEC_STRING("Val1");
    triggerEvent(0);
    BOOST_CHECK(checkForSingleOutputEventOccurence(0));
    BOOST_CHECK_EQUAL(true, mQO);
    BOOST_CHECK_EQUAL(1, static_cast<CIEC_INT::TValueType>(mOut));
  }

  BOOST_AUTO_TEST_CASE(secondLevel) {
    mMember = CIEC_STRING("Val2.Val2");
    triggerEvent(0);
    BOOST_CHECK(checkForSingleOutputEventOccurence(0));
    BOOST_CHECK_EQUAL(true, mQO);
    BOOST_CHECK_EQUAL(2, static_cast<CIEC_INT::TValueType>(mOut));
  }


  BOOST_AUTO_TEST_CASE(firstLevelWrongName) {
    mMember = CIEC_STRING("xVal1");
    triggerEvent(0);
    BOOST_CHECK(checkForSingleOutputEventOccurence(0));
    BOOST_CHECK_EQUAL(false, mQO);
  }

  BOOST_AUTO_TEST_CASE(firstLevelWrongNameWithSecondLevel) {
    mMember = CIEC_STRING("xVal1.Val2");
    triggerEvent(0);
    BOOST_CHECK(checkForSingleOutputEventOccurence(0));
    BOOST_CHECK_EQUAL(false, mQO);
  }

  BOOST_AUTO_TEST_CASE(secondLevelWrongName) {
    mMember = CIEC_STRING("Val2.xVal2");
    triggerEvent(0);
    BOOST_CHECK(checkForSingleOutputEventOccurence(0));
    BOOST_CHECK_EQUAL(false, mQO);
  }

  BOOST_AUTO_TEST_CASE(accessNonStruct) {
    mMember = CIEC_STRING("Val1.Val1");
    triggerEvent(0);
    BOOST_CHECK(checkForSingleOutputEventOccurence(0));
    BOOST_CHECK_EQUAL(false, mQO);
  }

  BOOST_AUTO_TEST_CASE(wrongOutputType) {
    mMember = CIEC_STRING("Val2.Val1");
    triggerEvent(0);
    BOOST_CHECK(checkForSingleOutputEventOccurence(0));
    BOOST_CHECK_EQUAL(false, mQO);
  }

  BOOST_AUTO_TEST_CASE(WrongEventInput) {
    mMember = CIEC_STRING("Val2.Val1");
    triggerEvent(1);
    BOOST_CHECK(eventChainEmpty());
  }

BOOST_AUTO_TEST_SUITE_END()


struct GET_STRUCT_VALUE_WRONG_OUTPUT_TYPE_TestFixture : public GET_STRUCT_VALUE_GenericTestFixture{

    GET_STRUCT_VALUE_WRONG_OUTPUT_TYPE_TestFixture() :
        GET_STRUCT_VALUE_GenericTestFixture(&mIn_struct, &mOut){
      mIn_struct = CIEC_INT(1);
      CFBTestFixtureBase::setup();
    }

    CIEC_INT mIn_struct;
    CIEC_INT mOut;
};

BOOST_FIXTURE_TEST_SUITE( GET_STRUCT_VALUE_WRONG_OUTPUT_TYPETests, GET_STRUCT_VALUE_WRONG_OUTPUT_TYPE_TestFixture)

  BOOST_AUTO_TEST_CASE(wrongInputType) {
    mMember = CIEC_STRING("Val1");
    triggerEvent(0);
    BOOST_CHECK(checkForSingleOutputEventOccurence(0));
    BOOST_CHECK_EQUAL(false, mQO);
  }

BOOST_AUTO_TEST_SUITE_END()
#endif
