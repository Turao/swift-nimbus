#include "User.h"
#include <string>

User::User(std::string name) : 
name(name),
loggedIn(false)
{

}

User::~User()
{

}

std::string User::getName()
{
  return this->name;
}

bool User::isLoggedIn()
{
  return this->loggedIn;
}