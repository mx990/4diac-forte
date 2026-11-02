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
#pragma once

#include <variant>

#include "forte/datatypes/forte_any.h"
#include "forte/datatypes/forte_any_struct.h"
#include "forte/datatypes/forte_any_unique_ptr.h"
#include "forte/datatypes/forte_array_common.h"
#include "forte/datatypes/forte_struct.h"

namespace forte {
  using TIecAnyDerivedVariantType = std::variant<
      // ANY_DERIVED
      CIEC_ANY_UNIQUE_PTR<CIEC_ARRAY>,
      CIEC_ANY_UNIQUE_PTR<CIEC_STRUCT>
      // end
      >;

  class CIEC_ANY_DERIVED_VARIANT : public CIEC_ANY, public TIecAnyDerivedVariantType {
      DECLARE_FIRMWARE_DATATYPE(ANY_DERIVED_VARIANT)
    public:
      using TIecAnyDerivedVariantType::variant;
      using TIecAnyDerivedVariantType::operator=;
      template<class>
      static inline constexpr bool always_false_v = false;

      CIEC_ANY_DERIVED_VARIANT() : variant(std::make_unique<CIEC_ANY_STRUCT>()) {
        traceState("default-constructed", -1, index() != 1);
      }

      CIEC_ANY_DERIVED_VARIANT(const CIEC_ANY_DERIVED_VARIANT &paVal) : variant(traceCopySource(paVal)) {
        traceState("copy-constructed");
      }

      CIEC_ANY_DERIVED_VARIANT(const CIEC_ANY &paVal) {
        traceState("any-construct-before-set", static_cast<int>(paVal.getDataTypeID()));
        setValue(paVal.unwrap());
        traceState("any-constructed", static_cast<int>(paVal.getDataTypeID()));
      }

      CIEC_ANY_DERIVED_VARIANT &operator=(const CIEC_ANY_DERIVED_VARIANT &paOther) {
        const bool abnormal = hasInvalidState() || paOther.hasInvalidState();
        traceState("copy-assign-dst-before", -1, abnormal);
        paOther.traceState("copy-assign-src-before", -1, abnormal);
        variant::operator=(paOther);
        traceState("copy-assign-dst-after", -1, abnormal);
        return *this;
      }

      CIEC_ANY_DERIVED_VARIANT &operator=(const CIEC_ANY &paOther) {
        setValue(paOther.unwrap());
        return *this;
      }

      void setValue(const CIEC_ANY &paValue) override;

      void reset() override {
        unwrap().reset();
      }

      [[nodiscard]] CIEC_ANY &unwrap() override;

      [[nodiscard]] const CIEC_ANY &unwrap() const override;

      int fromString(const char *paValue) override;
      void toString(std::string &paTargetBuf) const override;

      [[nodiscard]] bool equals(const CIEC_ANY &paOther) const override {
        return unwrap().equals(paOther.unwrap());
      }

    private:
      // Temporary CI diagnostics; state inspection never dereferences an alternative.
      [[nodiscard]] bool hasInvalidState() const;
      void traceState(const char *paOperation, int paInputType = -1, bool paForce = false) const;
      static const CIEC_ANY_DERIVED_VARIANT &traceCopySource(const CIEC_ANY_DERIVED_VARIANT &paSource) {
        paSource.traceState("copy-construct-src-before");
        return paSource;
      }
  };

  static_assert(std::is_copy_constructible_v<CIEC_ANY_DERIVED_VARIANT>);
  static_assert(std::is_move_constructible_v<CIEC_ANY_DERIVED_VARIANT>);
  static_assert(std::is_constructible_v<CIEC_ANY_DERIVED_VARIANT, const CIEC_ANY &>);
  static_assert(std::is_copy_assignable_v<CIEC_ANY_DERIVED_VARIANT>);
  static_assert(std::is_assignable_v<CIEC_ANY_DERIVED_VARIANT, const CIEC_ANY &>);
  static_assert(std::is_destructible_v<CIEC_ANY_DERIVED_VARIANT>);

  template<>
  struct is_generic_datatype<CIEC_ANY_DERIVED_VARIANT> : std::true_type {};
} // namespace forte
