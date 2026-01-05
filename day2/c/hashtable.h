typedef struct entry {
  long value;
  struct entry *next; // for chaining
} Entry;

typedef struct hashtable {
  Entry **buckets;
  long capacity;
} HashTable;

Entry* new_entry(long value);
void free_entries(Entry *ePtr);
void free_bucket_entries(long capacity, Entry **bucketArr);
void free_hash_table(HashTable *tPtr);
long hash_key(long value, long numBuckets);
int add(HashTable *tPtr, long value);
int delete(HashTable *tPtr, long value);
int contains(HashTable *tPtr, long value);
HashTable *new_hash_table(long capacity);

