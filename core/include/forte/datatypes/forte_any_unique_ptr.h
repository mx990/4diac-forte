/*******************************************************************************
 * Copyright (c) 2023 Martin Erich Jobst
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *    Martin Erich Jobst - initial implementation
 *******************************************************************************/
#pragma once

#include <cstdio>
#include <memory>

#include "forte/datatypes/forte_any.h"

namespace forte {
  template<typename T>
  class CIEC_ANY_UNIQUE_PTR : public std::unique_ptr<T> {
    public:
      using std::unique_ptr<T>::unique_ptr;
      using std::unique_ptr<T>::operator=;

      CIEC_ANY_UNIQUE_PTR(const CIEC_ANY_UNIQUE_PTR &paOther) :
          std::unique_ptr<T>(static_cast<T *>(reportNullSource("copy-construct", nullptr, paOther)->clone(nullptr))) {
      }

      CIEC_ANY_UNIQUE_PTR &operator=(const CIEC_ANY_UNIQUE_PTR &paOther) {
        reportNullSource("copy-assign", this, paOther);
        std::unique_ptr<T>::operator=(std::unique_ptr<T>(static_cast<T *>(paOther->clone(nullptr))));
        return *this;
      }

      ~CIEC_ANY_UNIQUE_PTR() = default;

    private:
      // Temporary CI diagnostic: do not change the original null-copy behavior.
      static const CIEC_ANY_UNIQUE_PTR &reportNullSource(const char *paOperation,
                                                         const CIEC_ANY_UNIQUE_PTR *paDestination,
                                                         const CIEC_ANY_UNIQUE_PTR &paSource) {
        if (!paSource.get()) {
          std::fprintf(stderr, "[derived-variant-diagnostic] pointer-%s dst=%p dst-pointee=%p src=%p src-pointee=%p\n",
                       paOperation, static_cast<const void *>(paDestination),
                       static_cast<const void *>(paDestination ? paDestination->get() : nullptr),
                       static_cast<const void *>(&paSource), static_cast<const void *>(paSource.get()));
          std::fflush(stderr);
        }
        return paSource;
      }
  };

  static_assert(std::is_copy_constructible_v<CIEC_ANY_UNIQUE_PTR<CIEC_ANY>>);
  static_assert(std::is_move_constructible_v<CIEC_ANY_UNIQUE_PTR<CIEC_ANY>>);
  static_assert(std::is_copy_assignable_v<CIEC_ANY_UNIQUE_PTR<CIEC_ANY>>);
  static_assert(std::is_move_assignable_v<CIEC_ANY_UNIQUE_PTR<CIEC_ANY>>);
} // namespace forte
