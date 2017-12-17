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
    LIST_FILES = 7,
    FILE_TOKEN = 8,
    PERMISSION_GRANTED = 9,
    PERMISSION_DENIED = 10,
    OTHER
  };

  typedef struct Message {
    Type type;
    Field field;
    int contentSize;
    char content[FILE_BLOCK_SIZE];
    int fileSize;
    time_t lastModified;
    time_t statusChange;
    time_t lastAccess;
  } Message;
  
}