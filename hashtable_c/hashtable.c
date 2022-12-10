#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int key_t;
typedef int value_t;

inline void int_cleanup(int* value) {
  (void)value;
}

inline void int_copy(const int* src, int* dst) {
  *dst = *src;
}

inline void int_init(int* value) {
  *value = 0;
}

inline size_t int_hash(const int* value) {
  return (size_t)*value;
}

inline bool int_eq(const int* l, const int* r) {
  return *l == *r;
}

typedef struct {
  key_t key;
  value_t value;
} hashtable_entry_t;

typedef struct {
  size_t assigned_index;
  hashtable_entry_t entry;
} hashtable_bucket_t;

typedef struct {
  size_t number_of_buckets;
  size_t size;
  hashtable_bucket_t buckets[];
} hashtable_t;

hashtable_t* create_hashtable(size_t number_of_buckets) {
  hashtable_t* ht = malloc(sizeof(hashtable_t) + number_of_buckets*sizeof(hashtable_bucket_t));
  ht->number_of_buckets = number_of_buckets;
  ht->size = 0;
  for (size_t i = 0; i < ht->number_of_buckets; ++i) {
    ht->buckets[i].assigned_index = SIZE_MAX;
  }
  return ht;
}

inline hashtable_bucket_t* bucket_cleanup(hashtable_t* ht, hashtable_bucket_t* bucket) {
  /* Bucket cleanup */
  int_cleanup(&(bucket->entry.key));
  int_cleanup(&(bucket->entry.value));
  bucket->assigned_index = SIZE_MAX;

  /* Let's search for an entry that can take the freed bucket */
  size_t vacant_index = bucket - &(ht->buckets[0]);
  size_t index = vacant_index;
  size_t assigned_index;
  do {
    if (++index == ht->number_of_buckets) {
      index = 0;
    }
    assigned_index = ht->buckets[index].assigned_index;
  } while (assigned_index != SIZE_MAX &&
           assigned_index > vacant_index &&
           assigned_index <= index);
  return assigned_index == SIZE_MAX? NULL : &(ht->buckets[index]);
}

inline bool empty_or_eq(const hashtable_bucket_t* bucket, const int* key) {
  return bucket->assigned_index == SIZE_MAX || int_eq(&(bucket->entry.key), key);
}

inline size_t hashtable_index(hashtable_t* ht, const int* key) {
  size_t hash = int_hash(key);
  return hash % ht->number_of_buckets;
}

hashtable_bucket_t* find_bucket(hashtable_t* ht, const int* key, size_t index) {
  hashtable_bucket_t* first = &(ht->buckets[index]);
  hashtable_bucket_t* current = first;
  if (!empty_or_eq(current,key)) {
    hashtable_bucket_t* end = &(ht->buckets[0]) + ht->number_of_buckets; 
    do {
      if (++current == end) {
        current = &(ht->buckets[0]);
      }
    } while (current != first && !empty_or_eq(current, key));
    if (current == first) {
      current = NULL;
    }
  }
  return current;
}

bool insert_init(hashtable_t* ht, const int* key, hashtable_entry_t** entry) {
  size_t index = hashtable_index(ht, key);
  bool insert_successful = false;
  hashtable_bucket_t* bucket = find_bucket(ht, key, index);
  if (bucket != NULL && bucket->assigned_index == SIZE_MAX) {
    bucket->assigned_index = index;
    int_copy(key, &(bucket->entry.key));
    int_init(&(bucket->entry.value));
    insert_successful = true;
    ++ht->size;
  }
  if (entry != NULL) {
    *entry = &(bucket->entry);
  }
  return insert_successful;
}

bool insert_copy(hashtable_t* ht, const int* key, const int* value, hashtable_entry_t** entry) {
  size_t index = hashtable_index(ht, key);
  bool insert_successful = false;
  hashtable_bucket_t* bucket = find_bucket(ht, key, index);
  printf("%lx\n", (size_t)bucket);
  if (bucket != NULL && bucket->assigned_index == SIZE_MAX) {
    bucket->assigned_index = index;
    int_copy(key, &(bucket->entry.key));
    int_copy(value, &(bucket->entry.value));
    insert_successful = true;
    ++ht->size;
  }
  if (entry != NULL) {
    *entry = bucket == NULL? NULL : &(bucket->entry);
  }
  return insert_successful;
}

hashtable_entry_t* find(hashtable_t* ht, const int* key) {
  size_t index = hashtable_index(ht, key);
  hashtable_bucket_t* bucket = find_bucket(ht, key, index);
  hashtable_entry_t* entry = NULL;
  if (bucket != NULL && bucket->assigned_index != SIZE_MAX) {
    entry = &(bucket->entry);
  }
  return entry;
}

bool erase(hashtable_t* ht, const int* key) {
  size_t index = hashtable_index(ht, key);
  bool erased = false;
  hashtable_bucket_t* bucket = find_bucket(ht, key, index);
  hashtable_bucket_t* substitute = NULL;
  if (bucket != NULL && bucket->assigned_index != SIZE_MAX) {
    substitute = bucket_cleanup(ht, bucket);
    erased = true;
    --ht->size;
  }
  while (substitute != NULL) {
    bucket->assigned_index = substitute->assigned_index;
    int_copy(&(substitute->entry.key), &(bucket->entry.key));
    int_copy(&(substitute->entry.value), &(bucket->entry.value));
    bucket = substitute;
    substitute = bucket_cleanup(ht, substitute);
  }
  return erased;
}

void show_hashtable(const hashtable_t* ht) {
  printf("Number of buckets=%lu; size=%lu\n", ht->number_of_buckets, ht->size);
  for (size_t i = 0; i < ht->number_of_buckets; ++i) {
    if (ht->buckets[i].assigned_index == SIZE_MAX) {
      printf("%lu: empty\n", i);
    }
    else {
      printf("%lu: Assigned index=%lu, key=%d, value=%d\n", i,
          ht->buckets[i].assigned_index,
          ht->buckets[i].entry.key,
          ht->buckets[i].entry.value);
    }
  }
}

int main() {
  hashtable_t* ht = create_hashtable(3);
  show_hashtable(ht);
  int cmd;
  scanf("%d", &cmd);
  while (cmd != -1) {
    if (cmd == 0) {
      int key, value;
      hashtable_entry_t* entry;
      scanf("%d%d", &key, &value);
      bool inserted = insert_copy(ht, &key, &value, &entry);
      if (inserted) {
        printf("New entry: %d -> %d\n", entry->key, entry->value);
      }
      else if (entry) {
        printf("Existing entry: %d -> %d\n", entry->key, entry->value);
      }
      else {
        printf("No more room in hash table\n");
      }
    }
    else if (cmd == 1) {
      int key;
      scanf("%d", &key);
      hashtable_entry_t* entry = find(ht, &key);
      if (entry == NULL) {
        printf("Entry not found\n");
      }
      else {
        printf("Found entry: %d -> %d\n", entry->key, entry->value);
      }
    }
    else if (cmd == 2) {
      int key;
      scanf("%d", &key);
      bool erased = erase(ht, &key);
      if (erased) {
        printf("Entry erased successfully\n");
      }
      else {
        printf("Entry not found\n");
      }
    }
    show_hashtable(ht);
    printf("----------------\n");
    scanf("%d", &cmd);
  }
}



