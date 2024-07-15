#include <htif/htif.h>
#include <sm.h>

static inline platform_lock_t *get_console_lock(){
  return &get_sm_state_ptr()->console_lock;
}

void console_init(){
  platform_lock_release(get_console_lock());
}

void putstring(const char* s)
{
  while(!platform_lock_acquire(get_console_lock())) {};
  while (*s)
    htif_putchar(*s++);
  platform_lock_release(get_console_lock());
}

// See LICENSE for license details.

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

int vsnprintf(char* out, size_t n, const char* s, va_list vl)
{
  bool format = false;
  bool longarg = false;
  bool longlongarg = false;
  size_t pos = 0;
  for( ; *s; s++)
  {
    if(format)
    {
      switch(*s)
      {
        case 'l':
          if (s[1] == 'l') {
              longlongarg = true;
              s++;
          }
          else
              longarg = true;
          break;
        case 'p':
          longarg = true;
          if (++pos < n) out[pos-1] = '0';
          if (++pos < n) out[pos-1] = 'x';
        case 'x':
        {
          long long num;
          size_t size;
          if (longarg) {
              num = va_arg(vl, long);
	      size = sizeof(long);
	  } else if (longlongarg) {
              num = va_arg(vl, long long);
	      size = sizeof(long long);
	  } else {
              num = va_arg(vl, int);
	      size = sizeof(int);
	  }
          for(int i = 2*(size)-1; i >= 0; i--) {
            int d = (num >> (4*i)) & 0xF;
            if (++pos < n) out[pos-1] = (d < 10 ? '0'+d : 'a'+d-10);
          }
	  longlongarg = false;
          longarg = false;
          format = false;
          break;
        }
        case 'd':
        {
          long long num;
          if (longarg)
              num = va_arg(vl, long);
          else if (longlongarg)
              num = va_arg(vl, long long);
          else
              num = va_arg(vl, int);
          if (num < 0) {
            num = -num;
            if (++pos < n) out[pos-1] = '-';
          }
          long digits = 1;
          for (long long nn = num; nn /= 10; digits++)
            ;
          for (int i = digits-1; i >= 0; i--) {
            if (pos + i + 1 < n) out[pos + i] = '0' + (num % 10);
            num /= 10;
          }
          pos += digits;
          longarg = false;
          longlongarg = false;
          format = false;
          break;
        }
        case 's':
        {
          const char* s2 = va_arg(vl, const char*);
          while (*s2) {
            if (++pos < n)
              out[pos-1] = *s2;
            s2++;
          }
          longarg = false;
          format = false;
          break;
        }
        case 'c':
        {
          if (++pos < n) out[pos-1] = (char)va_arg(vl,int);
          longarg = false;
          format = false;
          break;
        }
        default:
          break;
      }
    }
    else if(*s == '%')
      format = true;
    else
      if (++pos < n) out[pos-1] = *s;
  }
  if (pos < n)
    out[pos] = 0;
  else if (n)
    out[n-1] = 0;
  return pos;
}

int snprintf(char* out, size_t n, const char* s, ...)
{
  va_list vl;
  va_start(vl, s);
  int res = vsnprintf(out, n, s, vl);
  va_end(vl);
  return res;
}

void vprintm(const char* s, va_list vl)
{
  char buf[256];
  vsnprintf(buf, sizeof buf, s, vl);
  putstring(buf);
}

void printm(const char* s, ...)
{
  va_list vl;

  va_start(vl, s);
  vprintm(s, vl);
  va_end(vl);
}

void console_putchar(char c) {
  while(!platform_lock_acquire(get_console_lock())) {};
  htif_putchar(c);
  platform_lock_release(get_console_lock());
}

uint64_t console_getchar() {
  while(!platform_lock_acquire(get_console_lock())) {};
  uint64_t res =  htif_getchar();
  platform_lock_release(get_console_lock());
  return res;
}

void print_char(char c) {
  while(!platform_lock_acquire(get_console_lock())) {};
  htif_putchar(c);
  platform_lock_release(get_console_lock());
}

void print_str(char* s) {
  while (*s != 0) {
    print_char(*s++);
  }
}

void print_int(uint64_t n) {
   uint64_t ru = 1;
   for(uint64_t m = n; m > 1; m /= 10) {
      ru *= 10;
   }
   for(uint64_t i = ru; i >= 1; i /= 10) {
      char c = '0' + ((n / i) % 10);
      print_char(c);
   }
   return;
}

void send_exit_cmd(int c){
  if(c == 0) {
    *tohost = TOHOST_CMD(0, 0, 0b01); // report test done; 0 exit code
  }
  else {
    *tohost = TOHOST_CMD(0, 0, 0b11); // report test done; 1 exit code
  }
}
