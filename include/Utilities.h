#pragma once

#define FILE_BLOCK_SIZE 256

#include <time.h>

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
    DELETE_FILE = 6,
    OTHER
  };

  typedef struct Message {
    Type type;
    Field field;
    int contentSize;
    char content[FILE_BLOCK_SIZE];
    int fileSize;
    time_t lastModified;
  } Message;
  
}