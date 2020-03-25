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

#include <string>

#include "application_manager/commands/command_request_test.h"
#include "application_manager/commands/commands_test.h"
#include "application_manager/commands/request_to_hmi.h"
#include "application_manager/smart_object_keys.h"
#include "gtest/gtest.h"
#include "hmi/ui_get_language_request.h"
#include "smart_objects/smart_object.h"

namespace test {
namespace components {
namespace commands_test {
namespace hmi_commands_test {
namespace ui_get_language_request {

using ::testing::_;
using ::testing::Return;
namespace am = ::application_manager;
namespace strings = ::application_manager::strings;
using am::commands::CommandImpl;
using am::commands::RequestToHMI;
using sdl_rpc_plugin::commands::UIGetLanguageRequest;

typedef std::shared_ptr<RequestToHMI> RequestToHMIPtr;

namespace {
const uint32_t kConnectionKey = 2u;
const std::string kStrNumber = "123";
}  // namespace

class UIGetLanguageRequestTest
    : public CommandsTest<CommandsTestMocks::kIsNice> {};

TEST_F(UIGetLanguageRequestTest, RUN_SendRequest_SUCCESS) {
  MessageSharedPtr command_msg(CreateMessage(smart_objects::SmartType_Map));
  (*command_msg)[am::strings::msg_params][am::strings::number] = kStrNumber;
  (*command_msg)[am::strings::params][am::strings::connection_key] =
      kConnectionKey;
  RequestToHMIPtr command(CreateCommand<UIGetLanguageRequest>(command_msg));

  EXPECT_CALL(mock_rpc_service_, SendMessageToHMI(command_msg));
  ASSERT_TRUE(command->Init());

  command->Run();

  EXPECT_EQ((*command_msg)[strings::params][strings::protocol_type].asInt(),
            CommandImpl::hmi_protocol_type_);
  EXPECT_EQ((*command_msg)[strings::params][strings::protocol_version].asInt(),
            CommandImpl::protocol_version_);
}

TEST_F(UIGetLanguageRequestTest,
       onTimeOut_OnCapabilityInitialized_RemoveUIGetLanguage) {
  MessageSharedPtr command_msg(CreateMessage(smart_objects::SmartType_Map));
  (*command_msg)[am::strings::msg_params][am::strings::number] = kStrNumber;
  (*command_msg)[am::strings::params][am::strings::connection_key] =
      kConnectionKey;
  RequestToHMIPtr command(CreateCommand<UIGetLanguageRequest>(command_msg));

  EXPECT_CALL(mock_hmi_capabilities_,
              OnCapabilityInitialized(hmi_apis::FunctionID::UI_GetLanguage));
  ASSERT_TRUE(command->Init());

  command->Run();
  command->onTimeOut();

  EXPECT_EQ((*command_msg)[strings::params][strings::protocol_type].asInt(),
            CommandImpl::hmi_protocol_type_);
  EXPECT_EQ((*command_msg)[strings::params][strings::protocol_version].asInt(),
            CommandImpl::protocol_version_);
}

}  // namespace ui_get_language_request
}  // namespace hmi_commands_test
}  // namespace commands_test
}  // namespace components
}  // namespace test
