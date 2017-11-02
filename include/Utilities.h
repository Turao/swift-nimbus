#pragma once

#define FILE_BLOCK_SIZE 256

namespace Utilities
{

  enum Type {
    REQUEST = 1,
    REPLY
  };

  enum Field {
    USERNAME = 1,
    BEGIN_OF_FILE = 2,
    CONTENT_OF_FILE = 3,
    END_OF_FILE = 4,
    FILE = 5,
    OTHER
  };

  typedef struct Message {
    Type type;
    Field field;
    char content[FILE_BLOCK_SIZE];
  } Message;
  
}