//
// Created by wolek on 20.05.24.
//
#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

enum class MsgId : uint32_t {
  mInvalid,
  mImuData,
  mPidData,
  mStrData
};

struct MsgHeader {
  uint32_t msgPattern;
  MsgId msgId;
  uint32_t dataLength;
};

struct MsgPayload {
  uint8_t data[500];
  /* payloadOffset won't be sent to receiver so maximum possible length will be 512*/
  uint8_t* payloadOffset;
};

struct LogPacket {
  MsgHeader msgHeader;
  MsgPayload msgPayload;
  [[nodiscard]] size_t getMsgTotalLength() const;
  void fillHeader(size_t dataLength, MsgId msgId);
  void fillPayload(void* data, size_t dataLength);
};

static_assert(std::is_trivial_v<LogPacket> && std::is_standard_layout_v<LogPacket>, "LogPacket is not POD type\n");