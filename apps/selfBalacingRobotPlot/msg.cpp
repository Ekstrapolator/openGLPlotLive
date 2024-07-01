//
// Created by wolek on 20.05.24.
//
#include <cstring>
#include "msg.hpp"

void LogPacket::fillHeader(size_t dataLength, MsgId msgId) {
  this->msgHeader.msgPattern = 0xDEADBEEF;
  this->msgHeader.msgId = msgId;
  this->msgHeader.dataLength = dataLength;
  this->msgPayload.payloadOffset = &this->msgPayload.data[0];
  /* Clear data */
  memset(&this->msgPayload.data[0], 0, 500);
}
size_t LogPacket::getMsgTotalLength() const {
  return this->msgHeader.dataLength + sizeof(MsgHeader);
}
void LogPacket::fillPayload(void *data, size_t dataLength) {
  this->msgHeader.dataLength += dataLength;
  memcpy(this->msgPayload.payloadOffset, data, dataLength);
  this->msgPayload.payloadOffset += dataLength;
}
