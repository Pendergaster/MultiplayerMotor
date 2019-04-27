#ifndef MP_UTILS
#define MP_UTILS
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

typedef uint64_t	u64;
typedef uint32_t	u32;
typedef int32_t		i32;
typedef uint16_t	u16;
typedef uint8_t		u8;
static const u16 numeric_max_u16 = 0xFFFF;
static const u32 numeric_max_u32 = 0xFFFFFFFF;
static const u64 numeric_max_u64 = 0xFFFFFFFFFFFFFFFF;

#define LOG_STR "LOG: "
#define LOG(...) do{printf(LOG_STR __VA_ARGS__ ); \
printf(" in file : %s:%d \n", __FILE__, __LINE__);} while(0) 

#define ARRAY_SIZE(arr) (sizeof (arr) / sizeof *(arr))

#define ASSERT_MESSAGE(condition,...) do{ if(!(condition)){  LOG(__VA_ARGS__); assert(condition);}}while(0) 
#define ABORT_MESSAGE(...) do{ LOG(__VA_ARGS__);fflush(stdout); assert(false); }while(0) 

#define BIT_CHECK(a,b) ((a & b) > 0) // onks oikein?
#define BIT_SET(a,b) ( a |= b)
#define BIT_UNSET(a,b) (a &= ~b)

#define KILOS(NUM) ((NUM) * 1000)
#define MEGAS(NUM) ((NUM) * 1000000)
#define GIGAS(NUM) ((NUM) * 1000000000)

#define BETWEEN(a,x,b) (a < x && b > x)
#define CONCAT_INTERNAL(x,y) x##y
#define CONCAT(x,y) CONCAT_INTERNAL(x,y)
//https://pastebin.com/3YvWQa5c
template<typename T>
struct CleanUP {
    T lambda;
    CleanUP(T lambda):lambda(lambda){}
    ~CleanUP(){lambda();}
    CleanUP(const CleanUP&);
  private:
    CleanUP& operator =(const CleanUP&);
};

class CleanUpHelp {
  public:
    template<typename T>
        CleanUP<T> operator+(T t){ return t;}
};

#define defer const auto& CONCAT(defer__, __LINE__) = CleanUpHelp() + [&]()
#endif //MP_UTILS
