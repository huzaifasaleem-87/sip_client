/*
   Copyright Christian Taedcke <hacking@taedcke.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#include "asio.hpp"
#include "argparse.hpp"

#include "sip_client/asio_udp_client.h"
#include "sip_client/mbedtls_md5.h"
#include "sip_client/sip_client.h"
#include "sip_client/sip_client_event_handler.h"

#include "keyboard_input.h"

#include <cstring>
#include <string>

std::string CONFIG_SIP_USER;
std::string CONFIG_SIP_PASSWORD;
std::string CONFIG_SIP_SERVER_IP;
std::string CONFIG_SIP_SERVER_PORT;
std::string CONFIG_LOCAL_IP;

std::string CONFIG_CALL_TARGET_USER;
std::string CONFIG_CALLER_DISPLAY_MESSAGE;

static constexpr char const* TAG = "main";

struct MyArgs : public argparse::Args{
        std::string &SIP_user = kwarg("u,user", "Enter your username");
        std::string &SIP_password = kwarg("p,password", "Enter your password");
        std::string &server_ip = kwarg("s,server_ip", "Set server IP address");
        std::string &server_port = kwarg("port", "Set the port that server is listening on").set_default("5060");
        std::string &local_ip = kwarg("l,local_ip", "Set your local machine's IP address");
        std::string &target_user = kwarg("t,target_username", "Enter username of user to call");
        std::string &display_message = kwarg("m,display_message", "Text to be displayed on screen on target user during ringing");
    };

using SipClientT = SipClient<AsioUdpClient, MbedtlsMd5>;

struct handlers_t
{
    SipClientT& client;
    KeyboardInput& input;
    asio::io_context& io_context;
};

static void sip_task(void* pvParameters);

void sip_task(void* pvParameters)
{
    auto* ctx = static_cast<handlers_t*>(pvParameters);
    SipClientT& client = ctx->client;
    bool quit = false;

    static std::tuple handlers {
        SipEventHandlerLog {}
    };

    for (; !quit;)
    {
        if (!client.is_initialized())
        {
            const bool result = client.init();
            ESP_LOGI(TAG, "SIP client initialized %ssuccessfully", result ? "" : "un");
            if (!result)
            {
                ESP_LOGI(TAG, "Waiting to try again...");
                sleep(2); // sleep two seconds
                continue;
            }
            client.set_event_handler([](SipClientT& client, const SipClientEvent& event) {
                std::apply([event, &client](auto&... h) { (h.handle(client, event), ...); }, handlers);
            });
        }

        ctx->input.do_read([&client, &quit, ctx](char c) {
            ESP_LOGI(TAG, "keyinput=%c", c);
            if (c == 'c')
            {
                client.request_ring(CONFIG_CALL_TARGET_USER, CONFIG_CALLER_DISPLAY_MESSAGE);
            }
            else if (c == 'd')
            {
                client.request_cancel();
            }
            else if (c == 'q')
            {
                quit = true;
                client.deinit();
                ctx->io_context.stop();
            }
        });
        ctx->io_context.run();
    }
}

int main(int argc, char** argv)
{
    MyArgs args = argparse::parse<MyArgs>(argc, argv);

    std::string CONFIG_SIP_USER = args.SIP_user;
    std::string CONFIG_SIP_PASSWORD = args.SIP_password;
    std::string CONFIG_SIP_SERVER_IP = args.server_ip;
    std::string CONFIG_SIP_SERVER_PORT = args.server_port;
    std::string CONFIG_LOCAL_IP = args.local_ip;

    std::string CONFIG_CALL_TARGET_USER = args.target_user;
    std::string CONFIG_CALLER_DISPLAY_MESSAGE = args.display_message;
    
    // seed for std::rand() used in the sip client
    std::srand(static_cast<unsigned int>(time(nullptr)));

    // Execute io_context.run() only from one thread
    asio::io_context io_context { 1 };

    SipClientT client { io_context, CONFIG_SIP_USER, CONFIG_SIP_PASSWORD, CONFIG_SIP_SERVER_IP, CONFIG_SIP_SERVER_PORT, CONFIG_LOCAL_IP };

    KeyboardInput input { io_context };

    handlers_t handlers { client, input, io_context };

    sip_task(&handlers);
}
