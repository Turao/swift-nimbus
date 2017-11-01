#pragma once

namespace Utilities
{

  enum Type {
    REQUEST = 1,
    REPLY
  };

  enum Field {
    USERNAME = 1,
    OTHER
  };

  typedef struct Message {
    Type type;
    Field field;
  } Message;
  
}