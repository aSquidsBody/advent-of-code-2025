#include <stdio.h>

void swap(long **arr, int idx1, int idx2) {
  long *v = arr[idx1];
  arr[idx1] = arr[idx2];
  arr[idx2] = v;
}

// 13 81 3 61 8 34 12 51 24
// 13 3 81 61 8 34 12 51 24
// 13 3 8 61 81 34 12 51 24
// 13 3 8 12 81 34 61 51 24
// 12 3 8 13 81 34 61 51 24
//

void qsort(long **arr, int lo, int hi) {
  if (lo >= hi) {
    return;
  }

  printf("pivot %ld \n", *arr[0]);
  long pivot = arr[lo][0];

  int pb = lo+1;
  for (int idx = pb; idx < hi; idx++) {
    if (arr[idx][0] < pivot) {
      swap(arr, idx, pb); 
      pb++;
    }
  }

  swap(arr, lo, pb-1);
  qsort(arr, lo, pb-2);
  qsort(arr, pb, hi);
}

void sort(long **arr, int nElem) {
  qsort(arr, 0, nElem - 1); 
}


int main(int argc, char **argv) {
  long arr[3][2] = {
    { 3, 2 }, {1, 10}, {2, 10}
  };

  sort((long**)arr, 3);

  for (int i = 0; i < 3; i++) {
    printf("%ld %ld\n", arr[i][0], arr[i][1]);
  }

  return 0;
}

