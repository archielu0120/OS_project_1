#include <stdio.h>

#define MAX_ARRAY_LENGTH (100000 + 7)

int tmp[MAX_ARRAY_LENGTH];
void _sort(int l, int r, int index[], unsigned int R[]) { 
    if(l >= r){
        return;
    }
	
    int mid = (l + r) >> 1;
	
    _sort(l, mid, index, R);
    _sort(mid + 1, r, index, R);

    int i1 = l, i2 = mid + 1, p = l;
    while (i1 <= mid || i2 <= r) {
	if (i1 <= mid && i2 <= r) {
	    if (R[index[i1]] <= R[index[i2]])
		tmp[p++] = index[i1++];
	    else
		tmp[p++] = index[i2++];
	}
	else if (i1 <= mid) 
	    tmp[p++] = index[i1++];
	else 
	    tmp[p++] = index[i2++];
    }

    int i;
    for (i = l; i <= r; i++){
        index[i] = tmp[i];
    }
	
}

void inddex_sort(int n, int index[], unsigned int R[]) {
    // index: 0, 1, 2, ..., n;
    // sort index according to R[index];
    _sort(0, n - 1, index, R);
}

