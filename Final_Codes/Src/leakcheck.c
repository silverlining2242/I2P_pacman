#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define MAX_ALLOCATIONS 1000
#define WARN(msg, line) (printf("Warning %d: %s\n", line, msg))

typedef struct
{
  size_t address;
  size_t size;
  uint32_t line;
} Mem;

struct
{
  Mem mem[MAX_ALLOCATIONS];
  size_t total_allocated_size;
  size_t total_free_size;
} data;

Mem *find_by_address(size_t address)
{
  for (uint32_t i = 0; i < MAX_ALLOCATIONS; i++)
  {
    if (data.mem[i].address == address)
      return &data.mem[i];
  }
  return NULL;
}

void insert(size_t address, size_t size, uint32_t line)
{
  if (address == 0)
  {
    WARN("Memory allocation failed", line);
    return;
  }

  Mem *mem = find_by_address(0);
  if (mem == NULL)
  {
    WARN("Max allocations reached", line);
    return;
  }

  mem->address = address;
  mem->size = size;
  mem->line = line;
  data.total_allocated_size += size;
}

int erase(size_t address, uint32_t line)
{
  if (address == 0)
  {
    WARN("Tried to free a null ptr", line);
    return -1;
  }

  Mem *mem = find_by_address(address);
  if (mem == NULL)
  {
    WARN("Double free detected", line);
    return -1;
  }

  mem->address = 0;
  data.total_free_size += mem->size;
  return 0;
}

void print_report(const char *file_name)
{
  printf("\nLeak Summary: %s\n", file_name);
  printf("Total Memory allocated %lu bytes\n", data.total_allocated_size);
  printf("Total Memory freed     %lu bytes\n", data.total_free_size);
  printf("Memory Leaked          %lu bytes\n\n",
         data.total_allocated_size - data.total_free_size);

  if (data.total_free_size != data.total_allocated_size)
  {
    printf("Detailed Report\n");
    for (int i = 0; i < MAX_ALLOCATIONS; i++)
    {
      if (data.mem[i].address != 0)
      {
        printf("Memory leak at line %d: (%lu bytes)\n",
               data.mem[i].line, data.mem[i].size);
      }
    }
  }
}

void *_malloc(size_t size, uint32_t line)
{
  void *ptr = malloc(size);
  insert((size_t)ptr, size, line);
  return ptr;
}

void _free(void *ptr, uint32_t line)
{
  if (erase((size_t)ptr, line) == 0)
    free(ptr);
}

ALLEGRO_BITMAP *_al_load_bitmap(const char *filename, uint32_t line)
{
  ALLEGRO_BITMAP *bmp = al_load_bitmap(filename);
  insert((size_t)bmp, 0, line); // Size 0, as we don't know the actual size
  return bmp;
}

void _al_destroy_bitmap(ALLEGRO_BITMAP *bmp, uint32_t line)
{
  if (erase((size_t)bmp, line) == 0)
    al_destroy_bitmap(bmp);
}

ALLEGRO_SAMPLE *_al_load_sample(const char *filename, uint32_t line)
{
  ALLEGRO_SAMPLE *sample = al_load_sample(filename);
  insert((size_t)sample, 0, line); // Size 0, as we don't know the actual size
  return sample;
}

void _al_destroy_sample(ALLEGRO_SAMPLE *sample, uint32_t line)
{
  if (erase((size_t)sample, line) == 0)
    al_destroy_sample(sample);
}

ALLEGRO_FONT *_al_load_font(const char *filename, int size, int flags, uint32_t line)
{
  ALLEGRO_FONT *font = al_load_font(filename, size, flags);
  insert((size_t)font, 0, line); // Size 0, as we don't know the actual size
  return font;
}

void _al_destroy_font(ALLEGRO_FONT *font, uint32_t line)
{
  if (erase((size_t)font, line) == 0)
    al_destroy_font(font);
}

// Redefine allocator functions and Allegro resource functions
#define malloc(size) _malloc(size, __LINE__)
#define free(ptr) _free(ptr, __LINE__)
#define al_load_bitmap(filename) _al_load_bitmap(filename, __LINE__)
#define al_destroy_bitmap(bmp) _al_destroy_bitmap(bmp, __LINE__)
#define al_load_sample(filename) _al_load_sample(filename, __LINE__)
#define al_destroy_sample(sample) _al_destroy_sample(sample, __LINE__)
#define al_load_font(filename, size, flags) _al_load_font(filename, size, flags, __LINE__)
#define al_destroy_font(font) _al_destroy_font(font, __LINE__)

// // test main function for demonstration
// int main()
// {
//   // Example usage
//   ALLEGRO_BITMAP *bmp = al_load_bitmap("example.png");
//   al_destroy_bitmap(bmp);

//   ALLEGRO_SAMPLE *sample = al_load_sample("example.ogg");
//   al_destroy_sample(sample);

//   ALLEGRO_FONT *font = al_load_font("example.ttf", 12, 0);
//   al_destroy_font(font);

//   print_report();
//   return 0;
// }
