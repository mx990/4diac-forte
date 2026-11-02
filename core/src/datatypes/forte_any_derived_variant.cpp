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
#include "forte/datatypes/forte_any_derived_variant.h"

#include <atomic>
#include <cstdio>

using namespace forte::literals;

namespace forte {
  DEFINE_FIRMWARE_DATATYPE(ANY_DERIVED_VARIANT, "ANY_DERIVED"_STRID)

  bool CIEC_ANY_DERIVED_VARIANT::hasInvalidState() const {
    const auto &state = static_cast<const variant &>(*this);
    const auto *array = std::get_if<CIEC_ANY_UNIQUE_PTR<CIEC_ARRAY>>(&state);
    const auto *structure = std::get_if<CIEC_ANY_UNIQUE_PTR<CIEC_STRUCT>>(&state);
    return state.valueless_by_exception() || (array && !array->get()) || (structure && !structure->get());
  }

  void CIEC_ANY_DERIVED_VARIANT::traceState(const char *paOperation, int paInputType, bool paForce) const {
    const auto &state = static_cast<const variant &>(*this);
    const auto *array = std::get_if<CIEC_ANY_UNIQUE_PTR<CIEC_ARRAY>>(&state);
    const auto *structure = std::get_if<CIEC_ANY_UNIQUE_PTR<CIEC_STRUCT>>(&state);
    const void *pointee = array       ? static_cast<const void *>(array->get())
                          : structure ? static_cast<const void *>(structure->get())
                                      : nullptr;
    const bool abnormal = state.valueless_by_exception() || !pointee;
    static std::atomic<unsigned int> normalCount{0};
    if (paForce || abnormal || normalCount.fetch_add(1, std::memory_order_relaxed) < 128) {
      std::fprintf(stderr,
                   "[derived-variant-diagnostic] %s object=%p index=%zu valueless=%d pointee=%p input-type=%d\n",
                   paOperation, static_cast<const void *>(this), state.index(),
                   static_cast<int>(state.valueless_by_exception()), pointee, paInputType);
      std::fflush(stderr);
    }
  }

  void CIEC_ANY_DERIVED_VARIANT::setValue(const CIEC_ANY &paValue) {
    const auto inputType = paValue.getDataTypeID();
    traceState("set-value-before", static_cast<int>(inputType));
    switch (inputType) {
      case e_ANY: CIEC_ANY_DERIVED_VARIANT::setValue(paValue.unwrap()); break;
      case e_ARRAY:
        operator=(CIEC_ANY_UNIQUE_PTR<CIEC_ARRAY>(static_cast<CIEC_ARRAY *>(paValue.clone(nullptr))));
        break;
      case e_STRUCT:
        operator=(CIEC_ANY_UNIQUE_PTR<CIEC_STRUCT>(static_cast<CIEC_STRUCT *>(paValue.clone(nullptr))));
        break;
      default: break;
    }
    traceState("set-value-after", static_cast<int>(inputType));
  }

  CIEC_ANY &CIEC_ANY_DERIVED_VARIANT::unwrap() {
    if (hasInvalidState()) {
      traceState("unwrap-null");
    }
    return std::visit(
        [](auto &&value) -> CIEC_ANY & {
          using T = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<T, CIEC_ANY_UNIQUE_PTR<CIEC_ARRAY>>) {
            return *value;
          } else if constexpr (std::is_same_v<T, CIEC_ANY_UNIQUE_PTR<CIEC_STRUCT>>) {
            return *value;
          } else if constexpr (std::is_base_of_v<CIEC_ANY, T>) {
            return value;
          } else {
            static_assert(always_false_v<T>, "non-exhaustive visitor");
          }
        },
        static_cast<variant &>(*this));
  }

  const CIEC_ANY &CIEC_ANY_DERIVED_VARIANT::unwrap() const {
    if (hasInvalidState()) {
      traceState("unwrap-const-null");
    }
    return std::visit(
        [](auto &&value) -> const CIEC_ANY & {
          using T = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<T, CIEC_ANY_UNIQUE_PTR<CIEC_ARRAY>>) {
            return *value;
          } else if constexpr (std::is_same_v<T, CIEC_ANY_UNIQUE_PTR<CIEC_STRUCT>>) {
            return *value;
          } else if constexpr (std::is_base_of_v<CIEC_ANY, T>) {
            return value;
          } else {
            static_assert(always_false_v<T>, "non-exhaustive visitor");
          }
        },
        static_cast<const variant &>(*this));
  }

  int CIEC_ANY_DERIVED_VARIANT::fromString(const char *paValue) {
    int retVal = -1;
    const char *hashPos = strchr(paValue, '#');
    if (nullptr != hashPos) {
      StringId typeNameId = parseTypeName(paValue, hashPos);
      CIEC_ANY *value = createDataTypeInstance(typeNameId, nullptr);
      if (value) {
        retVal = value->fromString(hashPos + 1); // start after '#'
        if (retVal < 0) {
          return retVal;
        }
        retVal += static_cast<int>(hashPos - paValue + 1); // add count for type including '#'
        switch (value->getDataTypeID()) {
          case e_ARRAY: operator=(CIEC_ANY_UNIQUE_PTR<CIEC_ARRAY>(static_cast<CIEC_ARRAY *>(value))); break;
          case e_STRUCT: operator=(CIEC_ANY_UNIQUE_PTR<CIEC_STRUCT>(static_cast<CIEC_STRUCT *>(value))); break;
          default: setValue(*value); delete value;
        }
      }
    }
    return retVal;
  }

  void CIEC_ANY_DERIVED_VARIANT::toString(std::string &paTargetBuf) const {
    const CIEC_ANY &value = unwrap();
    paTargetBuf += value.getTypeNameID();
    paTargetBuf += '#';
    value.toString(paTargetBuf);
  }
} // namespace forte
