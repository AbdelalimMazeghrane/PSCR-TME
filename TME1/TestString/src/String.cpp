#include "String.h"
#include "strutil.h"

namespace pr
{

// TODO: Implement constructor e.g. using initialization list
String::String (const char *s)
{
  data=newcopy(s);
  std::cout << "String constructor called for: " << s << std::endl;
}

String::~String ()
{
  std::cout << "String destructor called for: " << (data ? data : "(null)")
      << std::endl;
  delete[] data;
}

String::String(const String& other){
  data=newcopy(other.data);
}


String& String::operator=(const String& other){
  if(this != &other){
    delete[] data;
    data=newcopy(other.data);

  }
  return *this;
}

String::String(String&& other) noexcept{
  data=other.data;
  other.data=nullptr;
}


String& String::operator=(String&& other) noexcept{
  if(this != &other){
    delete[] data;
    data=other.data;
    other.data=nullptr;

  } 
  return *this;
  
}

bool String::operator<(const String& other) const{
  if(compare(this->data,other.data)<0){
    return true;
  }
  return false;
}





// TODO : add other operators and functions

}// namespace pr

