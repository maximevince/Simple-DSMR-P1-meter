#pragma once

#include <string>
#include <queue>

#include "esphome.h"
#include "esphome/core/component.h"

#include "WiFiUdp.h"


#define TAG "udpserver"

namespace esphome {
  namespace udpserver {
    class OnStringDataTrigger;

    // Helper class to store UDP response context
    class UDPContext {
    public:
      UDPContext(WiFiUDP* udp_ptr, const char* ip, uint16_t port)
        : udp_(udp_ptr), remote_ip_(ip), remote_port_(port) {}

      // Send a response back to the sender
      bool send_response(const std::string& data) {
        if (!udp_ || !udp_->beginPacket(remote_ip_, remote_port_)) {
          return false;
        }
        udp_->write((const uint8_t*)data.c_str(), data.length());
        return udp_->endPacket();
      }

      const char* get_remote_ip() const { return remote_ip_; }
      uint16_t get_remote_port() const { return remote_port_; }

    private:
      WiFiUDP* udp_;
      const char* remote_ip_;
      uint16_t remote_port_;
    };

    class UdpserverComponent : public Component {
    public:
      UdpserverComponent() = default;
      void setup() override;
      void loop() override;

      void set_listen_port(uint16_t port) { this->port_ = port; }
      void add_string_trigger(OnStringDataTrigger *trigger) { this->string_triggers_.push_back(trigger); }

      // IP filtering methods
      void add_allowed_ip(const std::string& ip) { this->allowed_ips_.push_back(ip); }
      void set_allow_all_ips(bool allow) { this->allow_all_ips_ = allow; }

    protected:
        void process_(uint8_t *buf, size_t len, const char* remote_ip, uint16_t remote_port);
        bool is_ip_allowed(const char* ip);

        uint16_t port_{8888};
        WiFiUDP udp{};
        std::vector<OnStringDataTrigger *> string_triggers_{};
        std::vector<std::string> allowed_ips_{};
        bool allow_all_ips_{true};
        bool udp_initialized_{false};
    };

    class OnStringDataTrigger : public Trigger<std::string, UDPContext>, public Component {
      friend class UdpserverComponent;

    public:
      explicit OnStringDataTrigger(UdpserverComponent *parent) : parent_(parent){};

      void setup() override { this->parent_->add_string_trigger(this); }

      // Text filter methods
      void set_text_filter(const std::string& filter) { this->text_filter_ = filter; }
      void set_filter_mode(const std::string& mode) { this->filter_mode_ = mode; }

    protected:
      bool matches_filter(const std::string& data);

      UdpserverComponent *parent_;
      std::string text_filter_{};
      std::string filter_mode_{"none"}; // none, contains, starts_with, ends_with, equals
    };

  }
}
