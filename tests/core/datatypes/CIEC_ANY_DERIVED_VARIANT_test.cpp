/*******************************************************************************
 * Copyright (c) 2026 Martin Erich Jobst
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Martin Erich Jobst - initial implementation
 *******************************************************************************/

#include <boost/test/unit_test.hpp>

#include <sstream>

#include "forte_boost_output_support.h"

#include "forte/datatypes/forte_any_derived_variant.h"
#include "forte/datatypes/forte_array.h"
#include "forte/datatypes/forte_array_fixed.h"

using namespace forte::literals;

namespace forte::test {
  class CIEC_AnyDerivedTestStruct : public CIEC_STRUCT {
      DECLARE_FIRMWARE_DATATYPE(AnyDerivedTestStruct)
    public:
      CIEC_STRING Var1;
      CIEC_BOOL Var2;
      CIEC_INT Var3;

      CIEC_AnyDerivedTestStruct() = default;

      CIEC_AnyDerivedTestStruct(CIEC_STRING paVar1, CIEC_BOOL paVar2, CIEC_INT paVar3) :
          Var1(paVar1),
          Var2(paVar2),
          Var3(paVar3) {};

      size_t getStructSize() const override {
        return 3;
      }

      const StringId *elementNames() const override {
        return scmElementNames;
      }

      StringId getStructTypeNameID() const override {
        return "AnyDerivedTestStruct"_STRID;
      }

      CIEC_ANY *getMember(size_t paMemberIndex) override {
        switch (paMemberIndex) {
          case 0: return &Var1;
          case 1: return &Var2;
          case 2: return &Var3;
        }
        return nullptr;
      }

      const CIEC_ANY *getMember(size_t paMemberIndex) const override {
        switch (paMemberIndex) {
          case 0: return &Var1;
          case 1: return &Var2;
          case 2: return &Var3;
        }
        return nullptr;
      }

    private:
      static const StringId scmElementNames[];
  };

  const StringId CIEC_AnyDerivedTestStruct::scmElementNames[] = {"Var1"_STRID, "Var2"_STRID, "Var3"_STRID};

  DEFINE_FIRMWARE_DATATYPE(AnyDerivedTestStruct, "AnyDerivedTestStruct"_STRID)

  static const void *variantPointee(const CIEC_ANY_DERIVED_VARIANT &paValue) {
    const auto &state = static_cast<const TIecAnyDerivedVariantType &>(paValue);
    if (const auto *array = std::get_if<CIEC_ANY_UNIQUE_PTR<CIEC_ARRAY>>(&state)) {
      return array->get();
    }
    if (const auto *structure = std::get_if<CIEC_ANY_UNIQUE_PTR<CIEC_STRUCT>>(&state)) {
      return structure->get();
    }
    return nullptr;
  }

  static bool variantHasState(const CIEC_ANY_DERIVED_VARIANT &paValue, size_t paExpectedIndex) {
    return !paValue.valueless_by_exception() && paValue.index() == paExpectedIndex && variantPointee(paValue);
  }

  static std::string variantState(const CIEC_ANY_DERIVED_VARIANT &paValue) {
    std::ostringstream result;
    result << "object=" << &paValue << " index=" << paValue.index() << " valueless=" << paValue.valueless_by_exception()
           << " pointee=" << variantPointee(paValue);
    return result.str();
  }

#define CHECK_DERIVED_STATE(value, expected, operation)                                                                \
  BOOST_TEST_CHECKPOINT(operation);                                                                                    \
  BOOST_REQUIRE_MESSAGE(variantHasState(value, expected),                                                              \
                        operation << ": " << #value << " " << variantState(value) << " expected-index=" << expected)

  BOOST_AUTO_TEST_SUITE(CIEC_ANY_DERIVED_VARIANT_function_test)

  BOOST_AUTO_TEST_CASE(Type_test) {
    const CIEC_ANY_DERIVED_VARIANT test;
    BOOST_CHECK(test.getDataTypeID() == CIEC_ANY::e_ANY);
  }

  BOOST_AUTO_TEST_CASE(Initialization_test) {
    // assign struct value
    CIEC_ANY_DERIVED_VARIANT test1 = CIEC_AnyDerivedTestStruct("test"_STRING, true_BOOL, 17_INT);
    auto test1Value = static_cast<CIEC_AnyDerivedTestStruct &>(test1.unwrap());
    BOOST_TEST("test" == static_cast<CIEC_STRING::TValueType>(test1Value.Var1));
    BOOST_TEST(true == static_cast<CIEC_BOOL::TValueType>(test1Value.Var2));
    BOOST_TEST(17 == static_cast<CIEC_INT::TValueType>(test1Value.Var3));

    // assign generic struct value
    CIEC_ANY_DERIVED_VARIANT test3 = CIEC_AnyDerivedTestStruct("other"_STRING, false_BOOL, 4_INT);
    CIEC_ANY &test4 = test3;
    CIEC_ANY_DERIVED_VARIANT test2 = test4;
    auto test2Value = static_cast<CIEC_AnyDerivedTestStruct &>(test2.unwrap());
    BOOST_TEST("other" == static_cast<CIEC_STRING::TValueType>(test2Value.Var1));
    BOOST_TEST(false == static_cast<CIEC_BOOL::TValueType>(test2Value.Var2));
    BOOST_TEST(4 == static_cast<CIEC_INT::TValueType>(test2Value.Var3));

    // assign structs to each other
    CIEC_ANY_DERIVED_VARIANT test5 = test1;
    auto test5Value = static_cast<CIEC_AnyDerivedTestStruct &>(test5.unwrap());
    BOOST_TEST("test" == static_cast<CIEC_STRING::TValueType>(test5Value.Var1));
    BOOST_TEST(true == static_cast<CIEC_BOOL::TValueType>(test5Value.Var2));
    BOOST_TEST(17 == static_cast<CIEC_INT::TValueType>(test5Value.Var3));

    // assign array value
    CIEC_ANY_DERIVED_VARIANT test6 = CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2>{17_DINT, 4_DINT, 21_DINT};
    auto test6Value = static_cast<CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2> &>(test6.unwrap());
    BOOST_TEST(17 == static_cast<CIEC_DINT::TValueType>(test6Value[0]));
    BOOST_TEST(4 == static_cast<CIEC_DINT::TValueType>(test6Value[1]));
    BOOST_TEST(21 == static_cast<CIEC_DINT::TValueType>(test6Value[2]));

    // assign generic array value
    CIEC_ANY_DERIVED_VARIANT test8 = CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2>{1_DINT, 2_DINT, 4_DINT};
    CIEC_ANY &test9 = test8;
    CIEC_ANY_DERIVED_VARIANT test7 = test9;
    auto test7Value = static_cast<CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2> &>(test7.unwrap());
    BOOST_TEST(1 == static_cast<CIEC_DINT::TValueType>(test7Value[0]));
    BOOST_TEST(2 == static_cast<CIEC_DINT::TValueType>(test7Value[1]));
    BOOST_TEST(4 == static_cast<CIEC_DINT::TValueType>(test7Value[2]));

    // assign arrays to each other
    CIEC_ANY_DERIVED_VARIANT test10 = test6;
    auto test10Value = static_cast<CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2> &>(test10.unwrap());
    BOOST_TEST(17 == static_cast<CIEC_DINT::TValueType>(test10Value[0]));
    BOOST_TEST(4 == static_cast<CIEC_DINT::TValueType>(test10Value[1]));
    BOOST_TEST(21 == static_cast<CIEC_DINT::TValueType>(test10Value[2]));
  }

  BOOST_AUTO_TEST_CASE(Assignment_test) {
    BOOST_TEST_CHECKPOINT("Assignment: construct test1");
    CIEC_ANY_DERIVED_VARIANT test1;
    CHECK_DERIVED_STATE(test1, 1, "Assignment: test1 default constructed");
    BOOST_TEST_CHECKPOINT("Assignment: construct test2");
    CIEC_ANY_DERIVED_VARIANT test2;
    CHECK_DERIVED_STATE(test2, 1, "Assignment: test2 default constructed");

    // initial value must be ANY_STRUCT
    BOOST_TEST(CIEC_ANY::EDataTypeID::e_STRUCT == test1.unwrap().getDataTypeID());
    BOOST_TEST(CIEC_ANY::EDataTypeID::e_STRUCT == test2.unwrap().getDataTypeID());
    BOOST_TEST("ANY_STRUCT" == static_cast<CIEC_STRUCT &>(test1.unwrap()).getStructTypeNameID().get());
    BOOST_TEST("ANY_STRUCT" == static_cast<CIEC_STRUCT &>(test2.unwrap()).getStructTypeNameID().get());

    // assign struct value
    BOOST_TEST_CHECKPOINT("Assignment: assign struct to test1");
    test1 = CIEC_AnyDerivedTestStruct("test"_STRING, true_BOOL, 17_INT);
    CHECK_DERIVED_STATE(test1, 1, "Assignment: test1 after struct assignment");
    auto test1Value = static_cast<CIEC_AnyDerivedTestStruct &>(test1.unwrap());
    BOOST_TEST("test" == static_cast<CIEC_STRING::TValueType>(test1Value.Var1));
    BOOST_TEST(true == static_cast<CIEC_BOOL::TValueType>(test1Value.Var2));
    BOOST_TEST(17 == static_cast<CIEC_INT::TValueType>(test1Value.Var3));

    // assign generic struct value
    CIEC_AnyDerivedTestStruct test3 = CIEC_AnyDerivedTestStruct("other"_STRING, false_BOOL, 4_INT);
    CIEC_ANY &test4 = test3;
    BOOST_TEST_CHECKPOINT("Assignment: assign generic struct to test2");
    test2 = test4;
    CHECK_DERIVED_STATE(test2, 1, "Assignment: test2 after generic struct assignment");
    auto test2Value = static_cast<CIEC_AnyDerivedTestStruct &>(test2.unwrap());
    BOOST_TEST("other" == static_cast<CIEC_STRING::TValueType>(test2Value.Var1));
    BOOST_TEST(false == static_cast<CIEC_BOOL::TValueType>(test2Value.Var2));
    BOOST_TEST(4 == static_cast<CIEC_INT::TValueType>(test2Value.Var3));

    // assign each other
    CHECK_DERIVED_STATE(test1, 1, "Assignment: struct source before setValue");
    CHECK_DERIVED_STATE(test2, 1, "Assignment: struct destination before setValue");
    BOOST_TEST_CHECKPOINT("Assignment: set test2 from test1 struct");
    test2.setValue(test1);
    CHECK_DERIVED_STATE(test2, 1, "Assignment: test2 after struct setValue");
    test2Value = static_cast<CIEC_AnyDerivedTestStruct &>(test2.unwrap());
    BOOST_TEST("test" == static_cast<CIEC_STRING::TValueType>(test2Value.Var1));
    BOOST_TEST(true == static_cast<CIEC_BOOL::TValueType>(test2Value.Var2));
    BOOST_TEST(17 == static_cast<CIEC_INT::TValueType>(test2Value.Var3));

    // assign dynamic value
    BOOST_TEST_CHECKPOINT("Assignment: set test2 from temporary struct");
    test2.setValue(CIEC_AnyDerivedTestStruct("other"_STRING, false_BOOL, 4_INT));
    CHECK_DERIVED_STATE(test2, 1, "Assignment: test2 after temporary struct setValue");
    test2Value = static_cast<CIEC_AnyDerivedTestStruct &>(test2.unwrap());
    BOOST_TEST("other" == static_cast<CIEC_STRING::TValueType>(test2Value.Var1));
    BOOST_TEST(false == static_cast<CIEC_BOOL::TValueType>(test2Value.Var2));
    BOOST_TEST(4 == static_cast<CIEC_INT::TValueType>(test2Value.Var3));

    // assign array value
    BOOST_TEST_CHECKPOINT("Assignment: assign array to test1");
    test1 = CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2>{17_DINT, 4_DINT, 21_DINT};
    CHECK_DERIVED_STATE(test1, 0, "Assignment: test1 after array assignment");
    auto test1ArrayValue = static_cast<CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2> &>(test1.unwrap());
    BOOST_TEST(17 == static_cast<CIEC_DINT::TValueType>(test1ArrayValue[0]));
    BOOST_TEST(4 == static_cast<CIEC_DINT::TValueType>(test1ArrayValue[1]));
    BOOST_TEST(21 == static_cast<CIEC_DINT::TValueType>(test1ArrayValue[2]));

    // assign generic array value
    CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2> test5 = CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2>{1_DINT, 2_DINT, 4_DINT};
    CIEC_ANY &test6 = test5;
    BOOST_TEST_CHECKPOINT("Assignment: assign generic array to test2");
    test2 = test6;
    CHECK_DERIVED_STATE(test2, 0, "Assignment: test2 after generic array assignment");
    auto test2ArrayValue = static_cast<CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2> &>(test2.unwrap());
    BOOST_TEST(1 == static_cast<CIEC_DINT::TValueType>(test2ArrayValue[0]));
    BOOST_TEST(2 == static_cast<CIEC_DINT::TValueType>(test2ArrayValue[1]));
    BOOST_TEST(4 == static_cast<CIEC_DINT::TValueType>(test2ArrayValue[2]));

    // assign each other
    CHECK_DERIVED_STATE(test1, 0, "Assignment: array source before setValue");
    CHECK_DERIVED_STATE(test2, 0, "Assignment: array destination before setValue");
    BOOST_TEST_CHECKPOINT("Assignment: set test2 from test1 array");
    test2.setValue(test1);
    CHECK_DERIVED_STATE(test2, 0, "Assignment: test2 after array setValue");
    test2ArrayValue = static_cast<CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2> &>(test2.unwrap());
    BOOST_TEST(17 == static_cast<CIEC_DINT::TValueType>(test2ArrayValue[0]));
    BOOST_TEST(4 == static_cast<CIEC_DINT::TValueType>(test2ArrayValue[1]));
    BOOST_TEST(21 == static_cast<CIEC_DINT::TValueType>(test2ArrayValue[2]));

    // assign dynamic value
    BOOST_TEST_CHECKPOINT("Assignment: set test2 from temporary array");
    test2.setValue(CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2>{1_DINT, 2_DINT, 4_DINT});
    CHECK_DERIVED_STATE(test2, 0, "Assignment: test2 after temporary array setValue");
    test2ArrayValue = static_cast<CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2> &>(test2.unwrap());
    BOOST_TEST(1 == static_cast<CIEC_DINT::TValueType>(test2ArrayValue[0]));
    BOOST_TEST(2 == static_cast<CIEC_DINT::TValueType>(test2ArrayValue[1]));
    BOOST_TEST(4 == static_cast<CIEC_DINT::TValueType>(test2ArrayValue[2]));
  }

  void
  checkStringConversion(CIEC_ANY_DERIVED_VARIANT &paValue, const char *paString, CIEC_ANY::EDataTypeID paDataTypeId) {
    std::string buf;
    BOOST_CHECK_EQUAL(paValue.fromString(paString), strlen(paString));
    BOOST_CHECK_EQUAL(paValue.getDataTypeID(), CIEC_ANY::e_ANY);
    BOOST_CHECK_EQUAL(paValue.unwrap().getDataTypeID(), paDataTypeId);
    paValue.toString(buf);
    BOOST_CHECK_EQUAL(paString, buf);
  }

  BOOST_AUTO_TEST_CASE(String_Conversion_test) {
    CIEC_ANY_DERIVED_VARIANT test;
    checkStringConversion(test, "AnyDerivedTestStruct#(Var1:='test',Var2:=TRUE,Var3:=17)", CIEC_ANY::e_STRUCT);
  }

  BOOST_AUTO_TEST_CASE(Equality_test) {
    BOOST_TEST_CHECKPOINT("Equality: construct test1");
    CIEC_ANY_DERIVED_VARIANT test1;
    CHECK_DERIVED_STATE(test1, 1, "Equality: test1 default constructed");
    BOOST_TEST_CHECKPOINT("Equality: construct test2");
    CIEC_ANY_DERIVED_VARIANT test2;
    CHECK_DERIVED_STATE(test2, 1, "Equality: test2 default constructed");

    BOOST_TEST_CHECKPOINT("Equality: compare default values");
    BOOST_TEST(test1.equals(test2));
    BOOST_TEST_CHECKPOINT("Equality: assign struct to test1");
    test1 = CIEC_AnyDerivedTestStruct("test"_STRING, true_BOOL, 17_INT);
    CHECK_DERIVED_STATE(test1, 1, "Equality: test1 after struct assignment");
    CHECK_DERIVED_STATE(test2, 1, "Equality: test2 before unequal struct comparison");
    BOOST_TEST_CHECKPOINT("Equality: compare different structs");
    BOOST_TEST(!test1.equals(test2));
    BOOST_TEST_CHECKPOINT("Equality: set test2 from test1 struct");
    test2.setValue(test1);
    CHECK_DERIVED_STATE(test2, 1, "Equality: test2 after struct setValue");
    BOOST_TEST_CHECKPOINT("Equality: compare equal structs");
    BOOST_TEST(test1.equals(test2));
    BOOST_TEST_CHECKPOINT("Equality: set test2 from temporary struct");
    test2.setValue(CIEC_AnyDerivedTestStruct("other"_STRING, false_BOOL, 4_INT));
    CHECK_DERIVED_STATE(test2, 1, "Equality: test2 after temporary struct setValue");
    BOOST_TEST_CHECKPOINT("Equality: compare changed structs");
    BOOST_TEST(!test1.equals(test2));

    BOOST_TEST_CHECKPOINT("Equality: assign array to test1");
    test1 = CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2>{17_DINT, 4_DINT, 21_DINT};
    CHECK_DERIVED_STATE(test1, 0, "Equality: test1 after array assignment");
    CHECK_DERIVED_STATE(test2, 1, "Equality: test2 before array-struct comparison");
    BOOST_TEST_CHECKPOINT("Equality: compare array and struct");
    BOOST_TEST(!test1.equals(test2));
    BOOST_TEST_CHECKPOINT("Equality: set test2 from test1 array");
    test2.setValue(test1);
    CHECK_DERIVED_STATE(test2, 0, "Equality: test2 after array setValue");
    BOOST_TEST_CHECKPOINT("Equality: compare equal arrays");
    BOOST_TEST(test1.equals(test2));
    BOOST_TEST_CHECKPOINT("Equality: set test2 from temporary array");
    test2.setValue(CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2>{1_DINT, 2_DINT, 4_DINT});
    CHECK_DERIVED_STATE(test2, 0, "Equality: test2 after temporary array setValue");
    BOOST_TEST_CHECKPOINT("Equality: compare different arrays");
    BOOST_TEST(!test1.equals(test2));
  }

  BOOST_AUTO_TEST_CASE(Array_to_default_struct_reset_test) {
    BOOST_TEST_CHECKPOINT("Reset: construct destination from array");
    CIEC_ANY_DERIVED_VARIANT destination = CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2>{17_DINT, 4_DINT, 21_DINT};
    CHECK_DERIVED_STATE(destination, 0, "Reset: array destination before assignment");
    BOOST_TEST_CHECKPOINT("Reset: construct default source");
    const CIEC_ANY_DERIVED_VARIANT source;
    CHECK_DERIVED_STATE(source, 1, "Reset: default source before assignment");
    BOOST_TEST_CHECKPOINT("Reset: copy default source to array destination");
    destination = source;
    CHECK_DERIVED_STATE(destination, 1, "Reset: destination after copy assignment");
    CHECK_DERIVED_STATE(source, 1, "Reset: source after copy assignment");
    BOOST_TEST(destination.unwrap().getDataTypeID() == CIEC_ANY::e_STRUCT);

    BOOST_TEST_CHECKPOINT("Reset: restore array destination");
    destination = CIEC_ARRAY_FIXED<CIEC_DINT, 0, 2>{17_DINT, 4_DINT, 21_DINT};
    CHECK_DERIVED_STATE(destination, 0, "Reset: array destination before temporary assignment");
    BOOST_TEST_CHECKPOINT("Reset: assign default temporary to array destination");
    destination = CIEC_ANY_DERIVED_VARIANT();
    CHECK_DERIVED_STATE(destination, 1, "Reset: destination after temporary assignment");
    BOOST_TEST(destination.unwrap().getDataTypeID() == CIEC_ANY::e_STRUCT);
  }

  BOOST_AUTO_TEST_CASE(State_diagnostic_test) {
    CIEC_ANY_DERIVED_VARIANT value;
    CHECK_DERIVED_STATE(value, 1, "State diagnostic: struct");
    BOOST_TEST(!variantHasState(value, 0));
    static_cast<TIecAnyDerivedVariantType &>(value).emplace<0>();
    BOOST_TEST(value.index() == 0U);
    BOOST_TEST(!value.valueless_by_exception());
    BOOST_TEST(!variantHasState(value, 0));
    BOOST_TEST(variantPointee(value) == nullptr);
  }

  BOOST_AUTO_TEST_SUITE_END()
#undef CHECK_DERIVED_STATE
} // namespace forte::test
