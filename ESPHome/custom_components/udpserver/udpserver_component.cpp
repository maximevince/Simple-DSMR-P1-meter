#include "udpserver_component.h"

#ifdef USE_WIFI
#include "esphome/components/wifi/wifi_component.h"
#endif
#ifdef USE_ETHERNET
#include "esphome/components/ethernet/ethernet_component.h"
#endif

namespace esphome {
    namespace udpserver {

        static const size_t MAX_PACKET_SIZE = 1400;

        void UdpserverComponent::setup() {
            ESP_LOGCONFIG(TAG, "Setting up UDP Server on port %d...", this->port_);
            // Don't start UDP here - network may not be ready yet
            // Will start in loop() when network is available
        }

        bool UdpserverComponent::is_ip_allowed(const char* ip) {
            // If no IP filter is set, allow all
            if (this->allow_all_ips_ || this->allowed_ips_.empty()) {
                return true;
            }

            // Check if IP is in allowed list
            std::string ip_str(ip);
            for (const auto& allowed_ip : this->allowed_ips_) {
                if (ip_str == allowed_ip) {
                    ESP_LOGD(TAG, "IP %s is allowed", ip);
                    return true;
                }
            }

            ESP_LOGW(TAG, "IP %s is not in allowed list, rejecting packet", ip);
            return false;
        }

        void UdpserverComponent::process_(uint8_t *buf, const size_t len, const char* remote_ip, uint16_t remote_port) {
            ESP_LOGD(TAG, "Processing data, length=%d, data=%s", len, (char*)buf);

            // Create UDP context for response capability
            UDPContext udp_ctx(&udp, remote_ip, remote_port);
            std::string data_str((char*)buf, len);

            // Trigger all registered string data triggers with data and context
            for (auto& trigger : string_triggers_) {
                // Check if trigger has text filter
                if (trigger->matches_filter(data_str)) {
                    trigger->trigger(data_str, udp_ctx);
                }
            }
        }

        bool OnStringDataTrigger::matches_filter(const std::string& data) {
            // No filter set, allow all
            if (this->filter_mode_ == "none" || this->text_filter_.empty()) {
                return true;
            }

            // Apply filter based on mode
            if (this->filter_mode_ == "contains") {
                bool match = data.find(this->text_filter_) != std::string::npos;
                if (!match) {
                    ESP_LOGD(TAG, "Text filter 'contains' not matched: '%s' not in '%s'",
                             this->text_filter_.c_str(), data.c_str());
                }
                return match;
            }
            else if (this->filter_mode_ == "starts_with") {
                bool match = data.rfind(this->text_filter_, 0) == 0;
                if (!match) {
                    ESP_LOGD(TAG, "Text filter 'starts_with' not matched: '%s'", data.c_str());
                }
                return match;
            }
            else if (this->filter_mode_ == "ends_with") {
                bool match = data.length() >= this->text_filter_.length() &&
                           data.compare(data.length() - this->text_filter_.length(),
                                      this->text_filter_.length(), this->text_filter_) == 0;
                if (!match) {
                    ESP_LOGD(TAG, "Text filter 'ends_with' not matched: '%s'", data.c_str());
                }
                return match;
            }
            else if (this->filter_mode_ == "equals") {
                bool match = data == this->text_filter_;
                if (!match) {
                    ESP_LOGD(TAG, "Text filter 'equals' not matched: '%s' != '%s'",
                             data.c_str(), this->text_filter_.c_str());
                }
                return match;
            }

            return true;
        }

        void UdpserverComponent::loop() {
            // Initialize UDP on first loop iteration when network is ready
            if (!udp_initialized_) {
#ifdef USE_WIFI
                if (!wifi::global_wifi_component->is_connected()) {
                    return;  // Wait for WiFi
                }
#endif
#ifdef USE_ETHERNET
                if (!ethernet::global_eth_component->is_connected()) {
                    return;  // Wait for Ethernet
                }
#endif

                ESP_LOGI(TAG, "Network ready, starting UDP server on port %d", this->port_);
                if (!udp.begin(this->port_)) {
                    ESP_LOGE(TAG, "Failed to start UDP server on port %d", this->port_);
                    this->mark_failed();
                    return;
                }
                udp_initialized_ = true;
                ESP_LOGI(TAG, "UDP Server started successfully on port %d", this->port_);
            }

            int packetSize = udp.parsePacket();

            if (packetSize > 0) {
                if (packetSize > MAX_PACKET_SIZE) {
                    ESP_LOGW(TAG, "Received packet size %d exceeds maximum %d, truncating",
                             packetSize, MAX_PACKET_SIZE);
                    packetSize = MAX_PACKET_SIZE;
                }

                uint8_t buffer[MAX_PACKET_SIZE];
                int len = udp.read(buffer, packetSize);

                if (len > 0) {
                    buffer[len] = '\0'; // Null-terminate for string safety

                    // Get remote IP and port
                    IPAddress remote_addr = udp.remoteIP();
                    uint16_t remote_port = udp.remotePort();

                    // Convert IPAddress to string
                    char remote_ip_str[16];
                    snprintf(remote_ip_str, sizeof(remote_ip_str), "%d.%d.%d.%d",
                             remote_addr[0], remote_addr[1], remote_addr[2], remote_addr[3]);

                    ESP_LOGD(TAG, "Received UDP packet: %d bytes from %s:%d",
                             len, remote_ip_str, remote_port);

                    // Check IP filtering
                    if (is_ip_allowed(remote_ip_str)) {
                        process_(buffer, len, remote_ip_str, remote_port);
                    } else {
                        ESP_LOGW(TAG, "Packet from %s rejected by IP filter", remote_ip_str);
                    }
                } else {
                    ESP_LOGW(TAG, "Failed to read UDP packet data");
                }
            }
        }
    }
}
