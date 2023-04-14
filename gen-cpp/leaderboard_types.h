/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef leaderboard_TYPES_H
#define leaderboard_TYPES_H

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>




typedef struct _UserProperties__isset {
  _UserProperties__isset() : userID(false), date(false), strengh(false), magicPower(false), agility(false), maxHealth(false), maxMana(false), villager(false), level(false) {}
  bool userID;
  bool date;
  bool strengh;
  bool magicPower;
  bool agility;
  bool maxHealth;
  bool maxMana;
  bool villager;
  bool level;
} _UserProperties__isset;

class UserProperties {
 public:

  static const char* ascii_fingerprint; // = "C779C700B6D3727CDEBBA15EDD3C2ECA";
  static const uint8_t binary_fingerprint[16]; // = {0xC7,0x79,0xC7,0x00,0xB6,0xD3,0x72,0x7C,0xDE,0xBB,0xA1,0x5E,0xDD,0x3C,0x2E,0xCA};

  UserProperties() : userID(0), date(0), strengh(0), magicPower(0), agility(0), maxHealth(0), maxMana(0), villager(), level(0) {
  }

  virtual ~UserProperties() throw() {}

  int32_t userID;
  int64_t date;
  int32_t strengh;
  int32_t magicPower;
  int32_t agility;
  int32_t maxHealth;
  int32_t maxMana;
  std::string villager;
  int32_t level;

  _UserProperties__isset __isset;

  void __set_userID(const int32_t val) {
    userID = val;
  }

  void __set_date(const int64_t val) {
    date = val;
  }

  void __set_strengh(const int32_t val) {
    strengh = val;
  }

  void __set_magicPower(const int32_t val) {
    magicPower = val;
  }

  void __set_agility(const int32_t val) {
    agility = val;
  }

  void __set_maxHealth(const int32_t val) {
    maxHealth = val;
  }

  void __set_maxMana(const int32_t val) {
    maxMana = val;
  }

  void __set_villager(const std::string& val) {
    villager = val;
  }

  void __set_level(const int32_t val) {
    level = val;
  }

  bool operator == (const UserProperties & rhs) const
  {
    if (!(userID == rhs.userID))
      return false;
    if (!(date == rhs.date))
      return false;
    if (!(strengh == rhs.strengh))
      return false;
    if (!(magicPower == rhs.magicPower))
      return false;
    if (!(agility == rhs.agility))
      return false;
    if (!(maxHealth == rhs.maxHealth))
      return false;
    if (!(maxMana == rhs.maxMana))
      return false;
    if (!(villager == rhs.villager))
      return false;
    if (!(level == rhs.level))
      return false;
    return true;
  }
  bool operator != (const UserProperties &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UserProperties & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(UserProperties &a, UserProperties &b);



#endif
