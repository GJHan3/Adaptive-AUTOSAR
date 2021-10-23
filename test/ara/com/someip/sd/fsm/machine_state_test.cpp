#include <gtest/gtest.h>
#include "../../../../../../src/ara/com/someip/sd/fsm/notready_state.h"
#include "../../../../../../src/ara/com/someip/sd/fsm/initial_wait_state.h"
#include "../../../../../../src/ara/com/someip/sd/fsm/repetition_state.h"
#include "../../../../../../src/ara/com/someip/sd/fsm/main_state.h"
#include "../../../../../../src/ara/com/someip/sd/fsm/stopped_state.h"
#include "../../../../../../src/ara/com/someip/sd/fsm/service_notseen_state.h"
#include "../../../../../../src/ara/com/someip/sd/fsm/service_ready_state.h"
#include "../../../../../../src/ara/com/someip/sd/fsm/service_seen_state.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                namespace fsm
                {
                    TEST(MachineStateTest, NotReadyStateContructor)
                    {
                        const helper::SdServerState cExpectedState =
                            helper::SdServerState::NotReady;

                        NotReadyState _machineState(nullptr);
                        helper::SdServerState _actualState = _machineState.GetState();

                        EXPECT_EQ(_actualState, cExpectedState);
                    }

                    TEST(MachineStateTest, InitialWaitStateContructor)
                    {
                        const helper::SdServerState cPreviousState =
                            helper::SdServerState::NotReady;
                        const helper::SdServerState cExpectedState =
                            helper::SdServerState::InitialWaitPhase;
                        const helper::SdServerState cNextState =
                            helper::SdServerState::RepetitionPhase;
                        const helper::SdServerState cStoppedState =
                            helper::SdServerState::NotReady;
                        const int cInitialDelayMin = 100;
                        const int cInitialDelayMax = 200;
                        const auto cOnTimerExpired = []
                        {
                            // Empty callback
                        };

                        InitialWaitState<helper::SdServerState> _machineState(
                            cExpectedState,
                            cNextState,
                            cStoppedState,
                            cOnTimerExpired,
                            cInitialDelayMin,
                            cInitialDelayMax);

                        helper::SdServerState _actualState = _machineState.GetState();

                        EXPECT_EQ(_actualState, cExpectedState);

                        EXPECT_NO_THROW(_machineState.Activate(cPreviousState));
                        // The second immediate activation should throw
                        EXPECT_THROW(_machineState.Activate(cPreviousState), std::logic_error);
                        // Finish the test gracefully
                        _machineState.Join();
                    }

                    TEST(MachineStateTest, RepetitionStateConstructor)
                    {
                        const helper::SdServerState cPreviousState =
                            helper::SdServerState::InitialWaitPhase;
                        const helper::SdServerState cExpectedState =
                            helper::SdServerState::RepetitionPhase;
                        const helper::SdServerState cNextState =
                            helper::SdServerState::MainPhase;
                        const helper::SdServerState cStoppedState =
                            helper::SdServerState::NotReady;
                        const int cRepetitionsMax = 2;
                        const int cRepetitionsBaseDelay = 100;
                        uint32_t _counter = 0;
                        const auto cOnTimerExpired = [&_counter]()
                        {
                            ++_counter;
                        };

                        RepetitionState<helper::SdServerState> _machineState(
                            cExpectedState,
                            cNextState,
                            cStoppedState,
                            cOnTimerExpired,
                            cRepetitionsMax,
                            cRepetitionsBaseDelay);

                        helper::SdServerState _actualState = _machineState.GetState();

                        EXPECT_EQ(_actualState, cExpectedState);

                        EXPECT_NO_THROW(_machineState.Activate(cPreviousState));
                        // The second immediate activation should throw
                        EXPECT_THROW(_machineState.Activate(cPreviousState), std::logic_error);
                        // Due to async invocation, the counter value should be less than the maximum repetitions.
                        EXPECT_LT(_counter, cRepetitionsMax);
                        // Finish the test gracefully
                        _machineState.Join();
                    }

                    TEST(MachineStateTest, MainStateConstructor)
                    {
                        const helper::SdServerState cPreviousState =
                            helper::SdServerState::RepetitionPhase;
                        const helper::SdServerState cExpectedState =
                            helper::SdServerState::MainPhase;
                        const int cCyclicOfferDelay = 100;
                        const auto cOnTimerExpired = []()
                        {
                            // Empty callback
                        };

                        MainState _machineState(cOnTimerExpired, cCyclicOfferDelay);

                        helper::SdServerState _actualState = _machineState.GetState();

                        EXPECT_EQ(_actualState, cExpectedState);

                        EXPECT_NO_THROW(_machineState.Activate(cPreviousState));
                        // The second immediate activation should throw
                        EXPECT_THROW(_machineState.Activate(cPreviousState), std::logic_error);
                        // Stop the service, otherwise FSM never get out of the main phase
                        _machineState.ServiceStopped();
                        // Finish the test gracefully
                        _machineState.Join();
                    }

                    TEST(MachineStateTest, StoppedStateConstructor)
                    {
                        const helper::SdClientState cExpectedState =
                            helper::SdClientState::Stopped;

                        helper::TtlTimer _ttlTimer;
                        StoppedState _machineState(&_ttlTimer);
                        helper::SdClientState _actualState = _machineState.GetState();

                        EXPECT_EQ(_actualState, cExpectedState);
                    }

                    TEST(MachineStateTest, ServiceNotseenStateConstructor)
                    {
                        const helper::SdClientState cExpectedState =
                            helper::SdClientState::ServiceNotSeen;

                        helper::TtlTimer _ttlTimer;
                        ServiceNotseenState _machineState(&_ttlTimer);
                        helper::SdClientState _actualState = _machineState.GetState();

                        EXPECT_EQ(_actualState, cExpectedState);
                    }

                    TEST(MachineStateTest, ServiceReadyStateConstructor)
                    {
                        const helper::SdClientState cExpectedState =
                            helper::SdClientState::ServiceReady;

                        helper::TtlTimer _ttlTimer;
                        ServiceReadyState _machineState(&_ttlTimer);
                        helper::SdClientState _actualState = _machineState.GetState();

                        EXPECT_EQ(_actualState, cExpectedState);
                    }

                    TEST(MachineStateTest, ServiceSeenStateConstructor)
                    {
                        const helper::SdClientState cExpectedState =
                            helper::SdClientState::ServiceSeen;

                        helper::TtlTimer _ttlTimer;
                        ServiceSeenState _machineState(&_ttlTimer);
                        helper::SdClientState _actualState = _machineState.GetState();

                        EXPECT_EQ(_actualState, cExpectedState);
                    }
                }
            }
        }
    }
}