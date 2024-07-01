//
// Created by wolek on 20.05.24.
//
#pragma once

#include <cstdint>
#include <cstring>

enum class MsgId : uint32_t {
  mInvalid,
  mImuData,
  mPidData,
  mStrData
};

struct MsgHeader {
  uint32_t msgPattern{0};
  MsgId msgId{MsgId::mInvalid};
  uint32_t dataLength{0};
};

struct MsgPayload {
  uint8_t data[500] = {};
  /* payloadOffset won't be sent to receiver so maximum possible length will be 512*/
  uint8_t* payloadOffset = &data[0];
};

struct LogPacket {
  MsgHeader msgHeader;
  MsgPayload msgPayload;
  [[nodiscard]] size_t getMsgTotalLength() const;
  void fillHeader(size_t dataLength, MsgId msgId);
  void fillPayload(void* data, size_t dataLength);
};
