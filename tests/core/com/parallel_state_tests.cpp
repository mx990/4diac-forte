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

#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <memory>
#include <utility>

#include "forte_boost_output_support.h"
#include "../../../core/arch/common/src/com/impl/parallel_state.h"

namespace forte::com::test {
  namespace {
    struct MockCandidateState {
        bool mActive{true};
        std::size_t mResetCount{};
        std::size_t mReleaseCount{};
    };

    struct MockCandidate {
        std::shared_ptr<MockCandidateState> mState{std::make_shared<MockCandidateState>()};

        MockCandidate() = default;

        explicit MockCandidate(std::shared_ptr<MockCandidateState> paState) : mState(std::move(paState)) {
        }

        MockCandidate(const MockCandidate &) = delete;
        MockCandidate &operator=(const MockCandidate &) = delete;
        MockCandidate(MockCandidate &&paOther) noexcept = default;
        MockCandidate &operator=(MockCandidate &&paOther) noexcept = default;

        ~MockCandidate() {
          reset();
        }

        [[nodiscard]] explicit operator bool() const {
          return mState && mState->mActive;
        }

        void reset() {
          if (mState && mState->mActive) {
            mState->mActive = false;
            ++mState->mResetCount;
          }
        }

        [[nodiscard]] int release() {
          if (!mState || !mState->mActive) {
            return -1;
          }
          mState->mActive = false;
          ++mState->mReleaseCount;
          return 1;
        }
    };

    using MockParallelConnectionState = impl::ParallelConnectionState<MockCandidate>;
  } // namespace

  BOOST_AUTO_TEST_SUITE(PARALLEL_STATE)

  BOOST_AUTO_TEST_CASE(EMPTY_AND_PENDING) {
    MockParallelConnectionState state;

    BOOST_TEST(state.result() == ComResult::InvalidId);
    BOOST_TEST(!state.hasPendingCandidates());
    BOOST_TEST(!state.hasWinner());

    const std::size_t first = state.addCandidate({.mResult = ComResult::InProgress});
    const std::size_t second = state.addCandidate({.mResult = ComResult::InProgress});

    BOOST_TEST(second == first + 1);
    BOOST_TEST(state.result() == ComResult::InProgress);
    BOOST_TEST(state.hasPendingCandidates());
    BOOST_TEST(state.candidateCount() == 2);

    state.failed(first, ComResult::Unknown);
    BOOST_TEST(!state.candidateAt(first));
    BOOST_TEST(state.candidateAt(first).mState->mResetCount == 1);
    BOOST_TEST(state.result() == ComResult::InProgress);
    BOOST_TEST(state.candidateCount() == 2);

    state.failed(first, ComResult::Unknown);
    BOOST_TEST(state.candidateAt(first).mState->mResetCount == 1);
  }

  BOOST_AUTO_TEST_CASE(FIRST_SUCCESS_TAKES_WINNER_AND_CLOSES_LOSERS) {
    MockParallelConnectionState state;
    const auto loserState = std::make_shared<MockCandidateState>();
    const auto winnerState = std::make_shared<MockCandidateState>();
    const auto first = state.addCandidate({.mResult = ComResult::InProgress, .mValue = MockCandidate{loserState}});
    const auto second = state.addCandidate({.mResult = ComResult::InProgress, .mValue = MockCandidate{winnerState}});

    state.connected(second);
    state.connected(first);
    state.connected(second);

    BOOST_TEST(state.result() == ComResult::Ok);
    BOOST_TEST(state.hasWinner());
    BOOST_TEST(state.hasPendingCandidates());

    const MockCandidate winner = state.takeWinner();
    BOOST_TEST(static_cast<bool>(winner));
    BOOST_TEST(state.candidateCount() == 0);
    BOOST_TEST(!state.hasWinner());
    BOOST_TEST(state.result() == ComResult::InvalidId);
    BOOST_TEST(loserState->mResetCount == 1);
    BOOST_TEST(winnerState->mResetCount == 0);
  }

  BOOST_AUTO_TEST_CASE(TAKEN_WINNER_CAN_BE_RELEASED) {
    MockParallelConnectionState state;
    const auto winnerState = std::make_shared<MockCandidateState>();
    const auto pendingState = std::make_shared<MockCandidateState>();
    const auto winner = state.addCandidate({.mResult = ComResult::InProgress, .mValue = MockCandidate{winnerState}});
    const auto pending = state.addCandidate({.mResult = ComResult::InProgress, .mValue = MockCandidate{pendingState}});

    BOOST_TEST(pending == winner + 1);
    state.connected(winner);
    MockCandidate takenWinner = state.takeWinner();
    BOOST_TEST(takenWinner.release() == 1);
    BOOST_TEST(winnerState->mReleaseCount == 1);

    state.clear();
    BOOST_TEST(state.candidateCount() == 0);
    BOOST_TEST(state.result() == ComResult::InvalidId);
    BOOST_TEST(winnerState->mReleaseCount == 1);
    BOOST_TEST(winnerState->mResetCount == 0);
    BOOST_TEST(pendingState->mResetCount == 1);
  }

  BOOST_AUTO_TEST_CASE(CLEAR_RESETS_CANDIDATES) {
    MockParallelConnectionState state;
    const auto winnerState = std::make_shared<MockCandidateState>();
    const auto pendingState = std::make_shared<MockCandidateState>();
    const auto winner = state.addCandidate({.mResult = ComResult::InProgress, .mValue = MockCandidate{winnerState}});
    const auto pending = state.addCandidate({.mResult = ComResult::InProgress, .mValue = MockCandidate{pendingState}});

    BOOST_TEST(pending == winner + 1);
    state.connected(winner);
    state.clear();

    BOOST_TEST(state.result() == ComResult::InvalidId);
    BOOST_TEST(state.candidateCount() == 0);
    BOOST_TEST(winnerState->mResetCount == 1);
    BOOST_TEST(pendingState->mResetCount == 1);
  }

  BOOST_AUTO_TEST_CASE(TOTAL_FAILURE) {
    MockParallelConnectionState state;
    const auto first = state.addCandidate({.mResult = ComResult::InProgress});
    const auto second = state.addCandidate({.mResult = ComResult::InProgress});

    state.failed(first, ComResult::ConnectionRefused);
    state.failed(second, ComResult::HostUnreachable);
    BOOST_TEST(state.result() == ComResult::ConnectionRefused);
    BOOST_TEST(!state.hasPendingCandidates());
    BOOST_TEST(state.candidateAt(first).mState->mResetCount == 1);
    BOOST_TEST(state.candidateAt(second).mState->mResetCount == 1);

    state.clear();
    BOOST_TEST(state.result() == ComResult::InvalidId);
  }

  BOOST_AUTO_TEST_CASE(IMMEDIATE_OPEN_FAILURE) {
    MockParallelConnectionState state;
    const auto failedState = std::make_shared<MockCandidateState>(false);
    state.addCandidate({.mResult = ComResult::ConnectionRefused, .mValue = MockCandidate{failedState}});

    BOOST_TEST(!state.hasPendingCandidates());
    BOOST_TEST(state.result() == ComResult::ConnectionRefused);
  }

  BOOST_AUTO_TEST_CASE(IMMEDIATE_OPEN_SUCCESS) {
    MockParallelConnectionState state;
    state.addCandidate({.mResult = ComResult::Ok});

    BOOST_TEST(state.result() == ComResult::Ok);
    BOOST_TEST(state.hasWinner());
    const MockCandidate winner = state.takeWinner();
    BOOST_TEST(static_cast<bool>(winner));
    BOOST_TEST(state.candidateCount() == 0);
  }

  BOOST_AUTO_TEST_CASE(IN_PROGRESS_DOES_NOT_REPLACE_RECORDED_ERROR) {
    MockParallelConnectionState state;
    const auto failedState = std::make_shared<MockCandidateState>(false);
    state.addCandidate({.mResult = ComResult::ConnectionRefused, .mValue = MockCandidate{failedState}});
    const auto pending = state.addCandidate({.mResult = ComResult::InProgress});

    BOOST_TEST(state.result() == ComResult::InProgress);
    state.failed(pending, ComResult::HostUnreachable);

    BOOST_TEST(state.result() == ComResult::ConnectionRefused);
    BOOST_TEST(!state.hasPendingCandidates());
  }

  BOOST_AUTO_TEST_CASE(INVALID_ID_REPLACES_UNKNOWN_ERROR) {
    MockParallelConnectionState state;
    const auto first = state.addCandidate({.mResult = ComResult::InProgress});
    const auto second = state.addCandidate({.mResult = ComResult::InProgress});
    const auto third = state.addCandidate({.mResult = ComResult::InProgress});

    state.failed(first, ComResult::Unknown);
    state.failed(second, ComResult::InvalidId);
    state.failed(third, ComResult::Unknown);

    BOOST_TEST(state.result() == ComResult::InvalidId);
  }

  BOOST_AUTO_TEST_SUITE_END()
} // namespace forte::com::test
