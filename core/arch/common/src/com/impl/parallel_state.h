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
 *    Martin Erich Jobst - initial implementation
 *******************************************************************************/

#pragma once

#include <concepts>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

#include "forte/com/common.h"

namespace forte::com::impl {
  /**
   * \brief ParallelConnectionCandidate represents a candidate for a parallel connection.
   *
   * This concept defines the requirements for a type to be used as a parallel connection candidate.
   * It requires the type to be movable and provide certain operations.
   * Candidates own their resources and release them when destroyed.
   */
  template<typename Candidate>
  concept ParallelConnectionCandidate = std::movable<Candidate> && requires(Candidate &paCandidate) {
    { static_cast<bool>(paCandidate) } -> std::same_as<bool>;
    paCandidate.reset();
    paCandidate.release();
  };

  /**
   * \brief ParallelConnectionState represents the state of a parallel connection.
   *
   * This class manages a collection of parallel connection candidates and tracks the winner.
   * It provides methods to add candidates, check if a winner has been determined, and retrieve the winner.
   */
  template<ParallelConnectionCandidate Candidate>
  class ParallelConnectionState {
    public:
      ParallelConnectionState() = default;
      ParallelConnectionState(ParallelConnectionState &) = delete;
      ParallelConnectionState(ParallelConnectionState &&) = default;
      ParallelConnectionState &operator=(ParallelConnectionState &) = delete;
      ParallelConnectionState &operator=(ParallelConnectionState &&) = default;

      std::size_t addCandidate(ComResultValue<Candidate> paCandidate) {
        if (paCandidate.mResult == ComResult::Ok) {
          mWinner = mCandidates.size();
        } else if (paCandidate.mResult != ComResult::InProgress) {
          updateError(paCandidate.mResult);
        }
        mCandidates.emplace_back(std::move(paCandidate.mValue));
        return mCandidates.size() - 1;
      }

      void connected(const std::size_t paIndex) {
        if (paIndex < mCandidates.size() && mCandidates[paIndex] && !hasWinner()) {
          mWinner = paIndex;
        }
      }

      void failed(const std::size_t paIndex, const ComResult paError) {
        if (paIndex < mCandidates.size() && mCandidates[paIndex] && mWinner != paIndex) {
          updateError(paError);
          mCandidates[paIndex].reset();
        }
      }

      [[nodiscard]] ComResult result() const {
        if (hasWinner()) {
          return ComResult::Ok;
        }
        if (hasPendingCandidates()) {
          return ComResult::InProgress;
        }
        return mError.value_or(ComResult::InvalidId);
      }

      [[nodiscard]] bool hasPendingCandidates() const {
        for (const Candidate &candidate : mCandidates) {
          if (candidate) {
            return true;
          }
        }
        return false;
      }

      [[nodiscard]] bool hasWinner() const {
        return mWinner.has_value();
      }

      [[nodiscard]] Candidate takeWinner() {
        const std::size_t winnerIndex = mWinner.value();
        Candidate winner = std::move(mCandidates[winnerIndex]);
        mCandidates.clear();
        mWinner.reset();
        mError.reset();
        return winner;
      }

      [[nodiscard]] std::size_t candidateCount() const {
        return mCandidates.size();
      }

      [[nodiscard]] Candidate &candidateAt(const std::size_t paIndex) {
        return mCandidates[paIndex];
      }

      [[nodiscard]] const Candidate &candidateAt(const std::size_t paIndex) const {
        return mCandidates[paIndex];
      }

      void clear() {
        for (Candidate &candidate : mCandidates) {
          if (candidate) {
            candidate.reset();
          }
        }
        mCandidates.clear();
        mWinner.reset();
        mError.reset();
      }

    private:
      void updateError(const ComResult paError) {
        if (!mError && paError != ComResult::Ok && paError != ComResult::Unknown) {
          mError = paError;
        }
      }

      std::vector<Candidate> mCandidates;
      std::optional<std::size_t> mWinner;
      std::optional<ComResult> mError;
  };
} // namespace forte::com::impl
