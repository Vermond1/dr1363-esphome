#include "esphome.h"

class DR1363Component : public Component, public UARTDevice {
 public:
  DR1363Component(UARTComponent *parent) : UARTDevice(parent) {}

  void setup() override {}

  void loop() override {
    static std::string line;

    while (available()) {
      char c = read();

      if (c == '\r') {
        process_line(line);
        line.clear();
      } else {
        line.push_back(c);
      }
    }

    poll_service_42();
  }

 protected:
  unsigned long last_poll_ = 0;

  void poll_service_42() {
    if (millis() - last_poll_ > 2000) {
      const char *poll42 = "~22004A42E00200FD2A\r";
      write_str(poll42);
      last_poll_ = millis();
    }
  }

  void process_line(const std::string &line) {
    if (line.rfind("~22004A00E0", 0) != 0) return;

    size_t idx = line.find("E0C6");
    if (idx == std::string::npos) return;

    std::string payload = line.substr(idx + 4);

    int len = payload.length() / 2;
    std::vector<uint8_t> data(len);

    for (int i = 0; i < len; i++) {
      std::string byte_str = payload.substr(i * 2, 2);
      data[i] = strtoul(byte_str.c_str(), nullptr, 16);
    }

    parse_payload(data);
  }

  void parse_payload(const std::vector<uint8_t> &data) {
    float soc     = (data[1] << 8 | data[2]) / 100.0;
    float voltage = (data[3] << 8 | data[4]) / 100.0;
    uint8_t cellCount = data[5];

    id(bms_soc).publish_state(soc);
    id(bms_voltage).publish_state(voltage);
    id(bms_cellcount).publish_state(cellCount);

    int cellBase = 6;
    for (int i = 0; i < cellCount; i++) {
      int idx = cellBase + i * 2;
      float cellV = ((data[idx] << 8) | data[idx + 1]) / 1000.0;

      switch (i) {
        case 0: id(cell_1).publish_state(cellV); break;
        case 1: id(cell_2).publish_state(cellV); break;
        case 2: id(cell_3).publish_state(cellV); break;
        case 3: id(cell_4).publish_state(cellV); break;
        case 4: id(cell_5).publish_state(cellV); break;
        case 5: id(cell_6).publish_state(cellV); break;
        case 6: id(cell_7).publish_state(cellV); break;
        case 7: id(cell_8).publish_state(cellV); break;
        case 8: id(cell_9).publish_state(cellV); break;
        case 9: id(cell_10).publish_state(cellV); break;
        case 10: id(cell_11).publish_state(cellV); break;
        case 11: id(cell_12).publish_state(cellV); break;
        case 12: id(cell_13).publish_state(cellV); break;
        case 13: id(cell_14).publish_state(cellV); break;
        case 14: id(cell_15).publish_state(cellV); break;
        case 15: id(cell_16).publish_state(cellV); break;
      }
    }

    int tempBase = cellBase + cellCount * 2;

    float tempENV  = (data[tempBase]     << 8 | data[tempBase+1]) / 10.0;
    float tempPACK = (data[tempBase+2]   << 8 | data[tempBase+3]) / 10.0;
    float tempMOS  = (data[tempBase+4]   << 8 | data[tempBase+5]) / 10.0;

    id(temp_env).publish_state(tempENV);
    id(temp_pack).publish_state(tempPACK);
    id(temp_mos).publish_state(tempMOS);

    int tOffset = tempBase + 6 + 1;

    float tempT1 = (data[tOffset]     << 8 | data[tOffset+1]) / 10.0;
    float tempT2 = (data[tOffset+2]   << 8 | data[tOffset+3]) / 10.0;
    float tempT3 = (data[tOffset+4]   << 8 | data[tOffset+5]) / 10.0;

    id(temp_t1).publish_state(tempT1);
    id(temp_t2).publish_state(tempT2);
    id(temp_t3).publish_state(tempT3);

    float current = (int16_t)(data[53] << 8 | data[54]) / 100.0;
    id(bms_current).publish_state(current);
  }
};

