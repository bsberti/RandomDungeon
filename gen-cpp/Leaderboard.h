/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef Leaderboard_H
#define Leaderboard_H

#include <thrift/TDispatchProcessor.h>
#include "leaderboard_types.h"



class LeaderboardIf {
 public:
  virtual ~LeaderboardIf() {}
  virtual void setHighScore(const int32_t playerId, const int32_t highScore) = 0;
  virtual void getTop20(std::map<int32_t, int32_t> & _return) = 0;
  virtual bool login(const std::string& email, const std::string& password, bool& newLogin) = 0;
  virtual bool createAccount(const std::string& email, const std::string& hashedPassword, const std::string& salt) = 0;
};

class LeaderboardIfFactory {
 public:
  typedef LeaderboardIf Handler;

  virtual ~LeaderboardIfFactory() {}

  virtual LeaderboardIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(LeaderboardIf* /* handler */) = 0;
};

class LeaderboardIfSingletonFactory : virtual public LeaderboardIfFactory {
 public:
  LeaderboardIfSingletonFactory(const boost::shared_ptr<LeaderboardIf>& iface) : iface_(iface) {}
  virtual ~LeaderboardIfSingletonFactory() {}

  virtual LeaderboardIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(LeaderboardIf* /* handler */) {}

 protected:
  boost::shared_ptr<LeaderboardIf> iface_;
};

class LeaderboardNull : virtual public LeaderboardIf {
 public:
  virtual ~LeaderboardNull() {}
  void setHighScore(const int32_t /* playerId */, const int32_t /* highScore */) {
    return;
  }
  void getTop20(std::map<int32_t, int32_t> & /* _return */) {
    return;
  }
  bool login(const std::string& /* email */, const std::string& /* password */, const bool /* newLogin */) {
    bool _return = false;
    return _return;
  }
  bool createAccount(const std::string& /* email */, const std::string& /* hashedPassword */, const std::string& /* salt */) {
    bool _return = false;
    return _return;
  }
};

typedef struct _Leaderboard_setHighScore_args__isset {
  _Leaderboard_setHighScore_args__isset() : playerId(false), highScore(false) {}
  bool playerId;
  bool highScore;
} _Leaderboard_setHighScore_args__isset;

class Leaderboard_setHighScore_args {
 public:

  Leaderboard_setHighScore_args() : playerId(0), highScore(0) {
  }

  virtual ~Leaderboard_setHighScore_args() throw() {}

  int32_t playerId;
  int32_t highScore;

  _Leaderboard_setHighScore_args__isset __isset;

  void __set_playerId(const int32_t val) {
    playerId = val;
  }

  void __set_highScore(const int32_t val) {
    highScore = val;
  }

  bool operator == (const Leaderboard_setHighScore_args & rhs) const
  {
    if (!(playerId == rhs.playerId))
      return false;
    if (!(highScore == rhs.highScore))
      return false;
    return true;
  }
  bool operator != (const Leaderboard_setHighScore_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Leaderboard_setHighScore_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Leaderboard_setHighScore_pargs {
 public:


  virtual ~Leaderboard_setHighScore_pargs() throw() {}

  const int32_t* playerId;
  const int32_t* highScore;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Leaderboard_getTop20_args {
 public:

  Leaderboard_getTop20_args() {
  }

  virtual ~Leaderboard_getTop20_args() throw() {}


  bool operator == (const Leaderboard_getTop20_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const Leaderboard_getTop20_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Leaderboard_getTop20_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Leaderboard_getTop20_pargs {
 public:


  virtual ~Leaderboard_getTop20_pargs() throw() {}


  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Leaderboard_getTop20_result__isset {
  _Leaderboard_getTop20_result__isset() : success(false) {}
  bool success;
} _Leaderboard_getTop20_result__isset;

class Leaderboard_getTop20_result {
 public:

  Leaderboard_getTop20_result() {
  }

  virtual ~Leaderboard_getTop20_result() throw() {}

  std::map<int32_t, int32_t>  success;

  _Leaderboard_getTop20_result__isset __isset;

  void __set_success(const std::map<int32_t, int32_t> & val) {
    success = val;
  }

  bool operator == (const Leaderboard_getTop20_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Leaderboard_getTop20_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Leaderboard_getTop20_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Leaderboard_getTop20_presult__isset {
  _Leaderboard_getTop20_presult__isset() : success(false) {}
  bool success;
} _Leaderboard_getTop20_presult__isset;

class Leaderboard_getTop20_presult {
 public:


  virtual ~Leaderboard_getTop20_presult() throw() {}

  std::map<int32_t, int32_t> * success;

  _Leaderboard_getTop20_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _Leaderboard_login_args__isset {
  _Leaderboard_login_args__isset() : email(false), password(false), newLogin(false) {}
  bool email;
  bool password;
  bool newLogin;
} _Leaderboard_login_args__isset;

class Leaderboard_login_args {
 public:

  Leaderboard_login_args() : email(), password(), newLogin(0) {
  }

  virtual ~Leaderboard_login_args() throw() {}

  std::string email;
  std::string password;
  bool newLogin;

  _Leaderboard_login_args__isset __isset;

  void __set_email(const std::string& val) {
    email = val;
  }

  void __set_password(const std::string& val) {
    password = val;
  }

  void __set_newLogin(const bool val) {
    newLogin = val;
  }

  bool operator == (const Leaderboard_login_args & rhs) const
  {
    if (!(email == rhs.email))
      return false;
    if (!(password == rhs.password))
      return false;
    if (!(newLogin == rhs.newLogin))
      return false;
    return true;
  }
  bool operator != (const Leaderboard_login_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Leaderboard_login_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Leaderboard_login_pargs {
 public:


  virtual ~Leaderboard_login_pargs() throw() {}

  const std::string* email;
  const std::string* password;
  const bool* newLogin;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Leaderboard_login_result__isset {
  _Leaderboard_login_result__isset() : success(false) {}
  bool success;
} _Leaderboard_login_result__isset;

class Leaderboard_login_result {
 public:

  Leaderboard_login_result() : success(0) {
  }

  virtual ~Leaderboard_login_result() throw() {}

  bool success;

  _Leaderboard_login_result__isset __isset;

  void __set_success(const bool val) {
    success = val;
  }

  bool operator == (const Leaderboard_login_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Leaderboard_login_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Leaderboard_login_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Leaderboard_login_presult__isset {
  _Leaderboard_login_presult__isset() : success(false) {}
  bool success;
} _Leaderboard_login_presult__isset;

class Leaderboard_login_presult {
 public:


  virtual ~Leaderboard_login_presult() throw() {}

  bool* success;

  _Leaderboard_login_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _Leaderboard_createAccount_args__isset {
  _Leaderboard_createAccount_args__isset() : email(false), hashedPassword(false), salt(false) {}
  bool email;
  bool hashedPassword;
  bool salt;
} _Leaderboard_createAccount_args__isset;

class Leaderboard_createAccount_args {
 public:

  Leaderboard_createAccount_args() : email(), hashedPassword(), salt() {
  }

  virtual ~Leaderboard_createAccount_args() throw() {}

  std::string email;
  std::string hashedPassword;
  std::string salt;

  _Leaderboard_createAccount_args__isset __isset;

  void __set_email(const std::string& val) {
    email = val;
  }

  void __set_hashedPassword(const std::string& val) {
    hashedPassword = val;
  }

  void __set_salt(const std::string& val) {
    salt = val;
  }

  bool operator == (const Leaderboard_createAccount_args & rhs) const
  {
    if (!(email == rhs.email))
      return false;
    if (!(hashedPassword == rhs.hashedPassword))
      return false;
    if (!(salt == rhs.salt))
      return false;
    return true;
  }
  bool operator != (const Leaderboard_createAccount_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Leaderboard_createAccount_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Leaderboard_createAccount_pargs {
 public:


  virtual ~Leaderboard_createAccount_pargs() throw() {}

  const std::string* email;
  const std::string* hashedPassword;
  const std::string* salt;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Leaderboard_createAccount_result__isset {
  _Leaderboard_createAccount_result__isset() : success(false) {}
  bool success;
} _Leaderboard_createAccount_result__isset;

class Leaderboard_createAccount_result {
 public:

  Leaderboard_createAccount_result() : success(0) {
  }

  virtual ~Leaderboard_createAccount_result() throw() {}

  bool success;

  _Leaderboard_createAccount_result__isset __isset;

  void __set_success(const bool val) {
    success = val;
  }

  bool operator == (const Leaderboard_createAccount_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Leaderboard_createAccount_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Leaderboard_createAccount_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Leaderboard_createAccount_presult__isset {
  _Leaderboard_createAccount_presult__isset() : success(false) {}
  bool success;
} _Leaderboard_createAccount_presult__isset;

class Leaderboard_createAccount_presult {
 public:


  virtual ~Leaderboard_createAccount_presult() throw() {}

  bool* success;

  _Leaderboard_createAccount_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class LeaderboardClient : virtual public LeaderboardIf {
 public:
  LeaderboardClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  LeaderboardClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void setHighScore(const int32_t playerId, const int32_t highScore);
  void send_setHighScore(const int32_t playerId, const int32_t highScore);
  void getTop20(std::map<int32_t, int32_t> & _return);
  void send_getTop20();
  void recv_getTop20(std::map<int32_t, int32_t> & _return);
  bool login(const std::string& email, const std::string& password, bool& newLogin);
  void send_login(const std::string& email, const std::string& password, const bool newLogin);
  bool recv_login();
  bool createAccount(const std::string& email, const std::string& hashedPassword, const std::string& salt);
  void send_createAccount(const std::string& email, const std::string& hashedPassword, const std::string& salt);
  bool recv_createAccount();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class LeaderboardProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<LeaderboardIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (LeaderboardProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_setHighScore(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_getTop20(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_login(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_createAccount(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  LeaderboardProcessor(boost::shared_ptr<LeaderboardIf> iface) :
    iface_(iface) {
    processMap_["setHighScore"] = &LeaderboardProcessor::process_setHighScore;
    processMap_["getTop20"] = &LeaderboardProcessor::process_getTop20;
    processMap_["login"] = &LeaderboardProcessor::process_login;
    processMap_["createAccount"] = &LeaderboardProcessor::process_createAccount;
  }

  virtual ~LeaderboardProcessor() {}
};

class LeaderboardProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  LeaderboardProcessorFactory(const ::boost::shared_ptr< LeaderboardIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< LeaderboardIfFactory > handlerFactory_;
};

class LeaderboardMultiface : virtual public LeaderboardIf {
 public:
  LeaderboardMultiface(std::vector<boost::shared_ptr<LeaderboardIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~LeaderboardMultiface() {}
 protected:
  std::vector<boost::shared_ptr<LeaderboardIf> > ifaces_;
  LeaderboardMultiface() {}
  void add(boost::shared_ptr<LeaderboardIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void setHighScore(const int32_t playerId, const int32_t highScore) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->setHighScore(playerId, highScore);
    }
    ifaces_[i]->setHighScore(playerId, highScore);
  }

  void getTop20(std::map<int32_t, int32_t> & _return) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->getTop20(_return);
    }
    ifaces_[i]->getTop20(_return);
    return;
  }

  bool login(const std::string& email, const std::string& password, bool& newLogin) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->login(email, password, newLogin);
    }
    return ifaces_[i]->login(email, password, newLogin);
  }

  bool createAccount(const std::string& email, const std::string& hashedPassword, const std::string& salt) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->createAccount(email, hashedPassword, salt);
    }
    return ifaces_[i]->createAccount(email, hashedPassword, salt);
  }

};



#endif
