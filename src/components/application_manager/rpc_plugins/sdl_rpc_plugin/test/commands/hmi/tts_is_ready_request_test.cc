/*
 * Copyright (c) 2020, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "hmi/tts_is_ready_request.h"

#include "gtest/gtest.h"

#include "application_manager/commands/command_request_test.h"
#include "application_manager/event_engine/event.h"
#include "application_manager/hmi_interfaces.h"
#include "application_manager/mock_application_manager.h"
#include "application_manager/mock_hmi_capabilities.h"
#include "application_manager/mock_hmi_interface.h"
#include "application_manager/mock_message_helper.h"
#include "application_manager/smart_object_keys.h"
#include "smart_objects/smart_object.h"

namespace test {
namespace components {
namespace commands_test {
namespace hmi_commands_test {
namespace tts_is_ready_request {

using ::testing::_;
using ::testing::ReturnRef;
namespace am = ::application_manager;
namespace strings = ::application_manager::strings;
using am::commands::CommandImpl;
using am::commands::MessageSharedPtr;
using am::commands::RequestToHMI;
using am::event_engine::Event;
using sdl_rpc_plugin::commands::TTSIsReadyRequest;

typedef std::shared_ptr<RequestToHMI> RequestToHMIPtr;
typedef std::shared_ptr<TTSIsReadyRequest> TTSIsReadyRequestPtr;

namespace {
const uint32_t kConnectionKey = 2u;
}  // namespace

class TTSIsReadyRequestTest
    : public CommandRequestTest<CommandsTestMocks::kIsNice> {
 public:
  TTSIsReadyRequestTest() : command_(CreateCommand<TTSIsReadyRequest>()) {}

  void SetUpExpectations(bool is_tts_cooperating_available,
                         bool is_send_message_to_hmi,
                         bool is_message_contain_param,
                         am::HmiInterfaces::InterfaceState state) {
    const bool is_send_message_by_timeout = false;
    if (is_send_message_to_hmi) {
      ExpectSendMessagesToHMI(is_send_message_by_timeout);
    }
    EXPECT_CALL(mock_hmi_capabilities_,
                set_is_tts_cooperating(is_tts_cooperating_available));

    if (is_message_contain_param) {
      EXPECT_CALL(app_mngr_, hmi_interfaces())
          .WillRepeatedly(ReturnRef(mock_hmi_interfaces_));
      EXPECT_CALL(
          mock_hmi_interfaces_,
          SetInterfaceState(am::HmiInterfaces::HMI_INTERFACE_TTS, state));
    } else {
      EXPECT_CALL(app_mngr_, hmi_interfaces())
          .WillOnce(ReturnRef(mock_hmi_interfaces_));
      EXPECT_CALL(mock_hmi_interfaces_, SetInterfaceState(_, _)).Times(0);
    }
    EXPECT_CALL(mock_hmi_interfaces_,
                GetInterfaceState(am::HmiInterfaces::HMI_INTERFACE_TTS))
        .WillOnce(Return(state));
  }

  void ExpectSendMessagesToHMI(bool is_send_message_by_timeout) {
    smart_objects::SmartObjectSPtr language(
        new smart_objects::SmartObject(smart_objects::SmartType_Map));
    EXPECT_CALL(mock_message_helper_,
                CreateModuleInfoSO(hmi_apis::FunctionID::TTS_GetLanguage, _))
        .WillOnce(Return(language));
    EXPECT_CALL(mock_hmi_capabilities_, set_handle_response_for(*language));
    EXPECT_CALL(mock_rpc_service_, ManageHMICommand(language, _));

    smart_objects::SmartObjectSPtr support_language(
        new smart_objects::SmartObject(smart_objects::SmartType_Map));
    EXPECT_CALL(
        mock_message_helper_,
        CreateModuleInfoSO(hmi_apis::FunctionID::TTS_GetSupportedLanguages, _))
        .WillOnce(Return(support_language));
    EXPECT_CALL(mock_rpc_service_, ManageHMICommand(support_language, _));

    smart_objects::SmartObjectSPtr capabilities(
        new smart_objects::SmartObject(smart_objects::SmartType_Map));
    EXPECT_CALL(
        mock_message_helper_,
        CreateModuleInfoSO(hmi_apis::FunctionID::TTS_GetCapabilities, _))
        .WillOnce(Return(capabilities));
    EXPECT_CALL(mock_rpc_service_, ManageHMICommand(capabilities, _));
  }

  void PrepareEvent(bool is_message_contain_param,
                    Event& event,
                    bool is_tts_cooperating_available = false) {
    MessageSharedPtr msg = CreateMessage(smart_objects::SmartType_Map);
    if (is_message_contain_param) {
      (*msg)[am::strings::msg_params][am::strings::available] =
          is_tts_cooperating_available;
    }
    event.set_smart_object(*msg);
  }

  void HMICapabilitiesExpectations() {
    std::set<hmi_apis::FunctionID::eType> interfaces_to_update{
        hmi_apis::FunctionID::TTS_GetLanguage,
        hmi_apis::FunctionID::TTS_GetSupportedLanguages,
        hmi_apis::FunctionID::TTS_GetCapabilities};
    EXPECT_CALL(mock_hmi_capabilities_, GetDefaultInitializedCapabilities())
        .WillOnce(Return(interfaces_to_update));
  }

  TTSIsReadyRequestPtr command_;
};

TEST_F(TTSIsReadyRequestTest, RUN_SendRequest_SUCCESS) {
  MessageSharedPtr command_msg(CreateMessage(smart_objects::SmartType_Map));
  (*command_msg)[am::strings::msg_params][am::strings::number] = "123";
  (*command_msg)[am::strings::params][am::strings::connection_key] =
      kConnectionKey;

  RequestToHMIPtr command(CreateCommand<TTSIsReadyRequest>(command_msg));
  EXPECT_CALL(mock_rpc_service_, SendMessageToHMI(command_msg));
  ASSERT_TRUE(command->Init());

  command->Run();

  EXPECT_EQ((*command_msg)[strings::params][strings::protocol_type].asInt(),
            CommandImpl::hmi_protocol_type_);
  EXPECT_EQ((*command_msg)[strings::params][strings::protocol_version].asInt(),
            CommandImpl::protocol_version_);
}

TEST_F(TTSIsReadyRequestTest,
       Run_NoKeyAvailableInMessage_HmiInterfacesIgnored_CacheIsAbsent) {
  const bool is_tts_cooperating_available = false;
  const bool is_send_message_to_hmi = true;
  const bool is_message_contain_param = false;
  Event event(hmi_apis::FunctionID::TTS_IsReady);
  PrepareEvent(is_message_contain_param, event);
  HMICapabilitiesExpectations();
  SetUpExpectations(is_tts_cooperating_available,
                    is_send_message_to_hmi,
                    is_message_contain_param,
                    am::HmiInterfaces::STATE_NOT_RESPONSE);
  command_->on_event(event);
}

TEST_F(TTSIsReadyRequestTest,
       Run_KeyAvailableEqualToFalse_StateNotAvailable_CacheIsAbsent) {
  const bool is_tts_cooperating_available = false;
  const bool is_send_message_to_hmi = false;
  const bool is_message_contain_param = true;
  Event event(hmi_apis::FunctionID::TTS_IsReady);
  PrepareEvent(is_message_contain_param, event);
  SetUpExpectations(is_tts_cooperating_available,
                    is_send_message_to_hmi,
                    is_message_contain_param,
                    am::HmiInterfaces::STATE_NOT_AVAILABLE);
  command_->on_event(event);
}

TEST_F(TTSIsReadyRequestTest,
       Run_KeyAvailableEqualToTrue_StateAvailable_CacheIsAbsnet) {
  const bool is_tts_cooperating_available = true;
  const bool is_send_message_to_hmi = true;
  const bool is_message_contain_param = true;
  Event event(hmi_apis::FunctionID::TTS_IsReady);
  PrepareEvent(is_message_contain_param, event, is_tts_cooperating_available);
  HMICapabilitiesExpectations();
  SetUpExpectations(is_tts_cooperating_available,
                    is_send_message_to_hmi,
                    is_message_contain_param,
                    am::HmiInterfaces::STATE_AVAILABLE);
  command_->on_event(event);
}

TEST_F(TTSIsReadyRequestTest,
       Run_HMIDoestRespond_SendMessageToHMIByTimeout_CacheIsAbsent) {
  std::set<hmi_apis::FunctionID::eType> interfaces_to_update{
      hmi_apis::FunctionID::TTS_GetLanguage,
      hmi_apis::FunctionID::TTS_GetSupportedLanguages,
      hmi_apis::FunctionID::TTS_GetCapabilities};
  EXPECT_CALL(mock_hmi_capabilities_, GetDefaultInitializedCapabilities())
      .WillOnce(Return(interfaces_to_update));
  const bool is_send_message_by_timeout = true;
  ExpectSendMessagesToHMI(is_send_message_by_timeout);
  command_->onTimeOut();
}

}  // namespace tts_is_ready_request
}  // namespace hmi_commands_test
}  // namespace commands_test
}  // namespace components
}  // namespace test
