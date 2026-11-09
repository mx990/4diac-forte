/*******************************************************************************
 * Copyright (c) 2025 Martin Erich Jobst
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

#include "forte/com/common.h"

#include <optional>

namespace forte::com {

  template<typename T>
  class ComChannel;

  /**
   * \brief An observer for a channel
   * \note Implementations must not delete the passed channel.
   */
  template<typename T>
  class ComObserver {
    public:
      /**
       * \brief Receive data from a channel
       * \param paData The received data
       * \param paChannel The channel
       * \return The result of receiving the data
       * \retval Ok The data has been successfully received
       * \retval More Retry once more data is available
       * \note This is called only during ComChannel::poll().
       */
      virtual ComResult receive(T paData, ComChannel<T> &paChannel) = 0;

      /**
       * \brief Data was sent via the channel
       * \param paResult The result of sending the data
       * \param paChannel The channel
       * \note This completes a prior send operation that returned ComResult::Async
       * and is called only during ComChannel::poll().
       */
      virtual void sent(ComResult paResult, ComChannel<T> &paChannel) = 0;

      /**
       * \brief Channel was initialized asynchronously
       * \param paResult The result of initializing the channel
       * \param paChannel The channel
       * \note This completes a prior open operation that returned ComResult::Async
       * and is called only during ComChannel::poll().
       */
      virtual void init(ComResult paResult, ComChannel<T> &paChannel) = 0;

      /**
       * \brief Get the receive buffer
       * \return The receive buffer
       */
      virtual std::optional<T> getReceiveBuffer() {
        return {};
      }

      /**
       * \brief Get the send buffer
       * \return The send buffer
       */
      virtual std::optional<T> getSendBuffer() {
        return {};
      }

      virtual ~ComObserver() = default;

    protected:
      ComObserver() = default;
  };
} // namespace forte::com
